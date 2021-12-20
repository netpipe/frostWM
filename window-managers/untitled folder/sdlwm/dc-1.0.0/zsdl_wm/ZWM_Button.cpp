#include <stdlib.h>
#include <string.h>
#include "ZSDL_WM.h"
#include "ZWM_Button.h"

int __ZWMButtonCallback(CZWM_WINDOW* lpWnd,int nMsg,int nParam1,int nParam2)
{
	CZWM_BUTTON* lpBtn=(CZWM_BUTTON*)lpWnd;

	if(lpBtn->lpCallbackHook)
		lpBtn->lpCallbackHook(lpWnd,nMsg,nParam1,nParam2);
	
	switch(nMsg)
	{
	case ZWM_MOUSEBUTTONDOWN:
		{
			switch(nParam2)
			{
			case SDL_BUTTON_LEFT:
				{
					CZSDL_WM::lpClActiveWnd=lpBtn;
					lpBtn->bPressed=true;
					lpBtn->Update();
					return 1;
				}
			}
		}
		return 1;
	case ZWM_MOUSEBUTTONUP:
		{
			switch(nParam2)
			{
			case SDL_BUTTON_LEFT:
				{
					if(lpBtn->bPressed)
					{
						lpBtn->bPressed=false;
						if(lpWnd->lpParent->lpMsgCallback)
						{
							lpWnd->lpParent->lpMsgCallback(
								lpWnd->lpParent,ZWM_BUTTONDOWN,lpBtn->nIdx,0);
						}
						lpBtn->Update();
						return 1;
					}
				}
				return 1;
			}
		}
		return 1;
	case ZWM_GETMOUSEFOCUS:
		lpBtn->bHoverState=true;
		lpBtn->Update();
		return 1;
	case ZWM_LOSEMOUSEFOCUS:
		if(lpBtn->bPressed)
		{
			lpBtn->bPressed=false;
		}
		lpBtn->bHoverState=false;
		lpBtn->Update();
		return 1;
	}

	return 0;
}

CZWM_BUTTON::CZWM_BUTTON(int nIndex,CZWM_WINDOW* lpPar,SDL_Rect rcButton,
						 char* szCaption,SDL_Surface* lpBtnSurface):
	bHoverState(false),nIdx(nIndex),bPressed(false),lpBtnSrf(lpBtnSurface),
	lpCallbackHook(0)
{
	if(lpPar)
	{
		rcButton.x+=lpPar->rcClient.x;
		rcButton.y+=lpPar->rcClient.y;
	}

	rcWnd=rcButton;
	rcClient=rcButton;	

	szTitle=szCaption;
	nWndType=ZWND_VISIBLE|ZWND_SDLSURFACE|ZWND_CHILD;
	lpMsgCallback=__ZWMButtonCallback;
	lpParent=lpPar;
	nClientCol=0xFF;

	GetClientRect(&rcClient);

	SDL_PixelFormat* lpPF=CZSDL_WM::lpWM->lpDeskSrf->format;
	lpSurface=SDL_CreateRGBSurface(0,rcClient.w,rcClient.h,
		lpPF->BitsPerPixel,
		lpPF->Rmask,lpPF->Gmask,lpPF->Bmask,lpPF->Amask);
	ZWM_FillRect(lpSurface,0,nClientCol);

	Update();
	CZSDL_WM::lpWM->RegisterWindow(this);
}

void CZWM_BUTTON::Update()
{
	ZWM_FillRect(lpSurface,0,(bHoverState)?0xB0B0E0:0xE0E0E0);
	SDL_Rect rcBorder={0,0,lpSurface->w,lpSurface->h};
	
	SDL_Rect rcInner={rcBorder.x+1,rcBorder.y+1,rcBorder.w-2,rcBorder.h-2};
	SDL_Rect rcSrc={0,0,lpSurface->w,lpSurface->h};
	SDL_Rect rcInnerBmp={rcInner.x+1,rcInner.y+1,rcInner.w-2,rcInner.h-2};
	if(lpBtnSrf)
	{
		SDL_Rect rcDst=rcBorder;
		SDL_BlitSurface(lpBtnSrf,&rcSrc,lpSurface,&rcDst);
	}

	if(szTitle)
	{
		CZSDL_WM::lpWM->lpFont->DrawString(false,0x00,lpSurface,
			rcWnd.w/2-(strlen(szTitle)*4)-2,
			rcWnd.h/2-4,
			20,szTitle);
	}

	if(bPressed)
	{
		ZWM_DrawStereoRect(0x000000,0xFFFFFF,rcBorder,lpSurface);
		ZWM_DrawStereoRect(0x000000,0xFFFFFF,rcInner,lpSurface);
	}
	else
	{
		ZWM_DrawStereoRect((bHoverState)?0xB0B0E0:0xFFFFFF,0x000000,rcBorder,lpSurface);
		ZWM_DrawStereoRect((bHoverState)?0xB0B0E0:0xFFFFFF,0x000000,rcInner,lpSurface);
	}
	CZSDL_WM::lpWM->AddUpdateRectFromWnd(this);
}

CZWM_BUTTON::~CZWM_BUTTON()
{
	if(lpBtnSrf)
	{
		SDL_FreeSurface(lpBtnSrf);
		lpBtnSrf=0;
	}
}