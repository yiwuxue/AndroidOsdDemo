#include <iostream>
#include <string>
#include "OsdObj.h"
#include "OsdLog.h"

using namespace std;

typedef unsigned int uint_32;

OsdObj::OsdObj(){

}

OsdObj::OsdObj(uint_32 start_x, uint_32 start_y, uint_32 font_size, bool is_time_str, uint_32 id, string name, string content, bool hide){
	//LOGD("*******OsdObj Construct*******");
	osd_start_x = start_x;
	osd_start_y = start_y;
	osd_id = id;
	osd_name = name;
	osd_content = content;
	osd_hide = hide;
	osd_font_size = font_size;
	osd_is_time_str = is_time_str;
	osd_bmp_buf = NULL;
	osd_yuv_buf = NULL;
}

void OsdObj::show(){
	LOGD("osd_start_x: %d", this->osd_start_x);
	LOGD("osd_start_y: %d", this->osd_start_x);
	LOGD("osd_id: %d", this->osd_id);
	LOGD("osd_name: %s", this->osd_name.c_str());
	LOGD("osd_content: %s", this->osd_content.c_str());
	LOGD("osd_hide: %d", this->osd_hide);
}

uint_32 OsdObj::getOsdId(){
	return osd_id;
}

/*
void OsdObj::setOsdId(uint_32 id){
	osd_id = id;
}
*/

string OsdObj::getOsdName(){
	return osd_name;
}

void OsdObj::setOsdName(string name){
	osd_name = name;
}

string OsdObj::getOsdContentStr(){
	return osd_content;
}

void OsdObj::setOsdContentStr(string content){
	osd_content = content;
}

void OsdObj::getOsdPosition(uint_32 *off_x, uint_32 *off_y){
	*off_x = osd_start_x;
	*off_y = osd_start_y;
}
uint_32 OsdObj::getOsdFontSize(){

	return osd_font_size;
}

char* OsdObj::getOsdBmpBufP(){
	return osd_bmp_buf;
}

void OsdObj::setOsdBmpBufP(char *pBmpBuf){
	osd_bmp_buf = pBmpBuf;
}

char* OsdObj::getOsdYuvBufP(){
	return osd_yuv_buf;
}

void OsdObj::setOsdYuvBufP(char *pYuvBuf){
	osd_yuv_buf = pYuvBuf;
}

void OsdObj::changeOsdPosition(uint_32 x, uint_32 y){
	osd_start_x = x;
	osd_start_y = y;
}

void OsdObj::getOsdRes(uint_32 *x, uint_32 *y){
	*x = osd_res_x;
	*y = osd_res_y;
}

void OsdObj::setOsdRes(uint_32 x, uint_32 y){
	osd_res_x = x;
	osd_res_y = y;
}

bool OsdObj::isOsdHide(){
	return osd_hide;
}

void OsdObj::setOsdHide(bool is_hide){
	osd_hide = is_hide;
}
bool OsdObj::isOsdTimeStr(){
	return osd_is_time_str;
}

OsdObj::~OsdObj(){
	//LOGD("*******OsdObj Deconstruct*****");
#if 1
	if(osd_bmp_buf != NULL)
	{
		free(osd_bmp_buf);
		osd_bmp_buf = NULL;
	}

	if(osd_yuv_buf != NULL)
	{
		free(osd_yuv_buf);
		osd_yuv_buf = NULL;
	}
#endif
}
