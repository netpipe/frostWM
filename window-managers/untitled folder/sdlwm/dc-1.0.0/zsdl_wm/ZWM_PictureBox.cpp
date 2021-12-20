#include "config.h"
#include <SDL_image.h>
#include <math.h>
#include "ZSDL_WM.h"
#include "ZWM_PictureBox.h"


CZWM_PICTUREBOX::CZWM_PICTUREBOX(int nIndex,CZWM_WINDOW* lpPar,SDL_Rect rcRect):
	nIdx(nIndex)
{
	if(lpPar)
	{
		rcRect.x+=lpPar->rcClient.x;
		rcRect.y+=lpPar->rcClient.y;
	}

	CZSDL_WM* lpWM=CZSDL_WM::lpWM;

	rcWnd=rcRect;
	nWndType=ZWND_VISIBLE|ZWND_SDLSURFACE|ZWND_CHILD;
	lpParent=lpPar;
	nClientCol=0xC0C0C0;

	GetClientRect(&rcClient);

	SDL_PixelFormat* lpPF=CZSDL_WM::lpWM->lpDeskSrf->format;
	lpSurface=SDL_CreateRGBSurface(0,rcClient.w,rcClient.h,
		lpPF->BitsPerPixel,
		lpPF->Rmask,lpPF->Gmask,lpPF->Bmask,lpPF->Amask);
	ZWM_FillRect(lpSurface,0,nClientCol);
	SDL_Rect rcBound={0,0,lpSurface->w,lpSurface->h};
	ZWM_DrawStereoRect(0x00000000,0xFFFFFFFF,rcBound,lpSurface);

	CZSDL_WM::lpWM->RegisterWindow(this);

}

void CZWM_PICTUREBOX::LoadImage(char* szFile)
{
	ZWM_FillRect(lpSurface,0,nClientCol);
	CZSDL_WM::lpWM->AddUpdateRectFromWnd(this);

	SDL_Rect rcBound={0,0,lpSurface->w,lpSurface->h};
	ZWM_DrawStereoRect(0x00000000,0xFFFFFFFF,rcBound,lpSurface);

#ifdef HAVE_LIBSDL_IMAGE
	SDL_Surface* lpImgSurface=IMG_Load(szFile);
#else
	SDL_Surface* lpImgSurface=SDL_LoadBMP(szFile);
#endif
	if(lpImgSurface==0) return;

	Uint32 nPxl=0;
	int nSrc_W=lpImgSurface->w;
	int nSrc_H=lpImgSurface->h;

	int nDst_W=lpSurface->w-2;
	int nDst_H=lpSurface->h-2;
	

	for(int nDY=0;nDY<nDst_H;nDY++)
	{
		for(int nDX=0;nDX<nDst_W;nDX++)
		{
			int nSX=(nSrc_W*nDX)/nDst_W;
			int nSY=(nSrc_H*nDY)/nDst_H;

			SDL_Rect rcSrc={nSX,nSY,1,1};
			SDL_Rect rcDst={nDX+1,nDY+1,1,1};
			SDL_BlitSurface(lpImgSurface,&rcSrc,lpSurface,&rcDst);
		}
	}
	SDL_FreeSurface(lpImgSurface);
}

CZWM_PICTUREBOX::~CZWM_PICTUREBOX()
{

}
