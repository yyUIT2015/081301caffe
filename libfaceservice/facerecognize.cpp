#include "facerecognize.h"
#include "watchstop.h"



cv::Mat FaceRecognize::biasFace;
// compare score
bool compareCandidate(const IdentityCandidate & a, const IdentityCandidate & b) {
  return a.confidence > b.confidence;
}

FaceRecognize::FaceRecognize()//:Num_person(500),cnt(0)
{

}

//void FaceRecognize::init(const char* face_db_dir, const char* model_dir, const char* cascade_name)
//{
//    loadFaceDatabase(std::string(face_db_dir), std::string(model_dir));

//    if( !face_cascade.load( cascade_name ) )
//    { printf("--(!)Error loading\n");  };


//    // BIAS FACE
//    std::string file = std::string(model_dir) + std::string("/228.jpg");
//    biasFace = cv::imread(file.c_str());
//    biasFace.convertTo(biasFace,CV_32FC3);
//}

//void FaceRecognize::loadFaceDatabase(const std::string face_db_dir, const std::string model_dir)
//{

//    Caffe::set_mode(Caffe::CPU);

//    //Phase phase = TEST;
//    net_.reset(new Net<float>((model_dir+"/TEST__.prt"), TEST));
//    net_->CopyTrainedLayersFrom(model_dir+"/SEETA_.caffemodel");

//    // BIAS FACE
//    biasFace = cv::imread(model_dir+"/228.jpg");
//    biasFace.convertTo(biasFace,CV_32FC3);

//    DIR * dir;
//    struct dirent * ptr;
//    char file_list[Num_person][128];
//    dir = opendir(face_db_dir.c_str());

//    while((ptr = readdir(dir)) != NULL)
//    {
//        if(strcmp(ptr->d_name,".") != 0 && strcmp(ptr->d_name,"..") != 0)
//        {
//            strcpy(file_list[cnt],ptr->d_name );
//            cnt ++;
//        }
//    }
//    closedir(dir);

////    for(int kk=0;kk<DIM;kk++)//{
////        face_db_feature[0][kk]= 0;

//    cv::Mat IMG[Num_person];
//    char ImageName[128];
//    std::vector<std::string> names(cnt);
//    for(int i=0;i<cnt;i++)
//    {
//        std::string file = std::string(file_list[i]);
//        int idx = file.find_last_of(".");
//        std::string id_s = file.substr(0,idx);

//        names[i] = id_s;
//        sprintf(ImageName, "%s%s%s", face_db_dir.c_str(), "/", file_list[i]);
//        //std::cout << ImageName << id<< std::endl;
//        IMG[i] = cv::imread(ImageName);
//        IMG[i].convertTo(IMG[i],CV_32FC3);
//    }

//    std::vector<float> features(DIM);
//    if(cnt>0){
//        extract_feat_mul(IMG,cnt,net_, biasFace);
//        for(int i=0;i<cnt;i++)
//        {
//            for(int kk=0;kk<DIM;kk++)
//            {
//                features[kk]= net_->output_blobs()[0]->cpu_data()[kk+DIM*i];
//            }
//            std::string name = names[i];
//            face_db_feature[name] = features;
//        }
//    }
//}

void FaceRecognize::extract_feat_mul(cv::Mat* img_list, int NUM,
                                     Network_Ptr net_)
{  //multiple faces feature extract


  caffe::Caffe::set_mode(caffe::Caffe::CPU);
  std::vector<caffe::Datum> datum_vector;

  boost::shared_ptr<caffe::MemoryDataLayer<float> > mem_data_layer;
  mem_data_layer = boost::static_pointer_cast<caffe::MemoryDataLayer<float> >(net_->layers()[0]);

  for(int i=0;i<NUM;i++){

    img_list[i].convertTo(img_list[i],CV_32FC3);

    img_list[i]=(img_list[i]-127.5)*0.0078125;

    caffe::Datum datum;
    CvMatToDatumSignalChannel(img_list[i],&datum);
    datum_vector.push_back(datum);
  }
  mem_data_layer->set_batch_size(NUM);

  mem_data_layer->AddDatumVector(datum_vector);
  net_->Forward();

}

