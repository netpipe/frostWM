#include "config.h"
#include <stdlib.h>

#ifdef WIN32
#include <direct.h>
#else
#include <unistd.h>
#endif

#include <string.h>
#include "ZWM_Debug.h"
#include "ZSDL_WM.h"
#include "ZWM_Taskbar.h"
#include "SDL_mixer.h"

int				CZSDL_WM::nUpdateRects=0;
SDL_Rect		CZSDL_WM::rcUpdateList[256]={{0,0,0,0}};
bool			CZSDL_WM::bFullScreenUpdate=true;
CZSDL_WM*		CZSDL_WM::lpWM=0;
CZWM_WINDOW*	CZSDL_WM::lpMovingWnd=0;
CZWM_WINDOW*	CZSDL_WM::lpClActiveWnd=0;
char			CZSDL_WM::szWM_HDir[256]={0};
CZWM_WINDOW*	CZSDL_WM::lpMF_Parent=0;
CZWM_WINDOW*	CZSDL_WM::lpMF_Child=0;
CZWM_WINDOW*	CZSDL_WM::lpMF_TM=0;
SDL_Surface*	CZSDL_WM::lpDeskSrf=0;
CZWM_LLIST		CZSDL_WM::llWnd;
CZWM_WINDOW*	CZSDL_WM::lpActiveWnd=0;
CZWM_TASKBAR	CZSDL_WM::tbTaskbar;

CZSDL_WM::CZSDL_WM(Uint32 nDesktopColor,Uint32 nBorder1Color,Uint32 nBorder2Color,
				   Uint32 nTitleBarCol1,Uint32 nTitleBarCol2,
				   SDL_Rect rcDesktop,int nBPP,Uint32 nSDLFlags,
				   CZSDL_BITFONT* lpFnt):
	nDeskCol(nDesktopColor),lpFont(lpFnt),llMsg(),rcDesk(rcDesktop),
	nBorder1Col(nBorder1Color),nBorder2Col(nBorder2Color),
	nTBCol1(nTitleBarCol1),nTBCol2(nTitleBarCol2),szTarFile(0),
	nDepthBPP(nBPP),nSDLVideoFlags(nSDLFlags)
{
	lpWM=this;

	getcwd(szWM_HDir,256);
	strcat(szWM_HDir,"/");

	lpDeskSrf=SDL_SetVideoMode(rcDesk.w,rcDesk.h,nBPP,
		nSDLFlags);
	ZWM_LoadScaledImageToSrf("ZWM.bmp",lpDeskSrf);
	SDL_Flip(lpDeskSrf);

#ifdef HAVE_LIBSDL_MIXER
	int				nAudioRate		= 22050;
	unsigned short	nAudioFormat	= AUDIO_S16;
	int				nAudioChannels	= 2;
	int				nAudioBuffers	= 4096;

	Mix_OpenAudio(nAudioRate,nAudioFormat,nAudioChannels,nAudioBuffers);
	Mix_QuerySpec(&nAudioRate,&nAudioFormat,&nAudioChannels);
	Mix_SetMusicCMD(getenv("MUSIC_CMD"));
#endif

	tbTaskbar.CreateTB(0xC0C0C0,lpFont,&llWnd);

	rcFSWnd=rcDesktop;
	rcFSWnd.h-=36;
	
}

CZSDL_WM::~CZSDL_WM()
{
	ZSDLWM_SRELEASE(lpFont);
	llWnd.DeleteList();
}

CZWM_WINDOW* CZSDL_WM::ZWMCreateWindow(const char* szCaption,ZWM_MSGCALLBACK lpMCB,int nType,CZWM_WINDOW* lpParWnd,
						 Uint32 nClientColor,Uint32 nFontColor,SDL_Rect rcW)
{
	ZWMDBG_Print("ZWMCreateWindow(\"%s\")\n",szCaption);
	CZWM_WINDOW* lpNewWnd=new CZWM_WINDOW(szCaption,lpMCB,nType,lpParWnd,nClientColor,nFontColor,rcW);
	
	if(nType&ZWND_SDLSURFACE)
	{
		lpNewWnd->lpSurface=SDL_CreateRGBSurface(0,lpNewWnd->rcClient.w,lpNewWnd->rcClient.h,
			lpDeskSrf->format->BitsPerPixel,
			lpDeskSrf->format->Rmask,
			lpDeskSrf->format->Gmask,
			lpDeskSrf->format->Bmask,
			lpDeskSrf->format->Amask);
		ZWM_FillRect(lpNewWnd->lpSurface,NULL,lpNewWnd->nClientCol);
	}
	bFullScreenUpdate=true;
	return lpNewWnd;
}

