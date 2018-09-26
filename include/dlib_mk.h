#ifndef DLIB_MK
#define DLIB_MK

#include <opencv2/opencv.hpp>
#include <vector>
#include <dlib/dnn.h>
#include <dlib/gui_widgets.h>
#include <dlib/clustering.h>
#include <dlib/string.h>
#include <dlib/image_io.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/pixel.h>

int init_dlib_mk();
void alinear_cara(cv::Mat& crop);

int deserialize_resnet();
std::vector<dlib::matrix<float,0,1>> get_face_features(std::vector<dlib::matrix<dlib::rgb_pixel> >cara);

#endif