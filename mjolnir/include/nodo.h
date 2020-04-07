#ifndef NODO_H
#define NODO_H

#include <iostream>
#include <fstream>
#include <vector>
#include <opencv2/opencv.hpp>

const cv::Scalar COLOR_NEGRO = {0,0,0};
const cv::Scalar COLOR_BLANCO = {255,255,255};
const cv::Scalar COLOR_SELECCION(50, 255, 25);
const cv::Scalar COLOR_HIGHLIGHT_(150, 215, 50);

cv::Point transformar(const cv::Point p);
cv::Point transformacion_inversa(const cv::Point pp);
int transformar_escalar(int i);

std::vector<std::string> get_file_names(std::string dir_name);

using std::cout;
using std::endl;

struct nodo
{
  nodo(cv::Point c, int r): centro(c), radio(r), id(gid++), sid(std::to_string(id)) {};
  void info();
  void dibujarse();
  void arrastrar(const cv::Point pt);
  bool pertenece_a_area(const cv::Point pt) const;
  void seleccionar(bool b) { b_seleccionado = b; }
  void highlightear(bool b) { b_highlighteado = b; }
  virtual void suscribir_a(nodo* src);
  void desuscribir_de(nodo* src);
  /*la diferencia entre estos dos métodos es ambigüa*/
  virtual void procesar()=0;
  virtual void actuar() { b_mostrar = true; }

  virtual void mostrar() { if(!mmat.empty()) {cv::namedWindow(sid/*, cv::WINDOW_GUI_EXPANDED*/); cv::imshow(sid,mmat); } }
  virtual void desactivar() { b_mostrar = false; /*cv::destroyWindow(sid);*/ }
  virtual ~nodo();

  std::vector<nodo*> proveedores;
  std::vector<nodo*> suscriptores;

  cv::Point centro;
  cv::Scalar mcolor{78,123,200};
  bool b_seleccionado{false}, b_highlighteado{false}, resizeando{false}, b_mostrar{true};
  int radio{50};
  int id;
  std::string sid;
  static int gid; //global
  cv::Mat mmat;
  cv::Mat msrc; //ocupada solo por nodos que tengan input
};

cv::Mat make_mat_squared(cv::Mat& img);

#endif // NODO_H
