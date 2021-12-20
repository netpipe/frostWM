#include <string.h>
#include <stdlib.h>
#include "ZSDL_WM.h"
#include "ZWM_FInfo.h"

#define _ZFINFO_FEXT(a,b) {if(ZWM_IsFileOfType(szFile,a)){sFR.nExtType=b;break;}}

int __ZWMFileInfoCallback(CZWM_WINDOW* lpWnd,int nMsg,int nParam1,int nParam2)
{
	switch(nMsg)
	{
	case ZWM_CLOSE:
//		CZSDL_WM::lpWM->DestroyWindow(lpWnd);
		break;
	}
	return 0;	
}

CZWM_FINFO::CZWM_FINFO(char* szFile)
{
	rcWnd.x=10;
	rcWnd.y=10;
	rcWnd.w=300;
	rcWnd.h=250;
	
	szTitle="FINFO";
	nWndType=ZWND_VISIBLE|ZWND_CAPTION|ZWND_SDLSURFACE|ZWND_MAXIMIZEBOX|ZWND_SYSMENU|ZWND_MINIMIZEBOX;
	lpMsgCallback=__ZWMFileInfoCallback;
	nClientCol=0xF0F0F0;

	memset(&sFR,0,sizeof(sFR));

	GetClientRect(&rcClient);

	SDL_PixelFormat* lpPF=CZSDL_WM::lpWM->lpDeskSrf->format;
	lpSurface=SDL_CreateRGBSurface(0,rcClient.w,rcClient.h,
		lpPF->BitsPerPixel,
		lpPF->Rmask,lpPF->Gmask,lpPF->Bmask,lpPF->Amask);
	ZWM_FillRect(lpSurface,0,nClientCol);

	CZSDL_WM::lpWM->RegisterWindow(this);

	SDL_Rect rcTB={5,5,280,210};
	lpTxtBox=new CZWM_TEXTBOX(0,this,rcTB,ZTB_NOEDIT|ZTB_MULTILINE|
		ZTB_AUTOVSCROLL|ZTB_AUTOHSCROLL);
	
	AnalyseFileName(szFile);
	AnalyseFileByData(szFile);

	lpTxtBox->DrawText();
	CZSDL_WM::lpWM->AddUpdateRectFromWnd(this);
}

CZWM_FINFO::~CZWM_FINFO()
{
	
}


char* g_szFINFO_FileType[]=
{
	"Unknown",
	"Windows bitmap",
	"Windows wave-audio",
	"Midi",
	"Windows Dynamic Linkable Library",
	"Dos/Windows executable",
	"Mpeg 3 audio",
	"Mpeg 1 video",
	"C/C++ header",
	"C++ source"
};

