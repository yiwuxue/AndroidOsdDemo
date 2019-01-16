#include <iostream>
#include<string>
#include <vector>
#include "OsdCtl.h"
#include "OsdObj.h"
#include "OsdLog.h"
#include "yuvosd.h"

using namespace std;

OsdCtl *OsdCtl::sInstance = NULL;

OsdCtl *OsdCtl::OSD_Instance() {
	if (!sInstance)
		sInstance = new OsdCtl();
	return sInstance;
}

OsdCtl::OsdCtl(){
	//	LOGD("OsdCtl Constructor...");
}

OsdCtl::~OsdCtl(){
	//	LOGD("OsdCtl ~ Constructor...");
	//OSD_Release();
}

int OsdCtl::OSD_ReadYuvFile(OsdObj *objYuv)
{
	int ret = 0;
	int buf_size = 0;
	FILE *pYuvHandle = NULL;
	FILE *pYuvHandlebak = NULL;
	char path[100] = "/storage/emulated/0/osd/time_str.yuv";
	char *pYuvBuf = NULL;

#if OSD_DEBUG
	char path_bak[100] = "/storage/emulated/0/osd/time_str_bak.yuv";
#endif

	pYuvHandle = fopen(path, "rb");
	if(!pYuvHandle){
		LOGE("open %s failed...", path);
		return -1;
	}

#if OSD_DEBUG
	pYuvHandlebak = fopen(path_bak, "wb");
#endif

	objYuv->setOsdRes(16, 416);

	buf_size = 16 * 416 * 3 / 2;
	pYuvBuf = (char *)malloc(buf_size);
	ret = fread(pYuvBuf, 1, buf_size, pYuvHandle);
	if(ret != buf_size)
	{
		LOGE("read from yuv file failed...");
		fclose(pYuvHandle);
		return -1;
	}
	objYuv->setOsdYuvBufP(pYuvBuf);

#if OSD_DEBUG
	ret = fwrite(pYuvBuf, 1, buf_size, pYuvHandlebak);
	LOGD("fwrite " << ret << " bytes...");
	fclose(pYuvHandlebak);
#endif
	fclose(pYuvHandle);
	return 0;
}

