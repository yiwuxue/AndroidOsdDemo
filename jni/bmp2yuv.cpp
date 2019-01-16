#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "bmp2yuv.h"
#include <iostream>
#include <string>
#include "OsdLog.h"

using namespace std;

extern BMPSTRUCT bmp;
extern YUVSTRUCT yuv;

bool MakePalette(char *pBmpBuf,BITMAPFILEHEADER &file_h,BITMAPINFOHEADER & info_h,RGBQUAD *pRGB_out);

void ReadRGBFromBuf(char *pBmpBuf, BITMAPFILEHEADER & file_h, BITMAPINFOHEADER & info_h, unsigned char * rgbDataOut)
{
	unsigned long Loop, iLoop, jLoop, width, height, w, h;
	unsigned char mask, *Index_Data, * Data;

	if (((info_h.biWidth/8*info_h.biBitCount)%4) == 0)
		w = info_h.biWidth;
	else
		w = (info_h.biWidth*info_h.biBitCount+31)/32*4;
	if ((info_h.biHeight%2) == 0)
		h = info_h.biHeight;
	else
		h = info_h.biHeight - 1;

	width = w/8 * info_h.biBitCount;
	height = h;

	//LOGD("in %s, width:%d height:%d", __func__, width, height);
	Index_Data = (unsigned char *)malloc(height * width);
	Data = (unsigned char *)malloc(height * width);

	memcpy(Index_Data, (pBmpBuf + file_h.bfOffBits), height * width);
	for ( iLoop = 0; iLoop < height; iLoop ++)
		for (jLoop = 0; jLoop < width; jLoop++)
		{
 			Data[iLoop * width + jLoop] = Index_Data[(height - iLoop - 1) * width + jLoop];
		}

	switch(info_h.biBitCount)
	{
	case 24:
		memcpy(rgbDataOut, Data, height * width);
		if(Index_Data)
			free(Index_Data);
		if(Data)
			free(Data);
		return;
	case 16:
		if(info_h.biCompression == BI_RGB)
		{
			for (Loop = 0;Loop < height * width; Loop +=2)
			{
				*rgbDataOut = (Data[Loop]&0x1F)<<3;
				*(rgbDataOut + 1) = ((Data[Loop]&0xE0)>>2) + ((Data[Loop+1]&0x03)<<6);
				*(rgbDataOut + 2) = (Data[Loop+1]&0x7C)<<1;

				rgbDataOut +=3;
			}
		}
		if(Index_Data)
			free(Index_Data);
		if(Data)
			free(Data);
		return;
	default:
		RGBQUAD *pRGB = (RGBQUAD *)malloc(sizeof(RGBQUAD)*(unsigned char)pow(float(2),info_h.biBitCount));
		int temp = sizeof(pRGB);
		if(!MakePalette(pBmpBuf, file_h, info_h, pRGB))
			LOGD("in %s, No palette!\n", __func__);

		for (Loop=0;Loop<height*width;Loop++)
		{

			switch(info_h.biBitCount)
			{
			case 1:
				mask = 0x80;
				break;
			case 2:
				mask = 0xC0;
				break;
			case 4:
				mask = 0xF0;
				break;
			case 8:
				mask = 0xFF;
			}

			int shiftCnt = 1;

			while (mask)
			{
				unsigned char index = mask == 0xFF ? Data[Loop] : ((Data[Loop] & mask)>>(8 - shiftCnt * info_h.biBitCount));
				* rgbDataOut = pRGB[index].rgbBlue;
				* (rgbDataOut+1) = pRGB[index].rgbGreen;
				* (rgbDataOut+2) = pRGB[index].rgbRed;

				if(info_h.biBitCount == 8)
					mask =0;
				else
					mask >>= info_h.biBitCount;
				rgbDataOut+=3;
				shiftCnt ++;
			}
		}
		if(Index_Data)
			free(Index_Data);
		if(Data)
			free(Data);
		// 		if(pRGB)
		// 			free(pRGB);
	}
}

bool MakePalette(char *pBmpBuf, BITMAPFILEHEADER &file_h,BITMAPINFOHEADER & info_h,RGBQUAD *pRGB_out)
{
	if ((file_h.bfOffBits - sizeof(BITMAPFILEHEADER) - info_h.biSize) == sizeof(RGBQUAD)*pow(float(2),info_h.biBitCount))
	{
		//fseek(pFile,sizeof(BITMAPFILEHEADER)+info_h.biSize,0);
		//fread(pRGB_out,sizeof(RGBQUAD),(unsigned int)pow(float(2),info_h.biBitCount),pFile);
		memcpy(pRGB_out, pBmpBuf + sizeof(BITMAPFILEHEADER)+info_h.biSize, 4*(unsigned int)pow(float(2),info_h.biBitCount));
		return true;
	}
	else
		return false;
}

float RGBYUV02990[256],RGBYUV05870[256],RGBYUV01140[256];
float RGBYUV01684[256],RGBYUV03316[256];
float RGBYUV04187[256],RGBYUV00813[256];

