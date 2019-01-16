#ifdef __cplusplus
extern "C"
{
#endif

#ifndef __BMP2YUV__H
#define __BMP2YUV_H

typedef int   LONG;
typedef int   DWORD;
typedef int   BOOL;
typedef unsigned char  BYTE;
typedef short int      WORD;

#  define BI_RGB       0             /* No compression - straight BGR data */
#  define BI_RLE8      1             /* 8-bit run-length compression */
#  define BI_RLE4      2             /* 4-bit run-length compression */
#  define BI_BITFIELDS 3             /* RGB bitmap with RGB masks */

typedef struct tagRGBQUAD {
	BYTE rgbBlue;
	BYTE rgbGreen;
	BYTE rgbRed;
	BYTE rgbReserved;
}__attribute__ ((packed)) RGBQUAD;

typedef struct tagBITMAPINFOHEADER{
	DWORD      biSize;//4
	LONG        biWidth;//4
	LONG        biHeight;//4
	WORD       biPlanes;//2
	WORD       biBitCount;//2
	DWORD      biCompression;
	DWORD      biSizeImage;
	LONG        biXPelsPerMeter;
	LONG        biYPelsPerMeter;
	DWORD      biClrUsed;
	DWORD      biClrImportant;
}__attribute__ ((packed)) BITMAPINFOHEADER;

typedef struct tagBITMAPFILEHEADER {
	WORD    bfType;
	DWORD   bfSize;
	WORD    bfReserved1;
	WORD    bfReserved2;
	DWORD   bfOffBits;
}__attribute__ ((packed)) BITMAPFILEHEADER;

typedef struct
{
	BITMAPFILEHEADER file_header;
	BITMAPINFOHEADER info_header;
	unsigned long width,height;
	BYTE *rgbData;
	FILE* (* BMPopen)(const char *,const char *);
	bool (* BMPread)(FILE *);
	bool (* Convert2Yuv)(unsigned long,unsigned long,unsigned char*,unsigned char*,unsigned char*,unsigned char *);
	bool (* BMPclose)(FILE *);
}__attribute__ ((packed))BMPSTRUCT;

typedef struct
{
	BYTE *Y,*U,*V;
	FILE* (* YUVopen)(const char *,const char *);
	bool (* YUVwrite)(FILE *);
	bool (* YUVclose)(FILE *);
}__attribute__ ((packed))YUVSTRUCT;

int RGB2YUV(unsigned long w,unsigned long h,unsigned char * rgbData,unsigned char * y,unsigned char * u,unsigned char *v);

extern bool ReadBmpFromBuf(char *bmpBuf);
extern void ReadRGBFromBuf(char *pBmpBuf, BITMAPFILEHEADER &file_h, BITMAPINFOHEADER &info_h, unsigned char *rgbDataOut);

#endif

#ifdef __cplusplus
}
#endif
