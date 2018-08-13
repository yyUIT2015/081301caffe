#include "libfaceservicejava.h"
#include <iostream>
Libfaceservicejava::Libfaceservicejava()
{
    service = libfaceservice::getInstance();
}

std::string Libfaceservicejava::init(std::string json)
{
    QString Qjson = QString::fromStdString(json);
    QJsonDocument jsonDocument = QJsonDocument::fromJson(Qjson.toLocal8Bit().data());
    if( jsonDocument.isNull() )
    {
        qDebug()<< "===> please check the string "<< Qjson.toLocal8Bit().data();
    }
    QJsonObject jsonObject = jsonDocument.object();
    QJsonValue workurlvalue = jsonObject.value("workurl");
    QJsonValue FDnetmaxvalue = jsonObject.value("fdnetmax");
    QJsonValue FRnetmaxvalue = jsonObject.value("frnetmax");
    QJsonValue Factorvalue = jsonObject.value("factor");

    std::string workUrl;
    double Factor;
    int FDnetmax;
    int FRnetmax;

    workUrl = workurlvalue.toString().toStdString();
    FDnetmax = FDnetmaxvalue.toInt();
    FRnetmax = FRnetmaxvalue.toInt();
    Factor = Factorvalue.toDouble();
    //    std::cout<<__FILE__<<"   "<<__LINE__<<"\nworkurl "<<workUrl<<" FD "<<FDnetmax<<" FR "<<FRnetmax<<std::endl;
    service->init(workUrl,FDnetmax,FRnetmax);

    this->Factor = Factor;

    QJsonObject initJson ;
    initJson.insert("code",0);

    QJsonDocument document;
    document.setObject(initJson);
    QByteArray byteArray = document.toJson(QJsonDocument::Compact);
    QString initjson(byteArray);

    return initjson.toStdString();
}

void Libfaceservicejava::release()
{
    service->release();
}

std::string Libfaceservicejava::addfacegroup(std::string json)
{
    QString Qjson = QString::fromStdString(json);
    QJsonDocument jsonDocument = QJsonDocument::fromJson(Qjson.toLocal8Bit().data());
    if( jsonDocument.isNull() )
    {
        qDebug()<< "===> please check the string "<< Qjson.toLocal8Bit().data();
    }
    QJsonObject jsonObject = jsonDocument.object();
    QJsonValue idvalue = jsonObject.value("id");
    QJsonValue namevalue = jsonObject.value("name");
    QJsonValue groupurlvalue = jsonObject.value("groupurl");
    std::string id;
    std::string name;
    std::string groupUrl;
    groupUrl = groupurlvalue.toString().toStdString();
    id = idvalue.toString().toStdString();
    name = namevalue.toString().toStdString();
    std::string groupname = id+"_"+name;
    AddgroupResult ag = service->addfacegroup(groupname,groupUrl,this->Factor);

    //    std::cout << ag.err << std::endl;

    QString Addgroupjson = getAddgroupJson(ag);

    return Addgroupjson.toStdString();
}

std::string Libfaceservicejava::detectFace(std::string json)
{
    QString Qjson = QString::fromStdString(json);
    QJsonDocument jsonDocument = QJsonDocument::fromJson(Qjson.toLocal8Bit().data());
    if( jsonDocument.isNull() )
    {
        qDebug()<< "===> please check the string "<< Qjson.toLocal8Bit().data();
    }
    QJsonObject jsonObject = jsonDocument.object();

    QJsonValue imageurlvalue = jsonObject.value("imageurl");

    std::string imageUrl;

    imageUrl = imageurlvalue.toString().toStdString();

    //    std::cout<<"___TEST___"<<imageUrl<<std::endl;

    DetectResult dr = service->detectFace(imageUrl,this->Factor);

    //    std::cout<<"___TEST___"<<std::endl;

    QString DRjson = getDetectResultJson(dr);

    return DRjson.toStdString();
}

