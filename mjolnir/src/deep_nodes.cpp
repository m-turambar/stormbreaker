#include "deep_nodes.h"

using namespace std;
using namespace cv;

/*****************************************************/

nodo_caffe::nodo_caffe(cv::Point c, int r) : nodo(c, r)
{
    mcolor = cv::Scalar(12, 240, 20);
    sid = "Caffe" + sid;
}

nodo_caffe::nodo_caffe(cv::Point c, int r, std::string model_desc, std::string model_binary) : nodo(c, r)
{
    mcolor = cv::Scalar(12, 240, 20);
    sid = "Caffe" + sid;
    //clasificador = make_unique<Classifier>(model_desc, model_binary);
}

void nodo_caffe::procesar()
{
    if (!msrc.empty())
    {
        mmat = make_mat_squared(mmat);
        //get_bboxes(mmat);
        //predecir_etiquetas(msrc);
    }
}

void nodo_caffe::actuar()
{
    if (!msrc.empty())
    {
        mmat = msrc;
        //predecir_etiquetas(mmat);
    }
}
