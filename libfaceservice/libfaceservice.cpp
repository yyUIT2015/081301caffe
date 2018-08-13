#include "libfaceservice.h"
#include "define.h"
#include "resource.h"
#include "facedatabase.h"
#include "facerecognize.h"
#include "facedetect.h"
#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <thread>
#include <boost/thread.hpp>
#include "network.h"
#include <math.h>
#include "watchstop.h"

libfaceservice* libfaceservice::instance=nullptr;

libfaceservice* libfaceservice::getInstance()
{
    if(instance==nullptr)
        instance=new libfaceservice();
    return instance;
}

libfaceservice::libfaceservice():resource(nullptr)
{
    //  std::cout<<std::this_thread::get_id()<<"    "<<__FILE__<<"   "<<__LINE__<<"   " <<__FUNCTION__<<"   "<<__FILE__<<__LINE__<<"\n\n$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n\n"<<std::endl;
    resource = new Resource;
    //   std::cout<<std::this_thread::get_id()<<"    "<<__FILE__<<"   "<<__LINE__<<"   " <<__FUNCTION__<<"   "<<__FILE__<<__LINE__<<"\n\n$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n\n"<<std::endl;
    isinit = false;
}

libfaceservice::~libfaceservice()
{
    if(resource != nullptr)
        delete resource;
}

void libfaceservice::init(std::string working_dir_,int FDnetmax=30,int FRnetmax=30)
{
    working_dir = working_dir_;
    //    std::cout<<std::this_thread::get_id()<<"    "<<__FILE__<<"   "<<__LINE__<<"   " <<__FUNCTION__<<"   "<<__FILE__<<__LINE__<<"\n\n$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n\n"<<std::endl;
    resource->init(working_dir_);
    FaceRecognize::biasFace = resource->getBiasFace();
    resource->setFDnetmax(FDnetmax);
    resource->setFRnetmax(FRnetmax);
    isinit = true;
}
int libfaceservice::addFaceGroup(std::string groupId)
{
    std::string facedb_dir=working_dir + "/Face_DB";
    return FaceDatabase::createFaceGroup(facedb_dir, groupId);
}

int libfaceservice::addFace2Group(std::string groupId, std::string imgPath,double Factor)
{
    std::string facedb_dir=working_dir + "/Face_DB";
    FD_Networks  nets =(resource->getFaceDetectNetwork());
    if(isinit == false)
    {
        return 0;
    }
    if(nets.ONet == NULL)
    {
        std::cout<<std::this_thread::get_id()<<"    "<<__FILE__<<"   "<<__LINE__<<"   " <<__FUNCTION__<<"   "<<"Unable to apply for FDresources"<<std::endl;

        return -14;
    }
    Network_Ptr net_ = resource->getFaceRecognizeNetwork();
    if(net_.get() == NULL)
    {
        std::cout<<std::this_thread::get_id()<<"    "<<__FILE__<<"   "<<__LINE__<<"   " <<__FUNCTION__<<"   "<<"Unable to apply for FRresources"<<std::endl;
        return -13;
    }
    int code = FaceDatabase::addFace2Group(imgPath,facedb_dir,groupId, nets,net_,Factor);
    resource->collectFaceDetectNetwork(nets);
    resource->collectFaceRecognizeNetwork(net_);
    return code;
}

DetectResult libfaceservice::detectFace(std::string imgPath,double Factor)
{
    DetectResult result;
    if(isinit == false)
    {
        result.err = FaceserviceNotinit;
        return result;
    }
    FD_Networks  nets =(resource->getFaceDetectNetwork());

    if(nets.ONet == NULL)
    {
        std::cout<<std::this_thread::get_id()<<"    "<<__FILE__<<"   "<<__LINE__<<"   " <<__FUNCTION__<<"   "<<"Unable to apply for FDresources"<<std::endl;
        result.err = (ErrorCode)(-14);
        return result;
    }

    IplImage *img = cvLoadImage(imgPath.c_str(),1);
    cv::Mat imgMat(img);

    result.imageWidth = img->width;

    result.imageHeight=img->height;
    result.url=imgPath;
    FaceDetect::detectFace(imgMat,nets,result,Factor);

    resource->collectFaceDetectNetwork(nets);

    return result;
}

