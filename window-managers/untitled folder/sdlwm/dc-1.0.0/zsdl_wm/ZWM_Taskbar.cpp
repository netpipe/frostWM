#include "ZWM_Debug.h"
#include <assert.h>
#include "ZSDL_WM.h"
#include "ZWM_Taskbar.h"

void CZWM_TASKBAR::CreateTB(unsigned long nCCol,CZSDL_BITFONT* lpFont,
							CZWM_LLIST* lpWndList)
{
	lpFnt=lpFont;
	lpWndLL=lpWndList;
	rcGenTask.x=1; rcGenTask.y=4; rcGenTask.w=160; rcGenTask.h=16;

	rcTB.x=0;
	rcTB.y=CZSDL_WM::lpWM->rcDesk.h-36; 
	rcTB.w=CZSDL_WM::lpWM->rcDesk.w;
	rcTB.h=36;

	lpTBWnd=CZSDL_WM::lpWM->ZWMCreateWindow("",__ZWM_TaskbarCallback,
		ZWND_VISIBLE|ZWND_SDLSURFACE|ZWND_TOPMOST,0,nCCol,0x00,rcTB);
	CZSDL_WM::lpWM->RegisterWindow(lpTBWnd);
}

void CZWM_TASKBAR::DrawTask(CZWM_WINDOW* lpWnd,SDL_Rect* lpRect,bool bActive)
{
	unsigned int nCol1;
	unsigned int nCol2;
	if(bActive)
	{
		nCol1=0x000000; nCol2=0xFFFFFF;
	}
	else
	{
		nCol1=0xFFFFFF; nCol2=0x000000;
	}

	ZWM_DrawStereoRect(nCol1,nCol2,*lpRect,lpTBWnd->lpSurface);
	SDL_Rect rcMid={lpRect->x+1,lpRect->y+1,lpRect->w-2,lpRect->h-2};
	ZWM_DrawStereoRect(nCol1,nCol2,rcMid,lpTBWnd->lpSurface);
	
	if(lpWnd->szTitle)
	lpFnt->DrawString(0,0x00,lpTBWnd->lpSurface,lpRect->x+5,lpRect->y+3,
		lpRect->w/8,lpWnd->szTitle);
}

void CZWM_TASKBAR::UpdateTasks(CZWM_WINDOW* lpActive)
{
	SDL_Rect rcTBFill={0,0,rcTB.w,36};
	if(lpTBWnd)
	{
		ZWM_FillRect(lpTBWnd->lpSurface,&rcTBFill,0xC0C0C0);
		ZWM_DrawHLine(0x808080,0,rcTB.w,0,lpTBWnd->lpSurface);
		ZWM_DrawHLine(0xFFFFFF,0,rcTB.w,1,lpTBWnd->lpSurface);
		ZWM_DrawHLine(0x808080,0,rcTB.w,2,lpTBWnd->lpSurface);

		SDL_Rect rcActTsk=rcGenTask;
		CZWM_WINDOW* lpCurWnd=(CZWM_WINDOW*)lpWndLL->lpLastNode;
		for(unsigned int i=0;i<lpWndLL->nNumNodes;i++)
		{
			if(lpCurWnd!=lpTBWnd)
			{
				DrawTask(lpCurWnd,&rcActTsk,(lpActive==lpCurWnd));
				rcActTsk.x+=162;
			}
			lpCurWnd=(CZWM_WINDOW*)lpCurWnd->lpPrevNode;
		}
		CZSDL_WM::lpWM->AddUpdateRectFromWnd(lpTBWnd);
	}
}

int __ZWM_TaskbarCallback(CZWM_WINDOW* lpWnd,int nMsg,int nParam1,int nParam2)
{
	CZSDL_WM* lpWM=CZSDL_WM::lpWM;

	switch(nMsg)
	{
	case ZWM_MOUSEBUTTONDOWN:
		{
			int nX=ZWM_LOWORD(nParam1);
			int nY=ZWM_HIWORD(nParam1);
			SDL_Rect rcActTsk=lpWM->tbTaskbar.rcGenTask;

			CZWM_WINDOW* lpCurWnd=(CZWM_WINDOW*)lpWM->tbTaskbar.lpWndLL->lpLastNode;
			for(unsigned int i=0;i<lpWM->tbTaskbar.lpWndLL->nNumNodes;i++)
			{
				if(lpCurWnd!=lpWM->tbTaskbar.lpTBWnd)
				{
					if(ZWM_PtInRect(nX-lpWM->tbTaskbar.rcTB.x,
						nY-lpWM->tbTaskbar.rcTB.y,&rcActTsk))
					{
						if(lpCurWnd->nWndType&ZWND_MINIMIZED)
						{
							lpWM->MinimizeWindow(lpCurWnd,false);
						}
						lpWM->ActivateWindow(lpCurWnd);
						return 1;
					}
					rcActTsk.x+=162;
				}
				lpCurWnd=(CZWM_WINDOW*)lpCurWnd->lpPrevNode;
			}
			return 1;
		}
	default:
		return 0;		
	}
}