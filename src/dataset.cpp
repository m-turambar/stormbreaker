#include "dataset.h"
#include <fstream>
#include <sstream>

using namespace std;
using namespace dlib;

namespace mk
{

dataset::dataset(std::string path):
    mCsvPath(path)
{
}

dataset::dataset(std::vector<data_t> embedding_set)
{
    data = embedding_set;
}

void dataset::load_data(std::string path)
{
    mCsvPath = path;
    fstream fs(path);
    string linea;
    while(std::getline(fs, linea))
    {
        /*descomponemos la línea en label, embedding*/
        std::stringstream ss(linea);
        matrix<float,0,1> e;
        std::vector<float> vf;
        string label;
        getline(ss, label, ',');
        string impath;
        getline(ss, impath, ',');
        float f;
        while(ss >> f)
        {
            vf.push_back(f);
            if(ss.peek() == ',')
                ss.ignore();
        }
        e = dlib::mat(vf);
        auto tupla = make_tuple(label,impath,e);
        data.push_back(tupla);
    }
}

void dataset::save_data(std::string path)
{
    ofstream ofs(path);
    for(auto d : data)
    {
        string label;
        string impath;
        embedding_t e;
        std::tie(label, impath, e) = d;
        ofs << label << ',' << impath << ',';
        for(auto f : e)
            ofs << f << ',';
        ofs << '\n';
    }
}

void dataset::save_instance(data_t& instancia)
{
    ofstream ofs(mCsvPath);
    string label;
    string impath;
    embedding_t e;
    std::tie(label, impath, e) = instancia;
    ofs << label << ',' << impath << ',';
    for(auto f : e)
        ofs << f << ',';
    ofs << '\n';
}







} //namespace mk