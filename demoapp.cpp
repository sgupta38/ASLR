//
//	Authotr: Sonu Gupta
//
//	Description: This standalone application simply loads two DLL's. For One DLL ASLR flag is turned ON and for other its turned OFF.
//				I have used windows API 'ImageNtHeader()', using this we can explore 'Portable Executable' format of application.
//
//
//	Note: I have just loaded DLL and have not used any of its exported functions.

#include "aslr.h"


//
//	Input: Handle of loaded DLL
//
//	Output: We are just iterating over each sections available in DLL and printing
//			its starting address.
//

void GetMemoryImageInformation(HMODULE& hModule)
{
	PVOID pCodeStart;
	IMAGE_NT_HEADERS *pNtHdr;
	ImageSectionInfo *pSectionInfo = NULL;
	IMAGE_SECTION_HEADER *pSectionHdr, *pSectionLister;

	//	Get the address of NT Header
	pNtHdr = ImageNtHeader(hModule);

	//	After Nt headers comes the table of section, so get the addess of section table

	pSectionHdr = pSectionLister = (IMAGE_SECTION_HEADER *) (pNtHdr + 1);

	//	Iterate through the list of all sections, and check the section name in the if conditon. etc

	printf("\n Base address: %p", hModule);
	printf("\n========================================");
	printf("\n%*s %*s", 10, "SECTION", 20, "ADDRESS");
	printf("\n========================================");

	for ( int i = 0 ; i < pNtHdr->FileHeader.NumberOfSections ; i++ )
	{
		char *name = (char*) pSectionHdr->Name;
		if( 0 == strcmp(name, ".text") ||
			0 == strcmp(name, ".data") ||
			0 == strcmp(name, ".reloc")
		)
		{
			pSectionInfo = new ImageSectionInfo((char*) pSectionHdr->Name);
			pCodeStart = (PVOID)(((PBYTE)hModule) + (SIZE_T)((PBYTE)pSectionHdr->VirtualAddress) );
			printf("\n %*s %*p",10, (char*) pSectionHdr->Name, 20, pCodeStart);
			delete pSectionInfo;
		}
			pSectionHdr++;

	}
}

int
main()
{
	HMODULE hModule = LoadLibrary(L"DemoDll.dll");
	HMODULE hModule2 = LoadLibrary(L"DemoDll2.dll");

	printf("\nDLL with ASLR flag OFF.");
	GetMemoryImageInformation(hModule);
	printf("\n\n\nDLL with ASLR flag ON.");
	GetMemoryImageInformation(hModule2);

	FreeLibrary(hModule);
	FreeLibrary(hModule2);

	_getch();
	return 0;
}


/*
	Portable Execytable (PE)
                  BRIEF VIEW OF PE FILE

                .----------------------.
                |                      |
                |    Other stuff not   |
                |    touched in this   |
                |    program           |
                |                      |
                |----------------------|
                |                      |
                | Various Section like |
                |        .....         |
                |        .....         |
        .------>|       .reloc         |
        | .---->|       .idata         |
        | | .-->|       .data          |
        | | | .>|       .text          |
        | | | | |----------------------|
        '-|-|-|-|                      | <--- Each entry in section table have pointer
          '-|-|-|         Section      |      offsets to actual sections
            '-|-|     Header or Table  |
              '-|                      |      ---.----------------.
                |----------------------|-----/   |   PE Optional  |  1) ImageBase
                |                      |         |    Header      |
                |                      |         |                |
                |        NT Headers    |         |----------------|
                |                      |         |     COFF/PE    |  1) NumberOfSections
                |                      |         |   Header Info  |  2) SizeOfOptionalHeader
                |----------------------|-----    |----------------|
                |         UNUSED       |     \   |   PE Signature |
                |----------------------|      ---'----------------'
                |      MS-DOS stub     |
                |----------------------|
                |         UNUSED       |
                |----------------------|
                |     MS-DOS Header    | <-- Here at 0x3c location we have the offset of NT Header
                '----------------------'


Structres related to these exe headers
--------------------------------------
1)   MS-DOS Header   ---> IMAGE_DOS_HEADER
2)   NT Header       ---> IMAGE_NT_HEADERS --->contain 
                                           --->IMAGE_FILE_HEADER dealing with COFF/PE Header
                                           --->IMAGE_OPTIONAL_HEADER dealing with Optional PE Header

3)   Section Table   ---> IMAGE_SECTION_HEADER

Key Points 
----------

  dosHeader = Memory mapped base address
  ntHeader = (IMAGE_NT_HEADER)((DWORD)dosHeader + dosHeader->e_lfanew)
  sectionHeader = (IMAGE_SECTION_HEADER)((DWORD)ntHeader + OFFSET(OptionalHeader) + sizeof(OptionalHeader)) 
  each section = (char *)((DWORD)dosHeader + sectionHeader.PointerToRawData)

*/
