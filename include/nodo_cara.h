#ifndef MK_NODO_CARA
#define MK_NODO_CARA

#include "nodo.h"

#include <opencv2/core.hpp>

/*este es el nodo que actualmente usamos*/
struct nodo_cara : nodo
{
  nodo_cara(cv::Point c, int r);
  virtual void procesar() override;
};

#endif