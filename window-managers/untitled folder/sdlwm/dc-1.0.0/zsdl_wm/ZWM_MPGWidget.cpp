#include "config.h"
#ifdef HAVE_LIBSDL_IMAGE
	#include <SDL_image.h>
#endif
#include <math.h>
#include "ZSDL_WM.h"
#include "ZWM_MPGWidget.h"

#ifdef HAVE_LIBSMPEG

int __ZWMMPGWidgetCallback(CZWM_WINDOW* lpWnd,int nMsg,int nParam1,int nParam2)
{
	CZWM_MPGWIDGET* lpMPGW=(CZWM_MPGWIDGET*)lpWnd;

	switch(nMsg)
	{
		case ZWM_PAINT:
			{
				CZSDL_WM::lpWM->AddUpdateRectFromWnd(lpWnd);
			}
	}

	return 0;
}

CZWM_MPGWIDGET::CZWM_MPGWIDGET(int nIndex,CZWM_WINDOW* lpPar,SDL_Rect rcRect):
	nIdx(nIndex),lpMPG(0)
{
	if(lpPar)
	{
		rcRect.x+=lpPar->rcClient.x;
		rcRect.y+=lpPar->rcClient.y;
	}

	CZSDL_WM* lpWM=CZSDL_WM::lpWM;

	rcWnd=rcRect;
	nWndType=ZWND_VISIBLE|ZWND_SDLSURFACE|ZWND_CHILD|ZWND_FRMUPDATE;
	lpMsgCallback=__ZWMMPGWidgetCallback;
	lpParent=lpPar;
	nClientCol=0xC0C0C0;

	GetClientRect(&rcClient);

	SDL_PixelFormat* lpPF=CZSDL_WM::lpWM->lpDeskSrf->format;
	lpSurface=SDL_CreateRGBSurface(0,rcClient.w,rcClient.h,
		lpPF->BitsPerPixel,
		lpPF->Rmask,lpPF->Gmask,lpPF->Bmask,0);
	ZWM_FillRect(lpSurface,0,nClientCol);
	SDL_Rect rcBound={0,0,lpSurface->w,lpSurface->h};
	ZWM_DrawStereoRect(0x00000000,0xFFFFFFFF,rcBound,lpSurface);

	CZSDL_WM::lpWM->RegisterWindow(this);
}

void CZWM_MPGWIDGET::LoadMPG(char *szFile,bool bAutoSize)
{
	if(lpMPG)
	{
		SMPEG_stop(lpMPG);
		SMPEG_delete(lpMPG);
	}

	lpMPG=SMPEG_new(szFile,&mpgInfo,0);

	if(bAutoSize) Resize(mpgInfo.width,mpgInfo.height);

	SMPEG_enableaudio(lpMPG,0);
	SMPEG_enablevideo(lpMPG,true);
	SMPEG_setvolume(lpMPG,50);
	SMPEG_setdisplay(lpMPG,lpSurface,0,0);
	SMPEG_scaleXY(lpMPG,lpSurface->w,lpSurface->h);

	if(true) 
	{
        SDL_AudioSpec audiofmt;
        Uint16 format;
        int freq,channels;

        Mix_QuerySpec(&freq,&format,&channels);
        audiofmt.format=format;
        audiofmt.freq=freq;
        audiofmt.channels=channels;
        SMPEG_actualSpec(lpMPG,&audiofmt);

        Mix_HookMusic(SMPEG_playAudioSDL,lpMPG);
        SMPEG_enableaudio(lpMPG,1);
	}

	SMPEG_play(lpMPG);
}

CZWM_MPGWIDGET::~CZWM_MPGWIDGET()
{
	if(lpMPG)
	{
		SMPEG_stop(lpMPG);
		SMPEG_delete(lpMPG);
	}
}

#endif
