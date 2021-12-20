#include "ZSDL_WM.h"
#include "ZWM_ListBox.h"

CZWM_LB_NODE::CZWM_LB_NODE():
	nColor(0),bSelected(0)
{

}

int __ZWMListBoxCallback(CZWM_WINDOW* lpWnd,int nMsg,int nParam1,int nParam2)
{
	CZWM_LISTBOX* lpLB=(CZWM_LISTBOX*)lpWnd;
	switch(nMsg)
	{
	case ZWM_SBPOSCHANGE:
		lpLB->Update();
		return 1;
	case ZWM_MOUSEBUTTONDOWN:
		{
			int nY=ZWM_HIWORD(nParam1);
			int nSel=((nY-(lpLB->rcClient.y+1))/10)+lpLB->lpVS->GetPos();
			CZWM_LB_NODE* lpNode=(CZWM_LB_NODE*)lpLB->lpList->GetNode(nSel);
			if(!lpNode) return 1;
			if(lpLB->nStyle&ZLB_MULTISEL)
			{
				lpNode->bSelected=!lpNode->bSelected;
			}
			else
			{
				lpLB->nSelection=nSel;

				lpLB->lpParent->lpMsgCallback(
					lpLB->lpParent,ZWM_LBSELCHANGE,nSel,0);
			}
			lpLB->Update();
		}
		return 1;
	}
	return 0;
}

CZWM_LISTBOX::CZWM_LISTBOX(int nIndex,CZWM_WINDOW* lpPar,SDL_Rect rcLB,int nLBStyle):
	lpVS(0),lpList(0),nStyle(nLBStyle)
{
	if(lpPar)
	{
		rcLB.x+=lpPar->rcClient.x;
		rcLB.y+=lpPar->rcClient.y;
	}

	rcWnd=rcLB;
	rcClient=rcLB;	

	szTitle=0;
	nWndType=ZWND_SDLSURFACE|ZWND_CHILD;
	
	if(!(nStyle&ZLB_NOTVISIBLE))
	{
		nWndType|=ZWND_VISIBLE;
	}
	lpMsgCallback=__ZWMListBoxCallback;
	lpParent=lpPar;
	nClientCol=0xE0E0E0;
	nSelection=-1;

	GetClientRect(&rcClient);

	SDL_PixelFormat* lpPF=CZSDL_WM::lpWM->lpDeskSrf->format;
	lpSurface=SDL_CreateRGBSurface(0,rcClient.w,rcClient.h,
		lpPF->BitsPerPixel,
		lpPF->Rmask,lpPF->Gmask,lpPF->Bmask,lpPF->Amask);
	Update();

	SDL_Rect rcVS={lpSurface->w-17,1,15,lpSurface->h-2};
	lpVS=new CZWM_VSCROLLBAR(0,this,rcVS);
	
	CZSDL_WM::lpWM->RegisterWindow(this);
}

void CZWM_LISTBOX::SetList(CZWM_LLIST* lpNewList)
{
	lpList=lpNewList;
	lpVS->SetBaseValues(lpList->nNumNodes,(lpSurface->h-4)/10);
	Update();
}

void CZWM_LISTBOX::SetSelection(int nSel)
{
	nSelection=nSel;
}

void CZWM_LISTBOX::Update()
{
	ZWM_FillRect(lpSurface,0,nClientCol);
	SDL_Rect rcBound={0,0,lpSurface->w,lpSurface->h};
	ZWM_DrawStereoRect(0x000000,0xFFFFFF,rcBound,lpSurface);
	if(lpList)
	{
		CZWM_LB_NODE* lpCurN=(CZWM_LB_NODE*)lpList->lpFirstNode;
		int nCnt=0;
		int nPos=lpVS->GetPos();
		for(unsigned int i=0;i<lpList->nNumNodes;i++)
		{
			if((i>=(unsigned)nPos)&&(nCnt<(lpSurface->h-4)/10))
			{
				if(((nStyle&ZLB_MULTISEL)&&(lpCurN->bSelected))||
					(i==(unsigned)nSelection))
				{	
					SDL_Rect rcSel={1,(nCnt*10)+3,lpSurface->w-18,10};
					ZWM_FillRect(lpSurface,&rcSel,0x7070FF);
				}
				CZSDL_WM::lpWM->lpFont->DrawString(0,lpCurN->nColor,lpSurface,
					2,(nCnt*10)+3,
					(lpSurface->w-18)/8,lpCurN->szText);
				nCnt++;
			}
			lpCurN=(CZWM_LB_NODE*)lpCurN->lpNextNode;
		}
	}
	CZSDL_WM::lpWM->AddUpdateRectFromWnd(this);
}


CZWM_LISTBOX::~CZWM_LISTBOX()
{

}