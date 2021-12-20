#include <stdlib.h>
#include "ZSDL_WM.h"
#include "ZWM_Module.h"

void* ZWM_GetWMFunction(int nFkt)
{
	switch(nFkt)
	{
	case EZ_MOD_CREATE_WND:
		return CZSDL_WM::ZWMCreateWindow;
	case EZ_MOD_REGISTER_WND:
		return CZSDL_WM::RegisterWindow;
	default:
		return 0;
	}
}

CZWM_MODULE::CZWM_MODULE(char* szModName)
{
	int nRes=dllMod.LoadWinDLL(szModName);
	if(nRes<0) return;
	ZMOD_FK_INIT ModInitFkt=
		//(ZMOD_FK_INIT)dllMod.GetFunction("?ZWMMOD_Init@@YAHPAUSZWM_MODINFO@@PAVCZSDL_WM@@@Z");
		(ZMOD_FK_INIT)dllMod.GetFunctionLazy("?ZWMMOD_Init@");
	if(ModInitFkt==0) return;
	int nRes2=ModInitFkt(&wmModInfo,CZSDL_WM::lpWM,ZWM_GetWMFunction);
	if(nRes2!=0) _exit(0);

	CZSDL_WM::lpWM->llModule.AddNode(this);
}

CZWM_MODULE::~CZWM_MODULE()
{
		
}