void CZSDL_WM::RegisterWindow(CZWM_WINDOW* lpWnd)
{
	if((lpWnd->nWndType&ZWND_CHILD)&&(lpWnd->lpParent))
	{
		lpWnd->lpParent->llChildList.AddNode(lpWnd);
	}
	else
	{
		if(lpWnd->nWndType&ZWND_TOPMOST) llWnd.AddNode(lpWnd);
		else
		{
			llWnd.AddAfter(GetFirstNonTopmostWndFromTop(),lpWnd);
		}

		if(lpActiveWnd) 
			AddUpdateRectFromWnd(lpActiveWnd);
		lpActiveWnd=lpWnd;
		tbTaskbar.UpdateTasks(lpActiveWnd);
	}
}

bool ZWM_PtInRect(int nX,int nY,SDL_Rect* lpRect)
{
	if(nX<=lpRect->x||nY<=lpRect->y
		||nX>(lpRect->x+lpRect->w)||nY>(lpRect->y+lpRect->h)) 
		return false; else return true;
}

void CZSDL_WM::MinimizeWindow(CZWM_WINDOW* lpWnd,bool bMinimize)
{
	if(bMinimize)
	{
		lpWnd->nWndType|=ZWND_MINIMIZED;
	}
	else lpWnd->nWndType^=ZWND_MINIMIZED;

	AddUpdateRect(lpWnd->rcWnd);
}

void CZSDL_WM::AddUpdateRect(SDL_Rect rcRect)
{
	if(bFullScreenUpdate) return;
	if(rcRect.x==0&&rcRect.y==0&&rcRect.w==0&&rcRect.h==0) 
	{
		bFullScreenUpdate=true;
		return;
	}

	if(rcRect.x<0) 
	{
		rcRect.w-=rcRect.x;
		rcRect.x=0;
	}
	if(rcRect.y<0) 
	{
		rcRect.h-=rcRect.y;
		rcRect.y=0;
	}
	if(rcRect.x+rcRect.w>0+lpDeskSrf->w)
	{
		rcRect.w-=(rcRect.x+rcRect.w)-lpDeskSrf->w;
	}
	if(rcRect.y+rcRect.h>0+lpDeskSrf->h)
	{
		rcRect.h-=(rcRect.y+rcRect.h)-lpDeskSrf->h;
	}
	rcUpdateList[nUpdateRects++]=rcRect;
}

void CZSDL_WM::AddUpdateRectFromWnd(CZWM_WINDOW* lpWnd)
{
	AddUpdateRect(lpWnd->rcWnd);
}

void CZSDL_WM::MaximizeWindow(CZWM_WINDOW* lpWnd,bool bMaximize)
{
	if(bMaximize)
	{
		lpWnd->rcTmp=lpWnd->rcWnd;
		lpWnd->rcWnd=rcFSWnd;
		lpWnd->nWndType|=ZWND_MAXIMIZED;
		AddUpdateRect(lpWnd->rcWnd);
	}
	else 
	{
		AddUpdateRect(lpWnd->rcWnd);
		lpWnd->rcWnd=lpWnd->rcTmp;
		lpWnd->nWndType^=ZWND_MAXIMIZED;
	}
	lpWnd->GetClientRect(&lpWnd->rcClient);

	if(lpWnd->nWndType&ZWND_SDLSURFACE)
	{
		SDL_Surface* lpNewSrf=
			SDL_CreateRGBSurface(0,lpWnd->rcClient.w,lpWnd->rcClient.h,
				lpDeskSrf->format->BitsPerPixel,
				lpDeskSrf->format->Rmask,
				lpDeskSrf->format->Gmask,
				lpDeskSrf->format->Bmask,
				lpDeskSrf->format->Amask);
		ZWM_FillRect(lpNewSrf,NULL,lpWnd->nClientCol);
		SDL_BlitSurface(lpWnd->lpSurface,NULL,lpNewSrf,NULL);
		SDL_FreeSurface(lpWnd->lpSurface);
		lpWnd->lpSurface=lpNewSrf;
	}
}

