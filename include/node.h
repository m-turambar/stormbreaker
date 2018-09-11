#ifndef NODE_H
#define NODE_H

#include <iostream>
#include <fstream>
#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/dnn.hpp>

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
  void info();
  void dibujarse();
  void arrastrar(const cv::Point pt);
  bool pertenece_a_area(const cv::Point pt) const;
  void seleccionar(bool b) { b_seleccionado = b; }
  void highlightear(bool b) { b_highlighteado = b; }
  virtual void suscribir_a(nodo* src);
  void desuscribir_de(nodo* src);
  virtual void procesar()=0;
  virtual void mostrar() { if(!mmat.empty()) {cv::namedWindow(sid, cv::WINDOW_GUI_EXPANDED); cv::imshow(sid,mmat); } }
  virtual ~nodo();

  std::vector<nodo*> proveedores;
  std::vector<nodo*> suscriptores;

  cv::Point centro;
  cv::Scalar mcolor{78,123,200};
  bool b_seleccionado{false}, b_highlighteado{false}, resizeando{false}, b_mostrar{false};
  int radio{50};
  int id;
  std::string sid;
  static int gid; //global
  cv::Mat mmat;
  cv::Mat msrc; //ocupada solo por nodos que tengan input
};

struct nodo_im : nodo
{
  nodo_im(cv::Point c, int r):
    nodo(c,r)
    {
      int retorno = system("python scripts\\file_dialog.py > tmp_fname.txt");
      if(retorno==0) {
        std::ifstream ifs("tmp_fname.txt", std::ios::in);
        std::string fname;
        std::getline(ifs,fname);
        mmat = cv::imread(fname);
        sid = fname;
      }
      else
        sid = "Imagen" + sid;
    }
  void procesar() {};
};

struct nodo_video : nodo
{
  nodo_video(cv::Point c, int r):
    nodo(c,r)
    {
      sid = "Video" + sid;
      cap.open("/dev/video0");
    }
  virtual ~nodo_video(){}
  cv::VideoCapture cap;
  virtual void procesar() { cap >> mmat; };
};

struct nodo_gris : nodo
{
  nodo_gris(cv::Point c, int r):
    nodo(c,r)
    {
      mcolor = cv::Scalar(100,100,100);
      sid = "Gris" + sid;
    }
  virtual ~nodo_gris(){}
  virtual void procesar() { if(!msrc.empty()) cv::cvtColor(msrc, mmat, cv::COLOR_BGR2GRAY); };
};

struct nodo_blur : nodo
{
  nodo_blur(cv::Point c, int r):
    nodo(c,r)
    {
      cv::createTrackbar("kernel_sz", sid, &kernel_sz, 23);
      mcolor = cv::Scalar(100,50,190);
      sid = "blur" + sid;
    }
  virtual void procesar() {
    if(!msrc.empty())
    {
      kernel_sz = kernel_sz < 1 ? 1 : kernel_sz;
      cv::blur(msrc, mmat, cv::Size(kernel_sz, kernel_sz));
    }

    cv::createTrackbar("kernel_sz", sid, &kernel_sz, 13);
    };
  int kernel_sz{5};
};

