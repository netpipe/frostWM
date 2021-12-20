#ifndef _ZWM_MODULE_H
#define _ZWM_MODULE_H

#include "ZSDL_WM.h"

enum EZ_MOD_TYPE
{
	EZ_MOD_NONE,
	EZ_MOD_NORMAL
};

enum EZ_MOD_FKT
{
	EZ_MOD_CREATE_WND,
	EZ_MOD_REGISTER_WND,
	EZ_MOD_DRAW_PXL
};

struct SZWM_MODINFO
{
	EZ_MOD_TYPE		eModType;
};

class CZWM_MODULE:public CZWM_LL_NODE
{
public:
	CZWM_MODULE(char* szModName);
	~CZWM_MODULE();

	CZDLL_MODULE	dllMod;
	SZWM_MODINFO	wmModInfo;
};

typedef int (*ZMOD_FK_INIT)(SZWM_MODINFO* lpModInfo,CZSDL_WM* lpWM,void* lpLnkFkt);
typedef int (*ZMOD_FK_GETFKT)(int nFkt);
typedef CZWM_WINDOW* (*ZMOD_FK_CREATE_WND)(const char* szCaption,ZWM_MSGCALLBACK lpMCB,int nType,CZWM_WINDOW* lpParWnd,
		Uint32 nClientColor,Uint32 nFontColor,SDL_Rect rcW);
typedef void (*ZMOD_FK_REGISTER_WND)(CZWM_WINDOW* lpWnd);
typedef void (*ZMOD_FK_DRAW_PXL)(Uint32 nCol,int nX,int nY,SDL_Surface* lpSurface);

#endif