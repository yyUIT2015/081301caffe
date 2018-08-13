#include "define.h"


//void to_json(json &j, const Coordinates &coord)
//{

//    j["x"]=coord.x;
//    j["y"]=coord.y;
//}


//void to_json(json &j, const Landmark &lm)
//{
//    j["eyeLeft"] = lm.eyeLeft;
//    j["eyeRight"] = lm.eyeRight;
//    j["nose"] = lm.nose;
//    j["mouthLeft"] = lm.mouthLeft;
//    j["mouthRight"] = lm.mouthRight;
//}

//void to_json(json &j, const FaceBox &fb)
//{

//    j["left"] = fb.left;
//    j["top"] = fb.top;
//    j["right"] = fb.right;
//    j["bottom"] = fb.bottom;
//    j["width"] = fb.width;
//    j["height"] = fb.height;
//    j["prob"] = fb.prob;
//}

//void to_json(json &j, const Face &f)
//{
//    j["faceBox"]=f.faceBox;
//    j["landmark"]=f.landmark;
//}

//void to_json(json &j, const IdentityCandidate &ic)
//{
//    j["personId"]=ic.personId;
//    j["confidence"]=ic.confidence;
//}

//void from_json(const json &j, Coordinates &coord)
//{
//    coord.x = j.at("x").get<int>();
//    coord.y = j.at("y").get<int>();
//}

//void from_json(const json &j, Landmark &fb)
//{
//    fb.eyeLeft = j.at("eyeLeft").get<Coordinates>();
//    fb.eyeRight = j.at("eyeRight").get<Coordinates>();
//    fb.nose = j.at("nose").get<Coordinates>();
//    fb.mouthLeft = j.at("mouthLeft").get<Coordinates>();
//    fb.mouthRight = j.at("mouthRight").get<Coordinates>();

//    from_json(j.at("eyeLeft"),fb.eyeLeft);
//    from_json(j.at("eyeRight"),fb.eyeRight);
//    from_json(j.at("nose"),fb.nose);
//    from_json(j.at("mouthLeft"),fb.mouthLeft);
//    from_json(j.at("mouthRight"),fb.mouthRight);

//}
