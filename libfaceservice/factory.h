#ifndef FACTORY_H
#define FACTORY_H
#include <string>

#include "define.h"
#include "resource.h"
#include "facedatabase.h"
#include "facerecognize.h"
#include "facedetect.h"
CAFFE_NAMESPACE_BEGIN

class Factory
{
public:
    ~Factory();
    void init(std::string working_dir,int FDnetmax,int FRnetmax);
    int addFaceGroup(std::string groupId);
    int addFace2Group(std::string groupId, std::string img);
    DetectResult detectFace(std::string img);
    IdentifyResult faceIdentity(std::string group,std::string img, Landmark &lm);
    static void release();

    static Factory* getInstance();
    //   {
    //     if(instance==nullptr)
    //       instance = new Factory();
    /// return instance;
    //}
protected:

    Factory();
private:
    Resource *resource;
    std::string working_dir;
    static Factory* instance;
};
CAFFE_NAMESPACE_END

#endif // FACTORY_H
