#include "nodo_cara.h"
#include "input_nodes.h"
#include "detector_facial.h"
#include "dataset.h"
#include <array>
#include <tuple>

using namespace std;
using namespace cv;
using namespace dlib;

nodo_cara::nodo_cara(cv::Point c, int r) : nodo(c, r)
{
    mcolor = cv::Scalar(123, 123, 60);
    sid = "DNN" + sid;
}

void nodo_cara::procesar()
{
    //cv::createTrackbar("prob. threshold", sid, &kernel_sz, 23);
    if (!msrc.empty())
    {
        mmat = msrc;
        auto vec_tuplas = mDetectorFacial.procesar(mmat);

        for (auto tupla : vec_tuplas) //por cada deteccion...
        {
            float prob_det;
            cv::Rect rectangulo_bbox;
            dlib::matrix<float,0,1> embedding;
            std::tie(prob_det, rectangulo_bbox, embedding) = tupla;
            std::string label = mDetectorFacial.clasificar_cara(embedding);

            cv::rectangle(mmat, rectangulo_bbox, cv::Scalar(20, 20, 230), 1, 8, 0);
            cv::putText(mmat,label,rectangulo_bbox.tl(),cv::FONT_HERSHEY_PLAIN,1,cv::Scalar(255,255,255));
            
        }
        //esto es solo para acotar el render. No es necesario. Es para que el usuario vea
        int dx = (mmat.cols-mmat.rows)/2;
        mmat = mmat.colRange(dx, mmat.cols-dx);
        
    }
}

