#ifndef NETWORK_H
#define NETWORK_H
// caffe
#include <caffe/caffe.hpp>
#include <caffe/layers/memory_data_layer.hpp>

#include <dirent.h>

// c++
#include <string>
#include <vector>
#include <fstream>
// opencv
//#include <opencv.h>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
// boost
#include "boost/make_shared.hpp"
typedef boost::shared_ptr<caffe::Net<float> > Network_Ptr;
typedef std::map<std::string,std::vector<float> > FaceDB_Feature;
struct FD_Networks
{
    Network_Ptr PNet;
    Network_Ptr RNet;
    Network_Ptr ONet;
    int num_channels;
    bool operator==( FD_Networks const & b)
    {
        if((PNet == b.PNet) &&
                (RNet == b.RNet) &&
                (ONet == b.ONet) &&
                (num_channels == b.num_channels))
            return true;
        else
            return false;
    }
};
#endif // NETWORK_H
