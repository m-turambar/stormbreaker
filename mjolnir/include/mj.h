#ifndef MJ_H
#define MJ_H

#include <vector>
#include <memory>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>
#include "nodo.h"

struct mj
{
  mj();
  void cb_teclado(char k);
  void cb_mouse(int event, int x, int y, int flags);
  void run();

  static cv::Mat diagrama;
  const cv::Scalar bckgnd{cv::Scalar(153,125,130)};
  bool exit{false};
  const std::string wname = "sbreaker";
  static cv::Point dxy;
  static cv::Point despl;
  static int zoom;
  static std::vector<std::unique_ptr<nodo>> nodos;
};


#endif // MJ_H
