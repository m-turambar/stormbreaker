#include "detector_facial.h"
#include "dlib_mk.h" //alinear y reconocer cara
#include "input_nodes.h"
#include <dlib/opencv/cv_image.h>
#include <dlib/clustering/chinese_whispers.h>
#include <dlib/svm.h>
#include <dlib/pixel.h>
#include "dataset.h"

using namespace std;
using namespace cv;
using namespace dlib;
using namespace mk;

detector_facial::detector_facial()
{
    net = cv::dnn::readNetFromCaffe("/home/mike/data/deteccion_cara.prototxt",
                                    "/home/mike/data/res10_300x300_ssd_iter_140000.caffemodel");
}

/*Genera un tensor donde viene información sobre los bounding boxes y la probabilidad de que estos sean el objeto que se busca*/
cv::Mat detector_facial::forward_prop(cv::Mat &img)
{
    cv::Mat blob = cv::dnn::blobFromImage(img, 1.0, cv::Size(300, 300), cv::Scalar(104, 177, 123));
    net.setInput(blob);
    return net.forward();
}

cv::Rect detector_facial::transformar_rect(cv::Rect r, cv::Mat &orig, cv::Mat &sq)
{
    const int sz = sq.cols; //or rows, sq is guaranteed to be a square mat
    cv::Rect ret;
    if (orig.cols > orig.rows)
    {
        int offset = orig.cols - orig.rows;
        Point tl = r.tl();
        float factorx = (float)tl.x / sz;
        float factory = (float)tl.y / sz;
        Point tl2(orig.rows * factorx + offset / 2, orig.rows * factory);
        Point br = r.br();
        factorx = (float)br.x / sz;
        factory = (float)br.y / sz;
        Point br2(orig.rows * factorx + offset / 2, orig.rows * factory);
        ret = Rect(tl2, br2);
    }
    else
    {
        int offset = orig.rows - orig.cols;
        Point tl = r.tl();
        float factorx = (float)tl.x / sz;
        float factory = (float)tl.y / sz;
        Point tl2(orig.cols * factorx, orig.cols * factory + offset / 2);
        Point br = r.br();
        factorx = (float)br.x / sz;
        factory = (float)br.y / sz;
        Point br2(orig.cols * factorx, orig.cols * factory + offset / 2);
        ret = Rect(tl2, br2);
    }

    return ret;
}

std::vector<datos_faciales> detector_facial::procesar(cv::Mat &full_img)
{
    Mat img = make_mat_squared(full_img);
    cv::Mat tensorDeteccionCaras = forward_prop(img);
    auto detecciones = detectar_caras(tensorDeteccionCaras, img.cols, img.rows);

    std::vector<datos_faciales> ret;
    for (auto par_p_rect : detecciones) //por cada deteccion...
    {
        //cout << "Precision de la deteccion: " << par_p_rect.first << "%\n";
        cv::Rect rectangulo_bbox = par_p_rect.second;
        Mat cara = img(rectangulo_bbox);
        dlib::matrix<float, 0, 1> embedding;
        embedding = alinear_y_reconocer_cara(cara);
        datos_faciales tupla = std::make_tuple(par_p_rect.first, transformar_rect(rectangulo_bbox, full_img, img), embedding);
        ret.push_back(tupla);
    }
    return ret;
}
/*TODO: entender porque a veces se generan rectangulos fuera de la imagen*/
/* Extrae un arreglo de rectángulos, correspondientes a las caras detectadas en una imágen.
   Recibe como argumento un tensor producido por una red convolucional entrenada para detectar caras.*/