void CZSDL_WM::GetMFocusedFromList(int nX,int nY,CZWM_WINDOW** lpFW,
								  CZWM_LLIST* lpList)
{
	CZWM_WINDOW* lpCur=(CZWM_WINDOW*)lpList->lpLastNode;
	for(unsigned int i=0;i<lpList->nNumNodes;i++)
	{
		SDL_Rect rcVWnd=lpCur->rcWnd;

		if((!(lpCur->nWndType&ZWND_MINIMIZED))&&
			(lpCur->nWndType&ZWND_VISIBLE)&&
			(ZWM_PtInRect(nX,nY,&rcVWnd)))
		{
			*lpFW=lpCur;
			if(lpCur->llChildList.nNumNodes)
			{
				GetMFocusedFromList(nX,nY,lpFW,&lpCur->llChildList);
			}
			return;
		}
		lpCur=(CZWM_WINDOW*)lpCur->lpPrevNode;
	}
}

bool CZSDL_WM::GetMouseFocusedWnd(int nX,int nY,
		CZWM_WINDOW** lpWnd,CZWM_WINDOW** lpChild)
{
	*lpWnd=0;
	*lpChild=0;
	if(lpClActiveWnd) 
	{
		*lpChild=lpClActiveWnd;
		*lpWnd=*lpChild;
		while((*lpWnd)->lpParent)
		{
			(*lpWnd)=(*lpWnd)->lpParent;
		}
		return false;
	}
	bool bClient=false;
	CZWM_WINDOW* lpCurWnd=(CZWM_WINDOW*)llWnd.lpLastNode;
	for(unsigned int i=0;i<llWnd.nNumNodes;i++)
	{
		if((!(lpCurWnd->nWndType&ZWND_MINIMIZED))&&
			(lpCurWnd->nWndType&ZWND_VISIBLE)&&
			(ZWM_PtInRect(nX,nY,&lpCurWnd->rcWnd)))
		{
			if(ZWM_PtInRect(nX,nY,&lpCurWnd->rcClient)) bClient=true;
			*lpWnd=lpCurWnd;
			
			if(lpCurWnd->llChildList.nNumNodes)
			{
				GetMFocusedFromList(
					nX,nY,lpChild,&lpCurWnd->llChildList);
			}
			return bClient;
		}
		lpCurWnd=(CZWM_WINDOW*)lpCurWnd->lpPrevNode;
	}
	return bClient;
}

