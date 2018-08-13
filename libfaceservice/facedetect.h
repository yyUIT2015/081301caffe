#ifndef FACEDETECT_H
#define FACEDETECT_H

#include "define.h"

#include "network.h"




//enum ErrorCode_Detect
//{
//    NoError=0,
//    ImageBroken=-5,
//    OtherError=-11
//};


//struct DetectResult
//{
//    int imageWidth;
//    int imageHeight;
//    long timeUsed_ms;
//    ErrorCode err;
//    std::string url;
//    std::list<Face> faces;
//};



typedef struct FacePts {//feature points
  float x[5],y[5];
} FacePts;

typedef struct FaceInfos {
  FaceRect bbox;
  cv::Vec4f regression;
  FacePts facePts;
//  double roll;
//  double pitch;
//  double yaw;
} FaceInfos;

class FaceDetect
{
public:
    FaceDetect();

    //DetectResult detectFace(const char* image_path, FD_Networks &fd_nets);
    static void detectFace(cv::Mat &img, FD_Networks &fd_nets,DetectResult &result,double Factor);

protected:
    static void WrapInputLayer(std::vector<cv::Mat>* input_channels,caffe::Blob<float>* input_layer,
            const int height,const int width);
    static std::vector<FaceInfos> NonMaximumSuppression(std::vector<FaceInfos>& bboxes,
                  float thresh,char methodType);
    static void Bbox2Square(std::vector<FaceInfos>& bboxes);
    static void Padding(int img_w,int img_h,std::vector<FaceInfos> &regressed_rects_,std::vector<FaceInfos> &regressed_pading_);

    static void ClassifyFace(const std::vector<FaceInfos>& regressed_rects,cv::Mat &sample_single,
            boost::shared_ptr<caffe::Net<float> >& net,double thresh,char netName,std::vector<FaceInfos> &condidate_rects_,std::vector<FaceInfos> &regressed_pading_);

    static std::vector<FaceInfos> BoxRegress(std::vector<FaceInfos> &faceInfo_, int stage);

    static void Detect(const cv::Mat& img, std::vector<FaceInfos> &faceInfo, int minSize, double* threshold, double factor, FD_Networks &fd_nets);

};



#endif // FACEDETECT_H
