#ifndef NODE_H
#define NODE_H

#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/imgproc.hpp>

const cv::Scalar COLOR_NEGRO = {0,0,0};
const cv::Scalar COLOR_BLANCO = {255,255,255};
const cv::Scalar COLOR_SELECCION(50, 255, 25);
const cv::Scalar COLOR_HIGHLIGHT_(150, 215, 50);

cv::Point transformar(const cv::Point p);
cv::Point transformacion_inversa(const cv::Point pp);
int transformar_escalar(int i);

struct nodo
{
  nodo(cv::Point c, int r): centro(c), radio(r), id(gid++), sid(std::to_string(id)) {};
  void dibujarse();
  void arrastrar(const cv::Point pt);
  bool pertenece_a_area(const cv::Point pt) const;
  void seleccionar(bool b) { b_seleccionado = b; }
  void highlightear(bool b) { b_highlighteado = b; }
  void suscribir_a(nodo* src);
  virtual void procesar()=0;
  virtual void mostrar() { if(!mmat.empty()) cv::imshow(sid,mmat);}
  virtual ~nodo();

  std::vector<nodo*> proveedores;
  cv::Point inicio,fin,centro;
  cv::Scalar mcolor{78,123,200};
  bool b_seleccionado{false}, b_highlighteado{false}, resizeando{false}, b_mostrar{false};
  int radio{50};
  int id;
  const std::string sid;
  static int gid; //global
  cv::Mat mmat;
  cv::Mat msrc; //ocupada solo por nodos que tengan input
};

struct nodo_video : nodo
{
  nodo_video(cv::Point c, int r):
    nodo(c,r)
    {

    }
  virtual ~nodo_video(){}
  cv::VideoCapture cap{0};
  virtual void procesar() { cap >> mmat; };
};

struct nodo_gris : nodo
{
  nodo_gris(cv::Point c, int r):
    nodo(c,r)
    {
      mcolor = cv::Scalar(100,100,100);
    }
  virtual ~nodo_gris(){}
  virtual void procesar() { if(!msrc.empty()) cv::cvtColor(msrc, mmat, cv::COLOR_BGR2GRAY); };
};

struct nodo_blur : nodo
{
  nodo_blur(cv::Point c, int r):
    nodo(c,r)
    {
      cv::createTrackbar("kernel_sz", sid, &kernel_sz, 13);
      mcolor = cv::Scalar(100,50,190);
    }
  virtual void procesar() {
    if(!msrc.empty())
    {
      kernel_sz = kernel_sz < 1 ? 1 : kernel_sz;
      cv::blur(msrc, mmat, cv::Size(kernel_sz, kernel_sz));
    }

    cv::createTrackbar("kernel_sz", sid, &kernel_sz, 13);
    };
  int kernel_sz{5}; //modificar desde trackbar
};

struct nodo_canny : nodo
{
  nodo_canny(cv::Point c, int r):
    nodo(c,r)
    {
      mcolor = cv::Scalar(230,230,230);
    }
  virtual void procesar()
  {
    if(!msrc.empty())
    {
      cv::Canny(msrc, mmat, umbral_borde, umbral_borde*3);
    }
    cv::createTrackbar("umbral", sid, &umbral_borde, 13);
  };
  int umbral_borde{1};
};

struct nodo_hsv : nodo
{
  nodo_hsv(cv::Point c, int r):
    nodo(c,r)
    {
      mcolor = cv::Scalar(23,80,230);
    }
  virtual void procesar()
  {
    if(!msrc.empty())
    {
      cv::cvtColor(msrc, mmat, cv::COLOR_BGR2HSV);
    }
    //cv::createTrackbar("umbral", sid, &umbral_borde, 13);
  };
};

#endif // NODE_H
