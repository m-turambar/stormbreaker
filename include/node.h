#ifndef NODE_H
#define NODE_H

#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>

const cv::Scalar COLOR_NEGRO = {0,0,0};
const cv::Scalar COLOR_BLANCO = {255,255,255};
const cv::Scalar COLOR_SELECCION(50, 255, 25);
const cv::Scalar COLOR_HIGHLIGHT_(150, 215, 50);

cv::Point transformar(const cv::Point p);
cv::Point transformacion_inversa(const cv::Point pp);
int transformar_escalar(int i);

struct nodo
{
  nodo(cv::Point c, int r): centro(c), radio(r), id(gid++) {};
  void dibujarse();
  void arrastrar(const cv::Point pt);
  bool pertenece_a_area(const cv::Point pt) const;
  void seleccionar(bool b) { b_seleccionado = b; }
  void highlightear(bool b) { b_highlighteado = b; }
  void suscribir_a(nodo* src) { proveedores.push_back(src); }

  std::vector<nodo*> proveedores;
  cv::Point inicio,fin,centro;
  cv::Scalar mcolor{78,123,200};
  bool b_seleccionado{false}, b_highlighteado{false}, resizeando{false};
  int radio{50};
  int id;
  static int gid;
};

#endif // NODE_H