void CZWM_FINFO::AnalyseEXE(FILE* file,long nSize)
{
	_ZDOSEXE_HEADER zExeHeader;
	fread(&zExeHeader,28,1,file);

	lpTxtBox->SetColor(0xFF0000);
	lpTxtBox->printf("Last page size:\n");
	lpTxtBox->SetColor(0x000000);
	lpTxtBox->printf(" %i Bytes\n",zExeHeader.wLastPageSize);
	lpTxtBox->SetColor(0xFF0000);
	lpTxtBox->printf("Number of pages:\n");
	lpTxtBox->SetColor(0x000000);
	lpTxtBox->printf(" %i Pages\n",zExeHeader.wPages);
	lpTxtBox->SetColor(0xFF0000);
	lpTxtBox->printf("Relocation entries:\n");
	lpTxtBox->SetColor(0x000000);
	lpTxtBox->printf(" %i Entries\n",zExeHeader.wRelocationEntries);
	lpTxtBox->SetColor(0xFF0000);
	lpTxtBox->printf("Header size:\n");
	lpTxtBox->SetColor(0x000000);
	lpTxtBox->printf(" %i paragraphs\n",zExeHeader.wHeaderSize);
	lpTxtBox->SetColor(0xFF0000);
	lpTxtBox->printf("Minimal allocated memmory:\n");
	lpTxtBox->SetColor(0x000000);
	lpTxtBox->printf(" %i paragraphs\n",zExeHeader.wMinParagraphs);
	lpTxtBox->SetColor(0xFF0000);
	lpTxtBox->printf("Maximal allocated memmory:\n");
	lpTxtBox->SetColor(0x000000);
	lpTxtBox->printf(" %i paragraphs\n",zExeHeader.wMaxParagraphs);
	lpTxtBox->SetColor(0xFF0000);
	lpTxtBox->printf("Initial SS:\n");
	lpTxtBox->SetColor(0x000000);
	lpTxtBox->printf(" 0x%X\n",zExeHeader.wInitSS);
	lpTxtBox->SetColor(0xFF0000);
	lpTxtBox->printf("Initial SP:\n");
	lpTxtBox->SetColor(0x000000);
	lpTxtBox->printf(" 0x%X\n",zExeHeader.wInitSP);
	lpTxtBox->SetColor(0xFF0000);
	lpTxtBox->printf("Checksum:\n");
	lpTxtBox->SetColor(0x000000);
	lpTxtBox->printf(" %i\n",zExeHeader.wChecksum);
	lpTxtBox->SetColor(0xFF0000);
	lpTxtBox->printf("Entrypoint (CS:IP):\n");
	lpTxtBox->SetColor(0x000000);
	lpTxtBox->printf(" 0x%X\n",zExeHeader.dwCSIP);
	lpTxtBox->SetColor(0xFF0000);
	lpTxtBox->printf("Relocationtable offset:\n");
	lpTxtBox->SetColor(0x000000);
	lpTxtBox->printf(" 0x%X\n",zExeHeader.wRelocationTableOffset);
	lpTxtBox->SetColor(0xFF0000);
	lpTxtBox->printf("Overlay number:\n");
	lpTxtBox->SetColor(0x000000);
	lpTxtBox->printf(" %i\n",zExeHeader.wOverlayNumber);

	if(zExeHeader.wRelocationTableOffset==0x40)
	{
		if(nSize<64)
		{
			lpTxtBox->SetColor(0xFFAA00);
			lpTxtBox->printf("File broken ?");
			return;
		}

		_ZEXE_NEWHEADER zExeNewHeader;
		fread(&zExeNewHeader,36,1,file);

		if(zExeNewHeader.TLink.cID==0xFB)
		{
			lpTxtBox->SetColor(0xFFAA00);
			lpTxtBox->printf("Borland TLink Executable\n");
			lpTxtBox->SetColor(0xFF0000);
			lpTxtBox->printf("TLink Version:\n");
			lpTxtBox->SetColor(0x000000);
			lpTxtBox->printf(" %i\n",zExeNewHeader.TLink.cTLinkVer);
			return;
		}
		else if(strncmp(zExeNewHeader.OldArj.cID,"RJSX",4)==0)
		{
			lpTxtBox->SetColor(0xFFAA00);
			lpTxtBox->printf("Old ARJ SFX Archive\n");
			return;
		}
		else if(zExeNewHeader.LZ.cID[0]=='L'&&zExeNewHeader.LZ.cID[1]=='Z')
		{
			lpTxtBox->SetColor(0xFFAA00);
			lpTxtBox->printf("LZEXE compressed executable\n");
			lpTxtBox->SetColor(0xFF0000);
			lpTxtBox->printf("Version:\n");
			lpTxtBox->SetColor(0x000000);
			lpTxtBox->printf(" %c%c\n",zExeNewHeader.LZ.cVer[0],
				zExeNewHeader.LZ.cVer[1]);
			return;
		}
		else if(strncmp(zExeNewHeader.PKLite.cID,"PKLITE",6)==0)
		{
			lpTxtBox->SetColor(0xFFAA00);
			lpTxtBox->printf("PKLITE compressed executable\n");
			lpTxtBox->SetColor(0xFF0000);
			lpTxtBox->printf("Versionnumber:\n");
			lpTxtBox->SetColor(0x000000);
			lpTxtBox->printf(" %iMin %iMaj\n",zExeNewHeader.PKLite.cMinVer,
				zExeNewHeader.PKLite.cBitInfo&15);
			lpTxtBox->SetColor(0xFF0000);
			lpTxtBox->printf("Extra compression ?\n");
			lpTxtBox->SetColor(0x000000);
			lpTxtBox->printf(" %s\n",(zExeNewHeader.PKLite.cBitInfo&16)?"Yes":"No");
			lpTxtBox->SetColor(0xFF0000);
			lpTxtBox->printf("Multiple segments ?\n");
			lpTxtBox->SetColor(0x000000);
			lpTxtBox->printf(" %s\n",(zExeNewHeader.PKLite.cBitInfo&32)?"Yes":"No");
			return;
		}
		else if(strncmp(zExeNewHeader.LHARC.cID,"LHarc's SFX ",12)==0)
		{
			lpTxtBox->SetColor(0xFFAA00);
			lpTxtBox->printf("LHarc 1.x SFX archive\n");
			return;
		}
		else if(strncmp(zExeNewHeader.LHA.cID,"LHa's SFX ",10)==0)
		{
			lpTxtBox->SetColor(0xFFAA00);
			lpTxtBox->printf("LHA 2.10 SFX archive\n");
			return;
		}
		else if(strncmp(zExeNewHeader.LHA.cID,"LHA's SFX ",10)==0)
		{
			lpTxtBox->SetColor(0xFFAA00);
			lpTxtBox->printf("LHA 2.13 SFX archive\n");
			return;
		}
		else if(strncmp(zExeNewHeader.LH.cID,"LH's SFX ",8)==0)
		{
			lpTxtBox->SetColor(0xFFAA00);
			lpTxtBox->printf("LH SFX archive\n");
			return;
		}
		else if(zExeNewHeader.TopSpeed.dwID==0x018A0001&&
			zExeNewHeader.TopSpeed.wID==0x1565)
		{
			lpTxtBox->SetColor(0xFFAA00);
			lpTxtBox->printf("TopSpeed C 3.0 CRUNCH\ncompressed file\n");
			return;
		}
		else if(zExeNewHeader.PKARC.dwID==0x00020001&&
			zExeNewHeader.PKARC.wID==0x0700)
		{
			lpTxtBox->SetColor(0xFFAA00);
			lpTxtBox->printf("PKARC 3.5 SFX archive\n");
			return;
		}
		else if(zExeNewHeader.BSA.wID==0x000F&&
			zExeNewHeader.BSA.cID==0xA7)
		{
			lpTxtBox->SetColor(0xFFAA00);
			lpTxtBox->printf("BSA SFX archive\n");
			return;
		}
		else if(strncmp(zExeNewHeader.LARC.cID,"SFX by LARC ",11)==0)
		{
			lpTxtBox->SetColor(0xFFAA00);
			lpTxtBox->printf("LARC SFX archive\n");
			return;
		}
		else if(zExeNewHeader.NE_Header.dwNewHeaderOffset==0)
		{
			lpTxtBox->SetColor(0xFFAA00);
			lpTxtBox->printf("MS-DOS Executable\n");
			return;
		}
		else if((unsigned)nSize>=(zExeNewHeader.NE_Header.dwNewHeaderOffset+62))
		{
			fseek(file,zExeNewHeader.NE_Header.dwNewHeaderOffset,SEEK_SET);

			char cS_ID[4];
			fread(&cS_ID,4,1,file);

			if(cS_ID[0]=='N'&&cS_ID[1]=='E')
			{
				lpTxtBox->SetColor(0xFFAA00);
				lpTxtBox->printf("Windows Executable (NewExecutable NE)\n");
				fseek(file,zExeNewHeader.NE_Header.dwNewHeaderOffset,SEEK_SET);
			}
			else if(cS_ID[0]=='P'&&cS_ID[1]=='E'&&cS_ID[2]==0&&cS_ID[3]==0)
			{
				lpTxtBox->SetColor(0xFFAA00);
				lpTxtBox->printf("Windows Executable\n(PortableExecutable PE)\n");
				fseek(file,zExeNewHeader.NE_Header.dwNewHeaderOffset,SEEK_SET);

				_ZWINEXE_PE_HEADER zPEHdr;
				fread(&zPEHdr,248,1,file);

				lpTxtBox->SetColor(0xFF0000);
				lpTxtBox->printf("Sections:\n");
				lpTxtBox->SetColor(0x000000);
				lpTxtBox->printf(" %i\n",zPEHdr.FileHeader.NumberOfSections);
				//lpTxtBox->SetColor(0xFF0000);
				//lpTxtBox->printf("Section names:\n");
				/*lpTxtBox->SetColor(0x000000);*/

				/*for(Uint32 i=0;i<zPEHdr.FileHeader.NumberOfSections;i++)
				{
					_ZIMAGE_SECTION_HEADER zSec;
					fread(&zSec,40,1,file);
					lpTxtBox->printf(zSec.Name);
				}*/
			}
		}
		else
		{
			lpTxtBox->SetColor(0xFFAA00);
			lpTxtBox->printf("File broken ?\n");
			return;
		}
	}
	else
	{
		lpTxtBox->SetColor(0xFFAA00);
		lpTxtBox->printf("MS-DOS Executable\n");
	}
}

