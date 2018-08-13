//#include "facedetect.h"
#include "facedetect.h"
#include "watchstop.h"



// compare score
bool CompareBBox(const FaceInfos & a, const FaceInfos & b) {
  return a.bbox.score > b.bbox.score;
}


FaceDetect::FaceDetect()
{
    //std::cout<<"this is face detect"<<std::endl;
}



//DetectResult FaceDetect::detectFace(const char* image_path, FD_Networks &fd_nets)
void FaceDetect::detectFace(cv::Mat &img, FD_Networks &fd_nets,DetectResult &result,double Factor)
{

    WatchStop watch;
    watch.start();

    //DetectResult result;


    //read image
//    IplImage *frame_ = cvLoadImage(image_path,1);
//    //IplImage *frame = cvCreateImage(cvSize(228,228),frame_->depth,frame_->nChannels);
//    //cvResize(frame_,frame,CV_INTER_CUBIC);
//    cv::Mat image1(frame_); // = cv::imread(imageName);
//    cv::Mat image=image1.clone(); // = cv::imread(imageName);


    double threshold[3] = {0.6,0.7,0.7};
    double factor = 0.709;
//    std::cout<<"factor    "<<factor<<std::endl;
    int minSize = 50; //40;
    std::vector<FaceInfos> faceInfo;//

    Detect(img,faceInfo,minSize,threshold,factor,fd_nets);

//    result.imageWidth = frame_->width;
//    result.imageHeight=frame_->height;

    //key points
    //int face_=0;
    for(int i=0;i<faceInfo.size();i++)
    {
        FaceRect faceRect=faceInfo[i].bbox;

        Face face;
        face.faceBox.left=faceRect.x1;
        face.faceBox.top=faceRect.y1;
        face.faceBox.right=faceRect.x2;
        face.faceBox.bottom=faceRect.y2;
        face.faceBox.computeWH();
        face.faceBox.prob = faceRect.score;

        FacePts facePts = faceInfo[i].facePts;
        face.landmark.eyeLeft.y=facePts.x[0];
        face.landmark.eyeLeft.x=facePts.y[0];
        face.landmark.eyeRight.y=facePts.x[1];
        face.landmark.eyeRight.x=facePts.y[1];

        face.landmark.nose.y=facePts.x[2];
        face.landmark.nose.x=facePts.y[2];

        face.landmark.mouthLeft.y=facePts.x[3];
        face.landmark.mouthLeft.x=facePts.y[3];
        face.landmark.mouthRight.y=facePts.x[4];
        face.landmark.mouthRight.x=facePts.y[4];

        result.faces.push_back(face);
    }


    //result.url=image_path;
    result.timeUsed_ms = watch.time_elapsed_ms();
    result.err=NoError;
    //return result;
}


