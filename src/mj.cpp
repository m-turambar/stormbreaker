#include <memory>
#include "mj.h"
#include "node.h"
#include "rel.h"

using namespace std;
using namespace cv;

VideoCapture cap(0);
Mat vidframe;

cv::Mat mj::diagrama;
cv::Point mj::despl;
cv::Point mj::dxy;
int mj::zoom{8};

cv::Point puntoClickMouseDerecho{0,0}; //panning
cv::Point puntoInicioDesplazamiento{0,0}; //panning

bool b_dibujando_flecha{false}; //flechas temporales
bool b_dibujando_objeto{false}; //objeto temporal
cv::Point pt_inicio{0,0}; //flechas temporales
cv::Point pt_termino{0,0}; //flechas temporales

cv::Point puntoActualMouse{0,0}; //se actualiza en cada evento del mouse

nodo* ptr_seleccionado{nullptr};
nodo* ptr_highlight{nullptr};

bool b_drag{false};

std::vector<std::unique_ptr<nodo>> mj::nodos;

nodo* ptr_ultimo{nullptr};

template<typename Tipo_Nodo>
nodo* crear_nodo();

nodo* determinar_propiedades_ubicacion(cv::Point p);
nodo* encontrar_ptr_area(cv::Point& p);
void crear_relacion(nodo* src, nodo* dst);

void cb_mouse_static(int event, int x, int y, int flags, void* data)
{
  mj* ptr = static_cast<mj*>(data);
  ptr->cb_mouse(event,x,y,flags);
}

mj::mj()
{
  namedWindow(wname);
  diagrama = Mat(800,1200,CV_8UC3, bckgnd);
  setMouseCallback(wname,cb_mouse_static,this);
}

void mj::run()
{
  while(true)
  {
    char c = waitKey(33);
    cb_teclado(c);
    if(exit)
      break;

    if(b_dibujando_objeto) //dibujamos un objeto temporal
    {
      Point hipot = pt_termino - pt_inicio;
      int radio = std::hypot(hipot.x, hipot.y);
      ptr_ultimo->radio = radio;
    }

    diagrama = bckgnd;
    for(auto& uptr : nodos)
    {
      uptr->procesar();
      uptr->dibujarse();
    }

    if(b_dibujando_flecha) //dibujamos una flecha temporal
      arrowedLine(diagrama, transformar(pt_inicio), transformar(pt_termino), COLOR_BLANCO, 2, LINE_AA);



    imshow(wname,diagrama);
  }

}

auto vid = []() -> cv::Mat {
      cap >> vidframe;
      return vidframe;
    };
auto l3 = []() -> float { return 3.14; };
auto l4 = []() -> float { return 42; };

void mj::cb_teclado(char k)
{
  switch(k) {
  case 'q':
    exit=true;
    break;
  case 13: //tecla enter
    if(b_dibujando_objeto)
      b_dibujando_objeto = false;
    break;

  case '+': //zom-in
    if(zoom!=1)
      zoom = zoom/2;
    break;
  case '-': //zoom out
    if(zoom!=1024)
      zoom = zoom*2;
    break;
  case 'a':
    ptr_ultimo = crear_nodo<nodo_bitwise_and>();
    break;
  case 'b':
    ptr_ultimo = crear_nodo<nodo_blur>();
    break;
  case 'c':
    ptr_ultimo = crear_nodo<nodo_canny>();
    break;
  case 'e':
    ptr_ultimo = crear_nodo<nodo_erosion_dilacion>();
    break;
  case 'g':
    ptr_ultimo = crear_nodo<nodo_gris>();
    break;
  case 'h':
    ptr_ultimo = crear_nodo<nodo_hsv>();
    break;
  case 'i':
    for(auto& up : nodos)
    {
      cout << "nodo c:" << up->centro << " r:" << up->radio << endl;
    }
    break;
  case 'p': //piel
    ptr_ultimo = crear_nodo<nodo_mascara>();
  break;

  case 'v':
    ptr_ultimo = crear_nodo<nodo_video>();
    break;

  case '3':

    break;

  case '4':

    break;

  case 'x': //cerramos la ventana del objeto seleccionado
    if(ptr_seleccionado != nullptr)
    {
      ptr_seleccionado->b_mostrar = false;
      cv::destroyWindow(ptr_seleccionado->sid);
    }
    break;

  }
}

