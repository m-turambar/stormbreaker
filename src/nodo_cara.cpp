#include "nodo_cara.h"
#include "input_nodes.h"
#include "dlib_cnn_face_detector.h"
#include "clusters.h"
#include <array>
#include <tuple>

using namespace std;
using namespace cv;
using namespace dlib;

using datos_faciales = std::tuple<float,cv::Rect,dlib::matrix<float,0,1> >;

std::string clasificar_cara(dlib::matrix<float,0,1>& e);

struct detector_facial
{
    detector_facial() {
        net = cv::dnn::readNetFromCaffe("/home/mike/data/deteccion_cara.prototxt",
                                    "/home/mike/data/res10_300x300_ssd_iter_140000.caffemodel");
    }

    /*Genera un tensor donde viene información sobre los bounding boxes y la probabilidad de que estos sean el objeto que se busca*/
    cv::Mat forward_prop(cv::Mat& img)
    {
        cv::Mat blob = cv::dnn::blobFromImage(img, 1.0, cv::Size(300, 300), cv::Scalar(104, 177, 123));
        net.setInput(blob);
        return net.forward();
    }

    cv::Rect transformar_rect(cv::Rect r, cv::Mat& orig, cv::Mat& sq)
    {
        const int sz = sq.cols; //or rows, sq is guaranteed to be a square mat
        cv::Rect ret;
        if(orig.cols > orig.rows)
        {
            int offset = orig.cols - orig.rows;
            Point tl = r.tl();
            float factorx = (float)tl.x/sz;
            float factory = (float)tl.y/sz;
            Point tl2(orig.rows*factorx + offset/2, orig.rows*factory);
            Point br = r.br();
            factorx = (float)br.x/sz;
            factory = (float)br.y/sz;
            Point br2(orig.rows*factorx + offset/2, orig.rows*factory);
            ret = Rect(tl2, br2);
        }
        else
        {
            int offset = orig.rows - orig.cols;
            Point tl = r.tl();
            float factorx = (float)tl.x/sz;
            float factory = (float)tl.y/sz;
            Point tl2(orig.cols*factorx, orig.cols*factory + offset/2);
            Point br = r.br();
            factorx = (float)br.x/sz;
            factory = (float)br.y/sz;
            Point br2(orig.cols*factorx, orig.cols*factory + offset/2);
            ret = Rect(tl2, br2);
        }
        
        return ret;
    }

