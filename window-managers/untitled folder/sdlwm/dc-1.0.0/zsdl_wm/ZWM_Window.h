#ifndef _ZWM_WINDOW_H
#define _ZWM_WINDOW_H

#include <SDL.h>
#include "ZWM_Object.h"
#include "ZWM_Types.h"

class CZWM_WINDOW;

typedef int (*ZWM_MSGCALLBACK)(CZWM_WINDOW* lpWnd,int nMsg,int nParam1,int nParam2);

/*
          G  M  M  C  D  V  T  S  M  M  S  T  B
		  L	 A  I  H  I  I  O  D  A  I  Y  I  O
			 X  N  I  S  S  P  L  X  N  S  T  R
			 I  I  L  A  I  M  S  B  B  M  L  D
			 M  M  D  B  B  O  U  O  O  E  E  E
			 I  I     L  L  S  R  X  X  N  B  R
			 Z  Z     E  E  T  F        U  A
			 E  E              A	       R
			 D  D              C
					           E

 16	15 14 13 12 11 10 9  8  7  6  5  4  3  2  1
|XX|XX|XX|XX|XX|XX|XX|XX|XX|XX|XX|XX|XX|XX|XX|XX|

*/

enum EZWM_WNDTYPE
{
	ZWND_NONE=0,			// nothing
	ZWND_BORDER=1,			// border
	ZWND_TITLE=2,			// title bar
	ZWND_CAPTION=3,			// title bar + border
	ZWND_SYSMENU=4,			// system menu (close button)
	ZWND_MINBOX=8,			// minimize box
	ZWND_MAXBOX=16,			// maximize box
	ZWND_MINIMIZEBOX=12,	// minimize box + system menu
	ZWND_MAXIMIZEBOX=20,	// maximize box + system menu
	ZWND_SDLSURFACE=32,		// window manages a SDL surface in size of the client rect
	ZWND_TOPMOST=64,		// window is always on top
	ZWND_VISIBLE=128,		// window is visible
	ZWND_DISABLED=256,		// window is disabled (no user input)
	ZWND_CHILD=512,			// child window (window is owned by another window)
	ZWND_MINIMIZED=1024,	// window is minimized
	ZWND_MAXIMIZED=2048,	// window is maximized
	ZWND_GL=4096,			// window needs special gl draw code
	ZWND_FRMUPDATE=8192		// window needs to be redrawn every frame
};

enum EZWM_WNDBASETYPE
{
	ZWND_WINDOW,
	ZWND_BUTTONCOLLECTION
};

class CZWM_WINDOW:public CZWM_WIDGET
{
public:
	friend class CZSDL_WM;
	friend class CZWM_TASKBAR;
	friend int __ZWM_TaskbarCallback(CZWM_WINDOW* lpWnd,int nMsg,int nParam1,int nParam2);

	CZWM_WINDOW();
	CZWM_WINDOW(const char* szCaption,ZWM_MSGCALLBACK lpMCB,int nType,CZWM_WINDOW* lpParWnd,
			 Uint32 nClientColor,Uint32 nFontColor,SDL_Rect rcW);
	~CZWM_WINDOW();

	void GetClientRect(SDL_Rect* rcC);
	void Resize(int nW,int nH);
	void SetVisible(bool bState);

	SDL_Rect		rcClient;
	SDL_Surface*	lpSurface;
	void*			lpPrivateData;
	ZWM_MSGCALLBACK	lpMsgCallback;
protected:
	Uint32			nClientCol;
	Uint32			nFontCol;
};

#endif