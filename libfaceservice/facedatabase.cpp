#include "facedatabase.h"
#include "facerecognize.h"
#include "facedetect.h"
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>

#include <sys/stat.h>



bool isExist(std::string path)
{
    struct stat statbuf;
    if (stat(path.c_str(), &statbuf) != -1) {
        if (S_ISDIR(statbuf.st_mode)) {
            return 1;
        }
    }
    return 0;
}

int getdir (std::string dir, std::vector<std::string> &files)
{
    //    std::cout<<   dir<<std::endl;
    DIR *dp;
    struct dirent *dirp;
    if((dp  = opendir(dir.c_str())) == NULL) {
        //std::cout << "Error(" << errno << ") opening " << dir << endl;
        std::cout<<"Error: cannot open directory"<<std::endl;
        return -1;
    }

    while ((dirp = readdir(dp)) != NULL ) {
        if( strcmp(dirp->d_name,".") != 0 && strcmp(dirp->d_name,".."))
            files.push_back(std::string(dirp->d_name));
    }
    //        std::cout<<"filesssss   "<<   files.size()<<std::endl;
    closedir(dp);
    return 0;
}

FaceDatabase::FaceDatabase()
{

}
int FaceDatabase::createFaceGroup(std::string faceDB_dir,std::string groupId)
{
    if(!isExist(faceDB_dir))
    {//face db path is not exist
        //std::cout<<"faceDB_dir:"<<faceDB_dir<<std::endl;
        return FaceDatabaseDirNotExist;
    }

    std::string groupPath = faceDB_dir +"/"+groupId;
    if(isExist(groupPath))
    {//group already exist
        return FaceGroupAlreadyExist;
    }


    const int dir_err = mkdir(groupPath.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    if (-1 == dir_err)
    {
        std::cout<<"Error creating directory!"<<std::endl;
        return CannotCreateFaceGroupDir;
        //printf("Error creating directory!n");
        //exit(1);
    }
    return NoError;
}

int FaceDatabase::addFace2Group(std::string imgfile, std::string faceDB_dir,
                                std::string groupId, FD_Networks & fd_nets, Network_Ptr net_,double Factor)
{

    std::string groupPath = faceDB_dir +"/"+groupId;
    if(!isExist(groupPath))
    {//group already exist
        return FaceGroupNotExist;
    }


    DetectResult result ;

    IplImage *frame_ = cvLoadImage(imgfile.c_str(),1);
    cv::Mat img(frame_);
    FaceDetect::detectFace(img,fd_nets,result,Factor);

    if(result.faces.size()==0)
    {
        std::cout<<"Error: cannot detect face! "<<img<<std::endl;
        return OtherErr;
    }
    else if(result.faces.size()==1)
    {//face align then save to image
        std::list<Face>::iterator itr=result.faces.begin();
        Landmark &lm = (*itr).landmark;
        float keyPos[10];
        keyPos[0] = lm.eyeLeft.x;
        keyPos[1] = lm.eyeLeft.y;
        keyPos[2] = lm.eyeRight.x;
        keyPos[3] = lm.eyeRight.y;
        keyPos[4] = lm.nose.x;
        keyPos[5] = lm.nose.y;

        keyPos[6] = lm.mouthLeft.x;
        keyPos[7] = lm.mouthLeft.y;
        keyPos[8] = lm.mouthRight.x;
        keyPos[9] = lm.mouthRight.y;

        IplImage *M=cvCreateImage(cvSize(112,112),IPL_DEPTH_8U,3);
        FaceAlign::align(frame_,keyPos,M,112);

        cv::Mat aligned(M);

        boost::filesystem::path p(imgfile.c_str());
        std::string imgName = p.filename().string();

        FaceRecognize::extract_feat_mul(&aligned,1,net_);

        float feat_[DIM];//
        const boost::shared_ptr<caffe::Blob<float> > ft = net_->blob_by_name("fc1");//_bn");
        const float* ft_data = ft->cpu_data();
        for(int kk=0;kk<DIM;kk++)
            feat_[kk] = ft_data[kk];

        std::string file = std::string(imgName);
        int idx = file.find_last_of(".");
        std::string id_s = file.substr(0,idx);

        std::string featureFile = groupPath+"/"+id_s;
        std::ofstream ofs;
        ofs.open(featureFile, std::ofstream::binary);
        ofs.write((char*)feat_,DIM*sizeof(float));
        ofs.close();



    }
    if(result.faces.size()>1)
    {
        std::cout<<"Error: detected multiple face! "<<result.faces.size() <<std::endl;
        return OtherErr;
    }

    return NoError;
    return 0;
}

int FaceDatabase::loadFaceGroup(std::string faceDB_dir, std::string groupId, FaceDB_Feature &db_feature)
{
    std::string groupPath = faceDB_dir +"/"+groupId;
    //    std::cout<<   groupPath<<std::endl;
    //    std::cout<<   faceDB_dir<<std::endl;
    //        std::cout<<   groupId<<std::endl;
    if(!isExist(groupPath))
    {//group already exist
        return FaceGroupNotExist;
    }
    std::vector<std::string> files;
    getdir(groupPath, files);
    //    std::cout<<files.size()<<std::endl;
    db_feature.clear();

    std::ifstream ifs;
    for(auto f: files)
    {
        //        std::cout<<   f<<std::endl;
        std::string fname = groupPath + "/" + f;
        ifs.open(fname, std::ifstream::in | std::ifstream::binary);
        if(ifs.good())
        {
            ifs.seekg(0, ifs.end);
            int size = ifs.tellg();
            if(size%(sizeof(float)) != 0)
            {
                std::cout<<"Error: face feature file broken --"<<fname<<std::endl;
                continue;
            }
            else
            {
                std::vector<float> feature(size/sizeof(float));
                ifs.seekg (0, ifs.beg);
                ifs.read((char*)&feature[0], size);
                db_feature[f]=feature;
            }
        }
        else
        {
            std::cout<<"Error: cannot open face db feature file--"<<fname<<std::endl;
        }
        ifs.close();
    }
    return 0;
}



