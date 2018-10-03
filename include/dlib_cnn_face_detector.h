#ifndef MK_DLIB_CNN_FACE_DETECTOR
#define MK_DLIB_CNN_FACE_DETECTOR

#include <vector>
#include <opencv2/opencv.hpp>

std::vector<cv::Rect> get_bboxes(cv::Mat& m);

#endif