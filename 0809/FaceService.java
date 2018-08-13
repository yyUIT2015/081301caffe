/*public class faceservice
{


	public static void main(String[] args)
    	{
		        System.out.println("err code:");
	}


} */
package cn.smart.ai.face.service;
public class FaceService {


    public static final int NoError                        = 0;
    public static final int NetworkInternalErr             = -1;
    public static final int ImageNotExist                  = -2;
    public static final int ImageFormatErr                 = -3;
    public static final int ImageSizeErr                   = -4;
    public static final int ImageBroken                    = -5;
    public static final int ImageSizeSmall                 = -6;
    public static final int FaceNotProperAngle             = -7;
    public static final int FaceGroupNotExist              = -8;
    public static final int FaceGroupAlreadyExist          = -9;
    public static final int FaceDatabaseDirNotExist        = -10;
    public static final int CannotCreateFaceGroupDir       = -11;
    public static final int OtherErr                       = -12;
    public static final int UnabletoapplyforFRresources    = -13;
    public static final int UnabletoapplyforFDresources    = -14;
    public static final int FaceserviceNotinit             = -15;

    public  String faceinit(String initjson)
    {
        return init(initjson);
    }
    public  String addFaceGroup(String addfacejson)
    {
        return addfacegroup(addfacejson);
    }
    public  String detectFace(String detectjson)
    {
        return detectface(detectjson);
    }
    public  String identifyFace(String identifyjson)
    {
        return identifyface(identifyjson);
    }
    public  void Release()
    {
        release();
    }



    private native String init(String initjson);
    private native String addfacegroup(String addfacejson);
    private native String detectface(String detectjson);
    private native String identifyface(String identifyjson);
    private native void release();

}
