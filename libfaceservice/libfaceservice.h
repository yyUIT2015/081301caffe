#ifndef libfaceservice_H
#define libfaceservice_H
#include <string>
#include "define.h"
//#include "network.h"

class Resource;
class libfaceservice
{
public:
    ~libfaceservice();
    void init(std::string working_dir,int FDnetmax,int FRnetmax);
    int addFaceGroup(std::string groupId);
    int addFace2Group(std::string groupId, std::string img,double Factor);
    DetectResult detectFace(std::string img,double Factor);
    IdentifyResult faceIdentity(std::string group,std::string img, Landmark &lm);
    AddgroupResult addfacegroup(std::string groupname,std::string group_url,double Factor);
    int getimage(std::string dir,std::vector<std::string> &files);
    int getdir(std::string dir, std::vector<std::string> &files);
    static void release();

    static libfaceservice* getInstance();
    //   {
    //     if(instance==nullptr)
    //       instance = new libfaceservice();
    /// return instance;
    //}
protected:

    libfaceservice();
private:
    Resource *resource;
    std::string working_dir;
    static libfaceservice* instance;
    bool isinit;
};

#endif // libfaceservice_H
