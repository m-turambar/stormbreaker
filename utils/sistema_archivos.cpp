#include "utils/sistema_archivos.h"
#include <fstream>
#include <iostream>
#include <vector>
#ifdef _WIN32
#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;
#else
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#endif

using namespace std;
 
vector<string> get_file_names(std::string dir_name)
{
    vector<string> files;
    std::cout << "iterando sobre " << dir_name << std::endl;
    for(auto& p: fs::directory_iterator(dir_name)) {
        #ifdef _WIN32
        string s = p.path().string();
        #else
        string s = p.path();
        #endif
        std::cout << s << '\n';
        files.push_back(s);
    }
    return files;
}