void FaceDetect::Detect(const cv::Mat& image,std::vector<FaceInfos>& faceInfo,int minSize,double* threshold,double factor, FD_Networks &fd_nets){


    std::vector<FaceInfos> condidate_rects_;
    std::vector<FaceInfos> total_boxes_;
    std::vector<FaceInfos> regressed_rects_;
    std::vector<FaceInfos> regressed_pading_;
    // 2~3ms
    // invert to RGB color space and float type
    cv::Mat sample_single,resized;
    image.convertTo(sample_single,CV_32FC3);
    cv::cvtColor(sample_single,sample_single,cv::COLOR_BGR2RGB);
    //sample_single = sample_single.t();

    int height = image.rows;

    int width  = image.cols;
     // std::cout <<"height: "<< width << std::endl;
    int minWH = std::min(height,width);
    int factor_count = 1;
    double m = 12./minSize;
    minWH *= m;
    std::vector<double> scales;
    while (minWH >= 24)
    {
      scales.push_back(m * std::pow(factor,factor_count));
      minWH *= factor;
      ++factor_count;
    }
    factor_count = 1;
    // 11ms main consum
    caffe::Blob<float>* input_layer = fd_nets.PNet->input_blobs()[0];
    for(int i=0;i<factor_count;i++)
    {
      //double scale = scales[i];
      int ws = 448;//std::ceil(height*scale);
      int hs = 448;//std::ceil(width*scale);

      // wrap image and normalization using INTER_AREA method
      cv::resize(sample_single,resized,cv::Size(ws,hs));//,0,0,cv::INTER_AREA);

      //resized.convertTo(resized, CV_32FC3, 0.0078125,-127.5*0.0078125);
      resized.convertTo(resized, CV_32FC3, 0.0078125/2,0.);

      // input data
      input_layer->Reshape(1, 3, hs, ws);
      fd_nets.PNet->Reshape();
      std::vector<cv::Mat> input_channels;
      WrapInputLayer(&input_channels,fd_nets.PNet->input_blobs()[0],hs,ws);
      cv::split(resized,input_channels);

      // check data transform right
      //CHECK(reinterpret_cast<float*>(input_channels.at(0).data) == fd_nets.PNet->input_blobs()[0]->cpu_data())
      //    << "Input channels are not wrapping the input layer of the network.";
      fd_nets.PNet->Forward();

      caffe::Blob<float>* output = fd_nets.PNet->output_blobs()[0];/*
      std::cout <<"3: "<< output->shape(3) << std::endl;*/
      //int n_coor_box = 4;
      //int num_classes = 1;
      int num_boxes = 6;
      int out_h= 14;
      int out_w= 14;
      const int totals = (out_h* out_w)*num_boxes;
      float probs[totals];
      float thresh = 0.4;
      int c= 0;
      float anchors_w[6]={0.42066536, 0.67490536, 1.19909683, 2.21314398, 3.84516601,6.7461985};
      float anchors_h[6]={0.63799133, 1.05267263, 1.67870809, 2.73088404, 4.24705933,6.7461985};
      condidate_rects_.clear();
    for(int j = 0;j<out_h;j++){
      for(int i=0;i<out_w;i++){
          for(int k=0;k<num_boxes;k++){
              probs[c]=1.0 / (1.0 + exp(-output->cpu_data()[6*k*out_w* out_h + 4*out_w* out_h + j*out_w +i]) );
              if(probs[c]>thresh){
                  float xTop = 1.0 / (1.0 + exp(-output->cpu_data()[6*k*out_w* out_h + 1*out_w* out_h + j*out_w +i])) /out_w * height+ (j-0.)/out_w * height;
                  float yTop = 1.0 / (1.0 + exp(-output->cpu_data()[6*k*out_w* out_h + 0*out_w* out_h + j*out_w +i])) /out_h * width + (i-0.)/out_w * width;
                  float xBot = exp(output->cpu_data()[6*k*out_w* out_h + 3*out_w* out_h + j*out_w +i]) * anchors_h[k]/out_h * height;
                  float yBot = exp(output->cpu_data()[6*k*out_w* out_h + 2*out_w* out_h + j*out_w +i]) * anchors_w[k]/out_w * width;

                      FaceRect faceRect;
                      faceRect.x1 = xTop-xBot/2.;
                      faceRect.y1 = yTop-yBot/2.;
                      faceRect.x2 = xTop+xBot/2.;
                      faceRect.y2 = yTop+yBot/2.;
                      faceRect.score  = probs[c];
                      FaceInfos faceInfo_;
                      faceInfo_.bbox = faceRect;
                      condidate_rects_.push_back(faceInfo_);
              }
              c=c+1;
          }
      }
      }
      std::vector<FaceInfos> bboxes_nms = NonMaximumSuppression(condidate_rects_,0.5,'u');
      total_boxes_.insert(total_boxes_.end(),bboxes_nms.begin(),bboxes_nms.end());
    }

    int numBox = total_boxes_.size();
    if(numBox != 0){
      regressed_rects_ = NonMaximumSuppression(total_boxes_,0.7,'u');

      total_boxes_.clear();
      Bbox2Square(regressed_rects_);
      Padding(width,height,regressed_rects_,regressed_pading_);

      sample_single = sample_single.t();
      /// Second stage
  /**/
      #ifdef CPU_ONLY
      ClassifyFace(regressed_rects_,sample_single,fd_nets.RNet,threshold[1],'r',condidate_rects_,regressed_pading_);
      #else
      ClassifyFace_MulImage(regressed_rects_,sample_single,fd_nets.RNet,threshold[1],'r');
      #endif
      condidate_rects_ = NonMaximumSuppression(condidate_rects_,0.7,'u');
      regressed_rects_ = BoxRegress(condidate_rects_,2);

      Bbox2Square(regressed_rects_);
      Padding(width,height,regressed_rects_,regressed_pading_);

      /// three stage
      numBox = regressed_rects_.size();
      if(numBox != 0){
        #ifdef CPU_ONLY
        ClassifyFace(regressed_rects_,sample_single,fd_nets.ONet,threshold[2],'o',condidate_rects_,regressed_pading_);
        #else
        ClassifyFace_MulImage(regressed_rects_,sample_single,fd_nets.ONet,threshold[2],'o');
        #endif
        regressed_rects_ = BoxRegress(condidate_rects_,3);
        faceInfo = NonMaximumSuppression(regressed_rects_,0.7,'m');
      }
    }
    regressed_pading_.clear();
    regressed_rects_.clear();
    condidate_rects_.clear();

}

