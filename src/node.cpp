#include "node.h"
#include "mj.h"

#include <opencv2/imgproc.hpp>

using namespace std;
using namespace cv;

int nodo::gid;

nodo::~nodo()
{

}

Point operator/(Point p, const int d)
{
  return Point(p.x/d, p.y/d);
}

Point transformar(const Point p)
{
  Point pp = mj::dxy + (p - mj::dxy - mj::despl)/mj::zoom; //mj::dxy es la mitad del tamaño del diagrama
  return pp;
}

Point transformacion_inversa(const Point pp) /*magia*/
{
  Point p = mj::zoom*(pp - mj::dxy) + mj::dxy + mj::despl;
  return p;
}

int transformar_escalar(int i) { return (i)/mj::zoom; }
int transformar_escalar_inverso(int i) { return i*mj::zoom; }


void nodo::dibujarse()
{
  Point tcentro = transformar(centro);
  int tradio = transformar_escalar(radio);
  cv::circle(mj::diagrama, tcentro, tradio, mcolor, -2, LINE_AA);
  cv::circle(mj::diagrama, tcentro, tradio, COLOR_NEGRO, 1, LINE_AA);

  if(b_seleccionado)
    cv::circle(mj::diagrama, tcentro, tradio, COLOR_SELECCION, 2, LINE_AA);

  if(b_highlighteado)
    cv::circle(mj::diagrama, tcentro, tradio, COLOR_HIGHLIGHT_, 1, LINE_AA);

  //somos observadores de los nodos que son nuestro input
  for(auto ptr : proveedores)
  {
    if(ptr != nullptr)
    {
      Point prov_centro = transformar(ptr->centro);
      cv::arrowedLine(mj::diagrama, prov_centro, tcentro, COLOR_BLANCO, 1, LINE_AA);
    }

  }
  cv::putText(mj::diagrama, sid, tcentro, FONT_HERSHEY_PLAIN, 1, COLOR_BLANCO, 1, LINE_AA);

  if(b_mostrar)
    mostrar();
}

void nodo::arrastrar(const cv::Point pt)
{
  centro += pt;
}

bool nodo::pertenece_a_area(const cv::Point pt) const //pt debe ser absoluto
{
  double x2 = double(pt.x - centro.x);
  double y2 = double(pt.y - centro.y);
  double r2 = double(radio);
  return (x2*x2 + y2*y2 < r2*r2); //esto se hizo porque estaban overfloweando los valores de int
}

void nodo::suscribir_a(nodo* src)
{
  std::cout << "suscribiendo " << sid << " a " << src->sid << ". " << src->sid << " -> " << sid << endl;
  proveedores.push_back(src);
  msrc = src->mmat;
}

