#include "dlib_mk.h"
#include "resnet.h"
#include <dlib/image_processing.h>
#include <dlib/image_processing/render_face_detections.h>
#include <dlib/gui_widgets.h>
#include <dlib/image_io.h>
#include <dlib/opencv/cv_image.h>
#include <dlib/opencv/to_open_cv.h>
#include <dlib/pixel.h>

#include <iostream>
#include <algorithm>
#include <vector>

using namespace dlib;
using namespace std;
using namespace cv;

// This is the tool that will predict face
// landmark positions given an image and a face bounding box
shape_predictor sp;
//image_window win, win_aligned;

int deserializar_landmarks()
{
    cout << "Deserializando modelo para face landmarks... ";
    deserialize("./data/shape_predictor_68_face_landmarks.dat") >> sp;

    return 0;
}


/*crop is expected to be a face only*/
matrix<float,0,1> alinear_y_reconocer_cara(cv::Mat& crop)
{
    static bool landmarks_deserializados{false};
    if(!landmarks_deserializados)
    {
        deserializar_landmarks();
        landmarks_deserializados = true;
    }
    Mat crop_rgb;
    cv::cvtColor(crop, crop_rgb, cv::COLOR_BGR2RGB);

    dlib::cv_image<rgb_pixel> img(crop_rgb);
    dlib::full_object_detection shape = 
        sp(img, dlib::rectangle(0,0,crop.cols, crop.rows));

    //win.clear_overlay();
    //win.set_image(img);
    //win.add_overlay(render_face_detections(shape));

    std::vector<matrix<rgb_pixel>> caras; //solo llevará un elemento
    matrix<rgb_pixel> cara;
    extract_image_chip(img, get_face_chip_details(shape,150,0.25), cara);
    caras.push_back(std::move(cara));
    //win_aligned.set_image(caras[0]);
    
    std::vector<matrix<float,0,1>> fds = get_face_features(caras);
    //std::copy(fds[0].begin(), fds[0].end(), embedding.begin());
    //return embedding;
    return fds[0];
}

