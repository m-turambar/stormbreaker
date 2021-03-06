#ifndef DLIB_MK
#define DLIB_MK

#include <opencv2/opencv.hpp>
#include <vector>
#include <array>
#include <dlib/dnn.h>
#include <dlib/gui_widgets.h>
#include <dlib/clustering.h>
#include <dlib/string.h>
#include <dlib/image_io.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/pixel.h>

dlib::matrix<float,0,1> alinear_y_reconocer_cara(cv::Mat& crop);

#endif