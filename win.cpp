#include "win.h"

bool PEIs64Bit(ui8 *appBytes)
{
	bool result = false;
	struct MS_DOS_HEADER *dosHeader = NULL;
	struct PE_HEADER *peHeader = NULL;
	struct PE_OP_HEADER *peOpHeader=NULL;

	dosHeader = (MS_DOS_HEADER*) appBytes;
	peHeader = (PE_HEADER *) ( (  appBytes) +  dosHeader->PEOffset);
	peOpHeader = (PE_OP_HEADER *) ( (  appBytes) +  dosHeader->PEOffset+sizeof(PE_HEADER));

	if (peOpHeader->magic  ==  HDR64_MAGIC)
	{
		result = true;
	}

	return result;
}

char *ExeEmbement(char *file, char *outfile, char *varName)
{
	struct file stubFile = {};
	char *var = NULL;
	FILE *fileHeader = NULL;

	var = S32(varName);

	stubFile = ReadFile(file);

	if (stubFile.data)
	{
		char temp[256] = {};

		fileHeader = fopen(outfile, "wb");
		if (fileHeader)
		{
			sprintf(temp, "const char %s[] = \"", var);
			fwrite(temp,1, Strlen(temp),fileHeader);

			for (int i=0; i < stubFile.size; i++)
			{
				char hex[256] = {};
				sprintf(hex,"\\x%x",stubFile.data[i] & 0xFF);

				fwrite(hex,1, Strlen(hex),fileHeader);
			}
			char ending[3] = {};

			ending[0] = '\"';
			ending[1] = ';';
			fwrite(ending, 1, Strlen(ending), fileHeader);

			fclose(fileHeader);
		}
	} else {
		printf("Error opening file\n");
	}
	

	free(stubFile.data);
	stubFile.data = NULL;

	return NULL;
}

ui64 *CalcAddress(i32 size, ...)
{
	va_list vl;
	ui64 addressSum = 0;

	va_start(vl, size);

	for (int i=0; i < size; i++)
	{
		ui64 address = va_arg(vl, ui64);

		addressSum += address;
	}

	va_end(vl);

	return (ui64*) (addressSum);
}

// opAddress = address of the section from that Data directory  
ui32 FindRawAddress(struct PE_HEADER *peHeader, ui64 rva, ui64 *opAddress)
{
	ui32 returnAddress = 0;

	for (int headerIndex = 0; headerIndex < peHeader->NumOfSections; headerIndex++)
	{
		IMAGE_SECTION_HEADER *imgSecHeader = NULL;
		ui64 sectionAddress = 0;

		sectionAddress = ((ui64 ) opAddress + (ui64) sizeof(PE_OP_HEADER64) + (ui64) (sizeof(IMAGE_SECTION_HEADER)*headerIndex));
		imgSecHeader = (IMAGE_SECTION_HEADER*) sectionAddress;

		if (rva >= imgSecHeader->VirtualAddress && rva <= imgSecHeader->VirtualAddress + imgSecHeader->SizeOfRawData )
		{
			returnAddress =  imgSecHeader->PointerToRawData + (rva - imgSecHeader->VirtualAddress);
			break;
		}
	}

	return returnAddress;
}


ui32 FindRawAddress32(struct PE_HEADER *peHeader, ui32 rva, ui64 *opAddress)
{
	ui32 returnAddress = 0;

	for (int headerIndex = 0; headerIndex < peHeader->NumOfSections; headerIndex++)
	{
		IMAGE_SECTION_HEADER *imgSecHeader = NULL;
		ui64 sectionAddress = 0;

		sectionAddress = ((ui64 ) opAddress + (ui64) sizeof(PE_OP_HEADER32) + (ui64) (sizeof(IMAGE_SECTION_HEADER)*headerIndex));
		imgSecHeader = (IMAGE_SECTION_HEADER*) sectionAddress;

		if (rva >= imgSecHeader->VirtualAddress && rva <= imgSecHeader->VirtualAddress + imgSecHeader->SizeOfRawData )
		{
			returnAddress =  imgSecHeader->PointerToRawData + (rva - imgSecHeader->VirtualAddress);
			break;
		}
	}

	return returnAddress;
}
#ifdef COMPILER_MSVC

BOOL EnableTokenPrivilege (LPTSTR pPrivilege)
{
    HANDLE hToken;                        
    TOKEN_PRIVILEGES token_privileges;                  
    DWORD dwSize;                        
    ZeroMemory (&token_privileges, sizeof (token_privileges));
    token_privileges.PrivilegeCount = 1;
    if ( !OpenProcessToken (GetCurrentProcess(), TOKEN_ALL_ACCESS, &hToken))
    {
       printf("OpenProcessToken failed");
        return FALSE;
    }
    if (!LookupPrivilegeValue ( NULL, pPrivilege, &token_privileges.Privileges[0].Luid))
    { 
        printf("LookupPrivilegeValue failed");
        CloseHandle (hToken);
        return FALSE;
    }
    token_privileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    if (!AdjustTokenPrivileges ( hToken, FALSE, &token_privileges, 0, NULL, &dwSize))
    { 
        printf("AdjustTokenPrivileges failed");
        CloseHandle (hToken);
        return FALSE;
    }
    CloseHandle (hToken);
    return TRUE;
}


void InitDebugWindow()
{
    #ifdef DEBUGGING
 	AllocConsole();
	(void)freopen("CONIN$", "r", stdin);
	(void)freopen("CONOUT$", "w+", stdout);
	SetStdHandle(STD_INPUT_HANDLE,stdin);
	SetStdHandle(STD_OUTPUT_HANDLE, stdout);
    #endif

    return ;
}

#endif