bool CZSDL_WM::PollEvent(ZWMEVENT* event)
{
	static int			nMovWndX=0;
	static int			nMovWndY=0;
	SDL_Event			sdl_event;

	static bool			bMF_ClientHit=false;

	int nMX,nMY;
	SDL_GetMouseState(&nMX,&nMY);

	if(SDL_PollEvent(&sdl_event))
	{
		switch(sdl_event.type)
		{
		case SDL_QUIT:
			ZWM_FILLMSG((*event),ZWM_QUIT,0,0,0);
			return true;
		case SDL_KEYDOWN:
			if(lpActiveWnd)
			{
				ZWM_FILLMSG((*event),ZWM_KEYDOWN,
					ZWM_WORDCOMB(sdl_event.key.keysym.sym,
						sdl_event.key.keysym.mod),
					ZWM_WORDCOMB(sdl_event.key.keysym.unicode,
						sdl_event.key.keysym.scancode),
					lpActiveWnd);
				return true;
			}
		case SDL_KEYUP:
			if(lpActiveWnd)
			{
				ZWM_FILLMSG((*event),ZWM_KEYUP,
					ZWM_WORDCOMB(sdl_event.key.keysym.sym,
						sdl_event.key.keysym.mod),
					ZWM_WORDCOMB(sdl_event.key.keysym.unicode,
						sdl_event.key.keysym.scancode),
					lpActiveWnd);
				return true;
			}
		case SDL_MOUSEMOTION:
			{		
				CZWM_WINDOW* lpNewHoverWnd=0;
				bMF_ClientHit=GetMouseFocusedWnd(nMX,nMY,&lpMF_Parent,&lpMF_Child);

				if(lpMF_Child) lpNewHoverWnd=lpMF_Child; else lpNewHoverWnd=lpMF_Parent;
				if(lpMF_TM!=lpNewHoverWnd)
				{
					if(lpMF_TM&&lpMF_TM!=lpClActiveWnd)
						SendWindowMessage(lpMF_TM,ZWM_LOSEMOUSEFOCUS,0,0);
					if(lpNewHoverWnd)
						SendWindowMessage(lpNewHoverWnd,ZWM_GETMOUSEFOCUS,0,0);
				}
				lpMF_TM=lpNewHoverWnd;

				if(lpMovingWnd)
				{
					// Filter out all those nasty redudant mousemotion events ! :)
					while(SDL_PeepEvents(&sdl_event,1,SDL_GETEVENT,SDL_MOUSEMOTIONMASK)>0);

					MoveWindow(lpMovingWnd,sdl_event.motion.x-nMovWndX,
						sdl_event.motion.y-nMovWndY);
					return true;
				}
				if(lpMF_TM)
				{
					ZWM_FILLMSG((*event),ZWM_MOUSEMOVE,
						ZWM_WORDCOMB(nMX,nMY),
						ZWM_WORDCOMB(sdl_event.motion.xrel,
							sdl_event.motion.yrel),
						lpMF_TM);
					return true;
				}
				return false;
			}
		case SDL_MOUSEBUTTONUP:
			{
				if(lpClActiveWnd)
				{	
					if(lpClActiveWnd!=lpMF_TM)
					{
						lpClActiveWnd->lpMsgCallback(lpClActiveWnd,
							ZWM_LOSEMOUSEFOCUS,0,0);
					}
					lpClActiveWnd=0;
				}
				Uint8 nButton=sdl_event.button.button;
				if(lpMovingWnd&&nButton==SDL_BUTTON_LEFT) lpMovingWnd=0;
				if(lpMF_TM)
				{
					ZWM_FILLMSG((*event),ZWM_MOUSEBUTTONUP,
						ZWM_WORDCOMB(nMX,nMY),
						sdl_event.button.button,lpMF_TM);
					return true;
				}
				return false;
			}
		case SDL_MOUSEBUTTONDOWN:
			{
				Uint8 nButton=sdl_event.button.button;
				if(lpMF_TM)
				{
					if((lpMF_Parent)&&(nButton<=3))
					{
						ActivateWindow(lpMF_Parent);
						// window activation message
					}
					
					if(bMF_ClientHit)
					{
						ZWM_FILLMSG((*event),ZWM_MOUSEBUTTONDOWN,
							ZWM_WORDCOMB(nMX,nMY),
							nButton,lpMF_TM);
						return true;
					}
					SDL_Rect rcTB={lpMF_Parent->rcWnd.x+3,lpMF_Parent->rcWnd.y+3,
							lpMF_Parent->rcWnd.w-5,16};
					if((nButton==SDL_BUTTON_LEFT)&&(lpMF_Parent->nWndType&ZWND_TITLE)&&
						(ZWM_PtInRect(nMX,nMY,&rcTB)))
					{
						SDL_Rect rcCButton={lpMF_Parent->rcWnd.x+lpMF_Parent->rcWnd.w-14,
							lpMF_Parent->rcWnd.y+5,10,10};
						SDL_Rect rcMaxButton=rcCButton;
						if((lpMF_Parent->nWndType&ZWND_SYSMENU)&&(ZWM_PtInRect(nMX,nMY,&rcCButton)))
						{
							//ZWM_FILLMSG((*event),ZWM_CLOSE,0,0,lpMF_Parent);
							
							if(lpMF_Parent->lpMsgCallback)
							{
								lpMF_Parent->lpMsgCallback(lpMF_Parent,ZWM_CLOSE,0,0);
								DestroyWindow(lpMF_Parent);
							}
							//return true;
							return false;
						}
						if(lpMF_Parent->nWndType&ZWND_MAXBOX)
						{
							rcMaxButton.x-=12;
							if(ZWM_PtInRect(nMX,nMY,&rcMaxButton))
							{
								if(lpMF_Parent->nWndType&ZWND_MAXIMIZED)
								{
									ZWM_FILLMSG((*event),ZWM_MAXIMIZE,1,0,lpMF_Parent);
									MaximizeWindow(lpMF_Parent,false);
								}
								else
								{
									ZWM_FILLMSG((*event),ZWM_MAXIMIZE,0,0,lpMF_Parent);
									MaximizeWindow(lpMF_Parent,true);
								}
								return true;
							}
						}
						if(lpMF_Parent->nWndType&ZWND_MINBOX)
						{
							SDL_Rect rcMinButton=rcMaxButton;
							rcMinButton.x-=12;
							if(ZWM_PtInRect(nMX,nMY,&rcMinButton))
							{
								if(lpMF_Parent->nWndType&ZWND_MINIMIZED)
								{
									ZWM_FILLMSG((*event),ZWM_MINIMIZE,1,0,lpMF_Parent);
									MinimizeWindow(lpMF_Parent,false);
								}
								else
								{
									ZWM_FILLMSG((*event),ZWM_MINIMIZE,0,0,lpMF_Parent);
									MinimizeWindow(lpMF_Parent,true);
								}
								return true;
							}
						}
						lpMovingWnd=lpMF_Parent;
						nMovWndX=nMX-lpMF_Parent->rcWnd.x;
						nMovWndY=nMY-lpMF_Parent->rcWnd.y;
						// window starts moving message
					}
				}
			}
		}
		return false;
	}
	else if(llMsg.lpLastNode)
	{
		*event=*((ZWMEVENT*)llMsg.lpLastNode);
		llMsg.DeleteNode(llMsg.lpLastNode);
		return true;
	}
	return false;
}

