#ifdef __cplusplus
extern "C"
{
#endif
#ifndef __YUV_OSD_H
#define __YUV_OSD_H

extern void _OSD_overlap_caption(char *src_yuv, int src_w, int src_h,//源图及宽高
						 char *osd_yuv, int osd_w, int osd_h,//OSD图及宽高
                         int off_x, int off_y);//要叠加的字符串的位置及宽高

extern int OSD_overlap_time_caption(char *src_yuv, int src_w, int src_h,
						 char *osd_yuv, int osd_w, int osd_h,
                         int off_x, int off_y, const char *p_caption);
#endif
#ifdef __cplusplus
}
#endif