int OsdCtl::OSD_ReadBmpFile(OsdObj *objBmp)
{
	uint_32 ret;
	uint_32 x_res;
	uint_32 y_res;
	uint_32 buf_size;
	int i = 0;
	FILE *pBmpHandle = NULL;

#if OSD_DEBUG
	FILE *pBmpHandlebak = NULL;
	FILE *pYuvHandle = NULL;
	char *YUV_obj = NULL;
#endif

	char *pBmpBuf = NULL;
	unsigned char *rgbData = NULL;
	unsigned char *Y = NULL;
	unsigned char *U = NULL;
	unsigned char *V = NULL;
	char *YUV = NULL;
	int width = 0;
	int height = 0;

	char path[100] = "/storage/emulated/0/osd/";
	strcat(path, objBmp->getOsdName().c_str());
	snprintf(path + strlen(path), 5, ".bmp");
	//LOGD("path: " <<  path);

	objBmp->getOsdRes(&x_res, &y_res);
	//LOGD("x_res: " << x_res << " y_res: " << y_res <<  endl;
	buf_size = x_res * y_res * 3 + 14 + 40;
	//LOGD("BufSize: " <<  buf_size);

	pBmpBuf = (char *)malloc(buf_size);

	//open read to buffer
	pBmpHandle = fopen(path, "rb");
	if(!pBmpHandle){
		LOGE("open %s failed...", path);
		return -1;
	}

#if OSD_DEBUG
	char path_bmp[100] = "/storage/emulated/0/osd/";
	char path_yuv[100] = "/storage/emulated/0/osd/";
	strcat(path_bmp, objBmp->getOsdName().c_str());
	strcat(path_yuv, objBmp->getOsdName().c_str());
	snprintf(path_bmp + strlen(path_bmp), 9, "_bak.bmp");
	snprintf(path_yuv + strlen(path_bmp), 9, "_bak.yuv");
	pBmpHandlebak = fopen(path_bmp, "wb");
	pYuvHandle = fopen(path_yuv, "wb");
#endif

	ret = fread(pBmpBuf, 1, buf_size, pBmpHandle);
	if(ret != buf_size)
	{
		//LOGE("read from bmp file failed...");
		return -2;
	}
	//LOGE("fread " << ret << " bytes...");
	objBmp->setOsdBmpBufP(pBmpBuf);

#if OSD_DEBUG
	ret = fwrite(objBmp->getOsdBmpBufP(), 1, buf_size, pBmpHandlebak);
	LOGE("fwrite " << ret << " bytes...");
	fclose(pBmpHandlebak);
#endif

	fclose(pBmpHandle);

	rgbData = (unsigned char *) malloc(x_res * y_res * 3);
	memset(rgbData, 0, x_res * y_res *3);
	Y = (unsigned char * )malloc(x_res * y_res);
	U = (unsigned char * )malloc(x_res * y_res / 4);
	V = (unsigned char * )malloc(x_res * y_res / 4);
	YUV = (char * )malloc(x_res * y_res * 3 / 2);
	BITMAPFILEHEADER file_header;
	BITMAPINFOHEADER info_header;
	memcpy(&file_header, pBmpBuf, sizeof(BITMAPFILEHEADER));
	memcpy(&info_header, pBmpBuf + sizeof(BITMAPFILEHEADER), sizeof(BITMAPINFOHEADER));

#if OSD_DEBUG
	LOGD("BITMAPFILEHEADER size: " << sizeof(BITMAPFILEHEADER));
	LOGD("BITMAPINFOHEADER size: " << sizeof(BITMAPINFOHEADER));
	LOGD("bfSize: " << file_header.bfSize);
	LOGD("bfOffBits: " << file_header.bfOffBits);
	LOGD("biSize: " << info_header.biSize);
	LOGD("biWidth: " << info_header.biWidth);
	LOGD("biHeight: " << info_header.biHeight);
	LOGD("biBitCout: " << info_header.biBitCount);
#endif
	ReadRGBFromBuf(pBmpBuf, file_header, info_header, rgbData);

	/*LOGD("rgbData...Begin");
	  for(i=0; i<100; i++)
	  {
		LOGD("%x ", *(rgbData+i));
	  }
	  LOGD("rgbData...End");*/

	if (((info_header.biWidth / 8 * info_header.biBitCount) % 4) == 0)
		width = info_header.biWidth;
	else
		width = (info_header.biWidth * info_header.biBitCount + 31) / 32 * 4;

	if ((info_header.biHeight % 2) == 0)
		height = info_header.biHeight;
	else
		height = info_header.biHeight + 1;

	//LOGD("width: " << width << " height: " << height);
	ret = RGB2YUV(width, height, rgbData, Y, U, V);
	if(ret)
	{
		//LOGE("read from bmp file failed...");
		return -3;
	}

	/*LOGD("YData...Begin");
	  for(i=0; i<100; i++)
	  {
		LOGD("%x ", *(V+i));
	  }
	  LOGD("YData...End");*/

	char *pTemp = YUV;
	memcpy(pTemp, Y, width * height);
	pTemp += width * height;
	memcpy(pTemp, U, width * height / 4);
	pTemp += width * height / 4;
	memcpy(pTemp, V, width * height / 4);
	objBmp->setOsdYuvBufP(YUV);

#if OSD_DEBUG
	YUV_obj = objBmp->getOsdYuvBufP();
	ret = fwrite(YUV_obj, 1, width * height * 3 / 2, pYuvHandle);
	LOGD("Write " << ret << "Bytes to YUV file...");
	fclose(pYuvHandle);
#endif

	remove(path);

	return 0;
}

int OsdCtl::OSD_Add_Obj(OsdObj *obj){
	int ret = 0;
	int x = 0;
	int y = 0;

	if(obj->isOsdTimeStr()){
		ret = OSD_ReadYuvFile(obj);
		if(ret)
		{
			LOGE("OSD_ReadYuvFile failed...");
			return -1;
		}
		v.push_back(obj);
	}else{
		OsdObj *objBmp = obj;
		font = SDL_TTF_Init(obj->getOsdFontSize());
		if(!font)
		{
			LOGE("SDL_TTF_Init failed...");
			return -1;
		}

		//ttf convert to bmp
		ret = SDL_TTF_Render(font, (char *)obj->getOsdContentStr().c_str(), (char *)obj->getOsdName().c_str(), &x, &y);
		if(ret)
		{
			LOGE("TTF font rendering to bitmap failed... ret: %d", ret);
			return -2;
		}

		objBmp->setOsdRes(x, y);

		//Read bmp file to memory
		ret = OSD_ReadBmpFile(objBmp);
		if(ret)
		{
			LOGE("OSD read bmp file failed..., ret: %d", ret);
			return -3;
		}

		//LOGE("obj, name: " << obj->getOsdName());
		//Add obj to obj list
		v.push_back(obj);

		SDL_TTF_Deinit(font);
	}
	return 0;
}

