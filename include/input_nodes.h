#ifndef MK_INPUT_NODES
#define MK_INPUT_NODES

#include "node.h"

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
  int i{0};
};

struct nodo_video : nodo
{
  nodo_video(cv::Point c, int r):
    nodo(c,r)
    {
      sid = "Video" + sid;
      cap.open("/dev/video0");
    }
  virtual ~nodo_video() {}
  cv::VideoCapture cap;
  virtual void procesar() override { cap >> mmat; };
};



#endif