#include "dlib_cnn_face_detector.h"
#include <iostream>
#include <dlib/dnn.h>
#include <dlib/data_io.h>
#include <dlib/image_processing.h>
#include <dlib/gui_widgets.h>
#include <dlib/opencv/cv_image.h>


using namespace std;
using namespace dlib;

// ----------------------------------------------------------------------------------------

template <long num_filters, typename SUBNET> using con5d = con<num_filters,5,5,2,2,SUBNET>;
template <long num_filters, typename SUBNET> using con5  = con<num_filters,5,5,1,1,SUBNET>;

template <typename SUBNET> using downsampler  = relu<affine<con5d<32, relu<affine<con5d<32, relu<affine<con5d<16,SUBNET>>>>>>>>>;
template <typename SUBNET> using rcon5  = relu<affine<con5<45,SUBNET>>>;

using net_type = loss_mmod<con<1,9,9,1,1,rcon5<rcon5<rcon5<downsampler<input_rgb_image_pyramid<pyramid_down<6>>>>>>>>;

static unique_ptr<net_type> red;
static image_window win_h;

void init_dlib_face_detector()
{
    red = make_unique<net_type> ();
    deserialize("/home/mike/data/mmod_human_face_detector.dat") >> (*red);
}

std::vector<cv::Rect> get_bboxes(cv::Mat& m)
{
    static bool is_net_init{false};
    if(!is_net_init) {
        init_dlib_face_detector();
        is_net_init = true;
    }
    std::vector<cv::Rect> bboxes;
    //cv::Mat m_rgb;
    //cv::cvtColor(m,m_rgb,cv::COLOR_BGR2RGB);
    //auto img = dlib::cv_image<uchar>(m);
    matrix<rgb_pixel> img;
    dlib::assign_image(img, dlib::cv_image<bgr_pixel>(m));
    auto dets = red->operator()(img);
    win_h.clear_overlay();
    win_h.set_image(img);
    for(auto&& bb : dets)
    {
        win_h.add_overlay(bb);
    }

    return bboxes;
}