void CZSDL_WM::MoveChildWnd(CZWM_WINDOW* lpWnd,int nDX,int nDY)
{
	if(lpWnd->nWndType&ZWND_MAXIMIZED) return;

	lpWnd->rcWnd.x+=nDX;
	lpWnd->rcWnd.y+=nDY;
	lpWnd->GetClientRect(&lpWnd->rcClient);

	CZWM_WINDOW* lpCur=(CZWM_WINDOW*)lpWnd->llChildList.lpFirstNode;
	for(unsigned int i=0;i<lpWnd->llChildList.nNumNodes;i++)
	{
		MoveChildWnd(lpCur,nDX,nDY);
		lpCur=(CZWM_WINDOW*)lpCur->lpNextNode;
	}
}

void CZSDL_WM::MoveWindow(CZWM_WINDOW* lpWnd,int nX,int nY)
{
	if(lpWnd->nWndType&ZWND_MAXIMIZED) return;

	int nDX=nX-lpWnd->rcWnd.x;
	int nDY=nY-lpWnd->rcWnd.y;
	SDL_Rect rcOld=lpWnd->rcWnd;
	lpWnd->rcWnd.x=nX;
	lpWnd->rcWnd.y=nY;
	lpWnd->GetClientRect(&lpWnd->rcClient);

	CZWM_WINDOW* lpCur=(CZWM_WINDOW*)lpWnd->llChildList.lpFirstNode;
	for(unsigned int i=0;i<lpWnd->llChildList.nNumNodes;i++)
	{
		MoveChildWnd(lpCur,nDX,nDY);
		lpCur=(CZWM_WINDOW*)lpCur->lpNextNode;
	}

	AddUpdateRect(rcOld);
	AddUpdateRect(lpWnd->rcWnd);
}

