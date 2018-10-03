#include "nodo_cara.h"
#include "dlib_cnn_face_detector.h"

using namespace std;
using namespace cv;
using namespace dlib;

nodo_dnn::nodo_dnn(cv::Point c, int r) : nodo(c, r)
{
    mcolor = cv::Scalar(123, 123, 60);
    sid = "DNN" + sid;
    net = cv::dnn::readNetFromCaffe("/home/mike/data/deteccion_cara.prototxt",
                                    "/home/mike/data/res10_300x300_ssd_iter_140000.caffemodel");
}

void nodo_dnn::procesar()
{
    if (!msrc.empty())
    {
        make_mmat_squared();
        
        cv::Mat fw = forward_prop();
        auto detecciones = extract_detections(fw);

        for (auto par_p_rect : detecciones) //por cada deteccion...
        {
            cout << "Precision de la deteccion: " << par_p_rect.first << "%\n";
            cv::rectangle(mmat, par_p_rect.second, cv::Scalar(20, 20, 230), 1, 8, 0);
            Mat cara = mmat(par_p_rect.second);
            dlib::matrix<float, 0, 1> embedding = alinear_y_reconocer_cara(cara);

            //////////////////////
            mis_caras.push_back(cara);
            //////////////////////
            embeddings.push_back(std::move(embedding));
        }
        
    }
}

cv::Mat nodo_dnn::forward_prop()
{
    cv::Mat blob = cv::dnn::blobFromImage(mmat, 1.0, cv::Size(300, 300), cv::Scalar(104, 177, 123));
    net.setInput(blob);
    return net.forward();
}

std::vector<std::pair<float, cv::Rect>> nodo_dnn::extract_detections(cv::Mat &fw)
{
    /*Las detecciones empiezan por el índice 2, y las coordenadas son los 4
      subsecuentes valores. La matriz ya regresa sorteada, no sé porqué cada entrada
      tiene 7 valores extra de profundidad, pero no los estoy usando. Teóricamente
      si tomo el índice 9(?), me dé la probabilidad de la segunda cara por aparecer,
      y los siguientes 4 índices las regiones de ésta. */
    std::vector<std::pair<float, cv::Rect>> ret;
    for (int i = 2;; i += 7)
    {
        float proba = fw.at<cv::Vec3f>(0, 0)[i];
        if (proba < DETECTION_THRESHOLD)
            break;
        //cout << "proba = " << proba << "%\n";
        const int w{mmat.cols}, h{mmat.rows};
        int xi = fw.at<cv::Vec3f>(0, 0)[i + 1] * mmat.rows;
        int yi = fw.at<cv::Vec3f>(0, 0)[i + 2] * mmat.cols;
        int xf = fw.at<cv::Vec3f>(0, 0)[i + 3] * mmat.rows;
        int yf = fw.at<cv::Vec3f>(0, 0)[i + 4] * mmat.cols;
        if (xi > mmat.rows || yi > mmat.cols)
            break;
        cout << "{" << xi << ", " << yi << "} {" << xf << ", " << yf << "}pre\n";
        /*We correct against the bounding box going out of picture*/
        xi = xi < 0 ? 0 : xi;
        yi = yi < 0 ? 0 : yi;
        xf = xf > mmat.rows ? mmat.rows : xf;
        yf = yf > mmat.cols ? mmat.cols : yf;
        cout << "{" << xi << ", " << yi << "} {" << xf << ", " << yf << "}\n";
        ret.push_back(std::make_pair(proba, cv::Rect(xi, yi, xf - xi, yf - yi)));
        //remove this break if you want more than one isntance
        break;
    }
    return ret;
}