std::string Libfaceservicejava::faceIdentity(std::string json)
{
    QString Qjson = QString::fromStdString(json);
    QJsonDocument jsonDocument = QJsonDocument::fromJson(Qjson.toLocal8Bit().data());
    if( jsonDocument.isNull() )
    {
        qDebug()<< "===> please check the string "<< Qjson.toLocal8Bit().data();
    }
    QJsonObject jsonObject = jsonDocument.object();
    QJsonValue imageurlvalue = jsonObject.value("imageurl");
    QJsonValue groupvalue = jsonObject.value("group");
    QJsonValue Landmarkvalue = jsonObject.value("landmark");
    QJsonObject LandmarkObj = Landmarkvalue.toObject();

    std::string imageUrl;
    std::string group;
    Landmark land;

    imageUrl = imageurlvalue.toString().toStdString();
    group = groupvalue.toString().toStdString();
    QJsonObject eyeLeftvalue = LandmarkObj.value("eyeLeft").toObject();
    QJsonObject eyeRightvalue = LandmarkObj.value("eyeRight").toObject();
    QJsonObject mouthLeftvalue = LandmarkObj.value("mouthLeft").toObject();
    QJsonObject mouthRightvalue = LandmarkObj.value("mouthRight").toObject();
    QJsonObject nosevalue = LandmarkObj.value("nose").toObject();

    land.eyeLeft.x = eyeLeftvalue.value("x").toInt();
    land.eyeLeft.y = eyeLeftvalue.value("y").toInt();
    land.eyeRight.x = eyeRightvalue.value("x").toInt();
    land.eyeRight.y = eyeRightvalue.value("y").toInt();
    land.mouthLeft.x = mouthLeftvalue.value("x").toInt();
    land.mouthLeft.y = mouthLeftvalue.value("y").toInt();
    land.mouthRight.x = mouthRightvalue.value("x").toInt();
    land.mouthRight.y = mouthRightvalue.value("y").toInt();
    land.nose.x = nosevalue.value("x").toInt();
    land.nose.y = nosevalue.value("y").toInt();

    IdentifyResult dr = service->faceIdentity(group,imageUrl,land);

    QString DRjson = getIdentifyResultJson(dr);

    return DRjson.toStdString();
}

QString Libfaceservicejava::getDetectResultJson(DetectResult dr){
    QJsonArray facesjson;
    for(std::list<Face>::iterator it= dr.faces.begin(); it!=dr.faces.end(); it ++)
    {
        Face face = *it;
        facesjson.append( QJsonValue( getFaceJson(face) ) );
    }
    QJsonObject detectResult;
    detectResult.insert("imageWidth",dr.imageWidth);
    detectResult.insert("imageHeight",dr.imageHeight);
    int time = dr.timeUsed_ms;
    detectResult.insert("timeUsed",time);
    detectResult.insert("code",dr.err);
    detectResult.insert("faces",QJsonValue(facesjson));
    const char* url = dr.url.c_str();
    detectResult.insert("url",url);
    std::string messages;
    messages = getmessage(dr.err);
    const char * message = messages.c_str();
    detectResult.insert("message",message);

    QJsonDocument document;
    document.setObject(detectResult);
    QByteArray byteArray = document.toJson(QJsonDocument::Compact);
    QString strJson(byteArray);
    //qDebug()<<document;
    return strJson;
}
QJsonObject Libfaceservicejava::getFaceJson(Face face){
    QJsonObject facejson;
    QJsonObject faceBoxjson;
    faceBoxjson.insert("left",face.faceBox.top);
    faceBoxjson.insert("top",face.faceBox.left);
    faceBoxjson.insert("right",face.faceBox.bottom);
    faceBoxjson.insert("bottom",face.faceBox.right);
    faceBoxjson.insert("width",face.faceBox.height);
    faceBoxjson.insert("height",face.faceBox.width);
    QJsonObject landmarkjson;
    QJsonObject eyeLeftjson;
    eyeLeftjson.insert("x",face.landmark.eyeLeft.x);
    eyeLeftjson.insert("y",face.landmark.eyeLeft.y);
    QJsonObject eyeRightjson;
    eyeRightjson.insert("x",face.landmark.eyeRight.x);
    eyeRightjson.insert("y",face.landmark.eyeRight.y);
    QJsonObject mouthLeftjson;
    mouthLeftjson.insert("x",face.landmark.mouthLeft.x);
    mouthLeftjson.insert("y",face.landmark.mouthLeft.y);
    QJsonObject mouthRightjson;
    mouthRightjson.insert("x",face.landmark.mouthRight.x);
    mouthRightjson.insert("y",face.landmark.mouthRight.y);
    QJsonObject nosejson;
    nosejson.insert("x",face.landmark.nose.x);
    nosejson.insert("y",face.landmark.nose.y);
    landmarkjson.insert("eyeLeft",eyeLeftjson);
    landmarkjson.insert("eyeRight",eyeRightjson);
    landmarkjson.insert("mouthLeft",mouthLeftjson);
    landmarkjson.insert("mouthRight",mouthRightjson);
    landmarkjson.insert("nose",nosejson);

    facejson.insert("faceBox",QJsonValue(faceBoxjson));
    facejson.insert("landmark",QJsonValue(landmarkjson));
    return facejson;
}
QString Libfaceservicejava::getIdentifyResultJson(IdentifyResult Id){
    QJsonArray candidatesjson;
    for(std::list<IdentityCandidate>::iterator it= Id.candidates.begin(); it!=Id.candidates.end(); it ++)
    {
        candidatesjson.append( QJsonValue( getcandidateJson(*it) ) );
    }
    QJsonObject identifyResult;
    identifyResult.insert("imageWidth",Id.imageWidth);
    identifyResult.insert("imageHeight",Id.imageHeight);
    identifyResult.insert("timeUsed",Id.timeUsed_ms);
    identifyResult.insert("code",Id.err);
    identifyResult.insert("candidates",QJsonValue(candidatesjson));
    const char * url = Id.url.c_str();
    identifyResult.insert("url",url);
    std::string messages;
    messages = getmessage(Id.err);
    const char * message = messages.c_str();
    identifyResult.insert("message",message);

    QJsonDocument document;
    document.setObject(identifyResult);
    QByteArray byteArray = document.toJson(QJsonDocument::Compact);
    QString strJson(byteArray);

    //qDebug()<<document;
    return strJson;
}

