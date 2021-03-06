#include <memory>
#include <chrono>
#include "mj.h"
#include "input_nodes.h"
#include "processing_nodes.h"

using namespace std;
using namespace cv;

//function aliases
//not working const auto microsegundos(chrono::duration d) = chrono::duration_cast<chrono::microseconds >(chrono::duration d);

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

template<typename Tipo_Nodo, typename... Args>
nodo* crear_nodo(Args... args);

nodo* determinar_propiedades_ubicacion(cv::Point p);
nodo* encontrar_ptr_area(cv::Point& p);
void generar_red_piel();

void cb_mouse_static(int event, int x, int y, int flags, void* data)
{
  mj* ptr = static_cast<mj*>(data);
  ptr->cb_mouse(event,x,y,flags);
}

mj::mj()
{
  namedWindow(wname);//, cv::WINDOW_GUI_NORMAL | WINDOW_AUTOSIZE);
  //createButton("button2",[](int state, void* userdata) {},NULL,QT_CHECKBOX,0);
  diagrama = Mat(400,600,CV_8UC3, bckgnd);
  setMouseCallback(wname,cb_mouse_static,this);
}

void mj::run()
{

  while(true)
  {
      auto cyc1 = std::chrono::system_clock::now();
    char c = waitKey(33);
    //char c = waitKey(10);
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
    //auto tiempo1 = std::chrono::system_clock::now();
    for(auto& uptr : nodos)
    {
      uptr->procesar();
      uptr->dibujarse();
    }
    //auto tiempo2 = std::chrono::system_clock::now();
    //auto dur = std::chrono::duration_cast<chrono::microseconds>(tiempo2 - tiempo1).count();
    //cout << dur << "us\n";

    if(b_dibujando_flecha) //dibujamos una flecha temporal
      arrowedLine(diagrama, transformar(pt_inicio), transformar(pt_termino), COLOR_BLANCO, 2, LINE_AA);

    imshow(wname,diagrama);
      auto cyc2 = std::chrono::system_clock::now();
      auto dur = std::chrono::duration_cast<chrono::microseconds>(cyc2 - cyc1).count();
      cout << "cycle time: " << dur << "\n";
  }

}


void mj::cb_teclado(char k)
{
  switch(k) {

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
  case 'd':
    if(ptr_seleccionado!=nullptr)
    {
      auto itr = std::find_if(nodos.begin(), nodos.end(), [&](std::unique_ptr<nodo> const& n)
            { return n.get() == ptr_seleccionado; });
      (*itr)->desactivar();
      nodos.erase(itr);
      ptr_seleccionado = ptr_highlight = nullptr;
      b_drag = false;
    }
    break;
  case 'e':
    ptr_ultimo = crear_nodo<nodo_erosion_dilacion>();
    break;
  case 'g':
    ptr_ultimo = crear_nodo<nodo_gris>();
    break;
  case 'h':
    /*Leer el mapa de caracteres e imprimirlo*/
    break;
  case 'i':
    ptr_ultimo = crear_nodo<nodo_im>();
    break;
  case 'j':
    ptr_ultimo = crear_nodo<nodo_iter_dir>("./sw_team");
    break;
  case 'l':
    ptr_ultimo = crear_nodo<nodo_laplace>();
    break;
  case 'p': //piel
    ptr_ultimo = crear_nodo<nodo_mascara>();
    break;
  case 'q':
    exit=true;
    break;
  case 's':
    ptr_ultimo = crear_nodo<nodo_hough_circulo>();
    break;
  case 'u':
    ptr_ultimo = crear_nodo<nodo_hsv>();
    break;
  case 'v':
    ptr_ultimo = crear_nodo<nodo_video>();
    break;

  case 'x': //cerramos la ventana del objeto seleccionado
    if(ptr_seleccionado != nullptr)
    {
      ptr_seleccionado->desactivar();
    }
    break;
  case '1':
    if(ptr_seleccionado!=nullptr)
      ptr_seleccionado->info();
    break;
  case '9':
    generar_red_piel();
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
      dst->suscribir_a(ptr_seleccionado);
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
      std::cout << "mostrando " << ptr_seleccionado->sid << endl;
      ptr_seleccionado->actuar();
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

template<typename Tipo_Nodo, typename... Args>
nodo* crear_nodo(Args... args)
{
  b_dibujando_objeto = true;
  pt_termino = (pt_inicio = transformacion_inversa(puntoActualMouse));

  if(ptr_seleccionado != nullptr)
    ptr_seleccionado->seleccionar(false);

  Point hipot = pt_termino - pt_inicio;
  int radio = hypot(hipot.x, hipot.y);
  unique_ptr<Tipo_Nodo> uptr = std::make_unique<Tipo_Nodo>(pt_inicio, radio, args...);
  nodo* ptr_ult = uptr.get();
  mj::nodos.emplace_back(std::move(uptr));
  ptr_highlight=ptr_seleccionado=nullptr;
  return ptr_ult;
}

/*cuál es la diferencia entre crear nodo y generar nodo?*/
template<typename Tipo_Nodo>
nodo* generar_nodo(cv::Point centro, int radio)
{
  unique_ptr<Tipo_Nodo> uptr = std::make_unique<Tipo_Nodo>(centro, radio);
  nodo* p = uptr.get();
  mj::nodos.emplace_back(std::move(uptr));
  return p;
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

void generar_red_piel()
{
  const int radio = 500;
  nodo* nv = generar_nodo<nodo_video>(cv::Point(radio,radio), radio);
  nv->procesar(); // 1. necesarios para inicializar las cv::Mat
  nodo* nhsv = generar_nodo<nodo_hsv>(cv::Point(radio,radio*4), radio);
  nhsv->suscribir_a(nv);
  nhsv->procesar(); // 2.
  nodo* npiel = generar_nodo<nodo_mascara>(cv::Point(radio*4,radio*4), radio);
  npiel->suscribir_a(nhsv);
  npiel->procesar(); // 3.
  nodo* nerodila = generar_nodo<nodo_erosion_dilacion>(cv::Point(radio*8,radio*4), radio);
  nerodila->suscribir_a(npiel);
  nerodila->procesar(); // 4.
  nodo* nblur = generar_nodo<nodo_blur>(cv::Point(radio*12,radio*4), radio);
  nblur->suscribir_a(nerodila);
  nblur->procesar(); //5.
  nodo* nbwand = generar_nodo<nodo_bitwise_and>(cv::Point(radio*12,radio), radio);
  nbwand->suscribir_a(nv);
  nbwand->suscribir_a(nblur);
}