CZWM_WINDOW* CZSDL_WM::GetFirstNonTopmostWndFromTop()
{
	CZWM_WINDOW* lpCur=(CZWM_WINDOW*)llWnd.lpLastNode;
	for(unsigned int i=0;i<llWnd.nNumNodes;i++)
	{
		if(!(lpCur->nWndType&ZWND_TOPMOST)) return lpCur;
		lpCur=(CZWM_WINDOW*)lpCur->lpPrevNode;
	}
	return 0;
}

void CZSDL_WM::ActivateWindow(CZWM_WINDOW* lpWnd)
{
	if(lpWnd==lpActiveWnd) 
	{
		return;
	}
	CZWM_WINDOW* lpOld=lpActiveWnd;
	lpActiveWnd=lpWnd;
	if(!(lpWnd->nWndType&ZWND_TOPMOST))
	{
		llWnd.MoveAfter(GetFirstNonTopmostWndFromTop(),lpActiveWnd);
	}
	tbTaskbar.UpdateTasks(lpActiveWnd);
	if(lpOld) AddUpdateRect(lpOld->rcWnd);
	AddUpdateRect(lpActiveWnd->rcWnd); 
}

void CZSDL_WM::DestroyWindow(CZWM_WINDOW* lpWnd)
{
	if(lpActiveWnd) lpActiveWnd=0;
	
	if(lpMF_TM==lpWnd) lpMF_TM=0;
	if(lpMF_Child==lpWnd) lpMF_Child=0;
	if(lpMF_Parent==lpWnd) lpMF_Parent=0;
	

	ZWMEVENT* lpCur=(ZWMEVENT*)llMsg.lpFirstNode;
	ZWMEVENT* lpNxt=0;
	for(unsigned int i=0;i<llMsg.nNumNodes;i++)
	{
		lpNxt=(ZWMEVENT*)lpCur->lpNextNode;
		if(lpCur->lpWnd==lpWnd) llMsg.DeleteNode(lpCur);
		lpCur=lpNxt;
	}

	AddUpdateRect(lpWnd->rcWnd);
	llWnd.DeleteNode(lpWnd);
	if(llWnd.nNumNodes) lpActiveWnd=(CZWM_WINDOW*)llWnd.lpLastNode;
	tbTaskbar.UpdateTasks(lpActiveWnd);
}

void CZSDL_WM::DisptachEvent(ZWMEVENT* event)
{
	if(event->lpWnd&&event->lpWnd->lpMsgCallback)
		event->lpWnd->lpMsgCallback(event->lpWnd,event->nMsg,event->nParam1,event->nParam2);
}

