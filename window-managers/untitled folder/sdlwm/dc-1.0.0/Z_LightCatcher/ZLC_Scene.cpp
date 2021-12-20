#include "ZLC_Scene.h"
#include "ZLC_RayTraceKernel.h"

int CZLC_SCENE::SceneWndCallback(CZWM_WINDOW* lpWnd,int nMsg,
									   int nParam1,int nParam2)
{
	CZLC_SCENE* lpScene=(CZLC_SCENE*)lpWnd->lpPrivateData;
	switch(nMsg)
	{
	case ZWM_BUTTONDOWN:
		{
			switch(nParam1)
			{
			case 1:
				lpScene->lpViewPort->lpWnd->SetVisible(true);
				lpScene->SetActive();
				CZLC_KERNEL::StartTracing(&lpScene->cDataSet);
				return 1;
			}
			break;
		}
	}
	return 0;
}

CZLC_SCENE::CZLC_SCENE():lpCamera(0),lpSampler(0),lpViewPort(0),lpShader(0)
{
	SDL_Rect rcVP={30,30,200,200};
	lpWnd=CZSDL_WM::ZWMCreateWindow("Scene",SceneWndCallback,
		ZWND_VISIBLE|ZWND_CAPTION|ZWND_SDLSURFACE|ZWND_MAXIMIZEBOX|
		ZWND_SYSMENU|ZWND_MINIMIZEBOX,0,0xFFFFFF,0x0,rcVP);
	SDL_Rect rcCl=lpWnd->rcClient;
	
	rcVP.w=rcVP.w+(rcVP.w-rcCl.w);
	rcVP.h=rcVP.h+(rcVP.h-rcCl.h);
	lpWnd->Resize(rcVP.w,rcVP.h);

	CZSDL_WM::RegisterWindow(lpWnd);

	SDL_Rect rcBtn1={100,100,50,50};
	CZWM_BUTTON* lpButton=new CZWM_BUTTON(1,lpWnd,rcBtn1,"Start",0);

	lpWnd->lpPrivateData=this;
}

void CZLC_SCENE::SetActive()
{
	CZLC_KERNEL::cAmbiCol=cAmb;
	CZLC_KERNEL::lpCurCam=lpCamera;
	CZLC_KERNEL::lpCurViewPort=lpViewPort;
	CZLC_KERNEL::lpCurSampler=lpSampler;
	CZLC_KERNEL::lpCurShader=lpShader;
}

CZLC_SCENE::~CZLC_SCENE()
{

}