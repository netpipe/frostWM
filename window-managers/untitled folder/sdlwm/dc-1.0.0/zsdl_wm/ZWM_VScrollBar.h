#ifndef _ZWM_VSCROLLBAR_H
#define _ZWM_VSCROLLBAR_H

#include "ZWM_Window.h"
#include "ZWM_Button.h"

class CZWM_VSCROLLBAR:public CZWM_WINDOW
{
public:
	CZWM_VSCROLLBAR(int nIndex,CZWM_WINDOW* lpPar,SDL_Rect rcVS);
	~CZWM_VSCROLLBAR();
	void SetBaseValues(int nNumEntries,int nPageSize);

	void Update();
	void SetPos(int nNewPos);
	int GetPos();
	void SendPosChange();


	int					nIdx;

	int					nSB_Count;
	int					nSB_Page;
	CZWM_BUTTON*		lpSBtn;
};

#endif