bool FaceRecognize::CvMatToDatumSignalChannel(const cv::Mat& cv_mat, caffe::Datum* datum){
  if (cv_mat.empty())
    return false;
  int channels = cv_mat.channels();

  datum->set_channels(cv_mat.channels());
  datum->set_height(cv_mat.rows);
  datum->set_width(cv_mat.cols);
  datum->set_label(0);
  datum->clear_data();
  datum->clear_float_data();
  datum->set_encoded(false);

  int datum_height = datum->height();
  int datum_width  = datum->width();
  if(channels == 3){
    for(int c = 0;c < channels;c++){
      for (int h = 0; h < datum_height; ++h){
        for (int w = 0; w < datum_width; ++w){
          const float* ptr = cv_mat.ptr<float>(h);
          datum->add_float_data(ptr[w*channels+c]);
        }
      }
    }
  }
  else{
      for (int h = 0; h < datum_height; ++h){
        for (int w = 0; w < datum_width; ++w){
          const float* ptr = cv_mat.ptr<float>(h);
          datum->add_float_data(ptr[w]);
      }
    }
  }

  return true;
}


void FaceRecognize::cos_sim(float * a, std::vector<float> &b, float &score)
{//feature sim
  score = 0.;
  float a_norm = 0.;
  float b_norm = 0.;
  for(int i=0;i<DIM;i++)
  {
    score += a[i]*b[i];
    a_norm += a[i]*a[i];
    b_norm += b[i]*b[i];
  }
  score = score/(sqrt(a_norm)*sqrt(b_norm));
}


//IdentifyResult FaceRecognize::faceIdentify(const char* imagePath, Landmark &lm)
void FaceRecognize::faceIdentify(IplImage *frame_, Landmark &lm, FaceDB_Feature &face_db_feature, Network_Ptr net_,IdentifyResult &result)
{
//    IdentifyResult result;

    WatchStop watch;
    watch.start();

//    //1.read image
//    IplImage *frame_ = cvLoadImage(imagePath,1);
    result.imageWidth=frame_->width;
    result.imageHeight=frame_->height;

    //2.align
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
    //--Not End

//    //3.filter side face

//    cv::Mat GRAY_;//
//    cv::Mat mmm(M,0);
//    cvtColor( mmm, GRAY_, CV_BGR2GRAY );
//    cv::Mat frame_gray;
//    cv::equalizeHist( GRAY_, frame_gray );
//    std::vector<cv::Rect> faces;
//    face_cascade.detectMultiScale(frame_gray, faces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, cv::Size(50, 50) );



//    //4.compute score
//    if(faces.size() > 0)
//    {//proper face exist
        //std::vector<float> scores(face_db_feature.size());
        cv::Mat aligned(M);
        //cv::Mat IMG_;
        //aligned(rect).copyTo(IMG_);
        extract_feat_mul(&aligned,1,net_);
        float feat_[DIM];//
        const boost::shared_ptr<caffe::Blob<float> > ft = net_->blob_by_name("fc1");//_bn");
        const float* ft_data = ft->cpu_data();
        for(int kk=0;kk<DIM;kk++)
            feat_[kk] = ft_data[kk];

        IdentityCandidate ic;
        //for(int kk=0;kk<face_db_feature.size();kk++)
        //for(auto itr : face_db_feature)
        for(std::map<std::string,std::vector<float> >::iterator itr=face_db_feature.begin(); itr !=face_db_feature.end(); itr++)
        {
            ic.personId = itr->first;
            cos_sim(feat_,itr->second,ic.confidence);
            result.candidates.push_back(ic);
        }

        result.candidates.sort(compareCandidate);
        std::list<IdentityCandidate>::iterator it1=result.candidates.begin();
        std::list<IdentityCandidate>::iterator it2=result.candidates.end();

        if(result.candidates.size()>5)
        {
            std::advance(it1,5);

            result.candidates.erase(it1,it2);
        }
        //std::sort(result.candidates.begin(), result.candidates.end(), compareCandidate);
        result.err = NoError;
//    }
//    else
//    {
//        result.err = FaceNotProperAngle;
//    }


//    result.url =imagePath;
    result.timeUsed_ms = watch.time_elapsed_ms();

//    return result;
}

