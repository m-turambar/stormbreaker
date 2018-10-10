#ifndef MK_DATASET
#define MK_DATASET

#include <tuple>
#include <iostream>
#include <vector>
#include <dlib/matrix.h>

namespace mk
{

using embedding_t = dlib::matrix<float,0,1>;
                         /* label      impath       embedding */
using data_t = std::tuple<std::string, std::string, embedding_t>;

struct dataset
{
    dataset(std::string path);
    dataset(std::vector<data_t> embedding_set);
    void load_data(std::string path);
    void save_data(std::string path);
    void save_instance(data_t& instancia);
    std::vector<data_t> data;
    std::string mCsvPath;
};

}

#endif