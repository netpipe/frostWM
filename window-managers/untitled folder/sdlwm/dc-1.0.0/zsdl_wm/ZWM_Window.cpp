#include "ZWM_Window.h"
#include "ZSDL_WM.h"

void CZWM_WINDOW::SetVisible(bool bState)
{
	if(bState)
	{
		if(!(nWndType&ZWND_VISIBLE)) 
		{
			nWndType|=ZWND_VISIBLE;
		}
	}
	else
	{
		if(nWndType&ZWND_VISIBLE) nWndType^=ZWND_VISIBLE;
	}
	CZSDL_WM::lpWM->AddUpdateRectFromWnd(this);
}

void CZWM_WINDOW::Resize(int nW,int nH)
{
	if(nW==rcWnd.w&&nH==rcWnd.h) return;

	if(nW!=-1) rcWnd.w=nW;
	if(nH!=-1) rcWnd.h=nH;
	GetClientRect(&rcClient);

	if(nWndType&ZWND_SDLSURFACE)
	{
		SDL_PixelFormat* lpPF=lpSurface->format;
		SDL_Surface* lpNewSrf=
			SDL_CreateRGBSurface(0,rcClient.w,rcClient.h,
				lpPF->BitsPerPixel,
				lpPF->Rmask,lpPF->Gmask,lpPF->Bmask,lpPF->Amask);

		ZWM_FillRect(lpNewSrf,NULL,nClientCol);
		SDL_BlitSurface(lpSurface,NULL,lpNewSrf,NULL);
		SDL_FreeSurface(lpSurface);
		lpSurface=lpNewSrf;
	}
	CZSDL_WM::lpWM->AddUpdateRectFromWnd(this);
}

void CZWM_WINDOW::GetClientRect(SDL_Rect* rcC)
{
	SDL_Rect& rcCR=*rcC;
	rcCR=rcWnd;
	if(nWndType&ZWND_BORDER)
	{
		rcCR.x+=3;
		rcCR.y+=3;
		rcCR.w-=6;
		rcCR.h-=6;
	}
	if(nWndType&ZWND_TITLE)
	{
		rcCR.y+=19;
		rcCR.h-=19;
	}
}

CZWM_WINDOW::CZWM_WINDOW():
	lpSurface(0),lpMsgCallback(0),lpPrivateData(0),nFontCol(0),nClientCol(0)
{

}

CZWM_WINDOW::CZWM_WINDOW(const char* szCaption,ZWM_MSGCALLBACK lpMCB,int nType,CZWM_WINDOW* lpParWnd,
						 Uint32 nClientColor,Uint32 nFontColor,SDL_Rect rcW):
	lpMsgCallback(lpMCB),nClientCol(nClientColor),nFontCol(nFontColor),
	lpSurface(0),lpPrivateData(0)
{
	szTitle=szCaption;
	rcWnd=rcW;
	nWndType=nType;
	lpParent=lpParWnd;

	GetClientRect(&rcClient);
}

CZWM_WINDOW::~CZWM_WINDOW()
{
	llChildList.DeleteList();
	if(lpSurface) SDL_FreeSurface(lpSurface);
}