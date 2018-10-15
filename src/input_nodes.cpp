#include "input_nodes.h"
#include "utils/sistema_archivos.h"

using namespace std;

nodo_im::nodo_im(cv::Point c, int r):
    nodo(c,r)
{
    int retorno = system("python3 ./scripts/file_dialog.py > tmp_fname.txt");
    if(retorno==0) {
    std::ifstream ifs("tmp_fname.txt", std::ios::in);
    std::string fname;
    std::getline(ifs,fname);
    mmat = cv::imread(fname);
    sid = fname;
    }
    else
    sid = "Imagen" + sid;
}

/************************************************/

nodo_iter_dir::nodo_iter_dir(cv::Point c, int r):
    nodo(c,r)
{
    int retorno = system("python3 ./scripts/directory_dialog.py > tmp_fname.txt");
    if(retorno==0) {
    std::ifstream ifs("tmp_fname.txt", std::ios::in);
    std::getline(ifs,dir_name);
    files = get_file_names(dir_name);
    if(files.size() > 0)
        mmat = cv::imread(files[i]);
    sid = dir_name;
    }
    else
    sid = "Empty_dir";
}

nodo_iter_dir::nodo_iter_dir(cv::Point c, int r, std::string path):
    nodo(c,r)
{
    files = get_file_names(path);
    if(files.size() > 0)
    {
        mmat = cv::imread(files[i]);
        sid = dir_name;
    }
    else
        sid = "Empty_dir";
}

void nodo_iter_dir::actuar() {
    b_mostrar=true; 
    ++i; 
    if(i > files.size())
    i=0;
    mmat = cv::imread(files[i]);
    for(auto n : suscriptores)
    n->msrc = mmat;
  }