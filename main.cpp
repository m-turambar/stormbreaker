#include <iostream>
#include "mj.h"

using namespace std;
using namespace cv;

int main(int argc, char** argv)
{
    mj mjol;

    //mjol.test_init(); //definido en nodo.cpp, usado para pruebas
    
    //tensor_main(argv[1]);
    //cout << "cargando modelo en caffe... ";
    //cargar_modelo(argc, argv);
    //cout << " hecho.\n";
    
    mjol.run();
    return 0;
}
