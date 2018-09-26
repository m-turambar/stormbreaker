#include "dlib_mk.h"
#include <dlib/image_processing.h>
#include <dlib/image_processing/render_face_detections.h>
#include <dlib/gui_widgets.h>
#include <dlib/image_io.h>
#include <dlib/opencv/cv_image.h>
#include <dlib/opencv/to_open_cv.h>
#include <dlib/pixel.h>

#include <iostream>
#include <vector>

using namespace dlib;
using namespace std;
using namespace cv;

// This is the tool that will predict face
// landmark positions given an image and a face bounding box
shape_predictor sp;
image_window win, win_aligned;

int init_dlib_mk()
{
    cout << "Deserializando modelo para face landmarks... ";
    deserialize("/home/mike/data/shape_predictor_68_face_landmarks.dat") >> sp;
    cout << "hecho.\nDeserializando modelo para resnet... ";
    deserialize_resnet();
    cout << "hecho.\n";
    return 0;
}

/*crop is expected to be a face only*/
void alinear_cara(cv::Mat& crop)
{
    Mat crop_rgb;
    cv::cvtColor(crop, crop_rgb, cv::COLOR_BGR2RGB);

    dlib::cv_image<rgb_pixel> img(crop_rgb);
    dlib::full_object_detection shape = 
        sp(img, dlib::rectangle(0,0,crop.cols, crop.rows));

    //cout << "number of parts: "<< shape.num_parts() << endl;
    //cout << "pixel position of first part:  " << shape.part(0) << endl;
    //cout << "pixel position of second part: " << shape.part(1) << endl;

    win.clear_overlay();
    win.set_image(img);
    win.add_overlay(render_face_detections(shape));

    //dlib::array<array2d<rgb_pixel> >face_chips;
    //extract_image_chips(img, {get_face_chip_details(shape, 150, 0.25)}, face_chips);
    std::vector<matrix<rgb_pixel>> caras; //solo llevará un elemento
    matrix<rgb_pixel> cara;
    extract_image_chip(img, get_face_chip_details(shape,150,0.25), cara);
    caras.push_back(std::move(cara));
    win_aligned.set_image(caras[0]);
    
    std::vector<matrix<float,0,1>> fds = get_face_features(caras);
    matrix<float,0,1> descriptores = fds[0];
    for(auto f : descriptores)
        cout << f << " ";
    cout << '\n';
    //cout << descriptores.size() << endl;
}

