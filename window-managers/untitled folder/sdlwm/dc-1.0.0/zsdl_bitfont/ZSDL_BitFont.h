#ifndef _ZSDL_BITFONT_H
#define _ZSDL_BITFONT_H

class CZSDL_BITFONT
{
public:
	CZSDL_BITFONT(int nHeight,char* szTar);
	~CZSDL_BITFONT();

	void DrawString(bool bUpdate,unsigned long nCol,SDL_Surface* lpSrf,short nX,short nY,short nNum,const char* szFmt,...);

private:
	void DrawChar(unsigned long nCol,SDL_Surface* lpSrf,char c,short nX,short nY);
	void SetPixel(SDL_Surface* lpSrf,int nX,int nY,unsigned long nCol);

	int				nH;
	unsigned char*	lpData;
};

#endif