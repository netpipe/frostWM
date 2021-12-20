#include "ZSDL_WM.h"
#include "ZWM_WidgetContainer.h"

CZWM_WIDGETCONTAINER::CZWM_WIDGETCONTAINER(ZWM_MSGCALLBACK lpCB):lpChildWidget(0)
{
	CZSDL_WM* lpWM=CZSDL_WM::lpWM;

	rcWnd.x=100;
	rcWnd.y=100;
	rcWnd.w=10;
	rcWnd.h=10;
	
	szTitle="Container";
	nWndType=ZWND_VISIBLE|ZWND_CAPTION|ZWND_MAXIMIZEBOX|ZWND_SYSMENU|ZWND_MINIMIZEBOX;
	lpMsgCallback=lpCB;
	nClientCol=0xFF;
	
	GetClientRect(&rcClient);
	lpPrivateData=this;

	CZSDL_WM::lpWM->RegisterWindow(this);
}

void CZWM_WIDGETCONTAINER::Merge(CZWM_WINDOW* lpChild)
{
	lpChildWidget=lpChild;
	Resize(lpChild->rcWnd.w+6,lpChild->rcWnd.h+25);
}

CZWM_WIDGETCONTAINER::~CZWM_WIDGETCONTAINER()
{

}