void CZSDL_WM::DrawWindow(CZWM_WINDOW* lpWnd,int nParX,int nParY)
{
	if(!(lpWnd->nWndType&ZWND_VISIBLE)) return;
	if(lpWnd->nWndType&ZWND_MINIMIZED) return;

	SDL_Rect rcVWnd=lpWnd->rcWnd;
	SDL_Rect rcVClient=lpWnd->rcClient;

	if(lpWnd->nWndType&ZWND_BORDER)
	{
		SDL_Rect rcOut=rcVWnd;
		ZWM_DrawRect(nBorder1Col,rcOut,lpDeskSrf);
		SDL_Rect rcMid={rcOut.x+1,rcOut.y+1,rcOut.w-2,rcOut.h-2};
		ZWM_DrawRect(nBorder2Col,rcMid,lpDeskSrf);
		SDL_Rect rcInn={rcMid.x+1,rcMid.y+1,rcMid.w-2,rcMid.h-2};
		ZWM_DrawRect(nBorder1Col,rcInn,lpDeskSrf);
	}
	if(lpWnd->nWndType&ZWND_TITLE)
	{
		SDL_Rect rcTB={rcVWnd.x+3,rcVWnd.y+3,rcVWnd.w-5,16};
		SDL_Rect rcTBTmp=rcTB;
		ZWM_FillRect(lpDeskSrf,&rcTBTmp,(lpActiveWnd==lpWnd)?nTBCol2:nTBCol1);
		ZWM_DrawHLine(nBorder1Col,rcVWnd.x+3,rcVWnd.x+rcVWnd.w-3,
			rcTB.y+rcTB.h,lpDeskSrf);
		ZWM_DrawHLine(nBorder2Col,rcVWnd.x+3,rcVWnd.x+rcVWnd.w-3,
			rcTB.y+rcTB.h+1,lpDeskSrf);
		ZWM_DrawHLine(nBorder1Col,rcVWnd.x+3,rcVWnd.x+rcVWnd.w-3,
			rcTB.y+rcTB.h+2,lpDeskSrf);

		if(lpWnd->szTitle)
			lpFont->DrawString(0,lpWnd->nFontCol,lpDeskSrf,rcTB.x+2,rcTB.y+4,
			(rcTB.w-40)/8,lpWnd->szTitle);
		if(lpWnd->nWndType&ZWND_SYSMENU)
		{
			SDL_Rect rcCButton={rcVWnd.x+rcVWnd.w-14,rcVWnd.y+5,10,10};
			ZWM_DrawStereoRect(0xFFFFFF,0x000000,rcCButton,lpDeskSrf);
			lpFont->DrawString(0,0x00,lpDeskSrf,rcCButton.x,rcCButton.y,1,"X");
			
			SDL_Rect rcMaxButton=rcCButton;
			if(lpWnd->nWndType&ZWND_MAXBOX)
			{
				rcMaxButton.x-=12;

				SDL_Rect rcMaxWnd={rcMaxButton.x+2,rcMaxButton.y+2,
					rcMaxButton.w-4,rcMaxButton.h-4};

				ZWM_DrawStereoRect(0xFFFFFF,0x000000,rcMaxButton,lpDeskSrf);
				ZWM_DrawStereoRect(0x000000,0xFFFFFF,rcMaxWnd,lpDeskSrf);
			}
			if(lpWnd->nWndType&ZWND_MINBOX)
			{
				SDL_Rect rcMinButton=rcMaxButton;
				rcMinButton.x-=12;
				ZWM_DrawStereoRect(0xFFFFFF,0x000000,rcMinButton,lpDeskSrf);
				
				SDL_Rect rcMinBar={rcMinButton.x+2,rcMinButton.y+6,6,2};
				ZWM_DrawRect(0x000000,rcMinBar,lpDeskSrf);
			}
		}
	}
	if((lpWnd->nWndType&ZWND_SDLSURFACE)/*&&(lpWnd!=lpMovingWnd)*/)
	{
		SDL_Rect rcClient=rcVClient;
		SDL_BlitSurface(lpWnd->lpSurface,0,lpDeskSrf,&rcClient);
	}
}

void CZSDL_WM::SendWindowMessage(CZWM_WINDOW* lpWnd,
								 int nMsg,int nParam1,int nParam2)
{
	ZWMEVENT* lpNewEvent=new ZWMEVENT;
	lpNewEvent->lpWnd=lpWnd;
	lpNewEvent->nMsg=nMsg;
	lpNewEvent->nParam1=nParam1;
	lpNewEvent->nParam2=nParam2;
	llMsg.AddNode(lpNewEvent);
}

void CZSDL_WM::UpdateView()
{
	DrawDesktop_NonGL();
	if(bFullScreenUpdate)
	{
		SDL_Flip(lpDeskSrf);
		bFullScreenUpdate=false;
	}
	else if(nUpdateRects)
	{
		SDL_UpdateRects(lpDeskSrf,nUpdateRects,rcUpdateList);
		nUpdateRects=0;
	}
	if(nSDLVideoFlags&SDL_OPENGLBLIT)
	{
		DrawDesktop_GL();
		SDL_GL_SwapBuffers();
	}
}

