#ifndef _ZTAR_MODULE_H
#define _ZTAR_MODULE_H

#include <SDL.h>
#include "ZWM_Types.h"

#define ZTAR_RECORDSIZE	512
#define ZTAR_NAMSIZE	100
#define ZTAR_TUNMLEN	32
#define ZTAR_TGNMLEN	32

// linkflag definitions
#define ZTAR_LF_OLDNORMAL	'\0'
#define	ZTAR_LF_NORMAL		'0'
#define ZTAR_LF_LINK		'1'
#define ZTAR_LF_SYMLINK		'2'
#define ZTAR_LF_CHR			'3'
#define ZTAR_LF_BLK			'4'
#define ZTAR_LF_DIR			'5'
#define ZTAR_LF_FIFO		'6'
#define ZTAR_LF_CONTIG		'7'

union ZTAR_RECORD
{
	char	szRecord[ZTAR_RECORDSIZE];
	struct ZTAR_HEADER
	{
		char	szName[ZTAR_NAMSIZE];
		char	mode[8];
		char	uid[8];
		char	gid[8];
		char	size[12];
		char	mtime[12];
		char	chksum[8];
		char	linkflag;
		char	szLinkname[ZTAR_NAMSIZE];
		char	szMagic[8];
		char	szUname[ZTAR_TUNMLEN];
		char	szGname[ZTAR_TGNMLEN];
		char	devmajor[8];
		char	devminor[8];
	}tarHeader;
};

FILE* ZOpenTarFile(char* szTarFile,char* szFile);
SDL_Surface* ZLoadTarImage(char* szTarFile,char* szFile);
void ZLoadTarDirToList(CZWM_LLIST* lpList,char* szTar,char* szPath);
void ZLoadTarToList(CZWM_LLIST* lpList,char* szTar,char* szPath);

#endif