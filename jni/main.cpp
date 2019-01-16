#include <iostream>
#include <vector>
#include <string>
#include "OsdObj.h"
#include "OsdCtl.h"
#include "OsdLog.h"

using namespace std;
typedef unsigned int uint_32;

#define  IMAGEWIDTH     1920
#define  IMAGEHEIGHT    1080

unsigned char nYUV420spBuf[IMAGEWIDTH * IMAGEHEIGHT * 3 / 2];

int main(int argc, char *argv[]){

	FILE *pYUVRead,*pYUVWrite;
	unsigned int iSize;
	int i=0;
	int ret = 0;
	clock_t startTime,endTime;

	//1. Got OsdCtl Instance
	OsdCtl *ctl;
	if(!(ctl = OsdCtl::OSD_Instance())){
		LOGE("Got OsdCtl instance NULL...");
		return -1;
	}

	//2. Add OsdObjs
	OsdObj obj1(0, 0, 32, 1,  1, "video_time", "2018-1001-09:00:00", false);
	ret = ctl->OSD_Add_Obj(&obj1);
	if(ret)
	{
		LOGE("OSD Add obj failed...");
		return -2;
	}

	OsdObj obj2(100, 100, 32, 0,  2, "video_state", "正在录像", false);
	ret = ctl->OSD_Add_Obj(&obj2);
	if(ret)
	{
		LOGE("OSD Add obj failed...");
		return -3;
	}

	//test del obj func
	//ctl->OSD_Del_Obj(&obj1);

	//test obj hide func
	//obj2.setOsdHide(true);

	//test change content str func
	//obj1.setOsdContentStr("2018-1001-09:00:01");
	//ctl->OSD_Update_Obj_Content(&obj1);

	pYUVRead = fopen("/storage/emulated/0/osd/YuvIN.yuv","rb");
	pYUVWrite = fopen("/storage/emulated/0/osd/YuvOUT.yuv","wb");

	if (pYUVRead && pYUVWrite){
		while (!feof(pYUVRead)){
			iSize = IMAGEWIDTH * IMAGEHEIGHT * 3 / 2;
			if (iSize != fread(nYUV420spBuf, 1, iSize, pYUVRead)){
				break;
			}
			startTime = clock();
			//3.Start Composing
			ctl->OSD_Start_Composing(nYUV420spBuf, IMAGEWIDTH, IMAGEHEIGHT);
			endTime = clock();
			LOGD("OSD: composing per 1080p frame is %lf ms \n", (double)(endTime - startTime) / CLOCKS_PER_SEC * 1000);
			if (pYUVWrite){
				fwrite(nYUV420spBuf, 1, iSize, pYUVWrite);
			}
		}
	}else{
		LOGE("open file failed....");
		return -4;
	}

	//4. Stop Composing
	ctl->OSD_Stop_Composing();

	return 0;
}