    std::vector<datos_faciales> procesar(cv::Mat& full_img)
    {
        Mat img =  make_mat_squared(full_img);
        cv::Mat tensorDeteccionCaras = forward_prop(img);
        auto detecciones = detectar_caras(tensorDeteccionCaras, img.cols, img.rows);

        std::vector<datos_faciales> ret;
        for (auto par_p_rect : detecciones) //por cada deteccion...
        {
            //cout << "Precision de la deteccion: " << par_p_rect.first << "%\n";
            cv::Rect rectangulo_bbox = par_p_rect.second;
            Mat cara = img(rectangulo_bbox);
            dlib::matrix<float,0,1> embedding;
            embedding = alinear_y_reconocer_cara(cara);
            datos_faciales tupla = std::make_tuple(par_p_rect.first, transformar_rect(rectangulo_bbox, full_img, img), embedding);
            ret.push_back(tupla);
        }
        return ret;
    }
    /*TODO: entender porque a veces se generan rectangulos fuera de la imagen*/
/* Extrae un arreglo de rectángulos, correspondientes a las caras detectadas en una imágen.
   Recibe como argumento un tensor producido por una red convolucional entrenada para detectar caras.*/
    std::vector<std::pair<float, cv::Rect>> detectar_caras(cv::Mat &tensorDeteccion, unsigned int w, unsigned int h)
    {
        /*Las detecciones empiezan por el índice 2, y las coordenadas son los 4
        subsecuentes valores. La matriz ya regresa sorteada, no sé porqué cada entrada
        tiene 7 valores extra de profundidad, pero no los estoy usando. */

        float DETECTION_THRESHOLD{0.8};
        std::vector<std::pair<float, cv::Rect>> ret;
        for (int i = 2;; i += 7)
        {
            float proba = tensorDeteccion.at<cv::Vec3f>(0, 0)[i];
            if (proba < DETECTION_THRESHOLD)
                break;
                
            int xi = tensorDeteccion.at<cv::Vec3f>(0, 0)[i + 1] * h;
            int yi = tensorDeteccion.at<cv::Vec3f>(0, 0)[i + 2] * w;
            int xf = tensorDeteccion.at<cv::Vec3f>(0, 0)[i + 3] * h;
            int yf = tensorDeteccion.at<cv::Vec3f>(0, 0)[i + 4] * w;

            /*entender esto*/
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
        return ret;
    }

    cv::dnn::Net net;
};

detector_facial gDetectorFacial;
std::vector<dlib::matrix<float,0,1>> gEmbeddings;
std::vector<dlib::sample_pair> gEdges;
std::vector<unsigned long> gLabels;
std::vector<std::string> gLabelVector = {"Rommel","Leo","Kuri","Gibran","Pegueros","Abraham","Praneesh","Ian","Mike","Tintor","Diez","Once"};

void populate_and_cluster()
{
  nodo_iter_dir mNodoIteradorDirectorios(cv::Point(500,500), 500, "/home/mike/proyectos/caras/sw_team");
  for(auto s : mNodoIteradorDirectorios.files)
  {
    std::cout << "leyendo " << s << std::endl;
    cv::Mat src = imread(s);
    auto vec = gDetectorFacial.procesar(src);
    for(auto e : vec)
    {
        gEmbeddings.push_back(std::get<2>(e));
    }
  }

  for (size_t i = 0; i < gEmbeddings.size(); ++i)
  {
    for (size_t j = i; j < gEmbeddings.size(); ++j)
    {
        auto len = dlib::length(gEmbeddings[i]-gEmbeddings[j]);
        if (len < 0.49)
            gEdges.push_back(dlib::sample_pair(i,j));
    }
  }
  const auto num_clusters = dlib::chinese_whispers(gEdges, gLabels);
  cout << num_clusters << " clusters found in " << gLabels.size() << " samples" << endl;
}


std::string clasificar_cara(dlib::matrix<float,0,1>& e)
{
    static bool is_init_with_dataset{false};
    if(!is_init_with_dataset) {
        populate_and_cluster();
        is_init_with_dataset = true;
    }
    std::string label = "Unknown";
    std::vector<dlib::matrix<float,0,1>> copiaEmbeddings = gEmbeddings;
    std::vector<dlib::sample_pair> copiaEdges = gEdges;
    std::vector<unsigned long> copiaLabels = gLabels;
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

    return gLabelVector[copiaLabels[copiaEmbeddings.size()-1]];
}

nodo_cara::nodo_cara(cv::Point c, int r) : nodo(c, r)
{
    mcolor = cv::Scalar(123, 123, 60);
    sid = "DNN" + sid;
}

void nodo_cara::procesar()
{
    if (!msrc.empty())
    {
        mmat = msrc;
        auto vec_tuplas = gDetectorFacial.procesar(mmat);

        for (auto tupla : vec_tuplas) //por cada deteccion...
        {
            float prob_det;
            cv::Rect rectangulo_bbox;
            dlib::matrix<float,0,1> embedding;
            std::tie(prob_det, rectangulo_bbox, embedding) = tupla;
            std::string label = clasificar_cara(embedding);

            cv::rectangle(mmat, rectangulo_bbox, cv::Scalar(20, 20, 230), 1, 8, 0);
            cv::putText(mmat,label,rectangulo_bbox.tl(),cv::FONT_HERSHEY_PLAIN,1,cv::Scalar(255,255,255));
        }
        
    }
}

