package cn.smart.ai.face.service;

import cn.smart.ai.face.service.FaceService;
/*
public class faceserviceTest {

    public static void main(String[] args)
    {
	System.loadLibrary("libfaceservicejava");
        String addfacejson = "{\"id\":\"01\",\"name\":\"yy\",\"groupurl\":\"/home/yyprocess/Desktop/V/人脸库/a\"}";
        String identifyjson = "{\"imageurl\":\"/home/yyprocess/Desktop/V/人脸库/a/司枚艳.png\",\"group\":\"01\",\"landmark\":{\"eyeLeft\":{\"x\":74,\"y\":153},\"eyeRight\":{\"x\":136,\"y\":147},\"mouthLeft\":{\"x\":87,\"y\":219},\"mouthRight\":{\"x\":135,\"y\":213},\"nose\":{\"x\":107,\"y\":181}}}";
        String detectjson = "{\"imageurl\":\"/home/yyprocess/Desktop/V/人脸库/a/司枚艳.png\"}";
        String initjson = "{\"workurl\":\"/home/yyprocess/Desktop/models/models\",\"fdnetmax\":3,\"frnetmax\":3,\"factor\":0.709}";

        faceservice service = new faceservice();
        String code = service.faceinit(initjson);
        System.out.println("result:"+code);
        code = service.addFaceGroup(addfacejson);
        System.out.println("result:"+code);
        code = service.detectFace(detectjson);
        System.out.println("result:"+code);
        code = service.identifyFace(identifyjson);
        System.out.println("result:"+code);
        service.Release();

    }

}
*/
public class faceserviceTest extends Thread
{
	private  FaceService service;
	private  String d;
	String addfacejson = "{\"id\":\"01\",\"name\":\"yy\",\"groupurl\":\"/home/yyprocess/Desktop/V/人脸库/a\"}";
		
	String detectjson = "{\"imageurl\":\"/home/yyprocess/Desktop/V/人脸库/a/司枚艳.png\"}";


	public faceserviceTest(FaceService service,String d)
	{
		this.service = service;
		this.d = d;
	}
	public void run()
	{
		//String code = service.faceinit(initjson);
        	//System.out.println("result:"+code);
        	//String code = service.addFaceGroup(addfacejson);
		//System.out.println("result:"+code);
		String code = service.detectFace(d);
		System.out.println("result:"+code);
		//String code = service.identifyFace(d);
		//System.out.println("result:"+code);
		
	}
	public static void main(String[] args)
	{
        	String initjson = "{\"workurl\":\"/home/yyprocess/Desktop/models/models\",\"fdnetmax\":3,\"frnetmax\":3,\"factor\":0.709}";
		String d1 = "{\"imageurl\":\"/home/yyprocess/Desktop/V/人脸库/a/司枚艳.png\"}";
		String d2 = "{\"imageurl\":\"/home/yyprocess/Desktop/V/人脸库/a/司凯铭.png\"}";
	        String i1 = "{\"imageurl\":\"/home/yyprocess/Desktop/V/人脸库/a/司枚艳.png\",\"group\":\"01\",\"landmark\":{\"eyeLeft\":{\"x\":74,\"y\":153},\"eyeRight\":{\"x\":136,\"y\":147},\"mouthLeft\":{\"x\":87,\"y\":219},\"mouthRight\":{\"x\":135,\"y\":213},\"nose\":{\"x\":107,\"y\":181}}}";
        
	 	String i2 = "{\"imageurl\":\"/home/yyprocess/Desktop/V/人脸库/a/司凯铭.png\",\"group\":\"01\",\"landmark\":{\"eyeLeft\":{\"x\":59,\"y\":198},\"eyeRight\":{\"x\":137,\"y\":195},\"mouthLeft\":{\"x\":72,\"y\":292},\"mouthRight\":{\"x\":131,\"y\":290},\"nose\":{\"x\":85,\"y\":240}}}";
		System.loadLibrary("libfaceservicejava");
		FaceService service = new FaceService() ;
		String code = service.faceinit(initjson);
        	System.out.println("result:"+code);
		Thread thread = new faceserviceTest(service,i1);
		thread.start();
		//Thread thread1 = new faceserviceTest(service,i2);
		//thread1.start();
		//Thread thread2 = new faceserviceTest(service,i2);
		//thread2.start();
		//Thread thread3 = new faceserviceTest(service,i2);
		//thread3.start();
		service.Release();
	}
} 
