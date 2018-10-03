#include "caffe_mk.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <algorithm>
#include <iosfwd>
#include <memory>
#include <string>
#include <utility>
#include <vector>

using namespace caffe;  // NOLINT(build/namespaces)
using namespace std;
using cv::Mat;

unique_ptr<Classifier> mClassifier;

Classifier::Classifier(const string& model_file, const string& trained_file, const string& mean_file, const string& label_file) 
{
  Caffe::set_mode(Caffe::GPU);

  /* Load the network. */
  net.reset(new Net<float>(model_file, TEST)); /* TEST es un enum de caffe.pb.h donde */
  net->CopyTrainedLayersFrom(trained_file);

  CHECK_EQ(net->num_inputs(), 1) << "Network should have exactly one input.";
  CHECK_EQ(net->num_outputs(), 1) << "Network should have exactly one output.";

  Blob<float>* input_layer = net->input_blobs()[0];
  num_channels = input_layer->channels();

  CHECK(num_channels == 3 || num_channels == 1) << "Input layer should have 1 or 3 channels.";

  input_geometry = cv::Size(input_layer->width(), input_layer->height());
  std::cout << "estableciendo promedio...\n";
  /* Load the binaryproto mean file. */
  set_mean(mean_file);

  /* Load labels. */
  std::ifstream label_ifs(label_file.c_str());
  CHECK(label_ifs) << "Unable to open labels file " << label_file;

  string line;
  while (std::getline(label_ifs, line))
    labels.push_back(string(line));

  Blob<float>* output_layer = net->output_blobs()[0];
  CHECK_EQ(labels.size(), output_layer->channels())
    << "Number of labels is different from the output layer dimension.";
  cout << "Classifier ctor: \n"
       << "model_file = " << model_file << endl
       << "trained_file = " << trained_file << endl
       << "mean file = " << mean_file << endl
       << "label_file = " << label_file << endl
       << "input_geometry = " << input_geometry << endl
       << "mean = " << mean.size << " " << mean.channels() << endl;
}

Classifier::Classifier(const string& model_file, const string& trained_file) 
{
  Caffe::set_mode(Caffe::GPU);

  /* Load the network. */
  net.reset(new Net<float>(model_file, TEST)); /* TEST es un enum de caffe.pb.h donde */
  net->CopyTrainedLayersFrom(trained_file);

  CHECK_EQ(net->num_inputs(), 1) << "Network should have exactly one input.";
  CHECK_EQ(net->num_outputs(), 1) << "Network should have exactly one output.";

  Blob<float>* input_layer = net->input_blobs()[0];
  num_channels = input_layer->channels();

  CHECK(num_channels == 3 || num_channels == 1) << "Input layer should have 1 or 3 channels.";

  input_geometry = cv::Size(input_layer->width(), input_layer->height());
  std::cout << "estableciendo promedio...\n";

  //mean{promedio};
  mean = cv::Scalar(104, 177, 123);


  Blob<float>* output_layer = net->output_blobs()[0];
  cout << "Classifier ctor: \n"
       << "model_file = " << model_file << endl
       << "trained_file = " << trained_file << endl
       << "input_geometry = " << input_geometry << endl
       << "mean = " << mean.size << " " << mean.channels() << endl;
}

/* Return the indices of the top N values of vector v. */
static std::vector<int> Argmax(const std::vector<float>& v, int N) 
{
  vector<pair<float, int> > pairs;
  for (size_t i = 0; i < v.size(); ++i)
    pairs.push_back(make_pair(v[i], i));

  std::partial_sort(pairs.begin(), pairs.begin() + N, pairs.end(), []
    (const std::pair<float, int>& lhs, const std::pair<float, int>& rhs) 
    {
      return lhs.first > rhs.first;
    }
    );

  vector<int> result;
  for (int i = 0; i < N; ++i)
    result.push_back(pairs[i].second);
  return result;
}

/* Return the top N predictions. */
vector<Prediction> Classifier::Classify(const Mat& img, int N) {
  vector<Prediction> predictions;
  vector<float> output = Predict(img);

  N = std::min<int>(labels.size(), N);
  vector<int> maxN = Argmax(output, N);
  
  for (int i = 0; i < N; ++i)
  {
    int idx = maxN[i];
    predictions.push_back(make_pair(labels[idx], output[idx]));
  }

  return predictions;
}

