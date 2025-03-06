#include <opencv2/videoio/videoio_c.h>
#include <opencv2/videoio/registry.hpp>
#include "input_nodes.h"
#include "utils/sistema_archivos.h"
#include "Screenshot.h"

using namespace std;

nodo_video::nodo_video(cv::Point c, int r):
    nodo(c,r)
{
    sid = "Video" + sid;
    cap.open("/dev/video0");
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

    cap >> mmat;
    cout << mmat.rows << "x" << mmat.cols << endl;
    auto tiempo2 = std::chrono::system_clock::now();
    auto dur = std::chrono::duration_cast<chrono::milliseconds>(tiempo2 - tiempo1).count();
    cout << "nodo_video::procesar(): "<< dur << "ms\n";
}

nodo_im_selector::nodo_im_selector(cv::Point c, int r):
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

nodo_ss::nodo_ss(cv::Point c, int r) : nodo(c, r) {
    b_mostrar=false;
}

void nodo_ss::procesar() {
  int Width = 0;
  int Height = 0;
  int Bpp = 0;
  std::vector<std::uint8_t> Pixels;

  ImageFromDisplay(Pixels, Width, Height, Bpp);

  if (Width && Height)
  {
    cv::Mat img = cv::Mat(Height, Width, Bpp > 24 ? CV_8UC4 : CV_8UC3, &Pixels[0]); //Mat(Size(Height, Width), Bpp > 24 ? CV_8UC4 : CV_8UC3, &Pixels[0]);
    //cv::cvtColor(img, img, cv::COLOR_BGR2GRAY);
    img.copyTo(mmat);
  }
}

nodo_im::nodo_im(cv::Point c, int r, const std::string path) : nodo(c, r)
{
    b_mostrar=false;
    mmat = cv::imread(path, cv::IMREAD_COLOR);
    cout << "nodo_im::nodo_im -> " << path << " dims==" << mmat.dims << '\n';
    if(mmat.dims == 0)
        throw std::runtime_error("empty image read");
}

nodo_subs::nodo_subs(cv::Point c, int r, const cv::Rect rec_arg)
    : nodo(c, r), rec(rec_arg)
    {
        sid = "nodo_subs";
    }

void nodo_subs::procesar() {
    if(!msrc.empty()) {
        mmat = msrc(rec);
        //cout << mmat.rows << "x" << mmat.cols << endl;
    }

}
