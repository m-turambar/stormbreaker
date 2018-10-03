#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/dnn.hpp>
#include "mj.h"
#include "node.h"
#include "tensor_mk.hpp"
#include "caffe_mk.h"
#include "dlib_mk.h"


using namespace std;
using namespace cv;

int main(int argc, char** argv)
{
    /* Requerimos que el usuario introduzca el path a los modelos*/
    /*if(argc < 2)
    {
        std::cout << "Uso:\n\t"
                  << "stormbreaker path/to/model/\n";
                  return -1;
    }*/
    mj mjol;

    //mjol.test_init(); //definido en nodo.cpp, usado para pruebas
    
    //tensor_main(argv[1]);
    //cout << "cargando modelo en caffe... ";
    //cargar_modelo(argc, argv);
    //cout << " hecho.\n";
    
    mjol.run();
    return 0;
}
