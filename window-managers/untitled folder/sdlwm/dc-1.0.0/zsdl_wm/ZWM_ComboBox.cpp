#include <stdio.h>
#include "ZSDL_WM.h"
#include "ZWM_ComboBox.h"

int __ZWMComboBoxCallback(CZWM_WINDOW* lpWnd,int nMsg,int nParam1,int nParam2)
{
	CZWM_COMBOBOX* lpCB=(CZWM_COMBOBOX*)lpWnd;
	switch(nMsg)
	{
	case ZWM_BUTTONDOWN:
		if(lpCB->lpListBox->nWndType&ZWND_VISIBLE) lpCB->ShowList(false);
		else lpCB->ShowList(true);
		return 1;
	case ZWM_LBSELCHANGE:
		{
			CZWM_LB_NODE* lpLBN=(CZWM_LB_NODE*)lpCB->lpListBox->lpList->GetNode(nParam1);
			if(lpLBN)
			{
				lpCB->lpTextBox->SetText(lpLBN->szText);
				lpCB->ShowList(false);
			}
			lpCB->lpParent->lpMsgCallback(lpCB->lpParent,ZWM_CBSELCHANGE,nParam1,0);
			return 1;
		}
	}
	return 0;
}

CZWM_COMBOBOX::CZWM_COMBOBOX(int nIndex,CZWM_WINDOW* lpPar,SDL_Rect rcCB,int nCBStyle):
	nStyle(nCBStyle)
{
	if(lpPar)
	{
		rcCB.x+=lpPar->rcClient.x;
		rcCB.y+=lpPar->rcClient.y;
	}

	rcCB_FS=rcCB;
	rcCB_SS=rcCB_FS;
	rcCB_SS.h=16;

	rcWnd=rcCB_SS;
	rcClient=rcCB_SS;	

	nWndType=ZWND_VISIBLE|ZWND_CHILD;
	lpMsgCallback=__ZWMComboBoxCallback;
	lpParent=lpPar;
	nClientCol=0xE0E0E0;
	nSelection=-1;

	GetClientRect(&rcClient);

	SDL_Rect rcLB={0,16,rcCB_FS.w,rcCB_FS.h};
	lpListBox=new CZWM_LISTBOX(0,this,rcLB,ZLB_NOTVISIBLE);

	SDL_Rect rcBtn={rcCB_FS.w-16,0,15,16};
	lpOpenBtn=new CZWM_BUTTON(1,this,rcBtn,"^",0);

	SDL_Rect rcTB={0,0,rcCB_FS.w-16,16};
	lpTextBox=new CZWM_TEXTBOX(2,this,rcTB,0);
	
	CZSDL_WM::lpWM->RegisterWindow(this);
}

void CZWM_COMBOBOX::ShowList(bool bVis)
{
	if(bVis)
	{
		Resize(rcCB_FS.w,rcCB_FS.h);
	}
	else
	{
		Resize(rcCB_SS.w,rcCB_SS.h);
	}

	lpListBox->SetVisible(bVis);
}

void CZWM_COMBOBOX::SetList(CZWM_LLIST* lpNewList)
{
	lpListBox->SetList(lpNewList);
}

void CZWM_COMBOBOX::SetSelection(int nSel)
{
	lpListBox->SetSelection(nSel);

	CZWM_LB_NODE* lpLBN=(CZWM_LB_NODE*)lpListBox->lpList->GetNode(nSel);
	if(lpLBN)
	{
		lpTextBox->SetText(lpLBN->szText);
		ShowList(false);
	}
}

void CZWM_COMBOBOX::Update()
{
	CZSDL_WM::lpWM->AddUpdateRectFromWnd(this);
}


CZWM_COMBOBOX::~CZWM_COMBOBOX()
{

}