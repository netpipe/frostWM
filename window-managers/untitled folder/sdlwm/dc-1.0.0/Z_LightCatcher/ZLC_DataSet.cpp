#include "ZLC_DataSet.h"
#include "ZLA_Triangle.h"

CZLC_DATASET::CZLC_DATASET() {}
CZLC_DATASET::~CZLC_DATASET() {}

void CZLC_DATASET::Merge3DSFile(char* szFile)
{
	C3DS_OBJECT c3DS(szFile);

	DS_OBJECT* lpCurObj=c3DS.lpFirstObj;
	for(DSUINT32 i=0;i<c3DS.dwNumObj;i++)
	{
		DS_FACEMAT* lpCurFMat=lpCurObj->lpFirstFMat;
		for(DSUINT32 j=0;j<lpCurObj->nNumFMat;j++)
		{
			/*glBindTexture(GL_TEXTURE_2D,lpCurFMat->dwLM);*/

			for(DSUINT32 k=0;k<lpCurFMat->nFaces;k++)
			{
				DSUINT16 nFIDX=lpCurFMat->lpFace[k];
				DSUINT16 wA=lpCurObj->lpFaces[nFIDX].wA;
				DSUINT16 wB=lpCurObj->lpFaces[nFIDX].wB;
				DSUINT16 wC=lpCurObj->lpFaces[nFIDX].wC;

				CZLA_TRIANGLE* lpTri1=new CZLA_TRIANGLE
				(
					CZLA_VECTOR4
					(
						lpCurObj->lpVert[wA].fVal[0],
						lpCurObj->lpVert[wA].fVal[1],
						lpCurObj->lpVert[wA].fVal[2],1
					),
					CZLA_VECTOR4
					(
						lpCurObj->lpVert[wB].fVal[0],
						lpCurObj->lpVert[wB].fVal[1],
						lpCurObj->lpVert[wB].fVal[2],1
					),
					CZLA_VECTOR4
					(
						lpCurObj->lpVert[wC].fVal[0],
						lpCurObj->lpVert[wC].fVal[1],
						lpCurObj->lpVert[wC].fVal[2],1
					)
				);
				//lpTri1->cDiffCol=CZLC_COLOR(0.9,0.9,0.9);
				llObjects.AddNode(lpTri1);

				/*glBegin(GL_TRIANGLES);
					glTexCoord2f(lpCurObj->lpUV[wA].U,lpCurObj->lpUV[wA].V);
					glVertex3f(lpCurObj->lpVert[wA].fVal[0],
						lpCurObj->lpVert[wA].fVal[1],
						lpCurObj->lpVert[wA].fVal[2]);

					glTexCoord2f(lpCurObj->lpUV[wB].U,lpCurObj->lpUV[wB].V);
					glVertex3f(lpCurObj->lpVert[wB].fVal[0],
						lpCurObj->lpVert[wB].fVal[1],
						lpCurObj->lpVert[wB].fVal[2]);

					glTexCoord2f(lpCurObj->lpUV[wC].U,lpCurObj->lpUV[wC].V);
					glVertex3f(lpCurObj->lpVert[wC].fVal[0],
						lpCurObj->lpVert[wC].fVal[1],
						lpCurObj->lpVert[wC].fVal[2]);

				glEnd();*/
			}
			lpCurFMat=lpCurFMat->lpNext;
		}
		lpCurObj=lpCurObj->lpNext;
	}
}