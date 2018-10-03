#ifndef MK_NODO_CARA
#define MK_NODO_CARA

#include "node.h"
#include "dlib_mk.h"
#include "clusters.h"

#include <opencv2/dnn.hpp>

#include <dlib/opencv/cv_image.h>
#include <dlib/pixel.h>

/*este es el nodo que actualmente usamos*/
struct nodo_dnn : nodo
{
  nodo_dnn(cv::Point c, int r);
  virtual void procesar() override;
  cv::Mat forward_prop();
  std::vector<std::pair<float,cv::Rect>> extract_detections(cv::Mat& fw);

  const float DETECTION_THRESHOLD{0.90};
  std::vector<dlib::matrix<float,0,1>> embeddings;
  std::vector<cv::Mat> mis_caras;
  cv::dnn::Net net;
};

#endif