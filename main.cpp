#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/dnn.hpp>
#include "mj.h"
#include "node.h"
#include "tensor_mk.hpp"


using namespace std;
using namespace cv;

int main(int argc, char** argv)
{
    /* Requerimos que el usuario introduzca el path a los modelos*/
    if(argc < 3)
    {
        std::cout << "Uso:\n\t"
                  << "stormbreaker path/to/model/binary path/to/model/chkpt\n";
                  return -1;
    }
    mj mjol;
    tensor_main(argv[1], argv[2]);
    mjol.run();
    return 0;
}
