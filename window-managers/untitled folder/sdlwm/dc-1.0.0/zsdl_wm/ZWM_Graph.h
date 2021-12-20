#ifndef _ZWM_GRAPH_H
#define _ZWM_GRAPH_H

#include <SDL.h>

void ZWM_DrawPixel(Uint32 nCol,int nX,int nY,SDL_Surface* lpSurface);
Uint32 ZWM_GetPixel(int nX,int nY,SDL_Surface* lpSurface);
void ZWM_DrawHLine(Uint32 nCol,int nX1,int nX2,int nY,SDL_Surface* lpSurface);
void ZWM_DrawVLine(Uint32 nCol,int nX,int nY1,int nY2,SDL_Surface* lpSurface);
void ZWM_DrawRect(Uint32 nCol,SDL_Rect rcRect,SDL_Surface* lpSurface);
void ZWM_DrawStereoRect(Uint32 nCol1,Uint32 nCol2,SDL_Rect rcRect,SDL_Surface* lpSurface);
void ZWM_FillRect(SDL_Surface* lpSurface,SDL_Rect* lpRect,Uint32 nCol);
Uint32 ZWM_MapRGBA(SDL_PixelFormat *fmt,Uint32 nCol);
bool ZWM_LoadScaledImageToSrf(char* szFile,SDL_Surface* lpSrf);

#endif
