#include "config.h"
#include <SDL_image.h>
#include "ZWM_Graph.h"

inline Uint32 ZWM_MapRGBA(SDL_PixelFormat *fmt,Uint32 nCol)
{
	return SDL_MapRGBA(fmt,(nCol>>16)&0xFF,(nCol>>8)&0xFF,nCol&0xFF,0xFF);
}

void ZWM_FillRect(SDL_Surface* lpSurface,SDL_Rect* lpRect,Uint32 nCol)
{
	SDL_FillRect(lpSurface,lpRect,ZWM_MapRGBA(lpSurface->format,nCol));
}

void ZWM_DrawPixel(Uint32 nCol,int nX,int nY,SDL_Surface* lpSurface)
{
	Uint8	*bits,bpp;

	if(SDL_MUSTLOCK(lpSurface)) 
	{
		if(SDL_LockSurface(lpSurface)<0) return;
	}
	bpp=lpSurface->format->BytesPerPixel;
	bits=((Uint8*)lpSurface->pixels)+nY*lpSurface->pitch+nX*bpp;

	switch(bpp)
	{
		case 1:
			*((Uint8*)(bits))=(Uint8)nCol;
			break;
		case 2:
			*((Uint16*)(bits))=(Uint16)nCol;
			break;
		case 3: 
			{
				Uint8 r,g,b;
				r=(nCol>>lpSurface->format->Rshift)&0xFF;
				g=(nCol>>lpSurface->format->Gshift)&0xFF;
				b=(nCol>>lpSurface->format->Bshift)&0xFF;
				*((bits)+lpSurface->format->Rshift/8)=r; 
				*((bits)+lpSurface->format->Gshift/8)=g;
				*((bits)+lpSurface->format->Bshift/8)=b;
			}
			break;
		case 4:
			*((Uint32*)(bits))=(Uint32)nCol;
			break;
	}

	if(SDL_MUSTLOCK(lpSurface))
	{
		SDL_UnlockSurface(lpSurface);
	}
}

Uint32 ZWM_GetPixel(int nX,int nY,SDL_Surface* lpSurface)
{
	Uint32	nCol=0;
	Uint8	*bits,bpp;

	if(SDL_MUSTLOCK(lpSurface)) 
	{
		if(SDL_LockSurface(lpSurface)<0) return 0;
	}
	bpp=lpSurface->format->BytesPerPixel;
	bits=((Uint8*)lpSurface->pixels)+nY*lpSurface->pitch+nX*bpp;

	switch(bpp)
	{
		case 1:
			nCol=*((Uint8*)(bits));
			break;
		case 2:
			nCol=*((Uint16*)(bits));
			break;
		case 3: 
			/*{
				Uint8 r,g,b;
				r=(nCol>>lpSurface->format->Rshift)&0xFF;
				g=(nCol>>lpSurface->format->Gshift)&0xFF;
				b=(nCol>>lpSurface->format->Bshift)&0xFF;
				*((bits)+lpSurface->format->Rshift/8)=r; 
				*((bits)+lpSurface->format->Gshift/8)=g;
				*((bits)+lpSurface->format->Bshift/8)=b;
			}
			break;*/
		case 4:
			nCol=*((Uint32*)(bits));
			break;
	}

	if(SDL_MUSTLOCK(lpSurface))
	{
		SDL_UnlockSurface(lpSurface);
	}

	//nCol|=SDL_MapRGBA(lpSurface->format,0,0,0,255);

	return nCol;
}

void ZWM_ClipXToRect(int* nX,SDL_Rect& rcRect)
{
	if(*nX<rcRect.x) *nX=rcRect.x;
	if(*nX>rcRect.x+rcRect.w-1) *nX=rcRect.x+rcRect.w-1;
}

void ZWM_ClipYToRect(int* nY,SDL_Rect& rcRect)
{
	if(*nY<rcRect.y) *nY=rcRect.y;
	if(*nY>rcRect.y+rcRect.h-1) *nY=rcRect.y+rcRect.h-1;
}

void ZWM_DrawHLine(Uint32 nCol,int nX1,int nX2,int nY,SDL_Surface* lpSurface)
{
	nCol=ZWM_MapRGBA(lpSurface->format,nCol);
	if(nY<0) return;
	if(nY>lpSurface->h-1) return;
	SDL_Rect rcSurface={0,0,lpSurface->w,lpSurface->h};
	ZWM_ClipXToRect(&nX1,rcSurface);
	ZWM_ClipXToRect(&nX2,rcSurface);

	if(nX2<nX1)
	{
		int nTmp=nX2;
		nX2=nX1;
		nX1=nTmp;
	}

	if(SDL_MUSTLOCK(lpSurface)) 
	{
		if(SDL_LockSurface(lpSurface)<0) return;
	}
	
	Uint8	*bits,bpp;
	bpp=lpSurface->format->BytesPerPixel;

	for(int nX=nX1;nX<=nX2;nX++)
	{
		bits=((Uint8*)lpSurface->pixels)+nY*lpSurface->pitch+nX*bpp;
	
		switch(bpp)
		{
			case 1:
				*((Uint8*)(bits))=(Uint8)nCol;
				break;
			case 2:
				*((Uint16*)(bits))=(Uint16)nCol;
				break;
			case 3: 
				{
					Uint8 r,g,b;
	
					r=(nCol>>lpSurface->format->Rshift)&0xFF;
					g=(nCol>>lpSurface->format->Gshift)&0xFF;
					b=(nCol>>lpSurface->format->Bshift)&0xFF;
					*((bits)+lpSurface->format->Rshift/8)=r; 
					*((bits)+lpSurface->format->Gshift/8)=g;
					*((bits)+lpSurface->format->Bshift/8)=b;
				}
				break;
			case 4:
				*((Uint32*)(bits))=(Uint32)nCol;
				break;
		}
	}
	
	if(SDL_MUSTLOCK(lpSurface))
	{
		SDL_UnlockSurface(lpSurface);
	}
}

