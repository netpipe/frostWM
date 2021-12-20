#include <SDL.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "ZSDL_BitFont.h"
#include "ztar_module.h"

CZSDL_BITFONT::CZSDL_BITFONT(int nHeight,char* szTar=0):nH(nHeight)
{
	char szFNam[32];
	FILE* file=0;
	sprintf(szFNam,"font8x%i.fnt",nH);

	if(szTar) file=ZOpenTarFile(szTar,szFNam);
	else file=fopen(szFNam,"rb");
	

	lpData=new unsigned char[256*nH];
	fread(lpData,nH,256,file);

	fclose(file);
}

CZSDL_BITFONT::~CZSDL_BITFONT()
{
	delete[] lpData;
}

void CZSDL_BITFONT::SetPixel(SDL_Surface* lpSrf,int nX,int nY,unsigned long nCol)
{
	if(nX<0||nY<0) return;
	if(nX>=lpSrf->w||nY>=lpSrf->h) return;
	unsigned char bpp=lpSrf->format->BytesPerPixel;

	unsigned char* bits=((unsigned char*)lpSrf->pixels)+nY*lpSrf->pitch+nX*bpp;

	switch(bpp) 
	{
		case 1:
			*((unsigned char*)(bits))=(unsigned char)nCol;
			break;
		case 2:
			*((unsigned short*)(bits))=(unsigned short)nCol;
			break;
		case 3: 
			{
				unsigned char r,g,b;
					r=(unsigned char)(nCol>>lpSrf->format->Rshift)&0xFF;
					g=(unsigned char)(nCol>>lpSrf->format->Gshift)&0xFF;
					b=(unsigned char)(nCol>>lpSrf->format->Bshift)&0xFF;
					*((bits)+lpSrf->format->Rshift/8) = r; 
					*((bits)+lpSrf->format->Gshift/8) = g;
					*((bits)+lpSrf->format->Bshift/8) = b;
			}
			break;
		case 4:
			*((unsigned long*)(bits))=(unsigned long)nCol;
			break;
	}
}

void CZSDL_BITFONT::DrawChar(unsigned long nCol,SDL_Surface* lpSrf,char c,short nX,short nY)
{
	int x,y;
	
	for(y=0;y<nH;y++)
	{
		for(x=0;x<8;x++)
		{
			if(lpData[(c*nH)+y]&(1<<x))
			{ 
				SetPixel(lpSrf,nX+(8-x),nY+(nH-y),nCol);
			}
		}
	}
}

inline Uint32 ZBF_MapRGBA(SDL_PixelFormat *fmt,Uint32 nCol)
{
	return SDL_MapRGBA(fmt,(nCol>>16)&0xFF,(nCol>>8)&0xFF,nCol&0xFF,0xFF);
}

void CZSDL_BITFONT::DrawString(bool bUpdate,unsigned long nCol,SDL_Surface* lpSrf,short nX,short nY,short nNum,const char* szFmt,...)
{
	va_list vl;
	char szBuffer[256];
	va_start(vl,szFmt);
	vsprintf(szBuffer,szFmt,vl);
	va_end(vl);

	short nBLen=strlen(szBuffer);
	if(nBLen<nNum) nNum=nBLen;

	SDL_Rect rcTxt={nX,nY,nNum*8,nH};

	if(SDL_MUSTLOCK(lpSrf)) 
	{
		if(SDL_LockSurface(lpSrf)<0) return;
	}

	nCol=ZBF_MapRGBA(lpSrf->format,nCol);

	for(unsigned short i=0;i<nNum;i++)
	{
		DrawChar(nCol,lpSrf,szBuffer[i],nX+(i*8),nY);
	}

	if(SDL_MUSTLOCK(lpSrf))
	{
		SDL_UnlockSurface(lpSrf);
	}

	if(bUpdate) SDL_UpdateRects(lpSrf,1,&rcTxt);
}