#ifndef MK_DETECTOR_FACIAL
#define MK_DETECTOR_FACIAL

#include <opencv2/core.hpp>
#include <opencv2/dnn.hpp>
#include <tuple>
#include <dlib/matrix.h>

using datos_faciales = std::tuple<float,cv::Rect,dlib::matrix<float,0,1> >;

struct detector_facial
{
    detector_facial();

    /*Genera un tensor donde viene información sobre los bounding boxes y la probabilidad de que estos sean el objeto que se busca*/
    cv::Mat forward_prop(cv::Mat& img);

    cv::Rect transformar_rect(cv::Rect r, cv::Mat& orig, cv::Mat& sq);

    std::vector<datos_faciales> procesar(cv::Mat& full_img);

    /*TODO: entender porque a veces se generan rectangulos fuera de la imagen*/
/* Extrae un arreglo de rectángulos, correspondientes a las caras detectadas en una imágen.
   Recibe como argumento un tensor producido por una red convolucional entrenada para detectar caras.*/
    std::vector<std::pair<float, cv::Rect>> detectar_caras(cv::Mat &tensorDeteccion, unsigned int w, unsigned int h);

    cv::dnn::Net net;
    static float DETECTION_THRESHOLD;
};

void populate_and_cluster();
std::string clasificar_cara(dlib::matrix<float,0,1>& e);

extern detector_facial gDetectorFacial;
extern std::vector<dlib::matrix<float,0,1>> gEmbeddings;
extern std::vector<dlib::sample_pair> gEdges;
extern std::vector<unsigned long> gLabels;
extern std::vector<std::string> gLabelVector;

#endif