#include "config.h"
#ifdef WIN32
	#include <io.h>
	#include <direct.h>
#else
	#include <dirent.h>
	#include <sys/stat.h>
	#include <unistd.h>
#endif

#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "ZWM_Debug.h"
#include "ZSDL_WM.h"
#include "ZWM_CommonDlg.h"
#include "ztar_module.h"
#include "ZWM_FInfo.h"

bool ZWM_IsFileOfType(char* szFile,char* szType)
{
	char* szFE=szFile+strlen(szFile)-strlen(szType);

	for(unsigned int i=0;i<strlen(szType);i++)
	{
		char cFE=szFE[i];
		if(isupper(cFE)) cFE=tolower(cFE);
		if(cFE!=szType[i]) return false;
	}
	return true;
}

void CZWM_COMDLG_LOSA::RegisterFileType(char* szFT,ZWM_REGFT_CB lpCB)
{
	CZWM_LN_REGFT*	lpRegFT=new CZWM_LN_REGFT;
	lpRegFT->lpCallback=lpCB;
	strcpy(lpRegFT->szFTYPE,szFT);
	llRegFileTypes.AddNode(lpRegFT);
}

void CZWM_COMDLG_LOSA::ExplorerOpen(CZWM_FILENODE* lpFN)
{
	CZWM_LN_REGFT* lpCur=(CZWM_LN_REGFT*)llRegFileTypes.lpFirstNode;
	for(unsigned int i=0;i<llRegFileTypes.nNumNodes;i++)
	{
		if(ZWM_IsFileOfType(lpFN->szText,lpCur->szFTYPE)) 
		{
			lpCur->lpCallback(lpFN->szText);
			return;
		}
		lpCur=(CZWM_LN_REGFT*)lpCur->lpNextNode;
	}

	switch(lpFN->nType)
	{
#ifdef HAVE_LIBSMPEG
	case ZFL_MPG:
		{
			CZWM_WIDGETCONTAINER* lpWC=new CZWM_WIDGETCONTAINER(0);
			SDL_Rect rcMPG={0,0,0,0};
			CZWM_MPGWIDGET* lpMPG=new CZWM_MPGWIDGET(0xFF,lpWC,rcMPG);
			lpMPG->LoadMPG(lpFN->szText,true);
			lpWC->Merge(lpMPG);
		}
		break;
	case ZFL_MUS:
		{
			CZWM_WIDGETCONTAINER* lpWC=new CZWM_WIDGETCONTAINER(0);
			SDL_Rect rcMC={0,0,100,100};
			CZWM_MUSICCTRL* lpMC=new CZWM_MUSICCTRL(1,lpWC,rcMC);
			lpMC->Open(lpFN->szText);
			lpMC->Play();
			lpWC->Merge(lpMC);
		}
		break;
#endif 
	case ZFL_ZWM:
		{
			CZWM_MODULE* lpMod=new CZWM_MODULE(lpFN->szText);
		}
	default:
		break;
	}
}

int __ZWMCommonDlgLOSACallback(CZWM_WINDOW* lpWnd,int nMsg,int nParam1,int nParam2)
{
	CZWM_COMDLG_LOSA* lpLOSA=(CZWM_COMDLG_LOSA*)lpWnd->lpPrivateData;

	switch(nMsg)
	{
	case ZWM_BUTTONDOWN:
		switch(nParam1)
		{
		case 0:
			if(lpLOSA->lpParent)
			{
				CZWM_FILENODE* lpFN=(CZWM_FILENODE*)
					lpLOSA->llCurDir.GetNode(lpLOSA->lpLB->nSelection);
				if(lpFN)
				{
					lpLOSA->lpParent->lpMsgCallback(lpLOSA->lpParent,ZWM_OPENFILE,
						(int)lpFN,0);
				}
				CZSDL_WM::lpWM->DestroyWindow(lpWnd);
			}
			else if(lpLOSA->nStyle&ZCDS_EXPLORER)
			{
				CZWM_FILENODE* lpFN=(CZWM_FILENODE*)
					lpLOSA->llCurDir.GetNode(lpLOSA->lpLB->nSelection);
				if(lpFN)
				{
					lpLOSA->ExplorerOpen(lpFN);
				}
				CZSDL_WM::lpWM->DestroyWindow(lpWnd);
			}
			return 1;
		case 4:
			{
				CZWM_FILENODE* lpFN=(CZWM_FILENODE*)
					lpLOSA->llCurDir.GetNode(lpLOSA->lpLB->nSelection);
				if(lpFN)
				{
					new CZWM_FINFO(lpFN->szText);
				}
			}
			return 1;
		}
		return 1;
	case ZWM_SBPOSCHANGE:
		lpLOSA->Update();
		return 1;
#ifdef WIN32
	case ZWM_CBSELCHANGE:
		{
			CZWM_DRIVENODE* lpDrive=(CZWM_DRIVENODE*)lpLOSA->llDrives.GetNode(nParam1);
			if(!lpDrive) return 1;
			
			char szBuffer[256];
			lpLOSA->llCurDir.DeleteList();
			_chdrive(lpDrive->nID);
			_getdcwd(lpDrive->nID,szBuffer,256);
			lpLOSA->llCurDir.LoadDirectory(szBuffer);
			lpLOSA->lpLB->SetSelection(-1);
			lpLOSA->lpLB->SetList(&lpLOSA->llCurDir);
			lpLOSA->Update();

			return 1;
		}
#endif
	case ZWM_LBSELCHANGE:
		{
			CZWM_FILENODE* lpFile=(CZWM_FILENODE*)lpLOSA->llCurDir.GetNode(nParam1);
			if(!lpFile) return 1;
			if(lpFile->nType==ZFL_IMG)
			{
				if(lpLOSA->nStyle&ZCDS_IMAGEPRV)
				{
					lpLOSA->lpPBox->LoadImage(lpFile->szText);
				}
			}
			else if(lpFile->nType==ZFL_DIR) 
			{	
				char szBuffer[256]={0};
#ifdef WIN32
				_getdcwd(_getdrive(),szBuffer,256);
				strcat(szBuffer,"\\");
#else
				getcwd(szBuffer,256);
				strcat(szBuffer,"/");
#endif
				strcat(szBuffer,lpFile->szText);
				
				lpLOSA->llCurDir.DeleteList();

				chdir(szBuffer);

				lpLOSA->llCurDir.LoadDirectory(szBuffer);
				lpLOSA->lpLB->SetSelection(-1);
				lpLOSA->lpLB->SetList(&lpLOSA->llCurDir);
				lpLOSA->Update();
			}
		}
		return 1;
	}
	return 0;	
}

