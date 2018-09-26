#ifndef CAFFE_MK_H
#define CAFFE_MK_H

#include <iostream>
#include <opencv2/core/core.hpp>
#include <caffe/caffe.hpp>

/* Pair (label, confidence) representing a prediction. */
typedef std::pair<std::string, float> Prediction;

struct Classifier
{
  Classifier() {};
  Classifier(const std::string& model_file,
             const std::string& trained_file,
             const std::string& mean_file,
             const std::string& label_file);

  std::vector<Prediction> Classify(const cv::Mat& img, int N = 5);
  std::vector<float> Predict(const cv::Mat& img);

  void set_mean(const std::string& mean_file);
  void wrap_input_layer(std::vector<cv::Mat>* input_channels);
  void preprocess(const cv::Mat& img, std::vector<cv::Mat>* input_channels);

  std::shared_ptr<caffe::Net<float> > net;
  cv::Size input_geometry;
  int num_channels;
  cv::Mat mean;
  std::vector<std::string> labels;
};

int cargar_modelo(int argc, char** argv);
void predecir_etiquetas(cv::Mat& img);

#endif // CAFFE_MK_H
