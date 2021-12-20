#ifndef _ZWM_COMBOBOX_H
#define _ZWM_COMBOBOX_H

#include "ZWM_Window.h"
#include "ZWM_ListBox.h"
#include "ZWM_TextBox.h"

enum ZCB_SYTLE
{
	ZCB_NONE=0,
};

class CZWM_COMBOBOX:public CZWM_WINDOW
{
public:
	CZWM_COMBOBOX(int nIndex,CZWM_WINDOW* lpPar,SDL_Rect rcCB,int nCBStyle);
	~CZWM_COMBOBOX();

	void Update();
	void SetList(CZWM_LLIST* lpNewList);
	void SetSelection(int nSel);
	void ShowList(bool bVis);

	int					nStyle;
	int					nSelection;
	
	SDL_Rect			rcCB_FS;
	SDL_Rect			rcCB_SS;
	CZWM_BUTTON*		lpOpenBtn;
	CZWM_LISTBOX*		lpListBox;
	CZWM_TEXTBOX*		lpTextBox;
};

#endif