CZWM_LL_DIR::CZWM_LL_DIR():bTar(false)
{

}

#ifdef WIN32

CZWM_LL_DRIVES::CZWM_LL_DRIVES()
{

}

int CZWM_LL_DRIVES::LoadDrives()
{
	int nRet=0;
	char szWD[256];

	int nCurDrive=_getdrive();
	_getdcwd(nCurDrive,szWD,256);
	int nCnt=0;
	for(int i=2;i<=26;i++)
	{
		if(!_chdrive(i))
		{
			CZWM_DRIVENODE* lpNewDNode=new CZWM_DRIVENODE;
			char szBuffer[3];
			sprintf(szBuffer,"%c:",i+'A'-1);
			strcpy(lpNewDNode->szText,szBuffer);
			lpNewDNode->nColor=0;
			lpNewDNode->nID=i;
			AddNode(lpNewDNode);
			if(i==nCurDrive) nRet=nCnt;
			nCnt++;
		}
	}
	_chdrive(nCurDrive);
	_chdir(szWD);
	return nRet;
}

#endif

void CZWM_LL_DIR::LoadTarDirectory(char* szFile)
{
	bTar=true;
	strcpy(szTar,szFile);
	strcpy(szCurDir,"");
	ZLoadTarDirToList(this,szFile,"");
}

void CZWM_LL_DIR::SpecifyFileType(CZWM_FILENODE* lpFN)
{
#define ZWM_SETFT(x,a,b) {x->nType=a;x->nColor=b;}

	if(lpFN->nType==ZFL_DIR) lpFN->nColor=0xFF0000;
	if(lpFN->nType!=ZFL_FILE) return;

	ZWM_SETFT(lpFN,ZFL_FILE,0x000000);

	if(ZWM_IsFileOfType(lpFN->szText,".tar"))		ZWM_SETFT(lpFN,ZFL_TAR,0x0000FF)
	else if(ZWM_IsFileOfType(lpFN->szText,".bmp"))	ZWM_SETFT(lpFN,ZFL_IMG,0xFF00FF)
	else if(ZWM_IsFileOfType(lpFN->szText,".jpg"))	ZWM_SETFT(lpFN,ZFL_IMG,0xFF00FF)
	else if(ZWM_IsFileOfType(lpFN->szText,".xpm"))	ZWM_SETFT(lpFN,ZFL_IMG,0xFF00FF)
	else if(ZWM_IsFileOfType(lpFN->szText,".gif"))	ZWM_SETFT(lpFN,ZFL_IMG,0xFF00FF)
	else if(ZWM_IsFileOfType(lpFN->szText,".mpg"))	ZWM_SETFT(lpFN,ZFL_MPG,0xA05000)
	else if(ZWM_IsFileOfType(lpFN->szText,".mpeg")) ZWM_SETFT(lpFN,ZFL_MPG,0xA05000)
	else if(ZWM_IsFileOfType(lpFN->szText,".mp3"))	ZWM_SETFT(lpFN,ZFL_MUS,0x00FFAA)
	else if(ZWM_IsFileOfType(lpFN->szText,".mod"))	ZWM_SETFT(lpFN,ZFL_MUS,0xAA0FAA)
	else if(ZWM_IsFileOfType(lpFN->szText,".s3m"))	ZWM_SETFT(lpFN,ZFL_MUS,0x0FAAAA)
	else if(ZWM_IsFileOfType(lpFN->szText,".it"))	ZWM_SETFT(lpFN,ZFL_MUS,0xAAAA0F)
	else if(ZWM_IsFileOfType(lpFN->szText,".xm"))	ZWM_SETFT(lpFN,ZFL_MUS,0x0F0FAA)
	else if(ZWM_IsFileOfType(lpFN->szText,".zwm"))	ZWM_SETFT(lpFN,ZFL_ZWM,0xEB5530)
	
	
}

