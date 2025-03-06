#ifndef MK_INPUT_NODES
#define MK_INPUT_NODES

#include <thread>
#include "nodo.h"

struct nodo_im_selector : nodo
{
  nodo_im_selector(cv::Point c, int r);
  void procesar() override {};
};

struct nodo_im : nodo
{
    nodo_im(cv::Point c, int r, const std::string path);
    void procesar() override {};
};

struct nodo_subs : nodo
{
    nodo_subs(cv::Point c, int r, const cv::Rect);
    void procesar() override;

private:
    const cv::Rect rec;
};


struct nodo_iter_dir : nodo
{
    nodo_iter_dir(cv::Point c, int r);
    nodo_iter_dir(cv::Point c, int r, std::string path);

    void procesar() override { };

    void actuar() override;

  std::vector<std::string> files;
  std::string dir_name;
  unsigned int i{0};
};

struct nodo_video : nodo
{
    nodo_video(cv::Point c, int r);
    void procesar() override;
    std::vector<double> get_camera_properties();

    cv::VideoCapture cap;
};

struct nodo_ss : nodo
{
    nodo_ss(cv::Point c, int r);
    void procesar() override;
};



#endif