void FaceRecognize::faceIdentify_withSideFaceFilter(IplImage *frame_, Landmark &lm,
                                     FaceDB_Feature &face_db_feature,
                                     Network_Ptr net_,
                                     cv::CascadeClassifier &face_cascade,
                                     IdentifyResult &result)
{

    //    IdentifyResult result;

        WatchStop watch;
        watch.start();

    //    //1.read image
    //    IplImage *frame_ = cvLoadImage(imagePath,1);
    //    result.imageWidth=frame_->width;
    //    result.imageHeight=frame_->height;

        //2.align
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
        //--Not End

    //    //3.filter side face

        cv::Mat GRAY_;//
        cv::Mat mmm(M,0);
        cvtColor( mmm, GRAY_, CV_BGR2GRAY );
        cv::Mat frame_gray;
        cv::equalizeHist( GRAY_, frame_gray );
        std::vector<cv::Rect> faces;
        face_cascade.detectMultiScale(frame_gray, faces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, cv::Size(50, 50) );



    //    //4.compute score
     //   std::cout<<"\n\n$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n\n"<<std::endl;
     //   std::cout<<"\n\n$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n\n"<<std::endl;
     //   std::cout<<faces.size()<<std::endl;
     //   std::cout<<"\n\n$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n\n"<<std::endl;
     //   std::cout<<"\n\n$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n\n"<<std::endl;

        if(faces.size() > 0)
        {//proper face exist
            std::vector<float> scores(face_db_feature.size());
            cv::Mat aligned(M);

            extract_feat_mul(&aligned,1,net_);
            float feat_[DIM];//
            const boost::shared_ptr<caffe::Blob<float> > ft = net_->blob_by_name("fc1");//_bn");
            const float* ft_data = ft->cpu_data();
            for(int kk=0;kk<DIM;kk++)
                feat_[kk] = ft_data[kk];

            IdentityCandidate ic;
            //for(int kk=0;kk<face_db_feature.size();kk++)
            //for(auto itr : face_db_feature)
            for(std::map<std::string,std::vector<float> >::iterator itr=face_db_feature.begin(); itr !=face_db_feature.end(); itr++)
            {
                ic.personId = itr->first;
                cos_sim(feat_,itr->second,ic.confidence);
                result.candidates.push_back(ic);
            }

            result.candidates.sort(compareCandidate);
            std::list<IdentityCandidate>::iterator it1=result.candidates.begin();
            std::list<IdentityCandidate>::iterator it2=result.candidates.end();

            if(result.candidates.size()>5)
            {
                std::advance(it1,5);

                result.candidates.erase(it1,it2);
            }
            //std::sort(result.candidates.begin(), result.candidates.end(), compareCandidate);
            result.err = NoError;
        }
        else
        {
            result.err = FaceNotProperAngle;
        }


    //    result.url =imagePath;
        result.timeUsed_ms = watch.time_elapsed_ms();


}

