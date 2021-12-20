#ifndef _ZWM_MUSICCTRL_H
#define _ZWM_MUSICCTRL_H

#include "ZWM_Window.h"
#include "ZWM_Button.h"
#include "SDL_mixer.h"

class CZWM_MUSICCTRL:public CZWM_WINDOW
{
public:
	CZWM_MUSICCTRL(int nIndex,CZWM_WINDOW* lpPar,SDL_Rect rcBound);
	~CZWM_MUSICCTRL();
	void Open(char* szFile);
	void Play();

	CZWM_COMDLG_LOSA*	lpOpenDlg;
	int					nIdx;
	Mix_Music*			lpMus;
	bool				bMC_NewFileOpened;
};

#endif