#include "config.h"
#include "ZSDL_WM.h"
#include "ZWM_MusicCtrl.h"
#include "ztar_module.h"
#include "SDL_mixer.h"

int __ZWMMusicCtrlCallback(CZWM_WINDOW* lpWnd,int nMsg,int nParam1,int nParam2)
{
#ifdef HAVE_LIBSDL_MIXER
	CZWM_MUSICCTRL* lpMC=(CZWM_MUSICCTRL*)lpWnd;
	switch(nMsg)
	{
	case ZWM_OPENFILE:
		{
			lpMC->bMC_NewFileOpened=true;
			CZWM_FILENODE* lpFN=(CZWM_FILENODE*)nParam1;
			Mix_HaltMusic();
			if(lpMC->lpMus) Mix_FreeMusic(lpMC->lpMus);
			lpMC->lpMus=Mix_LoadMUS(lpFN->szText);
			lpMC->lpOpenDlg=0;
			return 1;
		}
	case ZWM_BUTTONDOWN:
		switch(nParam1)
		{
		case 0:
			{
				if(lpMC->bMC_NewFileOpened)
				{
					if(lpMC->lpMus)
						Mix_FadeInMusic(lpMC->lpMus,0,2000);
					lpMC->bMC_NewFileOpened=false;
				}
				else
				{
					if(Mix_PausedMusic())
					{
						Mix_ResumeMusic();
					}
					else
					{
						if(lpMC->lpMus)
							Mix_FadeInMusic(lpMC->lpMus,0,2000);
					}
				}
				return 1;
			}
		case 1:
			{
				Mix_HaltMusic();
				return 1;
			}
		case 2:
			{
				Mix_PauseMusic();
				return 1;
			}
		case 3:
			{
				if(!lpMC->lpOpenDlg)
				{
					lpMC->lpOpenDlg=new CZWM_COMDLG_LOSA(".","Open MusicFile",0);
					lpMC->lpOpenDlg->lpParent=lpMC;
				}
			}
		}
		return 1;
	}
#endif
	return 0;

}

void CZWM_MUSICCTRL::Open(char* szFile)
{
#ifdef HAVE_LIBSDL_MIXER
	Mix_HaltMusic();
	if(lpMus) Mix_FreeMusic(lpMus);
	lpMus=Mix_LoadMUS(szFile);
	lpOpenDlg=0;
#endif
}

void CZWM_MUSICCTRL::Play()
{
#ifdef HAVE_LIBSDL_MIXER
	if(lpMus)
	{
		Mix_FadeInMusic(lpMus,0,2000);
		bMC_NewFileOpened=false;
	}
#endif
}

CZWM_MUSICCTRL::CZWM_MUSICCTRL(int nIndex,CZWM_WINDOW* lpPar,SDL_Rect rcBound):
	nIdx(nIndex),lpOpenDlg(0),lpMus(0),bMC_NewFileOpened(0)
{
#ifdef HAVE_LIBSDL_MIXER
	rcBound.w=128;
	rcBound.h=32;
	if(lpPar)
	{
		rcBound.x+=lpPar->rcClient.x;
		rcBound.y+=lpPar->rcClient.y;
	}

	rcWnd=rcBound;
	rcClient=rcBound;	

	nWndType=ZWND_VISIBLE|ZWND_SDLSURFACE|ZWND_CHILD;
	lpMsgCallback=__ZWMMusicCtrlCallback;
	lpParent=lpPar;
	nClientCol=0xFF;

	GetClientRect(&rcClient);

	SDL_PixelFormat* lpPF=CZSDL_WM::lpWM->lpDeskSrf->format;
	lpSurface=SDL_CreateRGBSurface(0,rcClient.w,rcClient.h,
		lpPF->BitsPerPixel,
		lpPF->Rmask,lpPF->Gmask,lpPF->Bmask,lpPF->Amask);
	ZWM_FillRect(lpSurface,0,nClientCol);

	SDL_Rect rcB_Play={0,0,32,32};
	new CZWM_BUTTON(0,this,rcB_Play,0,ZLoadTarImage("wm.tar","mc_play.bmp"));
	
	SDL_Rect rcB_Stop={32,0,32,32};
	new CZWM_BUTTON(1,this,rcB_Stop,0,ZLoadTarImage("wm.tar","mc_stop.bmp"));

	SDL_Rect rcB_Pause={64,0,32,32};
	new CZWM_BUTTON(2,this,rcB_Pause,0,ZLoadTarImage("wm.tar","mc_pause.bmp"));

	SDL_Rect rcB_Open={96,0,32,32};
	new CZWM_BUTTON(3,this,rcB_Open,0,ZLoadTarImage("wm.tar","mc_open.bmp"));

	CZSDL_WM::lpWM->RegisterWindow(this);
#endif
}

CZWM_MUSICCTRL::~CZWM_MUSICCTRL()
{

}