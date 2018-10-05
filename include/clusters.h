#ifndef MK_CLUSTERS
#define MK_CLUSTERS

#include <vector>
#include <array>
#include <fstream>

namespace mk
{

/* cada cluster representa una entidad individual */

struct cluster
{
    cluster(std::array<float,128>& embedding);
    std::array<float,128> centroid; //I don't know if this is the best, but it's the only think I can think of, besides kNN.
    //std::vector<std::array<float,128>> data; //piensa bien si quieres usar esto.
    void agregar_punto(std::array<float,128>& new_point);
    float operator-(std::array<float,128>& other);
    static void log(std::array<float,128>& point);

    std::string label;
    constexpr static float THRESHOLD_DISTANCE{0.49};
    unsigned int N{0};
    static int sid;
    static std::fstream fs;
};

}

#endif