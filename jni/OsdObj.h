#ifndef _OSDOBJ_H
#define _OSDOBJ_H

#include <string>
#include <iostream>

typedef unsigned int uint_32;
using namespace std;

class OsdObj
{
private:
	uint_32 osd_start_x;
	uint_32 osd_start_y;
	uint_32 osd_res_x;
	uint_32 osd_res_y;
	uint_32 osd_id;
	string osd_name;
	string osd_content;
	bool osd_hide;
	char *osd_bmp_buf;
	char *osd_yuv_buf;
	int osd_font_size;
	bool osd_is_time_str;

public:
	OsdObj();
	OsdObj(uint_32 start_x, uint_32 start_y, uint_32 font_size, bool is_time_str, uint_32 id, string name, string content, bool hide);
	virtual ~OsdObj();
	void show();
	uint_32 getOsdId();
	//void setOsdId(uint_32 id);
	string getOsdName();
	void setOsdName(string name);
	string getOsdContentStr();
	void setOsdContentStr(string content);
	void getOsdPosition(uint_32 *off_x, uint_32 *off_y);
	void getOsdRes(uint_32 *x, uint_32 *y);
	void setOsdRes(uint_32 x, uint_32 y);
	uint_32 getOsdFontSize();
	char *getOsdBmpBufP();
	void setOsdBmpBufP(char *pBmpBuf);
	char *getOsdYuvBufP();
	void setOsdYuvBufP(char *pYuvBuf);
	void changeOsdPosition(uint_32 x, uint_32 y);
	bool isOsdHide();
	void setOsdHide(bool is_hide);
	bool isOsdTimeStr();
};
#endif