std::vector<std::pair<float, cv::Rect>> detector_facial::detectar_caras(cv::Mat &tensorDeteccion, unsigned int w, unsigned int h)
{
    /*Las detecciones empiezan por el índice 2, y las coordenadas son los 4
        subsecuentes valores. La matriz ya regresa sorteada, no sé porqué cada entrada
        tiene 7 valores extra de profundidad, pero no los estoy usando. */
    std::vector<std::pair<float, cv::Rect>> ret;
    for (int i = 2;; i += 7)
    {
        float proba = tensorDeteccion.at<cv::Vec3f>(0, 0)[i];
        if (proba < mDetectionThreshold)
            break;

        uint xi = tensorDeteccion.at<cv::Vec3f>(0, 0)[i + 1] * h;
        uint yi = tensorDeteccion.at<cv::Vec3f>(0, 0)[i + 2] * w;
        uint xf = tensorDeteccion.at<cv::Vec3f>(0, 0)[i + 3] * h;
        uint yf = tensorDeteccion.at<cv::Vec3f>(0, 0)[i + 4] * w;

        /*entender esto. A veces ocurría*/
        if (xi > h || yi > w)
            continue;
        //cout << "{" << xi << ", " << yi << "} {" << xf << ", " << yf << "}pre\n";

        /*We correct against the bounding box going out of picture*/
        xi = xi < 0 ? 0 : xi;
        yi = yi < 0 ? 0 : yi;
        xf = xf > h ? h : xf;
        yf = yf > w ? w : yf;

        //cout << "{" << xi << ", " << yi << "} {" << xf << ", " << yf << "}\n";

        ret.push_back(std::make_pair(proba, cv::Rect(xi, yi, xf - xi, yf - yi)));
        //remove this break if you want more than one isntance. super awesome comment
        //break;
    }
    for (int i = 2; i <= 22; i += 7)
        cout << tensorDeteccion.at<cv::Vec3f>(0, 0)[i] << " ";
    cout << endl;
    return ret;
}

void detector_facial::populate_and_cluster(std::string image_dir)
{
  nodo_iter_dir mNodoIteradorDirectorios(cv::Point(500,500), 500, image_dir);
  for(auto s : mNodoIteradorDirectorios.files)
  {
    std::cout << "leyendo " << s << std::endl;
    cv::Mat src = imread(s);
    std::vector<datos_faciales> vec = procesar(src);
    for(auto instancia : vec)
    {
        mEmbeddings.push_back(std::get<2>(instancia));
    }
  }

  for (size_t i = 0; i < mEmbeddings.size(); ++i)
  {
    for (size_t j = i; j < mEmbeddings.size(); ++j)
    {
        auto len = dlib::length(mEmbeddings[i]-mEmbeddings[j]);
        if (len < 0.49)
            mEdges.push_back(dlib::sample_pair(i,j));
    }
  }
  const auto num_clusters = dlib::chinese_whispers(mEdges, mLabels);
  cout << num_clusters << " clusters hallados en " << mLabels.size() << " muestras" << endl;
  mDetectionThreshold = 0.5;
}

std::string detector_facial::clasificar_cara(dlib::matrix<float,0,1>& e)
{
    static bool is_init_with_dataset{false};
    if(!is_init_with_dataset) {
        populate_and_cluster(mImageDir);
        is_init_with_dataset = true;
    }
    std::string label = "Unknown";
    std::vector<dlib::matrix<float,0,1>> copiaEmbeddings = mEmbeddings;
    std::vector<dlib::sample_pair> copiaEdges = mEdges;
    std::vector<unsigned long> copiaLabels = mLabels;
    copiaEmbeddings.push_back(e);
    for (size_t i = 0; i < copiaEmbeddings.size(); ++i)
    {
        auto len = dlib::length(copiaEmbeddings[i]-e);
        if (len < 0.49)
            copiaEdges.push_back(dlib::sample_pair(i,copiaEmbeddings.size()-1));
    }
    const auto num_clusters = dlib::chinese_whispers(copiaEdges, copiaLabels);
    cout << num_clusters << " clusters found in " << copiaLabels.size() << " samples" << endl;
    //gEtiquetas[gLabels];

    return mLabelVector[copiaLabels[copiaEmbeddings.size()-1]];
}