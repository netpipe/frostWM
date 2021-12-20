#include <SDL_image.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>
#include "ZSDL_WM.h"
#include "ZWM_TextBox.h"

int __ZWMTextBoxCallback(CZWM_WINDOW* lpWnd,int nMsg,int nParam1,int nParam2)
{
	CZWM_TEXTBOX* lpTB=(CZWM_TEXTBOX*)lpWnd;
	switch(nMsg)
	{
	case ZWM_SBPOSCHANGE:
		lpTB->DrawText();
		return 1;
	}
	return 0;
}

CZWM_TEXTBOX::CZWM_TEXTBOX(int nIndex,CZWM_WINDOW* lpPar,SDL_Rect rcRect,int nTBSytle):
	nIdx(nIndex),nStyle(nTBSytle),szTB_Text(0),nSize(0),lpVSBar(0),nCurCol(0)
{
	if(lpPar)
	{
		rcRect.x+=lpPar->rcClient.x;
		rcRect.y+=lpPar->rcClient.y;
	}

	CZSDL_WM* lpWM=CZSDL_WM::lpWM;

	rcWnd=rcRect;
	nWndType=ZWND_VISIBLE|ZWND_SDLSURFACE|ZWND_CHILD;

	lpMsgCallback=__ZWMTextBoxCallback;
	lpParent=lpPar;
	nClientCol=0xF0F0F0;

	GetClientRect(&rcClient);

	SDL_PixelFormat* lpPF=CZSDL_WM::lpWM->lpDeskSrf->format;
	lpSurface=SDL_CreateRGBSurface(0,rcClient.w,rcClient.h,
		lpPF->BitsPerPixel,
		lpPF->Rmask,lpPF->Gmask,lpPF->Bmask,lpPF->Amask);
	ZWM_FillRect(lpSurface,0,nClientCol);
	SDL_Rect rcBound={0,0,lpSurface->w,lpSurface->h};
	ZWM_DrawStereoRect(0x00000000,0xFFFFFFFF,rcBound,lpSurface);

	CZSDL_WM::lpWM->RegisterWindow(this);

	if(nStyle&ZTB_AUTOVSCROLL)
	{
		SDL_Rect rcVB={lpSurface->w-16,0,15,lpSurface->h};
		lpVSBar=new CZWM_VSCROLLBAR(0,this,rcVB);
	}
}

void CZWM_TEXTBOX::SetText(char* szNewText)
{
	if(szTB_Text) free(szTB_Text);
	szTB_Text=(char*)malloc(strlen(szNewText)+1);
	strcpy(szTB_Text,szNewText);
	nSize=strlen(szTB_Text);

	DrawText();
}

void CZWM_TEXTBOX::printf(char *szFmt,...)
{
	va_list vl;
	va_start(vl,szFmt);

	char szBuffer[256];
	vsprintf(szBuffer,szFmt,vl);
	int nBL=strlen(szBuffer);
	if(nBL)
	{
		char* szTmp=szBuffer;
		while(true)
		{
			char* szNL=strchr(szTmp,'\n');
			int nSL=(szNL)?szNL-szTmp:(szBuffer+strlen(szBuffer))-szTmp;

			CZWM_TEXT_NODE* lpNewTN=new CZWM_TEXT_NODE;
			lpNewTN->szText=new char[nSL+1];
			strncpy(lpNewTN->szText,szTmp,nSL);
			lpNewTN->szText[nSL]='\0';
			lpNewTN->nColor=nCurCol;
			llTextList.AddNode(lpNewTN);

			szTmp=szNL+1;

			if(szNL==0||szTmp[0]==0) break;
		}

		int nPageSize=(lpSurface->h-8)/10;
		lpVSBar->SetBaseValues(llTextList.nNumNodes,nPageSize);
	}

	va_end(vl);
}

void CZWM_TEXTBOX::SetColor(int nCol)
{
	nCurCol=nCol;
}

void CZWM_TEXTBOX::DrawText()
{
	ZWM_FillRect(lpSurface,0,nClientCol);
	SDL_Rect rcBound={0,0,lpSurface->w,lpSurface->h};
	ZWM_DrawStereoRect(0x00000000,0xFFFFFFFF,rcBound,lpSurface);

	int nPageSize=(lpSurface->h-8)/10;
	if(nStyle&ZTB_MULTILINE)
	{
		unsigned int nFPos=lpVSBar->GetPos();
		int nPCnt=0;
		CZWM_TEXT_NODE* lpCurN=(CZWM_TEXT_NODE*)llTextList.lpFirstNode;
		for(unsigned int i=0;i<llTextList.nNumNodes;i++)
		{
			if((i>=nFPos)&&(nPCnt<nPageSize))
			{
				CZSDL_WM::lpWM->lpFont->DrawString(false,lpCurN->nColor,
					lpSurface,2,2+(nPCnt*10),(lpSurface->w-4)/8,lpCurN->szText);
				nPCnt++;
			}
			lpCurN=(CZWM_TEXT_NODE*)lpCurN->lpNextNode;
		}
	}
	else
	{
		CZSDL_WM::lpWM->lpFont->DrawString(false,nFontCol,lpSurface,2,2,
			(lpSurface->w-4)/8,szTB_Text);
	}

	CZSDL_WM::lpWM->AddUpdateRectFromWnd(this);
}

CZWM_TEXTBOX::~CZWM_TEXTBOX()
{
	if(szTB_Text) free(szTB_Text);
}
