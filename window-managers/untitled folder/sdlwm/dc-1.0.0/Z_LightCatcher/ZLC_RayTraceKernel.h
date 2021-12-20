#ifndef _ZLC_RAYTRACEKERNEL_H
#define _ZLC_RAYTRACEKERNEL_H

#include <SDL_thread.h>
#include "ZLC_DataSet.h"
#include "ZLC_Sampler.h"
#include "ZLC_Camera.h"
#include "ZLC_ViewPort.h"
#include "ZLC_Shader.h"
#include "ZLC_Light.h"
#include "ZLC_Material.h"
#include "ZLC_ZMLParser.h"

class CZLC_KERNEL
{
public:
	CZLC_KERNEL();
	~CZLC_KERNEL();
	static void SetCamera(CZLC_CAMERA* lpNewCam);
	static void SetSampler(CZLC_SAMPLER* lpNewSampler);
	static void SetViewPort(CZLC_VIEWPORT* lpViewPort);
	static void SetShader(CZLC_SHADER* lpShader);
	static void SetAmbientColor(CZLC_COLOR cNewCol);
	static CZLC_COLOR GetRayColor(CZLA_RAY* lpRay,CZLC_DATASET* lpDS,int nDepth);
	static int ThreadMain(void* lpData);
	static void StartTracing(CZLC_DATASET* lpDS);

	static CZLC_CAMERA*		lpCurCam;
	static CZLC_SAMPLER*	lpCurSampler;
	static CZLC_VIEWPORT*	lpCurViewPort;
	static CZLC_SHADER*		lpCurShader;
	static SDL_Thread*		lpThread;
	static ZLA_FLOAT		fFractionalIndex;

	static CZLC_COLOR		cAmbiCol;
};

#endif