#ifndef _ZWM_BUTTON_H
#define _ZWM_BUTTON_H

#include "ZWM_Window.h"

class CZWM_BUTTON:public CZWM_WINDOW
{
public:
	CZWM_BUTTON(int nIndex,CZWM_WINDOW* lpPar,SDL_Rect rcButton,
		char* szCaption,SDL_Surface *lpBtnSurface);
	~CZWM_BUTTON();

	void Update();

	char*			szBmp;
	bool			bHoverState;
	bool			bPressed;
	int				nIdx;
	SDL_Surface*	lpBtnSrf;

	ZWM_MSGCALLBACK	lpCallbackHook;
};

#endif