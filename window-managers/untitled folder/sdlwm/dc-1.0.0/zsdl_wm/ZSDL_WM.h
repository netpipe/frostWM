#ifndef _ZSDL_WM_H
#define _ZSDL_WM_H

#include <SDL.h>
#include <ZSDL_BitFont.h>
#include "ZWM_Window.h"
#include "ZWM_Graph.h"
#include "ZWM_Taskbar.h"
#include "ZWM_ButtonCollection.h"
#include "ZWM_CommonDlg.h"
#include "ZWM_Button.h"
#include "ZWM_VScrollBar.h"
#include "ZWM_PictureBox.h"
#include "ZWM_ListBox.h"
#include "ZWM_ComboBox.h"
#include "ZWM_TextBox.h"
#include "ZWM_MPGWidget.h"
#include "ZWM_MusicCtrl.h"
#include "ZWM_WidgetContainer.h"
#include "ZWM_FInfo.h"
#include "ZDLL_PEObj.h"
#include "ZWM_Module.h"

bool ZWM_PtInRect(int nX,int nY,SDL_Rect* lpRect);

class ZWMEVENT:public CZWM_LL_NODE
{
public:
	int				nMsg;
	long			nParam1;
	long			nParam2;
	CZWM_WINDOW*	lpWnd;
};

enum EZWM_MSG
{
	ZWM_NONE,
	ZWM_QUIT,
	ZWM_CLOSE,
	ZWM_KEYDOWN,
	ZWM_KEYUP,
	ZWM_MOUSEMOVE,
	ZWM_MAXIMIZE,
	ZWM_MINIMIZE,
	ZWM_MOUSEBUTTONDOWN,
	ZWM_MOUSEBUTTONUP,
	ZWM_PAINT,
	ZWM_GETMOUSEFOCUS,
	ZWM_LOSEMOUSEFOCUS,
	
	ZWM_BUTTONDOWN,

	ZWM_SBPOSCHANGE,

	ZWM_LBSELCHANGE,

	ZWM_CBSELCHANGE,

	ZWM_OPENFILE
};

class CZSDL_WM
{
	friend class CZWM_TASKBAR;
	friend int __ZWM_TaskbarCallback(CZWM_WINDOW* lpWnd,int nMsg,int nParam1,int nParam2);
	friend int __ZWMBtnCollectionCallback(CZWM_WINDOW* lpWnd,int nMsg,int nParam1,int nParam2);

public:
	~CZSDL_WM();

	CZSDL_WM(Uint32 nDesktopColor,Uint32 nBorder1Color,Uint32 nBorder2Color,
		Uint32 nTitleBarCol1,Uint32 nTitleBarCol2,
		SDL_Rect rcDesktop,int nBPP,Uint32 nSDLFlags,CZSDL_BITFONT* lpFnt);

	static CZWM_WINDOW* ZWMCreateWindow(const char* szCaption,ZWM_MSGCALLBACK lpMCB,int nType,CZWM_WINDOW* lpParWnd,
		Uint32 nClientColor,Uint32 nFontColor,SDL_Rect rcW);

	static void RegisterWindow(CZWM_WINDOW* lpWnd);
	bool PollEvent(ZWMEVENT* event);
	void DisptachEvent(ZWMEVENT* event);
	void DrawWindow(CZWM_WINDOW* lpWnd,int nParX,int nParY);
	void DrawDesktop_NonGL();
	void DrawDesktop_GL();
	void DrawWindowList(CZWM_LLIST* lpList,int nParX,int nParY);
	void DrawGLWindowList(CZWM_LLIST* lpList);
	void SendWindowMessage(CZWM_WINDOW* lpWnd,int nMsg,int nParam1,int nParam2);
	void ActivateWindow(CZWM_WINDOW* lpWnd);
	void DestroyWindow(CZWM_WINDOW* lpWnd);
	void MoveWindow(CZWM_WINDOW* lpWnd,int nX,int nY);
	void MaximizeWindow(CZWM_WINDOW* lpWnd,bool bMaximize);
	void MinimizeWindow(CZWM_WINDOW* lpWnd,bool bMinimize);
	static void AddUpdateRect(SDL_Rect rcRect);
	void UpdateView();
	void UpdateGLWndIntersections(CZWM_WINDOW* lpWnd);
	static CZWM_WINDOW* GetFirstNonTopmostWndFromTop();
	void GetMFocusedFromList(int nX,int nY,CZWM_WINDOW** lpFW,
								  CZWM_LLIST* lpList);
	static void AddUpdateRectFromWnd(CZWM_WINDOW* lpWnd);
	bool GetMouseFocusedWnd(int nX,int nY,
		CZWM_WINDOW** lpWnd,CZWM_WINDOW** lpChild);
	void MoveChildWnd(CZWM_WINDOW* lpWnd,int nX,int nY);
//private:
	static SDL_Surface*		lpDeskSrf;
	SDL_Rect				rcDesk;
	Uint32					nDeskCol;
	Uint32					nBorder1Col;
	Uint32					nBorder2Col;
	Uint32					nTBCol1;
	Uint32					nTBCol2;
	static CZWM_WINDOW*		lpActiveWnd;
	SDL_Rect				rcFSWnd;
	static CZWM_TASKBAR		tbTaskbar;

	CZSDL_BITFONT*			lpFont;

	static CZWM_LLIST		llWnd;
	CZWM_LLIST				llMsg;
	CZWM_LLIST				llModule;

	static CZSDL_WM*		lpWM;

	static char				szWM_HDir[256];
	static SDL_Rect			rcUpdateList[256];
	static int				nUpdateRects;
	static bool				bFullScreenUpdate;
	static CZWM_WINDOW*		lpMovingWnd;
	static CZWM_WINDOW*		lpClActiveWnd;
	static CZWM_WINDOW*		lpMF_Parent;
	static CZWM_WINDOW*		lpMF_Child;
	static CZWM_WINDOW*		lpMF_TM;
	char*					szTarFile;

	int						nDepthBPP;
	int						nSDLVideoFlags;
};

#define ZSDLWM_SRELEASE(x) if(x) delete x;
#define ZWM_FILLMSG(x,a,b,c,d) {x.nMsg=a;x.nParam1=b;x.nParam2=c;x.lpWnd=d;}

#define ZWM_WORDCOMB(a,b) ((long)(((short)(a))|((long)((short)(b)))<<16))
#define ZWM_LOWORD(a) ((short)a)
#define ZWM_HIWORD(a) ((short)(((long)(a)>>16)&0xFFFF)) 

#endif