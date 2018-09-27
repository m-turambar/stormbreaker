#include "node.h"
#include "mj.h"
#include "caffe_mk.h"


#include <opencv2/imgproc.hpp>

#include <utility>

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

void nodo::make_mmat_squared()
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
}

void nodo_dnn::procesar()
  {
    if( !msrc.empty() )
    {
      make_mmat_squared();
      cv::Mat fw = forward_prop();
      auto detecciones = extract_detections(fw);
      //cv::imshow("face",mmat);
      //waitKey(0);

      for(auto par_p_rect : detecciones) //por cada deteccion...
      {
        cv::rectangle(mmat, par_p_rect.second, cv::Scalar(20,20,230), 1, 8, 0);
        Mat cara = mmat(par_p_rect.second);
        dlib::matrix<float,0,1> embedding = alinear_y_reconocer_cara(cara);

        /**************/
        mis_caras.push_back(cara);
        /**************/
        embeddings.push_back(std::move(embedding));
      }
      
    }
  }

cv::Mat nodo_dnn::forward_prop()
{
    cv::Mat blob = cv::dnn::blobFromImage(mmat, 1.0, cv::Size(300,300), cv::Scalar(104,177,123) );
    net.setInput(blob);
    return net.forward();
}

std::vector<std::pair<float,cv::Rect>> nodo_dnn::extract_detections(cv::Mat& fw)
{
    /*Las detecciones empiezan por el índice 2, y las coordenadas son los 4
      subsecuentes valores. La matriz ya regresa sorteada, no sé porqué cada entrada
      tiene 7 valores extra de profundidad, pero no los estoy usando. Teóricamente
      si tomo el índice 9(?), me dé la probabilidad de la segunda cara por aparecer,
      y los siguientes 4 índices las regiones de ésta. */
    std::vector<std::pair<float,cv::Rect>> ret;
    for(int i=2; ; i+=7)
    {
      float proba = fw.at<cv::Vec3f>(0,0)[i];
      if(proba < DETECTION_THRESHOLD)
        break;
      //cout << "proba = " << proba << "%\n";
      const int w{mmat.cols}, h{mmat.rows};
      int xi = fw.at<cv::Vec3f>(0,0)[i+1]*mmat.rows;
      int yi = fw.at<cv::Vec3f>(0,0)[i+2]*mmat.cols;
      int xf = fw.at<cv::Vec3f>(0,0)[i+3]*mmat.rows;
      int yf = fw.at<cv::Vec3f>(0,0)[i+4]*mmat.cols;

      /*We correct against the bounding box going out of picture*/
      xi = xi < 0 ? 0 : xi;
      yi = yi < 0 ? 0 : yi;
      xf = xf > mmat.rows ? mmat.rows : xf;
      yf = yf > mmat.cols ? mmat.cols : yf;
      //cout << "{" << xi << ", " << xf << "} {" << yi << ", " << yf << "}\n";
      ret.push_back( std::make_pair(proba, cv::Rect(xi,yi,xf-xi,yf-yi)) );
    }
    return ret;
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

