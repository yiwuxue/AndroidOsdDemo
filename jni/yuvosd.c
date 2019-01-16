#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "OsdLog.h"

void _OSD_overlap_caption(char *src_yuv, int src_w, int src_h,//源图及宽高
						 char *osd_yuv, int osd_w, int osd_h,//OSD图及宽高
                         int off_x, int off_y)//要叠加的字符串的位置及宽高
{
	int i, j, n_off;
	int size_src = src_w * src_h;
    int size_osd = osd_w * osd_h;
    char *psrc = src_yuv;
    char *posd = osd_yuv;

	if(!src_yuv || !osd_yuv)
		return;

	if(off_x + osd_w > src_w)
	{
		off_x = src_w - osd_w;
	}
	if(off_y + osd_h > src_h)
	{
		off_y = src_h - osd_h;
	}

    for (i = 0; i < osd_h; i++){
		n_off = src_w * (off_y + i) + off_x;
		for(j=0; j < osd_w; j++){
			//LOGD("*(posd + osd_w * i + j):%d\n", *(posd + osd_w * i + j));
			if(*(posd + osd_w * i + j) != 16){
				*(psrc + n_off + j) = *(posd + osd_w * i + j);
			}
		}
	}
}

int OSD_overlap_time_caption(char *src_yuv, int src_w, int src_h,
						 char *osd_yuv, int osd_w, int osd_h,
                         int off_x, int off_y, const char *p_caption)
{
	int i = 0;
	int offset = 0;
	int len = strlen(p_caption);

	if(!src_yuv || !osd_yuv || !p_caption || osd_w > src_w || osd_h > src_h)
		return -1;

	if(off_x + osd_w * len > src_w)
	{
		off_x = src_w - osd_w * len;
	}

	if(off_y + osd_h / 14 > src_h)
	{
		off_y = src_h - osd_h / 14;
	}

	//case time string
	osd_h /= 14;//time bmp have 14 index chars

	for(i = 0; i < len; i++){
		if(*(p_caption+i) == 45){//'-'
			offset = 11;
		}else if(*(p_caption+i) == 47){//'/'
			offset = 12;
		}else if(*(p_caption+i) == 58){//':'
			offset = 10;
		}else if(*(p_caption+i) == 32){// ' '
			offset = 13;
		} else {
			offset = *(p_caption+i) - 48;//0-9
		}
		//16 * 32 = 512 512 * 14 = 7168 7168 * 3 / 2 = 10752
		//8 * 16 = 128 128 * 14 = 1792 1792 * 3 / 2 = 2688
		/*_OSD_overlap_caption(src_yuv, src_w, src_h, osd_yuv + 512 * offset, 16, 32, off_x + 16*i, off_y);*/
		_OSD_overlap_caption(src_yuv, src_w, src_h, osd_yuv + osd_w * osd_h * offset, osd_w, osd_h, off_x + osd_w * i, off_y);
	}

	return 0;
}