void FaceDetect::WrapInputLayer(std::vector<cv::Mat>* input_channels,
        caffe::Blob<float>* input_layer, const int height, const int width) {
  float* input_data = input_layer->mutable_cpu_data();
  for (int i = 0; i < input_layer->channels(); ++i) {
    //cv::Mat channel(height, width, CV_32FC3, input_data);
    cv::Mat channel(height, width, CV_32FC1, input_data);
    input_channels->push_back(channel);
    input_data += width * height;
  }
}


// methodType : u is IoU(Intersection Over Union)
// methodType : m is IoM(Intersection Over Maximum)
  std::vector<FaceInfos> FaceDetect::NonMaximumSuppression(std::vector<FaceInfos>& bboxes,
                float thresh,char methodType)
  {
  std::vector<FaceInfos> bboxes_nms;
  std::sort(bboxes.begin(), bboxes.end(), CompareBBox);

  int32_t select_idx = 0;
  int32_t num_bbox = static_cast<int32_t>(bboxes.size());
  std::vector<int32_t> mask_merged(num_bbox, 0);
  bool all_merged = false;

  while (!all_merged) {
    while (select_idx < num_bbox && mask_merged[select_idx] == 1)
      select_idx++;
    if (select_idx == num_bbox) {
      all_merged = true;
      continue;
    }

    bboxes_nms.push_back(bboxes[select_idx]);
    mask_merged[select_idx] = 1;

    FaceRect select_bbox = bboxes[select_idx].bbox;
    float area1 = static_cast<float>((select_bbox.x2-select_bbox.x1+1) * (select_bbox.y2-select_bbox.y1+1));
    float x1 = static_cast<float>(select_bbox.x1);
    float y1 = static_cast<float>(select_bbox.y1);
    float x2 = static_cast<float>(select_bbox.x2);
    float y2 = static_cast<float>(select_bbox.y2);

    select_idx++;
    for (int32_t i = select_idx; i < num_bbox; i++) {
      if (mask_merged[i] == 1)
        continue;

      FaceRect& bbox_i = bboxes[i].bbox;
      float x = std::max<float>(x1, static_cast<float>(bbox_i.x1));
      float y = std::max<float>(y1, static_cast<float>(bbox_i.y1));
      float w = std::min<float>(x2, static_cast<float>(bbox_i.x2)) - x + 1;
      float h = std::min<float>(y2, static_cast<float>(bbox_i.y2)) - y + 1;
      if (w <= 0 || h <= 0)
        continue;

      float area2 = static_cast<float>((bbox_i.x2-bbox_i.x1+1) * (bbox_i.y2-bbox_i.y1+1));
      float area_intersect = w * h;

      switch (methodType) {
        case 'u':
          if (static_cast<float>(area_intersect) / (area1 + area2 - area_intersect) > thresh)
            mask_merged[i] = 1;
          break;
        case 'm':
          if (static_cast<float>(area_intersect) / std::min(area1 , area2) > thresh)
            mask_merged[i] = 1;
          break;
        default:
          break;
        }
    }
  }
  return bboxes_nms;
}


