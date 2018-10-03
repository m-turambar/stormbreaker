#ifndef MK_DEEP_NODES
#define MK_DEEP_NODES

#include "node.h"
#include "caffe_mk.h"

/*actualmente inutilizado. modificar ctor y ::procesar*/
struct nodo_caffe : nodo
{
  nodo_caffe(cv::Point c, int r);
  nodo_caffe(cv::Point c, int r, std::string model_desc, std::string model_binary);
  
  virtual void procesar() override;
  virtual void actuar() override;
  std::unique_ptr<Classifier> clasificador;
};


#endif