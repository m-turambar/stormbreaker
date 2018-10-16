#ifndef MK_DETECTOR_FACIAL
#define MK_DETECTOR_FACIAL

#include <opencv2/core.hpp>
#include <opencv2/dnn.hpp>
#include <tuple>
#include <dlib/matrix.h>
#include "dataset.h"

using datos_faciales = std::tuple<float,cv::Rect,dlib::matrix<float,0,1> >;

struct detector_facial
{
    detector_facial();

    /*Genera un tensor donde viene información sobre los bounding boxes y la probabilidad de que estos sean el objeto que se busca*/
    cv::Mat forward_prop(cv::Mat& img);

    /* esta funcion con un poco de trabajo puede omitirse/simplificarse. Transforma del espacio de una imagen a otra un punto renderizable*/
    cv::Rect transformar_rect(cv::Rect r, cv::Mat& orig, cv::Mat& sq);

    std::vector<datos_faciales> procesar(cv::Mat& full_img);
    void populate_and_cluster(std::string image_dir);
    std::string clasificar_cara(dlib::matrix<float,0,1>& e);

    /*TODO: entender porque a veces se generan rectangulos fuera de la imagen*/
/* Extrae un arreglo de rectángulos, correspondientes a las caras detectadas en una imágen.
   Recibe como argumento un tensor producido por una red convolucional entrenada para detectar caras.*/
    std::vector<std::pair<float, cv::Rect>> detectar_caras(cv::Mat &tensorDeteccion, unsigned int w, unsigned int h);

    cv::dnn::Net net;
    float mDetectionThreshold{0.7};
    std::vector<dlib::matrix<float,0,1>> mEmbeddings;
    std::vector<dlib::sample_pair> mEdges;
    std::vector<unsigned long> mLabels;
    std::vector<std::string> mLabelVector{"Tintor","Rommel","Leo","Kuri","Gibran","Pegueros","Abraham",
                                        "Praneesh","Ian","Mike","Luis","Luis","Nallely","Unknown1", "Unknown2","Unknown3","Unknown4"};
    std::string mImageDir{"./sw_team"};
    mk::dataset mDataset{"./face_data/log.csv"};
};

#endif