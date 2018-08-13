#include "faceservice.h"
#include <iostream>
#include "libfaceservicejava.h"
Libfaceservicejava *faceservice = new Libfaceservicejava();
/*
 * Class:     cn_faceservice
 * Method:    init
 * Signature: (Ljava/lang/String;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_cn_smart_ai_face_service_FaceService_init
(JNIEnv *env, jobject obj, jstring initjson)
{
    const char * initJson = env->GetStringUTFChars(initjson, 0);
    std::cout<<initJson<<std::endl;

    std::string result = faceservice->init(initJson);

    jstring result_str;
    result_str = env->NewStringUTF(result.c_str());
    return result_str;
}

/*
 * Class:     cn_faceservice
 * Method:    addfacegroup
 * Signature: (Ljava/lang/String;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_cn_smart_ai_face_service_FaceService_addfacegroup
(JNIEnv *env, jobject obj, jstring addfacejson)
{
    const char * addfaceJson = env->GetStringUTFChars(addfacejson, 0);
    std::cout<<addfaceJson<<std::endl;

    std::string result = faceservice->addfacegroup(addfaceJson);
    jstring result_str;
    result_str = env->NewStringUTF(result.c_str());
    return result_str;
}

/*
 * Class:     cn_faceservice
 * Method:    detectface
 * Signature: (Ljava/lang/String;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_cn_smart_ai_face_service_FaceService_detectface
(JNIEnv *env, jobject obj, jstring detectjson)
{
    const char * detectJson = env->GetStringUTFChars(detectjson, 0);
    std::cout<<detectJson<<std::endl;

    std::string result = faceservice->detectFace(detectJson);

    jstring result_str;
    result_str = env->NewStringUTF(result.c_str());
    return result_str;
}

/*
 * Class:     cn_faceservice
 * Method:    identifyface
 * Signature: (Ljava/lang/String;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_cn_smart_ai_face_service_FaceService_identifyface
(JNIEnv *env, jobject obj, jstring identifyjson)
{
    const char * identifyJson = env->GetStringUTFChars(identifyjson, 0);
    std::cout<<identifyJson<<std::endl;

    std::string result = faceservice->faceIdentity(identifyJson);

    jstring result_str;
    result_str = env->NewStringUTF(result.c_str());
    return result_str;
}


/*
 * Class:     cn_faceservice
 * Method:    release
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_cn_smart_ai_face_service_FaceService_release
(JNIEnv *env, jobject obj)
{
    //Libfaceservicejava::release();
    //std::cout<<"releaseJson"<<std::endl;
}

