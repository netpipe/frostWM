#ifndef _ZLC_SCENE_H
#define _ZLC_SCENE_H

#include "ZLC_Camera.h"
#include "ZLC_Sampler.h"
#include "ZLC_ViewPort.h"
#include "ZLC_Shader.h"

class CZLC_SCENE
{
public:
	CZLC_SCENE();
	~CZLC_SCENE();

	static int SceneWndCallback(CZWM_WINDOW* lpWnd,int nMsg,int nParam1,int nParam2);
	void SetActive();

	CZLC_CAMERA*	lpCamera;
	CZLC_SAMPLER*	lpSampler;
	CZLC_VIEWPORT*	lpViewPort;
	CZLC_SHADER*	lpShader;

	CZLC_COLOR		cAmb;
	CZLC_DATASET	cDataSet;

	CZWM_WINDOW*	lpWnd;
};

#endif