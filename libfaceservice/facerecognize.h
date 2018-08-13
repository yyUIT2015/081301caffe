#ifndef FACERECOGNIZE_H
#define FACERECOGNIZE_H
#include "define.h"
#include <map>
#include "network.h"


class FaceRecognize
{
public:
    FaceRecognize();

    static void faceIdentify(IplImage *frame_, Landmark &lm,
                      FaceDB_Feature &face_db_feature,
                      Network_Ptr net_, IdentifyResult &ir);
    static void faceIdentify_withSideFaceFilter(IplImage *frame_, Landmark &lm,
                                         FaceDB_Feature &face_db_feature,
                                         Network_Ptr net_,
                                         cv::CascadeClassifier &face_cascade,
                                         IdentifyResult &ir);


    static void extract_feat_mul(cv::Mat* img_list, int NUM,
                                 Network_Ptr net_);//add


    static bool CvMatToDatumSignalChannel(const cv::Mat& cv_mat, caffe::Datum* datum);

    static void cos_sim(float * a, std::vector<float> &b,  float &score );

    static cv::Mat biasFace;
};


class FaceAlign
{
public:
    static void align(IplImage * im, float* feat_points, IplImage * out, int side_);
};


#endif // FACERECOGNIZE_H
