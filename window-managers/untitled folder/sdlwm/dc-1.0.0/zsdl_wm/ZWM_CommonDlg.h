#ifndef _ZWM_COMMONDLG_H
#define _ZWM_COMMONDLG_H

#include "ZWM_Window.h"
#include "ZWM_VScrollBar.h"
#include "ZWM_PictureBox.h"
#include "ZWM_ListBox.h"
#include "ZWM_MPGWidget.h"

class CZWM_FILENODE:public CZWM_LB_NODE
{
public:
	int		nType;
};

#ifdef WIN32
class CZWM_DRIVENODE:public CZWM_LB_NODE
{
public:
	int		nID;
};

class CZWM_LL_DRIVES:public CZWM_LLIST
{
public:
	CZWM_LL_DRIVES();

	int LoadDrives();
};
#endif

class CZWM_LL_DIR:public CZWM_LLIST
{
public:
	CZWM_LL_DIR();

	void LoadDirectory(char* szPath);
	void LoadTarDirectory(char* szFile);
	void SpecifyFileType(CZWM_FILENODE* lpFN);

	char szTar[256];
	char szCurDir[256];
	bool bTar;
};

typedef void (*ZWM_REGFT_CB)(char* szFile);

class CZWM_LN_REGFT:public CZWM_LL_NODE
{
public:
	char			szFTYPE[16];
	ZWM_REGFT_CB	lpCallback;
};

enum ZFL_TYPE
{
	ZFL_NONE,
	ZFL_FILE,
	ZFL_DIR,
	ZFL_TAR,
	ZFL_IMG,
	ZFL_MPG,
	ZFL_MUS,
	ZFL_ZWM
};	

enum ZCDLG_SYTLE
{
	ZCDS_NONE=0,
	ZCDS_IMAGEPRV=1,
	ZCDS_EXPLORER=2,
	ZCDS_FINFO=4
};

bool ZWM_IsFileOfType(char* szFile,char* szType);

class CZWM_COMDLG_LOSA:public CZWM_WINDOW
{
public:
	CZWM_COMDLG_LOSA(char *szInitPath,const char* szDLGTitle,int nWndStyle);
	~CZWM_COMDLG_LOSA();
	void ExplorerOpen(CZWM_FILENODE* lpFN);
	void RegisterFileType(char* szFT,ZWM_REGFT_CB lpCB);

	void Update();

	CZWM_LLIST			llRegFileTypes;
	CZWM_LL_DIR			llCurDir;
#ifdef WIN32
	CZWM_LL_DRIVES		llDrives;
#endif
	CZWM_LISTBOX*		lpLB;
	int					nStyle;

	CZWM_PICTUREBOX*	lpPBox;
};

#endif