#ifndef _OSD_CTL_H
#define _OSD_CTL_H
#include <vector>
#include <iostream>
#include "OsdObj.h"
#include "SDL.h"
#include "SDL_ttf.h"
#include "ttf2bmp.h"
#include "bmp2yuv.h"

using namespace std;
typedef unsigned int uint_32;

class OsdCtl
{
private:
	static OsdCtl *sInstance;
	vector<OsdObj *> v;
	TTF_Font *font;
	int OSD_ReadYuvFile(OsdObj *objYuv);
	int OSD_ReadBmpFile(OsdObj *objBmp);
	void OSD_Show_Vector();

public:
	static OsdCtl * OSD_Instance();
	int OSD_Add_Obj(OsdObj *obj);
	void OSD_Del_Obj(OsdObj *obj);
	void OSD_Update_Obj_Content(OsdObj *obj);
	void OSD_Start_Composing(unsigned char * pYuv420Frame, uint_32 iSrcWidth, uint_32 iSrcHeight);
	void OSD_Stop_Composing();
	OsdCtl();
	virtual ~OsdCtl();
};
#endif