void CZWM_FINFO::AnalyseFileByData(char* szFile)
{
	FILE* file=fopen(szFile,"rb");
	if(file==0)
	{
		lpTxtBox->SetColor(0xFFAA00);
		lpTxtBox->printf("Couldn't open file for\nfurther analysis\n");
		return;
	}
	fseek(file,0,SEEK_END);
	long nFSize=ftell(file);
	fseek(file,0,SEEK_SET);
	lpTxtBox->SetColor(0xFF0000);
	lpTxtBox->printf("Filesize:\n");
	lpTxtBox->SetColor(0x000000);
	if(nFSize>0x100000)
	{
		lpTxtBox->printf(" %.2f MB\n",(float)nFSize/(float)0x100000);
	}
	else if(nFSize>0x400)
	{
		lpTxtBox->printf(" %.2f KB\n",(float)nFSize/1024.0f);
	}
	else
	{
		lpTxtBox->printf(" %i Bytes\n",nFSize);
	}

	if(nFSize==0)
	{
		lpTxtBox->SetColor(0xFFAA00);
		lpTxtBox->printf("File is not big enough for\ndata analysis\n");
		return;
	}

	unsigned short wID;
	fread(&wID,2,1,file);
	fseek(file,0,SEEK_SET);
	switch(wID)
	{
	case 'ZM':
		AnalyseEXE(file,nFSize); // DOS EXE
		break;
	}	
	fclose(file);
}

