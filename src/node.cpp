#include "node.h"
#include "mj.h"
#include "caffe_mk.h"
#include "dlib_mk.h"

#include <opencv2/imgproc.hpp>

using namespace std;
using namespace cv;

int nodo::gid;

nodo::~nodo()
{
  cout << sid << " dtor called\n";

  for(nodo* n : proveedores)
    desuscribir_de(n);

  for(nodo* n : suscriptores)
    n->desuscribir_de(this); //oye, me van a destruir, desuscribete de mi rapido


}

void nodo::suscribir_a(nodo* src)
{
  if(src!=nullptr)
  {
    std::cout << "suscribiendo " << sid << " a " << src->sid << ". " << src->sid << " -> " << sid << endl;
    proveedores.push_back(src);
    src->suscriptores.push_back(this);
    msrc = src->mmat;
  }
}

void nodo::desuscribir_de(nodo* src)
{
  if(src != nullptr)
  {
    std::cout << "desuscribiendo a " << sid << " de " << src->sid << endl;
    proveedores.erase( std::find(proveedores.begin(), proveedores.end(), src) );
    src->suscriptores.erase( std::find(src->suscriptores.begin(), src->suscriptores.end(), this) );
  }
}

Point operator/(Point p, const int d)
{
  return Point(p.x/d, p.y/d);
}

Point transformar(const Point p)
{
  Point pp = mj::dxy + (p - mj::dxy - mj::despl)/mj::zoom; //mj::dxy es la mitad del tama�o del diagrama
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
  for(nodo* prov : proveedores)
  {
    if(prov != nullptr)
    {
      Point prov_centro = transformar(prov->centro);
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

void nodo::info()
{
  cout << "<" << sid << ">\n";
  cout << "Mat: " << mmat.type() << '\n';
  cout << "proveedores:\n";
  for(nodo* pr : proveedores)
    cout << "\t" << pr->sid << '\n';
  cout << "suscriptores:\n";
  for(nodo* cl : suscriptores)
    cout << "\t" << cl->sid << '\n';
}

void nodo_dnn::procesar()
  {
    if( !msrc.empty() )
    {
      int dx = (msrc.cols - msrc.rows)/2;
      if(dx < 0)
      {
        dx = (msrc.rows - msrc.cols)/2;
        mmat = msrc.rowRange(dx, msrc.rows - dx);
      }
      else
      {
        mmat = msrc.colRange(dx, msrc.cols - dx);
      }
      cv::resize(mmat, mmat, cv::Size(300,300) );
      
      cv::Mat blob = cv::dnn::blobFromImage(mmat, 1.0, cv::Size(300,300), cv::Scalar(104,177,123) );
      net.setInput(blob);
      cv::Mat fw = net.forward();
      cv::Size sz = fw.size();
      /*Las detecciones empiezan por el índice 2, y las coordenadas son los 4
        subsecuentes valores. La matriz ya regresa sorteada, no sé porqué cada entrada
        tiene 7 valores extra de profundidad, pero no los estoy usando. Teóricamente
        si tomo el índice 9(?), me dé la probabilidad de la segunda cara por aparecer,
        y los siguientes 4 índices las regiones de ésta. */
      float proba = fw.at<cv::Vec3f>(0,0)[2];
      cout << "proba = " << proba << "%\n";
      const int w{mmat.cols}, h{mmat.rows};
      int xi = fw.at<cv::Vec3f>(0,0)[3]*mmat.rows;
      int yi = fw.at<cv::Vec3f>(0,0)[4]*mmat.cols;
      int xf = fw.at<cv::Vec3f>(0,0)[5]*mmat.rows;
      int yf = fw.at<cv::Vec3f>(0,0)[6]*mmat.cols;
      //cout << "{" << xi << ", " << xf << "} {" << yi << ", " << yf << "}\n";
      cout << fw.at<cv::Vec3f>(0,0)[9] << endl;
      //cv::Point pi(xi,yi), pf(xf,yf);
      xi = xi < 0 ? 0 : xi;
      yi = yi < 0 ? 0 : yi;
      xf = xf > mmat.rows ? mmat.rows : xf;
      yf = yf > mmat.cols ? mmat.cols : yf;

      cv::Rect roi_f(cv::Point(xi,yi), cv::Point(xf,yf));
      cv::rectangle(mmat, roi_f, cv::Scalar(20,20,230), 1, 8, 0);
      Mat cara = mmat(roi_f);
      alinear_cara(cara);
    }
  }

void nodo_caffe::procesar()
  {
    if( !msrc.empty() )
    {
      predecir_etiquetas(msrc);
    }
    
  }

void nodo_caffe::actuar()
{
  //b_mostrar = false;
  if( !msrc.empty() )
  {
    mmat = msrc;
    predecir_etiquetas(mmat);
  }
}

