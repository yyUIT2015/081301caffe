#ifndef RESOURCE_H
#define RESOURCE_H
#include "network.h"
#include "define.h"
#include <boost/thread.hpp>

class Resource
{
public:

    Resource();
    void init(const std::string model_dir);

    void setFDnetmax(int value);
    FD_Networks getFaceDetectNetwork();
    void collectFaceDetectNetwork(FD_Networks models);
    void loadNetworkModels(const std::string proto_model_dir,FD_Networks & fd_mods);

    void setFRnetmax(int value);
    Network_Ptr getFaceRecognizeNetwork();
    void collectFaceRecognizeNetwork(Network_Ptr net);
    void loadNetworkModels(const std::string model_dir, Network_Ptr &net);



    //void loadFaceDB(std::string face_db_dir, FaceDB_Feature &face_db, Network_Ptr net);
    cv::Mat getBiasFace() const;

    int addFaceDBGroup(std::string groupId, FaceDB_Feature &db_feature);

    void release();
    cv::CascadeClassifier getFace_cascade() const;

private:

    int FDnetmax =3;
    boost::mutex fr_mutex;
    std::list<Network_Ptr> list_fr_idle;
    std::list<Network_Ptr> list_fr_busy;
    Network_Ptr netptr;

    int FRnetmax =3;
    boost::mutex fd_mutex;
    std::list<FD_Networks> list_fd_busy;
    std::list<FD_Networks> list_fd_idle;
    FD_Networks netfd;
    std::string model_path;

    cv::Mat BiasFace;

    cv::CascadeClassifier face_cascade;

    std::map<std::string, FaceDB_Feature> face_db_group;
};




#endif // RESOURCE_H
