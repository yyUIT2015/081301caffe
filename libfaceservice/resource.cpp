#include "resource.h"
#include "facerecognize.h"
#include <iostream>
Resource::Resource()
{

}

void Resource::setFDnetmax(int value)
{
    FDnetmax = value;
}

void Resource::setFRnetmax(int value)
{
    FRnetmax = value;
}

FD_Networks Resource::getFaceDetectNetwork()
{
    FD_Networks net;
    fd_mutex.lock();

    if(list_fd_idle.size()<=0)
    {
        if(list_fd_busy.size()== FDnetmax ){
            std::cout<<"\nlist_fd_idle  MAX\n"<<std::endl;
            fd_mutex.unlock();
            return net;
        }

        loadNetworkModels(model_path, net);
        list_fd_busy.push_back(net);
    }
    else
    {
        net = list_fd_idle.front();
        list_fd_idle.pop_front();
        list_fd_busy.push_back(net);
    }

    fd_mutex.unlock();
   return net;
}

void Resource::collectFaceDetectNetwork(FD_Networks model)
{
    fd_mutex.lock();
    list_fd_busy.remove(model);
    list_fd_idle.push_back(model);
    fd_mutex.unlock();
}

void Resource::loadNetworkModels(const std::string proto_model_dir,FD_Networks & fd_mods)
{

#ifdef CPU_ONLY
    caffe::Caffe::set_mode(caffe::Caffe::CPU);
#else
    Caffe::set_mode(Caffe::GPU);
#endif
    /* Load the network. */
    fd_mods.PNet.reset(new caffe::Net<float>((proto_model_dir+"/db_sq_deploy.prototxt"), caffe::TEST));
    fd_mods.PNet->CopyTrainedLayersFrom(proto_model_dir+"/db_sq.caffemodel");
    //printf("--------------------------------------------PASS1!\n");
    //CHECK_EQ(fd_mods.PNet->num_inputs(), 1) << "Network should have exactly one input.";
    //CHECK_EQ(fd_mods.PNet->num_outputs(),2) << "Network should have exactly two output, one"
    //                                   " is bbox and another is confidence.";

#ifdef CPU_ONLY
    fd_mods.RNet.reset(new caffe::Net<float>((proto_model_dir+"/det2.prototxt"), caffe::TEST));
#else
    fd_mods.RNet.reset(new Net<float>((proto_model_dir+"/det2_input.prototxt"), caffe::TEST));
#endif
    fd_mods.RNet->CopyTrainedLayersFrom(proto_model_dir+"/det2.caffemodel");
    //printf("--------------------------------------------PASS2!\n");
    //  CHECK_EQ(fd_mods.RNet->num_inputs(), 0) << "Network should have exactly one input.";
    //  CHECK_EQ(fd_mods.RNet->num_outputs(),3) << "Network should have exactly two output, one"
    //                                     " is bbox and another is confidence.";

#ifdef CPU_ONLY
    fd_mods.ONet.reset(new caffe::Net<float>((proto_model_dir+"/det3.prototxt"), caffe::TEST));
#else
    fd_mods.ONet.reset(new caffe::Net<float>((proto_model_dir+"/det3_input.prototxt"), caffe::TEST));
#endif
    fd_mods.ONet->CopyTrainedLayersFrom(proto_model_dir+"/det3.caffemodel");
    //printf("--------------------------------------------PASS3!\n");
    //  CHECK_EQ(fd_mods.ONet->num_inputs(), 1) << "Network should have exactly one input.";
    //  CHECK_EQ(fd_mods.ONet->num_outputs(),3) << "Network should have exactly three output, one"
    //                                     " is bbox and another is confidence.";
//    std::cout<<"_____________"<<std::endl;
    caffe::Blob<float>* input_layer;
    input_layer = fd_mods.PNet->input_blobs()[0];
    fd_mods.num_channels = input_layer->channels();
    //CHECK(num_channels_ == 3 || num_channels_ == 1) << "Input layer should have 1 or 3 channels.";
}


Network_Ptr Resource::getFaceRecognizeNetwork()
{
    fr_mutex.lock();
    Network_Ptr net;
    if(list_fr_idle.size()<=0)
    {
        if(list_fr_busy.size()>= FRnetmax)
        {
            std::cout<<"\nlist_fd_idle  MAX\n"<<std::endl;
            fr_mutex.unlock();
            return NULL;

        }

        loadNetworkModels(model_path, net);
        list_fr_busy.push_back(net);
    }
    else
    {
        net = list_fr_idle.front();
        list_fr_idle.pop_front();
        list_fr_busy.push_back(net);
    }
    fr_mutex.unlock();
    return net;


}

void Resource::collectFaceRecognizeNetwork(Network_Ptr net)
{
    fr_mutex.lock();
    list_fr_busy.remove(net);
    list_fr_idle.push_back(net);
    fr_mutex.unlock();
}

void Resource::loadNetworkModels(const std::string model_dir, Network_Ptr &net)
{

    caffe::Caffe::set_mode(caffe::Caffe::CPU);
    //    fr_mutex.lock();
    //Phase phase = TEST;
    //net.reset(new Net<float>((model_dir+"/TEST__.prt"), TEST));
    //net->CopyTrainedLayersFrom(model_dir+"/SEETA_.caffemodel");
    //   std::cout<<"_____________"<<std::endl;
    net.reset(new caffe::Net<float>((model_dir+"/IMP.prototxt"), caffe::TEST));
    //   std::cout<<"_____________"<<std::endl;
    net->CopyTrainedLayersFrom(model_dir+"/IMP.caffemodel");
    //    fr_mutex.unlock();
    //    boost::shared_ptr<MemoryDataLayer<float> > mem_data_layer;
    //    mem_data_layer = boost::static_pointer_cast<MemoryDataLayer<float> >(net_->layers()[0]);
}


cv::Mat Resource::getBiasFace() const
{
    return BiasFace;
}

void Resource::init(const std::string model_dir)
{

    model_path = model_dir;

    BiasFace = cv::imread(model_dir+"/228.jpg");
    BiasFace.convertTo(BiasFace,CV_32FC3);


    if( !face_cascade.load( model_dir+ "/haarcascade_frontalface_alt.xml") )
    {
        printf("--(!)Error loading\n");
    };
    //    fr_mutex1.unlock();
    {
        FD_Networks net;
        loadNetworkModels(model_path, net);
        list_fd_idle.push_back(net);
    }
    {
        Network_Ptr net;
        loadNetworkModels(model_path, net);
        list_fr_idle.push_back(net);

    }
}

int Resource::addFaceDBGroup(std::string groupId, FaceDB_Feature &db_feature)
{
    if(face_db_group.find(groupId) != face_db_group.end())
    {
        face_db_group[groupId] = db_feature;
        return 0;
    }
    else
        return -1;
}

cv::CascadeClassifier Resource::getFace_cascade() const
{
    return face_cascade;
}
void Resource::release()
{

}

