#ifndef MK_INPUT_NODES
#define MK_INPUT_NODES

#include <thread>
#include "nodo.h"

struct nodo_im : nodo
{
  nodo_im(cv::Point c, int r);
  void procesar() {};
};


struct nodo_iter_dir : nodo
{
    nodo_iter_dir(cv::Point c, int r);
    nodo_iter_dir(cv::Point c, int r, std::string path);

    void procesar() override { };

    virtual void actuar() override;

  std::vector<std::string> files;
  std::string dir_name;
  unsigned int i{0};
};

struct nodo_video : nodo
{
    nodo_video(cv::Point c, int r, bool async=true);
    virtual ~nodo_video() { hilo_camara.join(); }
    virtual void procesar() override;
    std::vector<double> get_camera_properties();

    /* Aqui cambiamos a un modo asincrono de captura, para no tener que esperar a la camara cuando nos toque leer su frame. */
    void capture_loop();
    cv::Mat get_video_frame();
    void set_video_frame(cv::Mat& f);

    cv::VideoCapture cap;
    std::mutex mtx_v;
    std::thread hilo_camara;
    cv::Mat inter; //ugh
};



#endif