IdentifyResult libfaceservice::faceIdentity(std::string group,std::string imgPath, Landmark &lm)
{

    Network_Ptr net_ = resource->getFaceRecognizeNetwork();
    IdentifyResult result;
    if(isinit == false)
    {
        result.err = FaceserviceNotinit;
        return result;
    }
    if(net_.get() == NULL)
    {
        std::cout<<std::this_thread::get_id()<<"    "<<__FILE__<<"   "<<__LINE__<<"   " <<__FUNCTION__<<"   "<<"Unable to apply for FRresources"<<std::endl;
        result.err = (ErrorCode)(-13);
        return result;
    }

    std::string groupname;
    std::vector<std::string> files_d;
    getdir(working_dir+"/Face_DB",files_d);
    for(auto f :files_d)
    {
        int idx=f.find(group);//在a中查找b.
        if(idx != std::string::npos ){//不存在。
            groupname =f;
            break;
        }
    }


    IplImage *img = cvLoadImage(imgPath.c_str(),1);
    std::string facedb_dir = working_dir + "/Face_DB";
    FaceDB_Feature faceGroup ;

    int code = FaceDatabase::loadFaceGroup(facedb_dir,groupname,faceGroup);

    if(faceGroup.size()<=0)
    {
        result.err = (ErrorCode)code;
        return result;
    }
    //FaceRecognize::faceIdentify(img,lm,faceGroup,net_, result);
    cv::CascadeClassifier face_cascade = resource->getFace_cascade();
    //    FaceRecognize::faceIdentify_withSideFaceFilter(img,lm,faceGroup,net_,face_cascade, result);
    FaceRecognize::faceIdentify(img,lm,faceGroup,net_, result);
    resource->collectFaceRecognizeNetwork(net_);
    cv::Mat image = cv::imread(imgPath);
    result.imageHeight = image.rows;
//    std::cout<<result.imageHeight<<"__imageHeight__"<<std::endl;
    result.imageWidth = image.cols;
//    std::cout<<result.imageWidth<<"__imageWidth__"<<std::endl;
    return result;
}

void libfaceservice::release()
{
    if(instance != nullptr)
        delete instance;
    instance=nullptr;
}

AddgroupResult libfaceservice::addfacegroup(std::string groupname,std::string groupUrl,double Factor)
{
    AddgroupResult result;
    if(isinit == false)
    {
        result.err = FaceserviceNotinit;
        return result;
    }
    WatchStop watch;
    watch.start();
    int added = 0;

    int err1 = libfaceservice::addFaceGroup(groupname);

    std::string face_dir_f = working_dir + "/Face_DB/"+ groupname;
    std::vector<std::string> files_f;
    libfaceservice::getimage(face_dir_f,files_f);
    int count_f = files_f.size();

    //getAddGroup1Json(err1,groupid);
    if(err1 == 0||err1 == -9){
        std::string imageUrl;
        imageUrl = groupUrl;
        std::vector<std::string> files;
        libfaceservice::getimage(imageUrl,files);
        for(auto f : files){
            int idx1 = f.find_last_of(".");
            int idx2 = f.size()-1;
            int diff = idx2-idx1;
            std::string fileName = f.substr(idx1+1,diff);
            if(fileName == "png"||fileName == "jpg"||fileName == "jpeg")
            {
                err1 = libfaceservice::addFace2Group(groupname,f,Factor);
                if(err1 == 0)
                {
                    added++;

                }
            }
            if(err1 == -8){
                FailReason failreason;
                failreason.url = f;
                failreason.reason = "FaceGroupNotExist";
                result.failreasons.push_back(failreason);
            }else if(err1 == -11){
                FailReason failreason;
                failreason.url = f;
                failreason.reason = "CannotCreateFaceGroupDir";
                result.failreasons.push_back(failreason);
            }
            //            else{
            //                FailReason failreason;
            //                failreason.url = f;
            //                failreason.reason = "othererr";
            //                result.failreasons.push_back(failreason);
            //            }
        }
    }

    std::string face_dir = working_dir + "/Face_DB/"+ groupname;
    std::vector<std::string> files;
    libfaceservice::getimage(face_dir,files);
    result.count = files.size();
    result.added =  result.count - count_f;
    result.timeUsed = watch.time_elapsed_ms();
    result.err = (ErrorCode)err1;
    return result;
}

int libfaceservice::getimage(std::string dir, std::vector<std::string> &files){
    DIR *dp;
    struct dirent *dirp;
    if((dp  = opendir(dir.c_str())) == NULL) {
        //std::cout << "Error(" << errno << ") opening " << dir << endl;
        std::cout<<"Error: cannot open directory"<<std::endl;
        return -1;
    }

    while ((dirp = readdir(dp)) != NULL) {
        std::string name = dirp->d_name;
        if(name != "."&&name != ".."){
            if(dirp->d_type ==DT_DIR){
                std::string newUrl = dir+"/"+name;
                getimage(newUrl,files);
            }else{
                files.push_back(std::string(dir+"/"+name));
            }
        }
    }
    closedir(dp);
    return 0;
}

int libfaceservice::getdir(std::string dir, std::vector<std::string> &files){
    DIR *dp;
    struct dirent *dirp;
    if((dp  = opendir(dir.c_str())) == NULL) {
        //std::cout << "Error(" << errno << ") opening " << dir << endl;
        std::cout<<"Error: cannot open directory"<<std::endl;
        return -1;
    }

    while ((dirp = readdir(dp)) != NULL) {
        std::string name = dirp->d_name;
        if(name != "."&&name != ".."){
            if(dirp->d_type ==DT_DIR){
                files.push_back(name);
//                std::cout << name << std::endl;
            }
        }
    }
    closedir(dp);
    return 0;
}
