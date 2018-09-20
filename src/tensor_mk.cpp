#include <vector>
#include <iostream>
#include "tensor_mk.hpp"
#include "tensorflow/core/public/session.h"
#include "tensorflow/core/platform/env.h"
#include "tensorflow/core/protobuf/meta_graph.pb.h"
#include "tensorflow/core/public/session_options.h"
#include "tensorflow/cc/saved_model/loader.h"

void checkStatus(const tensorflow::Status& status) {
  if (!status.ok()) {
    std::cout << status.ToString() << std::endl;
    exit(1);
  }
}

using tensor_dict = std::vector<std::pair<std::string, tensorflow::Tensor>>;

tensorflow::Status cargar_modelo(tensorflow::Session* sess, std::string graph_fn,
                                std::string checkpoint_fn = "")
{
    namespace tf = tensorflow;
    tf::Status status;

    /* Leemos la grafica del protobuf que exportamos */
    tf::MetaGraphDef graph_def;
    status = tf::ReadBinaryProto(tf::Env::Default(), graph_fn, &graph_def);
    //status = tf::LoadSavedModel();
    if(status != tf::Status::OK()) return status;

    std::cout << "binario leido\n";

    /* Creamos una gráfica en la sessión actual */
    status = sess->Create(graph_def.graph_def());
    if(status != tf::Status::OK()) return status;

    /* Restauramos el model a partir del checkpoint, sólo si el chckpoint está dado*/
    if(checkpoint_fn != "")
    {
        const std::string restore_op_name = graph_def.saver_def().restore_op_name();
        const std::string archivo_nombre_tensor = graph_def.saver_def().filename_tensor_name();

        tf::Tensor mtensor(tf::DT_STRING, tf::TensorShape());
        mtensor.scalar<std::string>()() = checkpoint_fn;

        tensor_dict mdict = {{archivo_nombre_tensor, mtensor}};

        status = sess->Run(mdict, {}, {restore_op_name}, nullptr);
        if(status != tf::Status::OK()) return status;
    }

    else
    {
        std::cout << "Warn: chkpnt_str vacia\n";
        status = sess->Run( {}, {}, {"init"}, nullptr);
        if(status != tf::Status::OK()) return status;
    }
    return status;
}

using namespace std;

tensorflow::Status foobar(tensorflow::Session* sess, tensorflow::SessionOptions sessOpt, std::string export_dir)
{
    namespace tf = tensorflow;

    std::unordered_set<string> tags;

    tf::SavedModelBundle bundle;
    tf::Status load_status = tf::LoadSavedModel(
    sessOpt, tf::RunOptions(), export_dir, tags, &bundle);
    if (!load_status.ok()) {
        std::cout << "Error loading model: " << load_status << std::endl;
        exit(-1);
    }
    cout << "Modelo cargado";

    /* Creamos una gráfica en la sessión actual */
    //bundle.session->Run(bundle.meta_graph_def);
    //if(status != tf::Status::OK()) return status;

    return tf::Status::OK();
}

int tensor_main(char* dir_modelo)
{
    namespace tf = tensorflow;

    tf::Session* sess;
    tf::SessionOptions opciones;
    TF_CHECK_OK(tf::NewSession(opciones, &sess));
    bool bb = tf::MaybeSavedModelDirectory(dir_modelo);
    cout << "\n\n\ndirectorio valido? " << bb << "\n";

    //TF_CHECK_OK(cargar_modelo(sess, modelo_bin, modelo_ckpt));



    return 0;
}
