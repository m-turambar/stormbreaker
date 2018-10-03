#include "clusters.h"
#include "cmath"

using namespace std;

/*The purpose of this class is to organize the data obtained from faces into a data structure that is both efficient and easy to use

Face data is a 128-d<float> vector
*/

namespace mk
{

cluster::cluster()
{

}

void cluster::agregar_punto(std::array<float,128>& new_point)
{
    const int N = data.size();
    data.push_back(new_point);
    //averigua como paralelizar esto
    for(int i=0; i<128; ++i)
    {
        /* Se actualiza el centroide de este cluster particular con un nuevo punto*/
        centroid[i] = centroid[i]*(float)(N)/(float)(N+1) + new_point[i]/(float)(N+1);
    }
}

float cluster::operator-(cluster& other)
{
    float dif = 0;
    for(int i = 0; i<128; ++i)
        dif += std::pow(centroid[i] - other.centroid[i], 2);
    return dif;
}



}