struct nodo_canny : nodo
{
  nodo_canny(cv::Point c, int r):
    nodo(c,r)
    {
      mcolor = cv::Scalar(230,230,230);
      sid = "Canny" + sid;
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

struct nodo_laplace : nodo
{
  nodo_laplace(cv::Point c, int r):
    nodo(c,r)
    {
      mcolor = cv::Scalar(230,230,230);
      sid = "Laplace" + sid;
    }
  virtual void procesar()
  {
    if(!msrc.empty())
    {
      int ksize = sigma | 1;
      cv::GaussianBlur(msrc, smooth, cv::Size(ksize, ksize), sigma, sigma);
      cv::Laplacian(smooth, laplaciana, CV_16S, ksize, scale, 0);
      convertScaleAbs(laplaciana, mmat, (sigma+1)*0.25);
    }
    cv::createTrackbar("sigma", sid, &sigma, 13);
    cv::createTrackbar("scale", sid, &scale, 5);
  };
  int sigma{1};
  int scale{1};
  cv::Mat laplaciana, smooth;
};

struct nodo_hsv : nodo
{
  nodo_hsv(cv::Point c, int r):
    nodo(c,r)
    {
      mcolor = cv::Scalar(23,80,230);
      sid = "HSV" + sid;
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

struct nodo_erosion_dilacion : nodo
{
  nodo_erosion_dilacion(cv::Point c, int r):
    nodo(c,r)
    {
      mcolor = cv::Scalar(23,0,30);
      sid = "erosion_dilacion" + sid;
    }
  virtual void procesar()
  {
    if(!msrc.empty())
    {
      kernel_erosion_sz = kernel_erosion_sz < 1 ? 1 : kernel_erosion_sz;
      kernel_dilacion_sz = kernel_dilacion_sz < 1 ? 1 : kernel_dilacion_sz;
      auto kernel_erosion = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(kernel_erosion_sz, kernel_erosion_sz));
      auto kernel_dilacion = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(kernel_dilacion_sz, kernel_dilacion_sz));
      cv::erode(msrc, mmat, kernel_erosion, cv::Point(-1,-1), 2);
      cv::dilate(mmat, mmat, kernel_dilacion, cv::Point(-1,-1), 2);
    }
    cv::createTrackbar("kernel_erosion", sid, &kernel_erosion_sz, 19);
    cv::createTrackbar("kernel_dilacion", sid, &kernel_dilacion_sz, 19);
  }
  int kernel_erosion_sz{11};
  int kernel_dilacion_sz{11};
};

struct nodo_mascara : nodo
{
  nodo_mascara(cv::Point c, int r):
      nodo(c,r),
      hl(0), sl(95), vl(96),
      hh(20), sh(200), vh(175)
    {
      mcolor = cv::Scalar(0,255,200);
      sid = "mascara" + sid;
    }
  virtual void procesar()
  {
    if(!msrc.empty())
    {
      cv::Scalar lower = cv::Scalar(hl, sl, vl);
      cv::Scalar upper = cv::Scalar(hh, sh, vh);
      cv::inRange(msrc, lower, upper, mmat);
      cv::createTrackbar("Hue L", sid, &hl, 255);
      cv::createTrackbar("Hue H", sid, &hh, 255);
      cv::createTrackbar("Sat L", sid, &sl, 255);
      cv::createTrackbar("Sat H", sid, &sh, 255);
      cv::createTrackbar("Val L", sid, &vl, 255);
      cv::createTrackbar("Val H", sid, &vh, 255);
    }
  }
  int hl, sl, vl, hh, sh, vh;
  //Piel: {0, 48, 80} {20, 255, 255}
};

struct nodo_bitwise_and : nodo
{
  nodo_bitwise_and(cv::Point c, int r):
    nodo(c,r)
    {
      mcolor = cv::Scalar(255,0,0);
      sid = "bitwise_and" + sid;
    }

  void suscribir_a(nodo* src) override
  {
    if(src!=nullptr)
    {
      std::cout << "suscribiendo " << sid << " a " << src->sid << ". " << src->sid << " -> " << sid << std::endl;
      proveedores.push_back(src);
      src->suscriptores.push_back(this);
      if(src->mmat.type()==0)
        mmascara = src->mmat;
      else
        msrc = src->mmat;
    }
  }

  virtual void procesar()
  {
    if(proveedores.size()>=2)
    {
      mmat = cv::Scalar(0,0,0);
      cv::bitwise_and(msrc, msrc, mmat, mmascara);
      //cv::bitwise_and(proveedores[0]->mmat, proveedores[1]->mmat, mmat);
    }
  }
  cv::Mat mmascara;
};

struct nodo_filtro_bilateral : nodo
{
  nodo_filtro_bilateral(cv::Point c, int r):
    nodo(c,r)
    {
      mcolor = cv::Scalar(123,123,60);
      sid = "Filtro_bilateral" + sid;
    }
  virtual void procesar()
  {
    if( !msrc.empty() )
    {
      d = d < 1 ? 1 : d;
      cv::bilateralFilter(msrc, mmat, d, sigmaColor, sigmaSpace) ;
      cv::createTrackbar("d", sid, &d, 10);
      cv::createTrackbar("Sigma Color", sid, &sigmaColor, 500);
      cv::createTrackbar("Sigma Space", sid, &sigmaSpace, 500);
    }
  }
  int d{5};
  int sigmaColor{150}, sigmaSpace{150};
};

struct nodo_hough_circulo : nodo
{
  nodo_hough_circulo(cv::Point c, int r):
    nodo(c,r)
    {
      mcolor = cv::Scalar(127,200,40);
      sid = "Hough circulo" + sid;
    }
  virtual void procesar()
  {
    if( !msrc.empty() )
    {
      mmat = msrc.clone();
      cv::HoughCircles(msrc, circulos, cv::HOUGH_GRADIENT, 2, msrc.rows/4, 200, 100 );
      for( size_t i = 0; i < circulos.size(); i++ )
      {
        cv::Point center(cvRound(circulos[i][0]), cvRound(circulos[i][1]));
        int radius = cvRound(circulos[i][2]);
        cv::circle( mmat, center, radius, cv::Scalar(0,0,255), 3, 8, 0 );
      }
    }
  }
  std::vector<cv::Vec3f> circulos;
};

struct nodo_dnn : nodo
{
  nodo_dnn(cv::Point c, int r):
    nodo(c,r)
    {
      mcolor = cv::Scalar(123,123,60);
      sid = "DNN" + sid;
      red = cv::dnn::readNetFromTensorflow("something.pb");
    }
  virtual void procesar()
  {
    if( !msrc.empty() )
    {
    }
  }
  cv::dnn::Net red;
};
#endif // NODE_H
