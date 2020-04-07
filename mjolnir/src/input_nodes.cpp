#include <opencv2/videoio/videoio_c.h>
#include <opencv2/videoio/registry.hpp>
#include "input_nodes.h"
#include "utils/sistema_archivos.h"

using namespace std;

nodo_video::nodo_video(cv::Point c, int r, bool async):
    nodo(c,r)
{
    sid = "Video" + sid;
    cap.open("/dev/video1");
/*
    cap.open("/dev/video1");
    //cap.open("/dev/video0", cv::CAP_DSHOW);
    //cap.open("/dev/video0", cv::CAP_FFMPEG);

    for(auto prop : get_camera_properties())
        cout << prop << '\n';

    //cout << cv::getBuildInformation();
    //cap.set(cv::CAP_PROP_FRAME_WIDTH, 1280);
    //cap.set(cv::CAP_PROP_FRAME_HEIGHT, 720);

    for(auto prop : get_camera_properties())
        cout << prop << '\n';

    auto backends = cv::videoio_registry::getCameraBackends	();
    cout << "backends:\n";
    for(auto b : backends)
        cout << "\t" << b << "\n";
        */

    hilo_camara = std::thread(&nodo_video::capture_loop, this);
}

vector<double> nodo_video::get_camera_properties() {
    vector<double> props;
    props.push_back(cap.get(CV_CAP_PROP_FRAME_WIDTH));
    props.push_back(cap.get(CV_CAP_PROP_FRAME_HEIGHT));
    props.push_back(cap.get(CV_CAP_PROP_FPS));
    props.push_back(cap.get(CV_CAP_PROP_MODE));
    props.push_back(cap.get(CV_CAP_PROP_AUTOFOCUS));
    props.push_back(cap.get(CV_CAP_PROP_BUFFERSIZE));
    return props;
}

void nodo_video::procesar()
{
    auto tiempo1 = std::chrono::system_clock::now();

    //get_video_frame();
    cap >> mmat;
    auto tiempo2 = std::chrono::system_clock::now();
    auto dur = std::chrono::duration_cast<chrono::microseconds>(tiempo2 - tiempo1).count();
    cout << "nodo_video::procesar(): "<< dur << "us\n";
}

void nodo_video::capture_loop() {
    cv::Mat frame;
    cout << "hello?" << b_mostrar << endl;
    while(b_mostrar)
    {
        cap >> frame;
        cout << frame.rows << "x" << frame.cols << endl;
        set_video_frame(frame);
    }
}

cv::Mat nodo_video::get_video_frame() {
    lock_guard<mutex> lock(mtx_v);
    inter.copyTo(mmat);
    return mmat;
}

void nodo_video::set_video_frame(cv::Mat& f) {
    lock_guard<mutex> lock(mtx_v);
    f.copyTo(inter);
};

nodo_im::nodo_im(cv::Point c, int r):
    nodo(c,r)
{
    int retorno = system("python3 ./scripts/file_dialog.py > tmp_fname.txt");
    if(retorno==0) {
    std::ifstream ifs("tmp_fname.txt", std::ios::in);
    std::string fname;
    std::getline(ifs,fname);
    mmat = cv::imread(fname);
    sid = fname;
    }
    else
    sid = "Imagen" + sid;
}

/************************************************/

nodo_iter_dir::nodo_iter_dir(cv::Point c, int r):
    nodo(c,r)
{
    int retorno = system("python3 ./scripts/directory_dialog.py > tmp_fname.txt");
    if(retorno==0) {
    std::ifstream ifs("tmp_fname.txt", std::ios::in);
    std::getline(ifs,dir_name);
    files = get_file_names(dir_name);
    if(files.size() > 0)
        mmat = cv::imread(files[i]);
    sid = dir_name;
    }
    else
    sid = "Empty_dir";
}

nodo_iter_dir::nodo_iter_dir(cv::Point c, int r, std::string path):
    nodo(c,r)
{
    files = get_file_names(path);
    if(files.size() > 0)
    {
        mmat = cv::imread(files[i]);
        sid = dir_name;
    }
    else
        sid = "Empty_dir";
}

void nodo_iter_dir::actuar() {
    b_mostrar=true; 
    ++i; 
    if(i > files.size())
        i=0;
    mmat = cv::imread(files[i]);
    for(auto n : suscriptores)
        n->msrc = mmat;
  }