void initLookupTable();
int RGB2YUV(unsigned long w, unsigned long h, unsigned char *rgbData, unsigned char * y, unsigned char * u, unsigned char *v)
{
	initLookupTable();//初始化查找表
	unsigned char *ytemp = NULL;
	unsigned char *utemp = NULL;
	unsigned char *vtemp = NULL;
	utemp = (unsigned char *)malloc(w*h);
	vtemp = (unsigned char *)malloc(w*h);

	if(!y || !u || !v || !rgbData)
	{
		return -1;
	}

	unsigned long i,nr,ng,nb,nSize;
	//对每个像素进行 rgb -> yuv的转换
	for (i=0,nSize=0;nSize<w*h*3;nSize+=3)
	{
		nb = rgbData[nSize];
		ng = rgbData[nSize+1];
		nr = rgbData[nSize+2];
		y[i] = (unsigned char)(RGBYUV02990[nr]+RGBYUV05870[ng]+RGBYUV01140[nb]);
		utemp[i] = (unsigned char)(-RGBYUV01684[nr]-RGBYUV03316[ng]+nb/2+128);
		vtemp[i] = (unsigned char)(nr/2-RGBYUV04187[ng]-RGBYUV00813[nb]+128);
		i++;
	}
	//对u信号及v信号进行采样
	int k = 0;
	for (i = 0; i < h; i += 2)
		for(unsigned long j = 0; j < w; j += 2)
		{
			u[k]=(utemp[i*w+j]+utemp[(i+1)*w+j]+utemp[i*w+j+1]+utemp[(i+1)*w+j+1])/4;
			v[k]=(vtemp[i*w+j]+vtemp[(i+1)*w+j]+vtemp[i*w+j+1]+vtemp[(i+1)*w+j+1])/4;
			k++;
		}
	//对y、u、v 信号进行抗噪处理
	for (i = 0; i < w * h; i++)
	{
		if(y[i] < 16)
			y[i] = 16;
		if(y[i] > 235)
			y[i] = 235;
	}

	for(i = 0; i < h * w / 4; i++)
	{
		if(u[i]<16)
			u[i] = 16;
		if(v[i]<16)
			v[i] = 16;
		if(u[i]>240)
			u[i] = 240;
		if(v[i]>240)
			v[i] = 240;
	}

	if(utemp)
		free(utemp);
	if(vtemp)
		free(vtemp);

	return 0;
}

void initLookupTable()
{
	for (int i=0;i<256;i++)
	{
		RGBYUV02990[i] = (float)0.2990 * i;
		RGBYUV05870[i] = (float)0.5870 * i;
		RGBYUV01140[i] = (float)0.1140 * i;
		RGBYUV01684[i] = (float)0.1684 * i;
		RGBYUV03316[i] = (float)0.3316 * i;
		RGBYUV04187[i] = (float)0.4187 * i;
		RGBYUV00813[i] = (float)0.0813 * i;
	}
}

bool ReadBmpFromBuf(char *bmpBuf)
{
	//LOGD("sizeof(BITMAPFILEHEADER):%lu word:%lu dword:%lu long:%lu\n", sizeof(BITMAPFILEHEADER), sizeof(WORD), sizeof(DWORD), sizeof(LONG));
	memcpy(&bmp.file_header, bmpBuf, sizeof(BITMAPFILEHEADER));
	if (bmp.file_header.bfType != 0x4D42)
	{
		LOGE("Not bmp file!\n\n");
		return false;
	}
	bmpBuf += sizeof(BITMAPFILEHEADER);

	//LOGD("sizeof(BITMAPINFOHEADER):%lu\n", sizeof(BITMAPINFOHEADER));
	memcpy(&bmp.info_header, bmpBuf, sizeof(BITMAPINFOHEADER));
	if (((bmp.info_header.biWidth/8*bmp.info_header.biBitCount)%4) == 0)
		bmp.width = bmp.info_header.biWidth;
	else
		bmp.width = (bmp.info_header.biWidth*bmp.info_header.biBitCount+31)/32*4;

	if ((bmp.info_header.biHeight%2) == 0)
		bmp.height = bmp.info_header.biHeight;
	else
		bmp.height = bmp.info_header.biHeight + 1;

	//LOGD("bmp.width:%lu, height:%lu\n", bmp.width, bmp.height);

	bmp.rgbData = (unsigned char *) malloc(bmp.height *bmp.width * 3);
	memset(bmp.rgbData, 0, bmp.height*bmp.width*3);
	yuv.Y = (unsigned char * )malloc(bmp.height * bmp.width);
	yuv.U = (unsigned char * )malloc((bmp.height * bmp.width)/4);
	yuv.V = (unsigned char * )malloc((bmp.height * bmp.width)/4);

	//LOGD("This is a %d bits image!\n",bmp.info_header.biBitCount);
	//LOGD("bmp size: \t%d X %d\n\n", bmp.info_header.biWidth, bmp.info_header.biHeight);

	ReadRGBFromBuf(bmpBuf, bmp.file_header, bmp.info_header, bmp.rgbData);

	return true;
}
