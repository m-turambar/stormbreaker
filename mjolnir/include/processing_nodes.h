#ifndef MK_PROCESSING_NODES
#define MK_PROCESSING_NODES

#include "nodo.h"
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/imgproc.hpp>

struct nodo_gris : nodo
{
  nodo_gris(cv::Point c, int r);
  virtual ~nodo_gris(){}
  virtual void procesar();
};

struct nodo_blur : nodo
{
  nodo_blur(cv::Point c, int r);
  virtual void procesar();
  int kernel_sz{5};
};

struct nodo_canny : nodo
{
  nodo_canny(cv::Point c, int r);
  virtual void procesar();
  int umbral_borde{1};
};

struct nodo_laplace : nodo
{
  nodo_laplace(cv::Point c, int r);
  virtual void procesar();
  int sigma{1};
  int scale{1};
  cv::Mat laplaciana, smooth;
};

struct nodo_hsv : nodo
{
  nodo_hsv(cv::Point c, int r);
  virtual void procesar();
};

struct nodo_erosion_dilacion : nodo
{
  nodo_erosion_dilacion(cv::Point c, int r);
  virtual void procesar();
  int kernel_erosion_sz{11};
  int kernel_dilacion_sz{11};
};

struct nodo_mascara : nodo
{
  nodo_mascara(cv::Point c, int r);
  virtual void procesar();
  int hl, sl, vl, hh, sh, vh;
  //Piel: {0, 48, 80} {20, 255, 255}
};

struct nodo_bitwise_and : nodo
{
  nodo_bitwise_and(cv::Point c, int r);
  void suscribir_a(nodo* src);
  virtual void procesar();
  cv::Mat mmascara;
};

struct nodo_filtro_bilateral : nodo
{
  nodo_filtro_bilateral(cv::Point c, int r);
  virtual void procesar();
  int d{5};
  int sigmaColor{150}, sigmaSpace{150};
};

struct nodo_hough_circulo : nodo
{
  nodo_hough_circulo(cv::Point c, int r);
  virtual void procesar() override;
  std::vector<cv::Vec3f> circulos;
};

#endif