#ifndef _ZWM_PICTUREBOX_H
#define _ZWM_PICTUREBOX_H

#include "ZWM_Window.h"

class CZWM_PICTUREBOX:public CZWM_WINDOW
{
public:
	CZWM_PICTUREBOX(int nIndex,CZWM_WINDOW* lpPar,SDL_Rect rcRect);
	~CZWM_PICTUREBOX();
	void Update();
	void LoadImage(char* szFile);

	int nIdx;
};

#endif