#ifndef _ZWM_TASKBAR_H
#define _ZWM_TASKBAR_H

#include <SDL.h>
#include <ZSDL_BitFont.h>

class CZWM_TASKBAR
{
public:
	void CreateTB(unsigned long nCCol,CZSDL_BITFONT* lpFont,CZWM_LLIST* lpWndList);
	void UpdateTasks(CZWM_WINDOW* lpActive);
	void DrawTask(CZWM_WINDOW* lpWnd,SDL_Rect* lpRect,bool bActive);

	CZWM_WINDOW*	lpTBWnd;
	SDL_Rect		rcTB;
	SDL_Rect		rcGenTask;
	CZSDL_BITFONT*	lpFnt;
	CZWM_LLIST*		lpWndLL;
};

int __ZWM_TaskbarCallback(CZWM_WINDOW* lpWnd,int nMsg,int nParam1,int nParam2);

#endif