void CZWM_FINFO::AnalyseFileName(char* szFile)
{
	lpTxtBox->SetColor(0xFF0000);
	lpTxtBox->printf("filename:\n",szFile);
	lpTxtBox->SetColor(0x000000);
	lpTxtBox->printf(" %s\n",szFile);

	char* szExt=strrchr(szFile,'.');
	lpTxtBox->SetColor(0xFF0000);
	lpTxtBox->printf("extension:\n");
	lpTxtBox->SetColor(0x000000);
	if(szExt)
	{
		lpTxtBox->printf(" %s\n",++szExt);
	}
	else
	{
		lpTxtBox->printf(" n/a\n");
	}
	lpTxtBox->SetColor(0xFF0000);
	lpTxtBox->printf("associated file type:\n");
	lpTxtBox->SetColor(0x000000);
	
	if(szExt)
	{
		int nXTLen=strlen(szExt);
		switch(nXTLen)
		{
		case 1:
			{
				_ZFINFO_FEXT("h",FINFOFT_H);
				break;
			}
		case 3:
			{
				_ZFINFO_FEXT("bmp",FINFOFT_BMP);
				_ZFINFO_FEXT("wav",FINFOFT_WAV);
				_ZFINFO_FEXT("mid",FINFOFT_MID);
				_ZFINFO_FEXT("dll",FINFOFT_DLL);
				_ZFINFO_FEXT("exe",FINFOFT_EXE);
				_ZFINFO_FEXT("mp3",FINFOFT_MP3);
				_ZFINFO_FEXT("mpg",FINFOFT_MPG);
				_ZFINFO_FEXT("cpp",FINFOFT_CPP);
				break;
			}
		case 4:
			{
				_ZFINFO_FEXT("mpeg",FINFOFT_MPG);
				break;
			}
		default:
			{
				break;
			}
		}
		lpTxtBox->printf(" %s\n",g_szFINFO_FileType[sFR.nExtType]);
	}
	else
	{
		lpTxtBox->printf(" n/a\n");
	}
}