void OsdCtl::OSD_Del_Obj(OsdObj *obj){
	//Delete obj form obj list
	for(uint_32 i = 0; i < v.size(); i++){
		if(v[i]->getOsdId() == obj->getOsdId()){
			//		LOGE("VectorSize: %d, Del Obj-ID:%d ", v.size(), obj->getOsdId());
#if 1
			if(v[i]->getOsdBmpBufP() != NULL)
			{
				free(v[i]->getOsdBmpBufP());
				v[i]->setOsdBmpBufP(NULL);
			}
			if(v[i]->getOsdYuvBufP() != NULL)
			{
				free(v[i]->getOsdYuvBufP());
				v[i]->setOsdYuvBufP(NULL);
			}
			v.erase(v.begin() + i);
#else
			delete v[i];
			v.erase(v.begin() + i);
#endif
		}
	}
}

void OsdCtl::OSD_Show_Vector(){
	for(uint_32 i = 0; i < v.size(); i++){
		LOGD("ID: %d, Name:%s, ContenStr:%s\n", v[i]->getOsdId(), v[i]->getOsdName().c_str(), v[i]->getOsdContentStr().c_str());
	}
}

void OsdCtl::OSD_Update_Obj_Content(OsdObj *obj){
	if(!obj->isOsdTimeStr()){
		LOGE("Only support update time str content...");
		return;
	}
	//Update obj content
	for(uint_32 i = 0; i < v.size(); i++){
		if(v[i]->getOsdId() == obj->getOsdId()){
			v[i]->setOsdContentStr(obj->getOsdContentStr());
		}
	}
}

void OsdCtl::OSD_Start_Composing(unsigned char * pYuv420Frame, uint_32 iSrcWidth, uint_32 iSrcHeight){
	//LOGD("OSD Start Composing, there was %d osd objs...", v.size());
	char *pYuvObj = NULL;
	uint_32 iResX, iResY;
	uint_32 iStartX, iStartY;

	if(v.size() == 0)
	{
		LOGE("No OSD obj to composing, return...");
		return;
	}

	for(uint_32 i = 0; i < v.size(); i++){
		if(v[i]->isOsdHide() != true){
			v[i]->getOsdRes(&iResX, &iResY);
			v[i]->getOsdPosition(&iStartX, &iStartY);
			pYuvObj = v[i]->getOsdYuvBufP();
			if(pYuvObj == NULL)
			{
				LOGE("Yuv Obj Buffer NULL, Composing failed...");
				return;
			}
			//LOGD("Obj StartX: " << iStartX << " StatY: " << iStartY);
			//LOGD("Obj ResX: " << iResX << " ResY: " << iResY);
			//LOGD("src ResX: " << iSrcWidth << " src ResY: " << iSrcHeight);
			if(v[i]->isOsdTimeStr())
			{
				OSD_overlap_time_caption((char *)pYuv420Frame, iSrcWidth, iSrcHeight,
										 pYuvObj, iResX, iResY,//OSD图及宽高
										 iStartX, iStartY, (char *)v[i]->getOsdContentStr().c_str());//要叠加的字符串的位置及宽高
			}else{
				_OSD_overlap_caption((char *)pYuv420Frame, iSrcWidth, iSrcHeight,
									 pYuvObj, iResX, iResY,//OSD图及宽高
									 iStartX, iStartY);//要叠加的字符串的位置及宽高
			}
		}
	}
}

void OsdCtl::OSD_Stop_Composing(){
	for(uint_32 i = 0; i < v.size(); i++){
		if(v[i]->getOsdBmpBufP() != NULL)
		{
			free(v[i]->getOsdBmpBufP());
			v[i]->setOsdBmpBufP(NULL);
		}
		if(v[i]->getOsdYuvBufP() != NULL)
		{
			free(v[i]->getOsdYuvBufP());
			v[i]->setOsdYuvBufP(NULL);
		}
	}
	v.clear();
}
