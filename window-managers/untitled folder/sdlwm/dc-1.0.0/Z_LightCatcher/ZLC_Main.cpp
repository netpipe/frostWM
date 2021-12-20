#include <stdlib.h>
#include <SDL.h>
#include <SDL_error.h>
#include <ZSDL_WM.h>
#include "ZLA_Module.h"
#include "ZLC_RayTraceKernel.h"
#include "DCLib.h"
//#include "ZWM_MessageBox.h"

CZSDL_WM*					lpWM=0;
CZLC_ZMLPARSER				cParser;
CZWM_COMDLG_LOSA*			lpCD_LOSA=0;
CDCL_INIFILE				cDefaultIni;

void ZLC_InitApp();
int ZLC_ExitApp();
void ZLC_ProgLoop();
void ZLC_LoadZRTFile_Callback(char* szFile);

int main(int argc,char* argv[])
{
#define BCK_XRES	800
#define BCK_YRES	600
	SDL_Init(SDL_INIT_VIDEO);

	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE,0);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,1);
	SDL_GL_SetAttribute(SDL_GL_ACCUM_ALPHA_SIZE,0);

	SDL_Rect rcDesk={0,0,BCK_XRES,BCK_YRES};
	
	lpWM=new CZSDL_WM(0x8080FF,0x000000,0xFF0000,0x008000,0xFF8000,rcDesk,32,
		0,new CZSDL_BITFONT(8,"wm.tar"));
	
	ZLC_InitApp();

	ZWMEVENT zEvent;
	while(true)
	{
		if(lpWM->PollEvent(&zEvent))
		{
			if(zEvent.nMsg==ZWM_QUIT) return ZLC_ExitApp();
			else if(zEvent.nMsg==ZWM_KEYDOWN)
			{
				short nKey=ZWM_LOWORD(zEvent.nParam1);
				if((nKey==SDLK_F2)&&(lpCD_LOSA==0))
				{
					lpCD_LOSA=new CZWM_COMDLG_LOSA(".","File-Explorer",ZCDS_EXPLORER);
					lpCD_LOSA->RegisterFileType(".zml",ZLC_LoadZRTFile_Callback);
				}
				else if(nKey=SDLK_F5)
				{
					SDL_SaveBMP(CZLC_KERNEL::lpCurViewPort->lpWnd->lpSurface,"lc_screen00.bmp");
				}
			}
			lpWM->DisptachEvent(&zEvent);
		}
		else
		{
			lpWM->UpdateView();
		}
	}
}

int ZLC_ExitApp()
{
	delete lpWM;
	SDL_Quit();
	return 0;
}

void ZLC_LoadZRTFile_Callback(char* szFile)
{
	if(cParser.Parse(szFile)!=0)
	{
		//new CZWM_MESSAGEBOX(cParser.GetErrString());
	}
	else
	{		
		CZLC_SCENE* lpNewScene=cParser.LoadScene();
	}
	cParser.CleanUp();
	lpCD_LOSA=0;
}

void ZLC_InitApp()
{
	cDefaultIni.Load("ZLC_Defaults.ini",DCLINI_NONCASESENSITIV);
	lpCD_LOSA=new CZWM_COMDLG_LOSA(".","File-Explorer",ZCDS_EXPLORER);
	lpCD_LOSA->RegisterFileType(".zml",ZLC_LoadZRTFile_Callback);
}
