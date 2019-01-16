#ifdef __cplusplus
extern "C"
{
#endif

#ifndef __TTF2BMP__H
#define __TTF2BMP_H

extern TTF_Font * SDL_TTF_Init(int fontSize);
extern int SDL_TTF_Render(TTF_Font * font, char *str, char *name, int *x_res, int *y_res);
extern void SDL_TTF_Deinit(TTF_Font *font);

#endif

#ifdef __cplusplus
}
#endif
