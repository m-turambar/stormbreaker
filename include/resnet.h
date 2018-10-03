#ifndef MK_RESNET
#define MK_RESNET

#include <vector>
#include <dlib/matrix.h>

std::vector<dlib::matrix<float,0,1>> get_face_features(std::vector<dlib::matrix<dlib::rgb_pixel> >cara);

#endif