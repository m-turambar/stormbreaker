#ifndef MK_NODO_CARA
#define MK_NODO_CARA

#include "node.h"
#include "dlib_mk.h"
#include "clusters.h"

#include <opencv2/dnn.hpp>

#include <dlib/opencv/cv_image.h>
#include <dlib/pixel.h>

/*este es el nodo que actualmente usamos*/
struct nodo_cara : nodo
{
  nodo_cara(cv::Point c, int r);
  virtual void procesar() override;
};

#endif