#ifndef DEFINE
#define DEFINE




#include <math.h>
#include <vector>
#include <iostream>
#include <stdio.h>
#include <list>

//using json = nlohmann::json;
typedef struct FaceRect {
  float x1;
  float y1;
  float x2;
  float y2;
  float score; /**< Larger score should mean higher confidence. */
} FaceRect;


struct Coordinates
{
    int x;
    int y;

};
//void to_json(json &j, const Coordinates &coord);
//void from_json(const json &j, Coordinates &coord);

struct Landmark
{
    Coordinates eyeLeft;
    Coordinates eyeRight;
    Coordinates nose;
    Coordinates mouthLeft;
    Coordinates mouthRight;

};
//void to_json(json &j, const Landmark &lm);
//void from_json(const json &j, Landmark &fb);

struct FaceBox
{
    int left;
    int top;
    int right; //
    int bottom; //
    int width; // face width
    int height; // face height
    float prob; // probability of face

   FaceBox(){}
    FaceBox(int l, int t, int r, int b):left(l), top(t), right(r), bottom(b)
    {
        width = right-left;
        height = bottom-top;
    }
    void computeWH()
    {

        width = right-left;
        height = bottom-top;
    }
};
//void to_json(json &j, const FaceBox &fb);


struct Face
{
    FaceBox faceBox;
    Landmark landmark;
};
//void to_json(json &j, const Face &f);


struct IdentityCandidate
{
    std::string personId;
    float confidence;
};
//void to_json(json &j, const IdentityCandidate &can);

enum ErrorCode
{
    NoError=0,
    NetworkInternalErr=-1,
    ImageNotExist=-2,
    ImageFormatErr=-3,
    ImageSizeErr=-4,
    ImageBroken=-5,
    ImageSizeSmall=-6,
    FaceNotProperAngle=-7,
    FaceGroupNotExist=-8,
    FaceGroupAlreadyExist=-9,
    FaceDatabaseDirNotExist=-10,
    CannotCreateFaceGroupDir=-11,
    OtherErr=-12,
    UnabletoapplyforFRresources=-13,
    UnabletoapplyforFDresources=-14,
    FaceserviceNotinit=-15
};

struct DetectResult
{
    int imageWidth;
    int imageHeight;
    long timeUsed_ms;
    ErrorCode err;
    std::string url;
    std::list<Face> faces;
};

struct IdentifyResult
{
    int imageWidth;
    int imageHeight;
    int timeUsed_ms;
    std::string url;
    ErrorCode err;
    std::list<IdentityCandidate> candidates;
};

struct FailReason
{
    std::string url;
    std::string reason;
};

struct AddgroupResult
{
    int added;
    int count;
//    std::string Group_id;
//    std::string Face_id;
//    std::string Image_url;
    int timeUsed;
    ErrorCode err;
    std::list<FailReason> failreasons;
};

#define DIM 512
//#define DIM 2048
#endif // DEFINE

