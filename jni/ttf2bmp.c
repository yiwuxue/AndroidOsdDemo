#include<stdio.h>
#include<time.h>
#include <android/log.h>
#include "SDL.h"
#include "SDL_ttf.h"
#include "ttf2bmp.h"
#include "OsdLog.h"

#define  FONT_LIB "/storage/emulated/0/osd/simblack.ttf"

TTF_Font * SDL_TTF_Init(int fontSize)
{
    TTF_Font *font;

	if (TTF_Init() < 0 )
    {
        LOGE("Couldn't initialize TTF: %s\n", SDL_GetError());
        SDL_Quit();
		return NULL;
    }

	font = TTF_OpenFont(FONT_LIB, fontSize);
	if ( font == NULL )
	{
		LOGE("Couldn't load %d pt font from %s: %s\n",18,"ptsize", SDL_GetError());
		return NULL;
	}
	return font;
}

int SDL_TTF_Render(TTF_Font * font, char *str, char *name, int *x_res, int *y_res)
{
    SDL_PixelFormat *fmt;
    SDL_Surface *text, *temp;
	SDL_Color forecol = { 0xff, 0xff, 0xff, 0xff };
	char path[100] = "/storage/emulated/0/osd/";

	text = TTF_RenderUTF8_Solid(font, str, forecol);
	if(!text)
	{
		LOGE("TTF_RenderUTF8_Solid failed\n");
		return -1;
	}
	fmt = (SDL_PixelFormat*)malloc(sizeof(SDL_PixelFormat));
	memset(fmt,0,sizeof(SDL_PixelFormat));
	fmt->BitsPerPixel = 16;
	fmt->BytesPerPixel = 2;
	temp = SDL_ConvertSurface(text, fmt, 0);
	if(!temp)
	{
		LOGE("SDL_ConvertSurface failed\n");
		SDL_FreeSurface(text);
		return -2;
	}
	strcat(path, name);
	snprintf(path + strlen(path), 5, ".bmp");
	if(SDL_SaveBMP(temp, path)){
		LOGE("SDL_SaveBMP failed\n");
		SDL_FreeSurface(text);
		SDL_FreeSurface(temp);
		return -3;
	}

	*x_res = temp->w;
	*y_res = temp->h;

	SDL_FreeSurface(text);
	SDL_FreeSurface(temp);

	return 0;
}

void SDL_TTF_Deinit(TTF_Font *font)
{
	TTF_CloseFont(font);
	TTF_Quit();
}
