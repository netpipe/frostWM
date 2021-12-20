#ifndef _ZWM_LISTBOX_H
#define _ZWM_LISTBOX_H

#include "ZWM_Window.h"

class CZWM_LB_NODE:public CZWM_LL_NODE
{
public:
	CZWM_LB_NODE();
	char	szText[256];
	int		nColor;
	bool	bSelected;
};

enum ZLB_SYTLE
{
	ZLB_NONE=0,
	ZLB_MULTISEL=1,
	ZLB_NOTVISIBLE=2
};

class CZWM_LISTBOX:public CZWM_WINDOW
{
public:
	CZWM_LISTBOX(int nIndex,CZWM_WINDOW* lpPar,SDL_Rect rcLB,int nLBStyle);
	~CZWM_LISTBOX();

	void Update();
	void SetList(CZWM_LLIST* lpNewList);
	void SetSelection(int nSel);

	int					nStyle;
	int					nSelection;
	CZWM_VSCROLLBAR*	lpVS;
	CZWM_LLIST*			lpList;
};

#endif