void ZWM_DrawVLine(Uint32 nCol,int nX,int nY1,int nY2,SDL_Surface* lpSurface)
{
	nCol=ZWM_MapRGBA(lpSurface->format,nCol);
	if(nX<0) return;
	if(nX>lpSurface->w-1) return;
	SDL_Rect rcSurface={0,0,lpSurface->w,lpSurface->h};
	ZWM_ClipYToRect(&nY1,rcSurface);
	ZWM_ClipYToRect(&nY2,rcSurface);

	if(nY2<nY1)
	{
		int nTmp=nY2;
		nY2=nY1;
		nY1=nTmp;
	}

	if(SDL_MUSTLOCK(lpSurface)) 
	{
		if(SDL_LockSurface(lpSurface)<0) return;
	}
	
	Uint8	*bits,bpp;
	bpp=lpSurface->format->BytesPerPixel;

	for(int nY=nY1;nY<=nY2;nY++)
	{
		bits=((Uint8*)lpSurface->pixels)+nY*lpSurface->pitch+nX*bpp;
	
		switch(bpp)
		{
			case 1:
				*((Uint8*)(bits))=(Uint8)nCol;
				break;
			case 2:
				*((Uint16*)(bits))=(Uint16)nCol;
				break;
			case 3: 
				{
					Uint8 r,g,b;
	
					r=(nCol>>lpSurface->format->Rshift)&0xFF;
					g=(nCol>>lpSurface->format->Gshift)&0xFF;
					b=(nCol>>lpSurface->format->Bshift)&0xFF;
					*((bits)+lpSurface->format->Rshift/8)=r; 
					*((bits)+lpSurface->format->Gshift/8)=g;
					*((bits)+lpSurface->format->Bshift/8)=b;
				}
				break;
			case 4:
				*((Uint32*)(bits))=(Uint32)nCol;
				break;
		}
	}
	
	if(SDL_MUSTLOCK(lpSurface))
	{
		SDL_UnlockSurface(lpSurface);
	}
}

void ZWM_DrawRect(Uint32 nCol,SDL_Rect rcRect,SDL_Surface* lpSurface)
{
	ZWM_DrawVLine(nCol,rcRect.x,rcRect.y,rcRect.y+rcRect.h-1,lpSurface);
	ZWM_DrawVLine(nCol,rcRect.x+rcRect.w-1,rcRect.y,rcRect.y+rcRect.h-1,lpSurface);
	ZWM_DrawHLine(nCol,rcRect.x,rcRect.x+rcRect.w-1,rcRect.y,lpSurface);
	ZWM_DrawHLine(nCol,rcRect.x,rcRect.x+rcRect.w-1,rcRect.y+rcRect.h-1,lpSurface);
}

void ZWM_DrawStereoRect(Uint32 nCol1,Uint32 nCol2,SDL_Rect rcRect,SDL_Surface* lpSurface)
{
	ZWM_DrawHLine(nCol1,rcRect.x,rcRect.x+rcRect.w-1,rcRect.y,lpSurface);
	ZWM_DrawVLine(nCol1,rcRect.x,rcRect.y,rcRect.y+rcRect.h-1,lpSurface);

	ZWM_DrawHLine(nCol2,rcRect.x,rcRect.x+rcRect.w-1,rcRect.y+rcRect.h-1,lpSurface);
	ZWM_DrawVLine(nCol2,rcRect.x+rcRect.w-1,rcRect.y,rcRect.y+rcRect.h-1,lpSurface);
}

bool ZWM_LoadScaledImageToSrf(char* szFile,SDL_Surface* lpSrf)
{
	SDL_Rect rcBound={0,0,lpSrf->w,lpSrf->h};

#ifdef HAVE_LIBSDL_IMAGE
	SDL_Surface* lpImgSurface=IMG_Load(szFile);
#else
	SDL_Surface* lpImgSurface=SDL_LoadBMP(szFile);
#endif
	if(lpImgSurface==0) return false;

	Uint32 nPxl=0;
	int nSrc_W=lpImgSurface->w;
	int nSrc_H=lpImgSurface->h;

	int nDst_W=lpSrf->w;
	int nDst_H=lpSrf->h;
	

	for(int nDY=0;nDY<nDst_H;nDY++)
	{
		for(int nDX=0;nDX<nDst_W;nDX++)
		{
			int nSX=(nSrc_W*nDX)/nDst_W;
			int nSY=(nSrc_H*nDY)/nDst_H;

			SDL_Rect rcSrc={nSX,nSY,1,1};
			SDL_Rect rcDst={nDX+1,nDY+1,1,1};
			SDL_BlitSurface(lpImgSurface,&rcSrc,lpSrf,&rcDst);
		}
	}
	SDL_FreeSurface(lpImgSurface);
	return true;
}