QJsonObject Libfaceservicejava::getcandidateJson(IdentityCandidate candidates){
    QJsonObject  candidate;
    const char *personId = candidates.personId.c_str();
    candidate.insert("personId",personId);

    //std::string value = std::to_string(candidates.confidence*100);
    //    std::cout<<value<<std::endl;
    //std::string str = value.substr(0, value.length() - 4);
    //    const char* st1 =str.c_str();
    candidate.insert("confidence",candidates.confidence*100);
    return candidate;
}

QJsonObject Libfaceservicejava::getAddGroup1Json(int err,std::string Group_id){
    QJsonObject addgroup1result;

    const char * group_id = Group_id.c_str();
    addgroup1result.insert("group_id",group_id);
    addgroup1result.insert("code",err);
    std::string messages;
    messages = getmessage(err);
    const char * message = messages.c_str();
    addgroup1result.insert("message",message);

    QJsonDocument document;
    document.setObject(addgroup1result);
    QByteArray byteArray = document.toJson(QJsonDocument::Compact);
    QString strJson(byteArray);

    //qDebug()<<document;

    return addgroup1result;
}

QJsonObject Libfaceservicejava::getAddGroup2Json(int err,std::string Group_id,std::string Image_url,std::string Face_id){
    QJsonObject addgroup2result;

    const char * group_id = Group_id.c_str();
    addgroup2result.insert("group_id",group_id);
    const char * face_id = Face_id.c_str();
    addgroup2result.insert("face_id",face_id);
    const char * image_url = Image_url.c_str();
    addgroup2result.insert("image_url",image_url);
    addgroup2result.insert("code",err);
    std::string messages;
    messages = getmessage(err);
    const char * message = messages.c_str();
    addgroup2result.insert("message",message);

    QJsonDocument document;
    document.setObject(addgroup2result);
    QByteArray byteArray = document.toJson(QJsonDocument::Compact);
    QString strJson(byteArray);

    //qDebug()<<document;

    return addgroup2result;
}

std::string Libfaceservicejava::getmessage(int err)
{
    std::string message;
    switch (err)
    {
    case 0 : message = "noError";break;
    case -1 :message = "networkInternalErr";break;
    case -2 :message = "imageNotExist";break;
    case -3 :message = "imageFormatErr";break;
    case -4 :message = "imageSizeErr";break;
    case -5 :message = "imageBroken";break;
    case -6 :message = "imageSizeSmall";break;
    case -7 :message = "faceNotProperAngle";break;
    case -8 :message = "faceGroupNotExist";break;
    case -9 :message = "faceGroupAlreadyExist";break;
    case -10 :message = "faceDatabaseDirNotExist";break;
    case -11 :message = "cannotCreateFaceGroupDir";break;
    case -12 :message = "otherErr";break;
    case -13 :message = "unabletoapplyforFRresources";break;
    case -14 :message = "unabletoapplyforFDresources";break;
    case -15 :message = "faceserviceNotinit";break;
    }

    return message;
}

QString Libfaceservicejava::getAddgroupJson(AddgroupResult ag)
{
    QJsonArray FailReasonsjson;

    for(std::list<FailReason>::iterator it= ag.failreasons.begin(); it!=ag.failreasons.end(); it ++)
    {
        FailReason failreason = *it;
        FailReasonsjson.append( QJsonValue( Libfaceservicejava::getFailReasonJson(failreason) ) );
    }
    QJsonObject addgroupResult;
    addgroupResult.insert("added",ag.added);
    addgroupResult.insert("count",ag.count);
    int time = ag.timeUsed;
    addgroupResult.insert("timeUsed",time);
    addgroupResult.insert("code",ag.err);
    addgroupResult.insert("faces",QJsonValue(FailReasonsjson));
    std::string messages;
    messages = getmessage(ag.err);
    const char * message = messages.c_str();
    addgroupResult.insert("message",message);

    QJsonDocument document;
    document.setObject(addgroupResult);
    QByteArray byteArray = document.toJson(QJsonDocument::Compact);
    QString strJson(byteArray);
    //qDebug()<<document;
    return strJson;
}

QJsonObject Libfaceservicejava::getFailReasonJson(FailReason failreason)
{
    QJsonObject fail;
    const char* Url = failreason.url.c_str();
    fail.insert("url",Url);
    const char* Reason = failreason.reason.c_str();
    fail.insert("reason",Reason);
    return fail;
}