void CZWM_LL_DIR::LoadDirectory(char* szPath)
{
	if(nNumNodes) DeleteList();

#ifdef WIN32
	_finddata_t fData;
	int			nHandle;
	int			nRet=0;

	char szBuffer[256]={0};
	strcpy(szBuffer,szPath);
	strcat(szBuffer,"\\*");
	for(nHandle=_findfirst(szBuffer,&fData),nRet=nHandle;
		nRet!=-1;
		nRet=_findnext(nHandle,&fData))
	{
		CZWM_FILENODE* lpNewFNode=new CZWM_FILENODE;
		strcpy(lpNewFNode->szText,fData.name);

		if(fData.attrib&_A_SUBDIR) lpNewFNode->nType=ZFL_DIR;
		else lpNewFNode->nType=ZFL_FILE;

		SpecifyFileType(lpNewFNode);
		AddNode(lpNewFNode);
	}
#else
	dirent* lpData;
	char szBuffer[256]={0};
	strcpy(szBuffer,szPath);
	DIR* cur_dir;
	struct stat cur_stat;
	for(cur_dir=opendir(szBuffer),lpData=readdir(cur_dir);
		lpData!=0;lpData=readdir(cur_dir))
	{
		CZWM_FILENODE* lpNewFNode=new CZWM_FILENODE;
		strcpy(lpNewFNode->szText,lpData->d_name);
		stat(lpData->d_name,&cur_stat);

		if(cur_stat.st_mode&S_IFDIR) lpNewFNode->nType=ZFL_DIR;
		else lpNewFNode->nType=ZFL_FILE;

		SpecifyFileType(lpNewFNode);
		AddNode(lpNewFNode);
	}
	closedir(cur_dir);
#endif
}

CZWM_COMDLG_LOSA::CZWM_COMDLG_LOSA(char* szInitPath,
								   const char* szDLGTitle,int nWndStyle):
	nStyle(nWndStyle),lpPBox(0)
{
	CZSDL_WM* lpWM=CZSDL_WM::lpWM;

	rcWnd.x=10;
	rcWnd.y=10;
	rcWnd.w=(nStyle&ZCDS_IMAGEPRV)?450:320;
	rcWnd.h=200;
	
	szTitle=szDLGTitle;
	nWndType=ZWND_VISIBLE|ZWND_CAPTION|ZWND_SDLSURFACE|ZWND_MAXIMIZEBOX|ZWND_SYSMENU|ZWND_MINIMIZEBOX;
	lpMsgCallback=__ZWMCommonDlgLOSACallback;
	nClientCol=0xFF;

	GetClientRect(&rcClient);
	lpPrivateData=this;

	llCurDir.LoadDirectory(szInitPath);

	SDL_PixelFormat* lpPF=CZSDL_WM::lpWM->lpDeskSrf->format;
	lpSurface=SDL_CreateRGBSurface(0,rcClient.w,rcClient.h,
		lpPF->BitsPerPixel,
		lpPF->Rmask,lpPF->Gmask,lpPF->Bmask,lpPF->Amask);
	ZWM_FillRect(lpSurface,0,nClientCol);

	CZSDL_WM::lpWM->RegisterWindow(this);

	SDL_Rect rcBtn={250,150,60,23};
	new CZWM_BUTTON(0,this,rcBtn,"OK",0);

	SDL_Rect rcLB={5,25,305,122};
	lpLB=new CZWM_LISTBOX(1,this,rcLB,0);
	lpLB->SetList(&llCurDir);

#ifdef WIN32
	int nCurDN=llDrives.LoadDrives();
	SDL_Rect rcCB={5,5,100,80};
	CZWM_COMBOBOX* lpCB=new CZWM_COMBOBOX(2,this,rcCB,0);
	lpCB->SetList(&llDrives);
	lpCB->SetSelection(nCurDN);
#endif

	if(nStyle&ZCDS_IMAGEPRV)
	{
		SDL_Rect rcPB={320,5,115,160};
		lpPBox=new CZWM_PICTUREBOX(3,this,rcPB);
	}
	if(nStyle&ZCDS_FINFO)
	{
		SDL_Rect rcIBtn={50,150,60,23};
		new CZWM_BUTTON(4,this,rcIBtn,"Info",0);
	}

	Update();
}

CZWM_COMDLG_LOSA::~CZWM_COMDLG_LOSA()
{
	
}

void CZWM_COMDLG_LOSA::Update()
{
	SDL_Rect rcUpdate=rcWnd;
	ZWM_FillRect(lpSurface,&rcUpdate,nClientCol);

	CZSDL_WM::lpWM->AddUpdateRect(rcWnd);
}