void CZSDL_WM::DrawWindowList(CZWM_LLIST* lpList,int nParX,int nParY)
{
	CZWM_WINDOW* lpCur=(CZWM_WINDOW*)lpList->lpFirstNode;
	for(unsigned int i=0;i<lpList->nNumNodes;i++)
	{
		if(lpCur->nWndType&ZWND_VISIBLE)
		{
			if(lpCur->nWndType&ZWND_FRMUPDATE)
			{
				lpCur->lpMsgCallback(lpCur,ZWM_PAINT,0,0);
			}
			DrawWindow(lpCur,nParX,nParY);
			
			if((!(lpCur->nWndType&ZWND_MINIMIZED))&&(lpCur->llChildList.lpFirstNode)) 
			{
				
				DrawWindowList(&lpCur->llChildList,
					nParX+lpCur->rcClient.x,
					nParY+lpCur->rcClient.y);
			}
		}
		lpCur=(CZWM_WINDOW*)lpCur->lpNextNode;
	}
}

void CZSDL_WM::DrawDesktop_NonGL()
{
	ZWM_FillRect(lpDeskSrf,NULL,nDeskCol);
	DrawWindowList(&llWnd,0,0);
}

bool IntervalIntersect(int a1,int a2,int b1,int b2,int* r1,int* r2)
{
	if(a2<b1||b2<a1) return false;

	if(a1<b1)
	{
		*r1=b1;
		if(b2<a2) *r2=b2; else *r2=a2;
	}
	else
	{
		*r1=a1;
		if(a2<b2) *r2=a2; else *r2=b2;
	}
	return true;
}

bool RectIntersect(SDL_Rect rcA,SDL_Rect rcB,SDL_Rect* rcR)
{
	int nRX1,nRX2,nRY1,nRY2;
	bool bX=IntervalIntersect(rcA.x,rcA.x+rcA.w-1,rcB.x,rcB.x+rcB.w-1,
		&nRX1,&nRX2);
	bool bY=IntervalIntersect(rcA.y,rcA.y+rcA.h-1,rcB.y,rcB.y+rcB.h-1,
		&nRY1,&nRY2);
	if(bX&&bY)
	{
		rcR->x=nRX1;
		rcR->w=nRX2-nRX1+1;
		rcR->y=nRY1;
		rcR->h=nRY2-nRY1+1;
		return true;
	}
	else return false;
}

void CZSDL_WM::UpdateGLWndIntersections(CZWM_WINDOW* lpWnd)
{
	SDL_Rect rcWnd;
	RectIntersect(lpWnd->rcClient,rcDesk,&rcWnd);
	SDL_Rect rcInt;

	CZWM_WINDOW* lpCur=(CZWM_WINDOW*)lpWnd->lpNextNode;
	while(lpCur)
	{
		if((!(lpCur->nWndType&ZWND_MINIMIZED))&&(lpCur->nWndType&ZWND_VISIBLE))
		{
			if(RectIntersect(rcWnd,lpCur->rcWnd,&rcInt))
			{
				SDL_SetClipRect(lpDeskSrf,&rcInt);
				DrawWindow(lpCur,0,0);
				SDL_UpdateRects(lpDeskSrf,1,&rcInt);
			}
		}
		lpCur=(CZWM_WINDOW*)lpCur->lpNextNode;
	}
	SDL_SetClipRect(lpDeskSrf,0);
}

void CZSDL_WM::DrawGLWindowList(CZWM_LLIST* lpList)
{
	CZWM_WINDOW* lpCur=(CZWM_WINDOW*)lpList->lpFirstNode;
	for(unsigned int i=0;i<lpList->nNumNodes;i++)
	{
		if(lpCur->nWndType&ZWND_GL)
		{	
			DrawWindow(lpCur,0,0);
			if(lpCur->lpMsgCallback&&(!(lpCur->nWndType&ZWND_MINIMIZED))&&
				(lpCur->nWndType&ZWND_VISIBLE))
			{
				lpCur->lpMsgCallback(lpCur,ZWM_PAINT,0,0);
			}
			if(lpCur->llChildList.lpFirstNode) 
			{
				DrawWindowList(&lpCur->llChildList,0,0);
			}
			UpdateGLWndIntersections(lpCur);
		}
		lpCur=(CZWM_WINDOW*)lpCur->lpNextNode;
	}
}

void CZSDL_WM::DrawDesktop_GL()
{
	ZWM_FillRect(lpDeskSrf,NULL,nDeskCol);
	DrawGLWindowList(&llWnd);
}

