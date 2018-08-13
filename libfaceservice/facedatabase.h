#ifndef FACEDATABASE_H
#define FACEDATABASE_H


#include "define.h"
#include "facedetect.h"




class FaceDatabase
{
public:
    FaceDatabase();

    static int createFaceGroup(std::string faceDB_dir,std::string groupId);
    static int addFace2Group(std::string imgfile,std::string faceDB_dir, std::string groupId,
                             FD_Networks & fd_nets, Network_Ptr net_,double Factor);
    static int loadFaceGroup(std::string faceDB_dir, std::string groupId, FaceDB_Feature &db_feature);

};

#endif // FACEDATABASE_H
