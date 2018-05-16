#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <Dbghelp.h>

struct ImageSectionInfo
{
	char SectionName[IMAGE_SIZEOF_SECTION_HEADER];//the macro is defined WinNT.h
	char *SectionAddress;
	int SectionSize;
	ImageSectionInfo(const char* name)
	{
		strcpy_s(SectionName, IMAGE_SIZEOF_SECTION_HEADER, name); 
	}
};

void GetMemoryImageInformation(HMODULE& hModule);