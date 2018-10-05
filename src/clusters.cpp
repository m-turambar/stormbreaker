#include "clusters.h"
#include "cmath"

using namespace std;

/*The purpose of this class is to organize the data obtained from faces into a data structure that is both efficient and easy to use

Face data is a 128-d<float> vector
*/

namespace mk
{

int cluster::sid{0};

cluster::cluster(std::array<float,128>& embedding )
{
    centroid = embedding;
    label = std::to_string(sid);
    ++sid;
}

void cluster::agregar_punto(std::array<float,128>& new_point)
{
    //const int N = data.size();
    //data.push_back(new_point);
    //averigua como paralelizar esto
    for(int i=0; i<128; ++i)
    {
        /* Se actualiza el centroide de este cluster particular con un nuevo punto*/
        centroid[i] = centroid[i]*(float)(N)/(float)(N+1) + new_point[i]/(float)(N+1);
    }
}

float cluster::operator-(std::array<float,128>& other)
{
    float dif = 0;
    for(int i = 0; i<128; ++i)
        dif += std::pow(centroid[i] - other[i], 2);
    return dif;
}

void cluster::log(std::array<float,128>& point)
{

}



}
