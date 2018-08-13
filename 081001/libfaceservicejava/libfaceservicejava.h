#ifndef LIBFACESERVICEJAVA_H
#define LIBFACESERVICEJAVA_H

#include <string>
#include <define.h>
#include <libfaceservice.h>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonValue>
#include <QDebug>
#include <QString>
#include <sstream>
#include "libfaceservicejava_global.h"

class LIBFACESERVICEJAVASHARED_EXPORT Libfaceservicejava
{

public:
    std::string detectFace(std::string json);
    std::string faceIdentity(std::string json);
    std::string addfacegroup(std::string json);
    std::string init(std::string json);
    void release();
    Libfaceservicejava();

protected:
    QString getDetectResultJson(DetectResult dr);
    QString getIdentifyResultJson(IdentifyResult Id);
    QString getAddgroupJson(AddgroupResult ag);
    QJsonObject getFailReasonJson(FailReason failreason);
    QJsonObject getcandidateJson(IdentityCandidate candidates);
    QJsonObject getFaceJson(Face face);
    QJsonObject getAddGroup1Json(int err,std::string Group_id);
    QJsonObject getAddGroup2Json(int err,std::string Group_id,std::string Image_url,std::string Face_id);
    std::string getmessage(int err);
private:
    libfaceservice *service;
    double Factor;
};

#endif // LIBFACESERVICEJAVA_H
