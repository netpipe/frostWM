#include "ZSDL_WM.h"
#include "ZWM_ButtonCollection.h"
#include "ZWM_Graph.h"
#include "ztar_module.h"
#include <SDL_image.h>

CZWM_BUTTON_COLLECTION::CZWM_BUTTON_COLLECTION(char* szBmp,SDL_Rect rcButton,
											   int nSizeX,int nSizeY,
											   ZWM_MSGCALLBACK lpCalllback,
											   int nX,int nY)
{
	SDL_Surface*		lpBtnSrf=0;
	rcBtn=rcButton;
	nSX=nSizeX;
	nSY=nSizeY;

	if(CZSDL_WM::lpWM->szTarFile)
	{
		lpBtnSrf=ZLoadTarImage("bckdata.tar","buttonlist/toolbuttons.bmp");
	}
	else
	{
		lpBtnSrf=IMG_Load(szBmp);
	}

	rcWnd.x=nX;
	rcWnd.y=nY;
	rcWnd.w=(nSizeX*40)+16;
	rcWnd.h=(nSizeY*40)+32;
	

	szTitle="Tools";
	nWndType=ZWND_VISIBLE|ZWND_CAPTION|ZWND_SDLSURFACE|ZWND_MAXIMIZEBOX|ZWND_SYSMENU|ZWND_MINIMIZEBOX;
	lpMsgCallback=lpCalllback;
	nClientCol=0xFF;
	nCurSel=-1;

	GetClientRect(&rcClient);

	SDL_PixelFormat* lpPF=CZSDL_WM::lpWM->lpDeskSrf->format;
	lpSurface=SDL_CreateRGBSurface(0,rcClient.w,rcClient.h,
		lpPF->BitsPerPixel,
		lpPF->Rmask,lpPF->Gmask,lpPF->Bmask,lpPF->Amask);
	ZWM_FillRect(lpSurface,0,nClientCol);

	for(int j=0;j<nSY;j++)
	{
		for(int i=0;i<nSX;i++)
		{
			SDL_PixelFormat* lpPF=CZSDL_WM::lpWM->lpDeskSrf->format;
			SDL_Surface* lpNewSurface=SDL_CreateRGBSurface(0,rcBtn.w,rcBtn.h,
				lpPF->BitsPerPixel,
				lpPF->Rmask,lpPF->Gmask,lpPF->Bmask,lpPF->Amask);

			SDL_Rect rcBtnSfSrc={i*32,j*32,rcBtn.w,rcBtn.h};
			SDL_Rect rcBtnSfDst={0,0,rcBtn.w,rcBtn.h};
			SDL_BlitSurface(lpBtnSrf,&rcBtnSfSrc,lpNewSurface,&rcBtnSfDst);
			
			SDL_Rect rcBtnDst={(i*40)+8,(j*40)+8,rcBtn.w,rcBtn.h};
			new CZWM_BUTTON(i+(j*nSX),this,rcBtnDst,
						 0,lpNewSurface);
		}
	}
	SDL_FreeSurface(lpBtnSrf);
	CZSDL_WM::lpWM->RegisterWindow(this);
}