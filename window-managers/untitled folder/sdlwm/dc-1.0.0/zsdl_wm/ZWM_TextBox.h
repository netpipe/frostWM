#ifndef _ZWM_TEXTBOX_H
#define _ZWM_TEXTBOX_H

#include "ZWM_Window.h"

enum ZTB_SYTLE
{
	ZTB_NONE=0,
	ZTB_NOEDIT=1,
	ZTB_MULTILINE=2,
	ZTB_AUTOVSCROLL=4,
	ZTB_AUTOHSCROLL=8
};

class CZWM_TEXT_NODE:public CZWM_LL_NODE
{
public:
	int		nColor;
	char*	szText;
};

class CZWM_TEXTBOX:public CZWM_WINDOW
{
public:
	CZWM_TEXTBOX(int nIndex,CZWM_WINDOW* lpPar,SDL_Rect rcRect,int nTBSytle);
	~CZWM_TEXTBOX();
	void Update();
	void SetText(char* szNewText);
	void DrawText();
	void printf(char *szFmt,...);
	void SetColor(int nCol);

	int					nIdx;
	char*				szTB_Text;
	CZWM_LLIST			llTextList;
	int					nSize;
	int					nStyle;
	int					nCurCol;

	CZWM_VSCROLLBAR*	lpVSBar;
};

#endif