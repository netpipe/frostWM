#include "ZLC_RayTraceKernel.h"
#include "ZLC_ViewPort.h"

int CZLC_VIEWPORT::ViewportWndCallback(CZWM_WINDOW* lpWnd,int nMsg,
									   int nParam1,int nParam2)
{
	return 0;
}

CZLC_VIEWPORT::CZLC_VIEWPORT(int nWidth,int nHeight):nW(nWidth),nH(nHeight)
{
	SDL_Rect rcVP={10,10,nWidth,nHeight};
	lpWnd=CZSDL_WM::ZWMCreateWindow("Viewport",ViewportWndCallback,
		ZWND_CAPTION|ZWND_SDLSURFACE|ZWND_MAXIMIZEBOX|
		ZWND_SYSMENU|ZWND_MINIMIZEBOX,0,0xFFFFFF,0x0,rcVP);
	SDL_Rect rcCl=lpWnd->rcClient;
	
	rcVP.w=rcVP.w+(rcVP.w-rcCl.w);
	rcVP.h=rcVP.h+(rcVP.h-rcCl.h);
	lpWnd->Resize(rcVP.w,rcVP.h);

	CZSDL_WM::RegisterWindow(lpWnd);
}

CZLC_VIEWPORT::~CZLC_VIEWPORT()
{
	
}

void CZLC_VIEWPORT::RayTrace(CZLC_DATASET* lpDS)
{
	for(int y=0;y<nH;y++)
	{
		for(int x=0;x<nW;x++)
		{
			CZLC_COLOR cCol=CZLC_KERNEL::lpCurSampler->TracePxl(x,y,lpDS);
			
			long dwCol=cCol.GetRGB();
			ZWM_DrawPixel(dwCol,x,y,lpWnd->lpSurface);
		}
		CZSDL_WM::lpWM->AddUpdateRect(lpWnd->rcWnd);
	}
}