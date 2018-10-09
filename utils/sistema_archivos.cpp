#include "utils/sistema_archivos.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <experimental/filesystem>

namespace fs = std::experimental::filesystem;
using namespace std;
 
vector<string> get_file_names(std::string dir_name)
{
    vector<string> files;
    std::cout << "iterando sobre " << dir_name << std::endl;
    for(auto& p: fs::directory_iterator(dir_name)) {
        string s = p.path();
        std::cout << s << '\n';
        files.push_back(s);
    }
    return files;
}
