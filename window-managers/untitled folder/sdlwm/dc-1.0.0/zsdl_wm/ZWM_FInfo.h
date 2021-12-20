#ifndef _ZWM_FINFO_H
#define _ZWM_FINFO_H

#include "ZWM_Window.h"
#include "ZWM_VScrollBar.h"
#include "ZWM_PictureBox.h"
#include "ZWM_ListBox.h"
#include "ZWM_MPGWidget.h"

// Dos/Windows Exe/Dll

struct _ZDOSEXE_HEADER
{
	char	wID[2];
	Uint16	wLastPageSize;
	Uint16	wPages;
	Uint16	wRelocationEntries;
	Uint16	wHeaderSize;
	Uint16	wMinParagraphs;
	Uint16	wMaxParagraphs;
	Uint16	wInitSS;
	Uint16	wInitSP;
	Uint16	wChecksum;
	Uint32	dwCSIP;
	Uint16	wRelocationTableOffset;
	Uint16	wOverlayNumber;
};	// 28

struct _ZWINEXE_NE_HEADER
{
	char	wID[2];
	Uint8	cLinkerVer;
	Uint8	cLinkerRev;
	Uint16	wEntryTableOffset;
	Uint16	wEntryTableSize;
	Uint32	dwReserved;
	Uint16	wBitFlags;
	Uint16	wHeapSize;
	Uint16	wStackSize;
	Uint32	dwCSIPOffset;
	Uint32	dwSSSPOffset;
	Uint16	wSegmentTableSize;
	Uint16	wModuleRefTableSize;
	Uint16	wNonResNameTableSize;
	Uint16	wSegTableOffset;
	Uint16	wResTableOffset;
	Uint16	wResNameTableOffset;
	Uint16	wModRefTableOffset;
	Uint16	wImpNameTableOffset;
	Uint32	dwNonResNameTableOffset;
	Uint16	wMoveableEntryPoints;
	Uint16	wLogicalSecShiftCount;
	Uint16	wResSegments;
	Uint8	cTargetOS;
	Uint8	cOSInfo;
	Uint16	wFastLoadOffset;
	Uint16	wFastLoadLength;
	Uint16	wReserved;
	Uint16	wExpecWinVer;
}; // 62

struct _ZIMAGE_FILE_HEADER 
{
    Uint16	Machine;
    Uint16	NumberOfSections;
    Uint32	TimeDateStamp;
    Uint32	PointerToSymbolTable;
    Uint32	NumberOfSymbols;
    Uint16	SizeOfOptionalHeader;
    Uint16	Characteristics;
}; // 20

struct _ZIMAGE_DATA_DIRECTORY {
    Uint32   VirtualAddress;
    Uint32   Size;
}; //8

#define _ZIMAGE_NUMBEROF_DIRECTORY_ENTRIES    16

struct _ZIMAGE_OPTIONAL_HEADER 
{
    Uint16    Magic;
    Uint8    MajorLinkerVersion;
    Uint8    MinorLinkerVersion;
    Uint32   SizeOfCode;
    Uint32   SizeOfInitializedData;
    Uint32   SizeOfUninitializedData;
    Uint32   AddressOfEntryPoint;
    Uint32   BaseOfCode;
    Uint32   BaseOfData;

    Uint32   ImageBase;
    Uint32   SectionAlignment;
    Uint32   FileAlignment;
    Uint16    MajorOperatingSystemVersion;
    Uint16    MinorOperatingSystemVersion;
    Uint16    MajorImageVersion;
    Uint16    MinorImageVersion;
    Uint16    MajorSubsystemVersion;
    Uint16    MinorSubsystemVersion;
    Uint32   Win32VersionValue;
    Uint32   SizeOfImage;
    Uint32   SizeOfHeaders;
    Uint32   CheckSum;
    Uint16    Subsystem;
    Uint16    DllCharacteristics;
    Uint32   SizeOfStackReserve;
    Uint32   SizeOfStackCommit;
    Uint32   SizeOfHeapReserve;
    Uint32   SizeOfHeapCommit;
    Uint32   LoaderFlags;
    Uint32   NumberOfRvaAndSizes;
    _ZIMAGE_DATA_DIRECTORY DataDirectory[_ZIMAGE_NUMBEROF_DIRECTORY_ENTRIES];
}; // 224

struct _ZWINEXE_PE_HEADER
{
	char					cID[4];			// 4
	_ZIMAGE_FILE_HEADER		FileHeader;		// 20
	_ZIMAGE_OPTIONAL_HEADER	OptionalHeader;	// 224
}; // 248

struct _ZIMAGE_SECTION_HEADER 
{
    char    Name[8];
    union {
            Uint32   PhysicalAddress;
            Uint32   VirtualSize;
    }Misc;
    Uint32   VirtualAddress;
    Uint32   SizeOfRawData;
    Uint32   PointerToRawData;
    Uint32   PointerToRelocations;
    Uint32   PointerToLinenumbers;
    Uint16    NumberOfRelocations;
    Uint16    NumberOfLinenumbers;
    Uint32   Characteristics;
}; // 40

union _ZEXE_NEWHEADER
{
	struct
	{
		Uint8	cUK[4];
		Uint16	wBehaviourBits;
		Uint8	cReserved[26];
		Uint32	dwNewHeaderOffset;
	}NE_Header; // 36
	
	struct
	{
		Uint8	cUK[2];
		char	cID;
		Uint8	cTLinkVer;
		Uint8	cUK2[2];
	}TLink; // 6

	struct
	{
		char	cID[4];
	}OldArj; // 4

	struct
	{
		char	cID[2];
		Uint8	cVer[2];
	}LZ; // 4

	struct
	{
		Uint8	cMinVer;
		Uint8	cBitInfo;
		char	cID[6];
	}PKLite; // 8

	struct
	{
		Uint8	cUnused[4];
		Uint8	cExCodeJmp[3];
		Uint8	cUK[2];
		char	cID[12];
	}LHARC; // 21

	struct
	{
		Uint8	cUK[8];
		char	cID[10];
	}LHA; // 18

	struct
	{
		Uint8	cUK[8];
		char	cID[8];
	}LH; // 16

	struct
	{
		Uint32	dwID;
		Uint16	wID; // 6
	}TopSpeed;

	struct
	{
		Uint32	dwID;
		Uint16	wID;
	}PKARC; // 6

	struct
	{
		Uint16	wID;
		Uint8	cID;
	}BSA; // 3

	struct
	{
		Uint8	cUK[4];
		char	cID[11];
	}LARC; // 15
}; //36

// General Stuff
enum EFINFO_FTYPE
{
	FINFOFT_NONE,
	FINFOFT_BMP,
	FINFOFT_WAV,
	FINFOFT_MID,
	FINFOFT_DLL,
	FINFOFT_EXE,
	FINFOFT_MP3,
	FINFOFT_MPG,
	FINFOFT_H,
	FINFOFT_CPP
};

struct SFINFO_RESULT
{
	int		nExtType;
};

class CZWM_FINFO:public CZWM_WINDOW
{
public:
	CZWM_FINFO(char* szFile);
	~CZWM_FINFO();

	void AnalyseFileName(char* szFile);
	void AnalyseFileByData(char* szFile);
	void AnalyseEXE(FILE* file,long nSize);

	CZWM_TEXTBOX*		lpTxtBox;
	SFINFO_RESULT		sFR;
};

#endif