void FaceDetect::Bbox2Square(std::vector<FaceInfos>& bboxes){
for(int i=0;i<bboxes.size();i++){
  float h = bboxes[i].bbox.x2 - bboxes[i].bbox.x1;
  float w = bboxes[i].bbox.y2 - bboxes[i].bbox.y1;
  float side = h>w ? h:w;
  bboxes[i].bbox.x1 += (h-side)*0.5;
  bboxes[i].bbox.y1 += (w-side)*0.5;

  bboxes[i].bbox.x2 = (int)(bboxes[i].bbox.x1 + side);
  bboxes[i].bbox.y2 = (int)(bboxes[i].bbox.y1 + side);
  bboxes[i].bbox.x1 = (int)(bboxes[i].bbox.x1);
  bboxes[i].bbox.y1 = (int)(bboxes[i].bbox.y1);

}
}


// compute the padding coordinates (pad the bounding boxes to square)
void FaceDetect::Padding(int img_w,int img_h,std::vector<FaceInfos> &regressed_rects_,std::vector<FaceInfos> &regressed_pading_){
for(int i=0;i<regressed_rects_.size();i++){
  FaceInfos tempFaceInfo;
  tempFaceInfo = regressed_rects_[i];
  tempFaceInfo.bbox.y2 = (regressed_rects_[i].bbox.y2 >= img_w) ? img_w : regressed_rects_[i].bbox.y2;
  tempFaceInfo.bbox.x2 = (regressed_rects_[i].bbox.x2 >= img_h) ? img_h : regressed_rects_[i].bbox.x2;
  tempFaceInfo.bbox.y1 = (regressed_rects_[i].bbox.y1 <1) ? 1 : regressed_rects_[i].bbox.y1;
  tempFaceInfo.bbox.x1 = (regressed_rects_[i].bbox.x1 <1) ? 1 : regressed_rects_[i].bbox.x1;
  regressed_pading_.push_back(tempFaceInfo);
}
}


