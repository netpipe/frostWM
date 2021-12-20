#include <stdlib.h>
#include <string.h>
#include "ztar_module.h"
#include "ZSDL_WM.h"
#include "ZWM_VScrollBar.h"

int __ZWMVScrollBarCallback(CZWM_WINDOW* lpWnd,int nMsg,int nParam1,int nParam2)
{
	CZWM_VSCROLLBAR* lpVS=(CZWM_VSCROLLBAR*)lpWnd;

	switch(nMsg)
	{
	case ZWM_BUTTONDOWN:
		switch(nParam1)
		{
		case 0:
			{
				int nPos=lpVS->GetPos();
				lpVS->SetPos(--nPos);
				return 1;
			}
		case 1:
			{
				int nPos=lpVS->GetPos();
				lpVS->SetPos(++nPos);
				return 1;
			}
		}
		break;
	case ZWM_MOUSEBUTTONDOWN:
		switch(nParam2)
		{
		case 5:
			{
				int nPos=lpVS->GetPos()+(lpVS->nSB_Page/3);
				lpVS->SetPos(nPos);
				return 1;
			}
		case 4:
			{
				int nPos=lpVS->GetPos()-(lpVS->nSB_Page/3);
				lpVS->SetPos(nPos);
				return 1;
			}
		}
	}
	return 0;
}

int __ZWMVScrollButtonCallback(CZWM_WINDOW* lpWnd,int nMsg,int nParam1,int nParam2)
{
	CZWM_BUTTON*		lpBtn=(CZWM_BUTTON*)lpWnd;
	CZWM_VSCROLLBAR*	lpVS=(CZWM_VSCROLLBAR*)lpBtn->lpPrivateData;
	static int			nHotSpotDY=0;
	static bool			bMoving=false;

	switch(nMsg)
	{
	case ZWM_MOUSEBUTTONDOWN:
		{
			bMoving=true;
			int nY=ZWM_HIWORD(nParam1);
			nHotSpotDY=nY-lpBtn->rcClient.y;
			return 1;
		}
	case ZWM_MOUSEMOVE:
		{
			if(bMoving)
			{
				int nBarH=lpVS->rcClient.h-30;

				int nY=ZWM_HIWORD(nParam1);
				
				int nNewY=nY-nHotSpotDY;
				int nNewRY=nNewY-(lpVS->rcClient.y+15);

				if(nNewRY<0) 
					nNewY=(lpVS->rcClient.y+15);
				else if(nNewRY+(lpBtn->rcWnd.h-1)>nBarH)
					nNewY=(lpVS->rcClient.y+(lpVS->rcClient.h-16))
						-(lpBtn->rcWnd.h-1);

				CZSDL_WM::lpWM->MoveWindow(lpBtn,lpBtn->rcWnd.x,nNewY);
				CZSDL_WM::lpWM->AddUpdateRectFromWnd(lpVS);
				lpVS->SendPosChange();		
			}
			return 1;
		}
	case ZWM_MOUSEBUTTONUP:
	case ZWM_LOSEMOUSEFOCUS:
		{
			bMoving=false;
			return 1;
		}
	}

	return 0;
}

void CZWM_VSCROLLBAR::SendPosChange()
{
	if(lpParent->lpMsgCallback)
		lpParent->lpMsgCallback(lpParent,ZWM_SBPOSCHANGE,0,0);
}

void CZWM_VSCROLLBAR::SetBaseValues(int nNumEntries,int nPageSize)
{
	if((nSB_Count!=nNumEntries)||(nSB_Page!=nPageSize))
	{
		nSB_Count=nNumEntries;
		nSB_Page=nPageSize;

		int nBarHMax=rcClient.h-30;

		int nBarH=(nSB_Count)?(nSB_Page*nBarHMax)/nSB_Count:0;
		if(nBarH>=nBarHMax) 
		{
			nBarH=nBarHMax;
			SetVisible(false);
		}
		else
		{
			SetVisible(true);
		}

		SetPos(0);

		lpSBtn->Resize(-1,nBarH);
		lpSBtn->Update();
	}
}

int CZWM_VSCROLLBAR::GetPos()
{
	int nBarH=rcClient.h-30;
	int nY=lpSBtn->rcWnd.y-(rcClient.y+15);
	int nRet=((nY*nSB_Count)/nBarH);
	return nRet;
}

void CZWM_VSCROLLBAR::SetPos(int nNewPos)
{
	if(nSB_Count==0) return;
	int nBarH=rcClient.h-30;
	
	int nNewY=rcClient.y+16+((nNewPos*nBarH)/nSB_Count);
	int nNewRY=nNewY-(rcClient.y+15);

	if(nNewRY<0) 
		nNewY=(rcClient.y+15);
	else if(nNewRY+(lpSBtn->rcWnd.h-1)>nBarH)
		nNewY=(rcClient.y+(rcClient.h-16))
			-(lpSBtn->rcWnd.h-1);

	CZSDL_WM::lpWM->MoveWindow(lpSBtn,lpSBtn->rcWnd.x,nNewY);
	CZSDL_WM::lpWM->AddUpdateRectFromWnd(this);
	SendPosChange();
}

CZWM_VSCROLLBAR::CZWM_VSCROLLBAR(int nIndex,CZWM_WINDOW* lpPar,SDL_Rect rcVS):
		nIdx(nIndex),nSB_Count(0),nSB_Page(0)
{
	if(lpPar)
	{
		rcVS.x+=lpPar->rcClient.x;
		rcVS.y+=lpPar->rcClient.y;
	}

	rcWnd=rcVS;
	rcClient=rcVS;

	nWndType=ZWND_VISIBLE|ZWND_SDLSURFACE|ZWND_CHILD;
	lpMsgCallback=__ZWMVScrollBarCallback;
	lpParent=lpPar;
	nClientCol=0xC0C0C0;

	GetClientRect(&rcClient);

	SDL_PixelFormat* lpPF=CZSDL_WM::lpWM->lpDeskSrf->format;
	lpSurface=SDL_CreateRGBSurface(0,rcClient.w,rcClient.h,
		lpPF->BitsPerPixel,
		lpPF->Rmask,lpPF->Gmask,lpPF->Bmask,lpPF->Amask);
	ZWM_FillRect(lpSurface,0,nClientCol);

	Update();
	CZSDL_WM::lpWM->RegisterWindow(this);

	SDL_Rect rcB_UP={0,0,rcWnd.w,15};
	new CZWM_BUTTON(0,this,rcB_UP,0,ZLoadTarImage("wm.tar","vs_up.bmp"));

	SDL_Rect rcB_DOWN={0,rcWnd.h-16,rcWnd.w,15};
	new CZWM_BUTTON(1,this,rcB_DOWN,0,ZLoadTarImage("wm.tar","vs_down.bmp"));

	SDL_Rect rcB_SBtn={0,16,rcWnd.w,rcWnd.h-30};
	lpSBtn=new CZWM_BUTTON(2,this,rcB_SBtn,0,0);

	lpSBtn->lpCallbackHook=__ZWMVScrollButtonCallback;
	lpSBtn->lpPrivateData=this;
}

void CZWM_VSCROLLBAR::Update()
{
	
}

CZWM_VSCROLLBAR::~CZWM_VSCROLLBAR()
{
	
}