//hay espacio para alt y shift.
void mj::cb_mouse(int event, int x, int y, int flags)
{
  static bool botonMouseDerechoAbajo(false); //panning

  puntoActualMouse = cv::Point(x,y);

  if(event == EVENT_RBUTTONDOWN) //panning
  {
    botonMouseDerechoAbajo = true;
    puntoClickMouseDerecho = puntoActualMouse;
    puntoInicioDesplazamiento = despl; //XXXXXXXXXXXXXXXX????????????
  }

  if(event == EVENT_RBUTTONUP)
    botonMouseDerechoAbajo = false;

  if(event == EVENT_LBUTTONDOWN)
  {
    /*Si est�bamos dibujando un objeto y dimos click, lo insertamos y no hacemos nada m�s*/
    if(b_dibujando_objeto)
      b_dibujando_objeto = false;

    /*no est�bamos dibujando un objeto, evaluamos el punto y establecemos condiciones para la selecci�n y el arrastre*/
    else
    {
      auto ptr = determinar_propiedades_ubicacion(transformacion_inversa(puntoActualMouse));

      if(ptr_seleccionado!=nullptr)
        ptr_seleccionado->seleccionar(false);

      if(ptr == nullptr) //estamos dentro del �rea de un objeto y dimos click
      {
        ptr_seleccionado=nullptr;
        return;
      }

      ptr->seleccionar(true);
      ptr_seleccionado = ptr;

      pt_termino = (pt_inicio = transformacion_inversa(puntoActualMouse));

      if(flags & EVENT_FLAG_CTRLKEY) //vamos a dibujar flecha, no a arrastrar
        b_dibujando_flecha = true; //a�adir condici�n
      else
        b_drag = true;
    }
  }

  if(event == EVENT_LBUTTONUP)
  {
    auto dst = determinar_propiedades_ubicacion(transformacion_inversa(puntoActualMouse));

    if(b_dibujando_flecha && ptr_seleccionado!=nullptr && dst!=nullptr && dst != ptr_seleccionado)
    {
      std::cout << "{ "  << ptr_seleccionado->id <<"->" << dst->id <<" }\n";
      crear_relacion(ptr_seleccionado,dst);
    }
    b_drag = false;
    b_dibujando_flecha = false;
  }

  if(event == EVENT_MOUSEMOVE)
  {
    pt_termino = transformacion_inversa(puntoActualMouse);

    if(botonMouseDerechoAbajo)
      despl = puntoInicioDesplazamiento + zoom*(puntoClickMouseDerecho - puntoActualMouse);

    if(b_drag)
    {
      Point dif = pt_termino - pt_inicio;
      ptr_seleccionado->arrastrar(dif);
      pt_inicio = pt_termino;
    }

    determinar_propiedades_ubicacion(pt_termino);

  } //CV_EVENT_MOUSEMOVE

  if(event==EVENT_LBUTTONDBLCLK) //doble_click -> vemos informacion del objeto
  {
    if(ptr_seleccionado != nullptr)
    {
      ptr_seleccionado->b_mostrar = true;
    }
  }

} //manejarInputMouse


nodo* determinar_propiedades_ubicacion(cv::Point p)
{
  nodo* ptr = encontrar_ptr_area(p); //obtenemos un apuntador al objeto que es due�o de esa �rea

  if(ptr_highlight!=nullptr)
      ptr_highlight->highlightear(false);//des-highlighteamos el anterior

  /*Hovereamos en un area sin objetos*/
  if(ptr==nullptr)
    return (ptr_highlight = nullptr);

  ptr->highlightear(true); //highlighteamos al nuevo

  return (ptr_highlight = ptr); //actualizamos la llave highlight
}

template<typename Tipo_Nodo>
nodo* crear_nodo()
{
  b_dibujando_objeto = true;
  pt_termino = (pt_inicio = transformacion_inversa(puntoActualMouse));

  if(ptr_seleccionado != nullptr)
    ptr_seleccionado->seleccionar(false);

  Point hipot = pt_termino - pt_inicio;
  int radio = hypot(hipot.x, hipot.y);
  unique_ptr<Tipo_Nodo> uptr = std::make_unique<Tipo_Nodo>(pt_inicio, radio);
  nodo* ptr_ult = uptr.get();
  mj::nodos.emplace_back(std::move(uptr));
  ptr_highlight=ptr_seleccionado=nullptr;
  return ptr_ult;
}

void crear_relacion(nodo* src, nodo* dst)
{
  dst->suscribir_a(src);
}

nodo* encontrar_ptr_area(cv::Point& p)
{
  for(const auto& uptr : mj::nodos)
  {
    if(uptr->pertenece_a_area(p)) //si el punto cae dentro del �rea de un objeto...
      return uptr.get();
  }

  return nullptr;
};