/* Load the mean file in binaryproto format. */
void Classifier::set_mean(const string& mean_file) {
  BlobProto blob_proto;
  ReadProtoFromBinaryFileOrDie(mean_file.c_str(), &blob_proto);

  /* Convert from BlobProto to Blob<float> */
  Blob<float> mean_blob;
  mean_blob.FromProto(blob_proto);
  CHECK_EQ(mean_blob.channels(), num_channels) << "Number of channels of mean file doesn't match input layer.";

  /* The format of the mean file is planar 32-bit float BGR or grayscale. */
  vector<Mat> channels;
  float* data = mean_blob.mutable_cpu_data();
  for (int i = 0; i < num_channels; ++i) 
  {
    /* Extract an individual channel. */
    Mat channel(mean_blob.height(), mean_blob.width(), CV_32FC1, data);
    channels.push_back(channel);
    data += mean_blob.height() * mean_blob.width(); //literal desplaza el pointer un canal completo
  }

  /* Merge the separate channels into a single image. */
  Mat mean2;
  cv::merge(channels, mean2);

  /* Compute the global mean pixel value and create a mean image
   * filled with this value. */
  cv::Scalar channel_mean = cv::mean(mean2);
  mean = Mat(input_geometry, mean2.type(), channel_mean);
}

vector<float> Classifier::Predict(const Mat& img) 
{
  Blob<float>* input_layer = net->input_blobs()[0];
  input_layer->Reshape(1, num_channels,
                       input_geometry.height, input_geometry.width);
  /* Forward dimension change to all layers. */
  net->Reshape();

  vector<Mat> input_channels;
  wrap_input_layer(&input_channels);

  preprocess(img, &input_channels);

  net->Forward();

  /* Copy the output layer to a std::vector */
  Blob<float>* output_layer = net->output_blobs()[0];
  const float* begin = output_layer->cpu_data();
  const float* end = begin + output_layer->channels();
  return vector<float>(begin, end);
}

/* Wrap the input layer of the network in separate Mat objects
 * (one per channel). This way we save one memcpy operation and we
 * don't need to rely on cudaMemcpy2D. The last preprocessing
 * operation will write the separate channels directly to the input
 * layer. */
void Classifier::wrap_input_layer(std::vector<Mat>* input_channels)
{
  Blob<float>* input_layer = net->input_blobs()[0];
  int width = input_layer->width();
  int height = input_layer->height();
  float* input_data = input_layer->mutable_cpu_data();
  for (int i = 0; i < input_layer->channels(); ++i) 
  {
    Mat channel(height, width, CV_32FC1, input_data);
    input_channels->push_back(channel);
    input_data += width * height;
  }
}

void Classifier::preprocess(const Mat& img, std::vector<Mat>* input_channels)
{
  /* Convert the input image to the input image format of the network. */
  Mat sample;
  if (img.channels() == 3 && num_channels == 1)
    cv::cvtColor(img, sample, cv::COLOR_BGR2GRAY);
  else if (img.channels() == 4 && num_channels == 1)
    cv::cvtColor(img, sample, cv::COLOR_BGRA2GRAY);
  else if (img.channels() == 4 && num_channels == 3)
    cv::cvtColor(img, sample, cv::COLOR_BGRA2BGR);
  else if (img.channels() == 1 && num_channels == 3)
    cv::cvtColor(img, sample, cv::COLOR_GRAY2BGR);
  else
    sample = img.clone();

  Mat sample_resized;
  if (sample.size() != input_geometry)
    cv::resize(sample, sample_resized, input_geometry);
  else
    sample_resized = sample;

  Mat sample_float;
  if (num_channels == 3)
    sample_resized.convertTo(sample_float, CV_32FC3);
  else
    sample_resized.convertTo(sample_float, CV_32FC1);

  Mat sample_normalized;
  cv::subtract(sample_float, mean, sample_normalized);

  /* This operation will write the separate BGR planes directly to the
   * input layer of the network because it is wrapped by the Mat
   * objects in input_channels. */
  cv::split(sample_normalized, *input_channels);

  CHECK(reinterpret_cast<float*>(input_channels->at(0).data)  ==  net->input_blobs()[0]->cpu_data() )
    << "Input channels are not wrapping the input layer of the network.";
}

int cargar_modelo(string model_file, string trained_file, string mean_file, string label_file) {

  //investiga: qué hace Google Logging
  //::google::InitGoogleLogging(argv[0]);

  mClassifier = make_unique<Classifier>(model_file, trained_file, mean_file, label_file);
 
  return 0;
}

void predecir_etiquetas(Mat& img)
{
  CHECK(!img.empty()) << "Unable to decode image";

  /******************************************************/
  std::vector<Prediction> predictions = mClassifier->Classify(img);
  /******************************************************/

  /* Print the top N predictions. Que hace std::fixed? */
  /*for (auto p : predictions) {
    std::cout << std::fixed << std::setprecision(4) << p.second << " - \""
              << p.first << "\"" << std::endl;
  }
  */
  std::cout << std::fixed << std::setprecision(4) << predictions[0].first << " - \""
            << predictions[0].second << "\"" << endl;
}