void FaceDetect::ClassifyFace(const std::vector<FaceInfos>& regressed_rects,cv::Mat &sample_single,
        boost::shared_ptr<caffe::Net<float> >& net,double thresh,char netName,std::vector<FaceInfos> &condidate_rects_,std::vector<FaceInfos> &regressed_pading_){
  int numBox = regressed_rects.size();
  caffe::Blob<float>* crop_input_layer = net->input_blobs()[0];
  int input_channels = crop_input_layer->channels();
  int input_width  = crop_input_layer->width();
  int input_height = crop_input_layer->height();
  crop_input_layer->Reshape(1, input_channels, input_width, input_height);
  net->Reshape();

  condidate_rects_.clear();
  // load crop_img data to datum
  for(int i=0;i<numBox;i++){
    std::vector<cv::Mat> channels;
    WrapInputLayer(&channels,net->input_blobs()[0],input_width,input_height);
    int pad_top   = std::abs(regressed_pading_[i].bbox.x1 - regressed_rects[i].bbox.x1);
    int pad_left  = std::abs(regressed_pading_[i].bbox.y1 - regressed_rects[i].bbox.y1);
    int pad_right = std::abs(regressed_pading_[i].bbox.y2 - regressed_rects[i].bbox.y2);
    int pad_bottom= std::abs(regressed_pading_[i].bbox.x2 - regressed_rects[i].bbox.x2);

    cv::Mat crop_img = sample_single(cv::Range(regressed_pading_[i].bbox.y1-1,regressed_pading_[i].bbox.y2),
                         cv::Range(regressed_pading_[i].bbox.x1-1,regressed_pading_[i].bbox.x2));
    cv::copyMakeBorder(crop_img,crop_img,pad_left,pad_right,pad_top,pad_bottom,cv::BORDER_CONSTANT,cv::Scalar(0));
#ifdef INTER_FAST
    cv::resize(crop_img,crop_img,cv::Size(input_width,input_height),0,0,cv::INTER_NEAREST);
#else
    cv::resize(crop_img,crop_img,cv::Size(input_width,input_height),0,0,cv::INTER_AREA);
#endif
    crop_img = (crop_img-127.5)*0.0078125;
    cv::split(crop_img,channels);

    //CHECK(reinterpret_cast<float*>(channels.at(0).data) == net->input_blobs()[0]->cpu_data())
    //      << "Input channels are not wrapping the input layer of the network.";
    net->Forward();

    int reg_id = 0;
    int confidence_id = 1;
    if(netName == 'o') confidence_id = 2;
    const caffe::Blob<float>* reg = net->output_blobs()[reg_id];
    const caffe::Blob<float>* confidence = net->output_blobs()[confidence_id];
    // ONet points_offset != NULL
    const caffe::Blob<float>* points_offset = net->output_blobs()[1];

    const float* confidence_data = confidence->cpu_data() + confidence->count()/2;
    const float* reg_data = reg->cpu_data();
    const float* points_data;
    if(netName == 'o') points_data = points_offset->cpu_data();

    if(*(confidence_data) > thresh){
      FaceRect faceRect;
      faceRect.x1 = regressed_rects[i].bbox.x1;
      faceRect.y1 = regressed_rects[i].bbox.y1;
      faceRect.x2 = regressed_rects[i].bbox.x2;
      faceRect.y2 = regressed_rects[i].bbox.y2 ;
      faceRect.score  = *(confidence_data);
      FaceInfos faceInfo;
      faceInfo.bbox = faceRect;
      faceInfo.regression = cv::Vec4f(reg_data[0],reg_data[1],reg_data[2],reg_data[3]);

      // x x x x x y y y y y
      if(netName == 'o'){
        FacePts face_pts;
        float w = faceRect.y2 - faceRect.y1 + 1;
        float h = faceRect.x2 - faceRect.x1 + 1;
        for(int j=0;j<5;j++){
          face_pts.y[j] = faceRect.y1 + *(points_data+j) * h - 1;
          face_pts.x[j] = faceRect.x1 + *(points_data+j+5) * w -1;
        }
        faceInfo.facePts = face_pts;
      }
      condidate_rects_.push_back(faceInfo);
    }
  }
  regressed_pading_.clear();
}

std::vector<FaceInfos> FaceDetect::BoxRegress(std::vector<FaceInfos>& faceInfo,int stage){
  std::vector<FaceInfos> bboxes;
  for(int bboxId =0;bboxId<faceInfo.size();bboxId++){
      FaceRect faceRect;
      FaceInfos tempFaceInfo;
      float regw = faceInfo[bboxId].bbox.y2 - faceInfo[bboxId].bbox.y1;
      regw += (stage == 1)? 0:1;
      float regh = faceInfo[bboxId].bbox.x2 - faceInfo[bboxId].bbox.x1;
      regh += (stage == 1)? 0:1;
      faceRect.y1 = faceInfo[bboxId].bbox.y1 + regw * faceInfo[bboxId].regression[0];
      faceRect.x1 = faceInfo[bboxId].bbox.x1 + regh * faceInfo[bboxId].regression[1];
      faceRect.y2 = faceInfo[bboxId].bbox.y2 + regw * faceInfo[bboxId].regression[2];
      faceRect.x2 = faceInfo[bboxId].bbox.x2 + regh * faceInfo[bboxId].regression[3];
      faceRect.score = faceInfo[bboxId].bbox.score;

      tempFaceInfo.bbox = faceRect;
      tempFaceInfo.regression = faceInfo[bboxId].regression;
      if(stage == 3)
        tempFaceInfo.facePts = faceInfo[bboxId].facePts;
      bboxes.push_back(tempFaceInfo);
  }
  return bboxes;
}