void FaceAlign::align(IplImage * im, float* feat_points, IplImage * out, int side_)
{//矫正
  int Side = 112;
  int Side1 = side_;
  int Side2 = side_;
  const float std_points[10] = {38.2946, 51.6963, 73.5318, 51.5014, 56.0252, 71.7366, 41.5493, 92.3655, 70.7299, 92.2041}; //Side = 256
  //const float std_points[10] = {89.3095, 72.9025, 169.3095, 72.9025, 127.8949, 127.0441, 96.8796, 184.8907, 159.1065, 184.7601}; //Side = 256
  int points_num_ = 5;
  float sum_x = 0, sum_y = 0;
  float sum_u = 0, sum_v = 0;
  float sum_xx_yy = 0;
  float sum_ux_vy = 0;
  float sum_vx__uy = 0;
  for (int c = 0; c < points_num_; ++ c) {
    int x_off = c * 2;//points_num_ * 2 + c * 2;
    int y_off = x_off + 1;
    sum_x += std_points[c * 2];
    sum_y += std_points[c * 2 + 1];
    sum_u += feat_points[x_off];
    sum_v += feat_points[y_off];
    sum_xx_yy += std_points[c * 2] * std_points[c * 2] +
                 std_points[c * 2 + 1] * std_points[c * 2 + 1];
    sum_ux_vy += std_points[c * 2] * feat_points[x_off] +
                 std_points[c * 2 + 1] * feat_points[y_off];
    sum_vx__uy += feat_points[y_off] * std_points[c * 2] -
                  feat_points[x_off] * std_points[c * 2 + 1];
  }
  float q = sum_u - sum_x * sum_ux_vy / sum_xx_yy
             + sum_y * sum_vx__uy / sum_xx_yy;
  float p = sum_v - sum_y * sum_ux_vy / sum_xx_yy
             - sum_x * sum_vx__uy / sum_xx_yy;

  float r = points_num_ - (sum_x * sum_x + sum_y * sum_y) / sum_xx_yy;

  float a = (sum_ux_vy - sum_x * q / r - sum_y * p / r) / sum_xx_yy;

  float b = (sum_vx__uy + sum_y * q / r - sum_x * p / r) / sum_xx_yy;

  float c = q / r;

  float d = p / r;
  int THETA_SIZE = 6;
  float theta_data[THETA_SIZE];
  theta_data[0] = theta_data[4] = a;
  theta_data[1] = -b;
  theta_data[3] = b;
  theta_data[2] = c;
  theta_data[5] = d;

  int H=im->height;
  int W=im->width;

  float src_x = 0.;
  float src_y = 0.;
  int ux = 0;
  int uy = 0;
  float ans =0.;
  float cof_x =0.;
  float cof_y =0.;

  uchar *ptr = (uchar*)im->imageData;
  uchar *out_data = (uchar*)out->imageData;
  for(int x = 0;x < Side2; x++)
  {
      for (int y = 0; y < Side1; y++)
      {
          src_y = theta_data[0] * (y+(Side-Side1)/2) + theta_data[1] * (x+(Side-Side2)/2) + theta_data[2];
          src_x = theta_data[3] * (y+(Side-Side1)/2) + theta_data[4] * (x+(Side-Side2)/2) + theta_data[5];

          for(int c = 0; c < 3; c++)
          {
              ux = floor(src_x);
              uy = floor(src_y);
              ans = 0.;
              if ((ux >= 0) && (ux < H - 1) && (uy >= 0) && (uy < W - 1))
              {
                  cof_x = src_x - ux;
                  cof_y = src_y - uy;
                  ans = (1 - cof_y) * float(ptr[ux * im->widthStep + uy * 3 + c]) + cof_y * float(ptr[ux * im->widthStep + (uy + 1) * 3 + c]);
                  ans = (1 - cof_x) * ans + cof_x * ((1 - cof_y) * float(ptr[(ux + 1) * im->widthStep + uy * 3 + c]) + cof_y * float(ptr[(ux + 1) * im->widthStep + (uy + 1) * 3 + c]));
              }
              unsigned int val = ans;
              out_data[x * out->widthStep + y * 3 + c] = val;
          }
      }
  }
//  }
}

//void FaceRecognize::release()
//{
//    net_.reset();
//    face_db_feature.clear();
//}


