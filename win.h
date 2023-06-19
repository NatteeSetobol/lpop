#ifndef __WIN__H
#define __WIN__H

#include "../lpop/required/intrinsic.hpp"
#include "../lpop/array.hpp"
#include "../lpop/bucket.hpp"
#include "../lpop/strings.hpp"

#define IMAGE_SIZEOF_SHORT_NAME 8

#ifndef COMPILER_MSVC
typedef unsigned short WORD;
typedef unsigned char BYTE;
typedef unsigned int DWORD;
#endif

typedef unsigned __int64    QWORD;
typedef __int64             INT64;
typedef QWORD PTR64;
//typedef QWORD HANDLE64;

#define IsDOSHeader(magicBytes) magicBytes == 0x5A4D ? true:false
#define CheckNTHeader(magicBytes) magicBytes == 0x4550 ? true:false

#define IMAGE_DIRECTORY_ENTRY_RESOURCE 2
#define IMAGE_DIRECTORY_ENTRY_IMPORT 1
#define IMAGE_DIRECTORY_ENTRY_IAT 12


#ifndef COMPILER_MSVC
typedef struct _LIST_ENTRY {
	struct _LIST_ENTRY *Flink;
	struct _LIST_ENTRY *Blink;
} LIST_ENTRY, *PLIST_ENTRY, PRLIST_ENTRY;

union LARGE_INTEGER
{
    struct
    {
        DWORD   LowPart;
        INT32   HighPart;
    } u;
    INT64       QuadPart;
};
typedef struct _UNICODE_STRING {
	USHORT Length;
	USHORT MaximumLength;
	PWSTR Buffer;
} 

UNICODE_STRING;

typedef struct _RTL_USER_PROCESS_PARAMETERS {
	BYTE Reserved1[16];
	PVOID Reserved2[10];
	UNICODE_STRING ImagePathName;
	UNICODE_STRING CommandLine;
} RTL_USER_PROCESS_PARAMETERS,*PRTL_USER_PROCESS_PARAMETERS;

typedef struct _PEB {
	BYTE                          Reserved1[2];
	BYTE                          BeingDebugged;
	BYTE                          Reserved2[1];
	PVOID                         Reserved3[2];
	PPEB_LDR_DATA                 Ldr;
	PRTL_USER_PROCESS_PARAMETERS  ProcessParameters;
	PVOID                         Reserved4[3];
	PVOID                         AtlThunkSListPtr;
	PVOID                         Reserved5;
	ULONG                         Reserved6;
	PVOID                         Reserved7;
	ULONG                         Reserved8;
	ULONG                         AtlThunkSListPtr32;
	PVOID                         Reserved9[45];
	BYTE                          Reserved10[96];
	PVOID *PostProcessInitRoutine;
	BYTE                          Reserved11[128];
	PVOID                         Reserved12[1];
	ULONG                         SessionId;
} PEB, *PPEB;

typedef struct _PEB_LDR_DATA {
	BYTE       Reserved1[8];
	PVOID      Reserved2[3];
	LIST_ENTRY InMemoryOrderModuleList;
} PEB_LDR_DATA, *PPEB_LDR_DATA;


#endif


struct UNICODE_STRING64
{
    union
    {
        struct
        {
            WORD Length;
            WORD MaximumLength;
        } u;
        QWORD dummyalign;
    };
    QWORD Buffer;
};

struct PEB64
{
    union
    {
        struct
        {
            BYTE InheritedAddressSpace;                                 //0x000
            BYTE ReadImageFileExecOptions;                              //0x001
            BYTE BeingDebugged;                                         //0x002
            BYTE _SYSTEM_DEPENDENT_01;                                  //0x003
        } flags;
        QWORD dummyalign;
    } dword0;
    QWORD                           Mutant;                             //0x0008
    QWORD                           ImageBaseAddress;                   //0x0010
    PTR64                           Ldr;                                //0x0018
    PTR64                           ProcessParameters;                  //0x0020 / pointer to RTL_USER_PROCESS_PARAMETERS64
    QWORD                           SubSystemData;                      //0x0028
    QWORD                           ProcessHeap;                        //0x0030
    QWORD                           FastPebLock;                        //0x0038
    QWORD                           _SYSTEM_DEPENDENT_02;               //0x0040
    QWORD                           _SYSTEM_DEPENDENT_03;               //0x0048
    QWORD                           _SYSTEM_DEPENDENT_04;               //0x0050
    union
    {
        QWORD                       KernelCallbackTable;                //0x0058
        QWORD                       UserSharedInfoPtr;                  //0x0058
    };
    DWORD                           SystemReserved;                     //0x0060
    DWORD                           _SYSTEM_DEPENDENT_05;               //0x0064
    QWORD                           _SYSTEM_DEPENDENT_06;               //0x0068
    QWORD                           TlsExpansionCounter;                //0x0070
    QWORD                           TlsBitmap;                          //0x0078
    DWORD                           TlsBitmapBits[2];                   //0x0080
    QWORD                           ReadOnlySharedMemoryBase;           //0x0088
    QWORD                           _SYSTEM_DEPENDENT_07;               //0x0090
    QWORD                           ReadOnlyStaticServerData;           //0x0098
    QWORD                           AnsiCodePageData;                   //0x00A0
    QWORD                           OemCodePageData;                    //0x00A8
    QWORD                           UnicodeCaseTableData;               //0x00B0
    DWORD                           NumberOfProcessors;                 //0x00B8
    union
    {
        DWORD                       NtGlobalFlag;                       //0x00BC
        DWORD                       dummy02;                            //0x00BC
    };
    LARGE_INTEGER                   CriticalSectionTimeout;             //0x00C0
    QWORD                           HeapSegmentReserve;                 //0x00C8
    QWORD                           HeapSegmentCommit;                  //0x00D0
    QWORD                           HeapDeCommitTotalFreeThreshold;     //0x00D8
    QWORD                           HeapDeCommitFreeBlockThreshold;     //0x00E0
    DWORD                           NumberOfHeaps;                      //0x00E8
    DWORD                           MaximumNumberOfHeaps;               //0x00EC
    QWORD                           ProcessHeaps;                       //0x00F0
    QWORD                           GdiSharedHandleTable;               //0x00F8
    QWORD                           ProcessStarterHelper;               //0x0100
    QWORD                           GdiDCAttributeList;                 //0x0108
    QWORD                           LoaderLock;                         //0x0110
    DWORD                           OSMajorVersion;                     //0x0118
    DWORD                           OSMinorVersion;                     //0x011C
    WORD                            OSBuildNumber;                      //0x0120
    WORD                            OSCSDVersion;                       //0x0122
    DWORD                           OSPlatformId;                       //0x0124
    DWORD                           ImageSubsystem;                     //0x0128
    DWORD                           ImageSubsystemMajorVersion;         //0x012C
    QWORD                           ImageSubsystemMinorVersion;         //0x0130
    union
    {
        QWORD                       ImageProcessAffinityMask;           //0x0138
        QWORD                       ActiveProcessAffinityMask;          //0x0138
    };
    QWORD                           GdiHandleBuffer[30];                //0x0140
    QWORD                           PostProcessInitRoutine;             //0x0230
    QWORD                           TlsExpansionBitmap;                 //0x0238
    DWORD                           TlsExpansionBitmapBits[32];         //0x0240
    QWORD                           SessionId;                          //0x02C0
    ULARGE_INTEGER                  AppCompatFlags;                     //0x02C8
    ULARGE_INTEGER                  AppCompatFlagsUser;                 //0x02D0
    QWORD                           pShimData;                          //0x02D8
    QWORD                           AppCompatInfo;                      //0x02E0
    UNICODE_STRING64                CSDVersion;                         //0x02E8
    QWORD                           ActivationContextData;              //0x02F8
    QWORD                           ProcessAssemblyStorageMap;          //0x0300
    QWORD                           SystemDefaultActivationContextData; //0x0308
    QWORD                           SystemAssemblyStorageMap;           //0x0310
    QWORD                           MinimumStackCommit;                 //0x0318

}; //struct PEB64



enum WIN_SUBSYSTEM
{
	SUBSYSTEM_UNKNOWN,
	SUBSYSTEM_NATIVE,
	SUBSYSTEM_WINDOWS_GUI,
	SUBSYSTEM_WINDOWS_CUI,
	SUBSYSTEM_OS2_CUI,
	SUBSYSTEM_POSIX_CUI,
	SUBSYSTEM_NATIVE_WINDOWS,
	SUBSYSTEM_WINDOWS_CE_GUI,
	SUBSYSTEM_EFI_APPLICATION,
	SUBSYSTEM_EFI_SERVICE_DRIVER,
	SUBSYSTEM_EFI_RUNTIME_DRIVER,
	SUBSYSTEM_EFI_ROM,
	SUBSYSTEM_WINDOWS_BOOT_APP
};

enum IMAGE_NT_OP_HDR_MAGIC
{
	HDR32_MAGIC = 0x10b,
	HDR64_MAGIC = 0x20b,
	IMAGE_HDR_MAGIC  = 0x107
};

enum  IMAGE_DIRECTORY
{
	DIRECTORY_ENTRY_EXPORT,
	DIRECTORY_ENTRY_IMPORT,
	DIRECTORY_ENTRY_RESOURCES,
	DIRECTORY_ENTRY_EXPECTION,
	DIRECTORY_ENTRY_SECURITY,
	DIRECTORY_ENTRY_BASELOG,
	DIRECTORY_ENTRY_DEBUG,
	DIRECTORY_ENTRY_ARCHITECTURE,
	DIRECTORY_ENTRY_GLOBAL_PTR,
	DIRECTORY_ENTRY_TS,
	DIRECTORY_ENTRY_LOAD_CONFIG,
	DIRECTORY_ENTRY_BOUND_IMPORT,
	DIRECTORY_ENTRY_IAT,
	DIRECTORY_ENTRY_DELAY_IMPORT,
	DIRECTORY_ENTRY_TLS,
	DIRECTORY_ENTRY_COM_DESCRIPTOR,
};

#ifndef COMPILER_MSVC
struct _IMAGE_DATA_DIRECTORY {
  DWORD VirtualAddress;
  DWORD Size;
};
#endif

struct MS_DOS_HEADER
{
	ui16 magic;
	ui16 LastPageBytes;
	ui16 PagesInFile;
	ui16 Relocations;
	ui16 SizeInHeader;
	ui16 MinParagraph;
	ui16 MaxParagraph;
	ui16 SSValue;
	ui16 SPValue;
	ui16 CheckSum;
	ui16 IPValue;
	ui16 CSVALUE;
	ui16 FileAddressOf;
	ui16 OverlayNumber;
	ui16 ReservedWordsFour[4];
	ui16 OEMID;
	ui16 OEMInfo;
	ui16 ReservedWordsTen[10];
	ui32 PEOffset;
};
#ifdef COMPILER_MSVC
#pragma pack(push,1)
struct PE_HEADER 
#else
struct __attribute__((packed)) PE_HEADER
#endif
{
	ui32 Sign;
	ui16 Machine;
	ui16 NumOfSections;
	ui32 TimeDateStamp;
	ui32 PointerToSymbolTable;
	ui32 NumberOfSymbols;
	ui16 SizeOfOptionalHeader;
	ui16 Characteristics;
};
#ifdef COMPILER_MSVC
#pragma pack(pop)
#endif

struct data_directory
{
	ui32 VirtualAddress;
	ui32 size;
};


struct PE_OP_HEADER
{
	ui16 magic;
};

struct  PE_OP_HEADER64
{
	ui16 magic;
	uchar minorLinkVer;
	uchar majorLinkVer;
	ui32 sizeOfInitData;
	ui32 sizeOfUnInitData;
	ui32 addrOfEntryPoint;
	ui32 baseOfCode;

	ui64 ImageBase;
	ui32 sectionAlign;
	ui32 fileAlign;
	ui16 majorOSVer;
	ui16 minorOSVer;
	ui16 majorImageVer;
	ui16 minorImageVer;
	ui16 majorSubVer;
	ui16 minorSubVer;
	ui32 win32VersionValue;
	ui32 sizeOfImage;
	ui32 sizeOfHeader;
	ui32 checkSum;
	ui16 subSystem;
	ui16 DLLChar;
	ui64 sizeOfStackReserve;
	ui64 sizeOfStackCommit;
	ui64 sizeOfHeapReserve;
	ui64 sizeOfHeapCommit;
	ui32 loaderFlag;
	ui32 numberOfRvaAndSize;
	data_directory DataDirectory[16];
};

struct  PE_OP_HEADER32
{
	ui16 magic;
	uchar minorLinkVer;
	uchar majorLinkVer;
	ui32 sizeOfInitData;
	ui32 sizeOfUnInitData;
	ui32 addrOfEntryPoint;
	ui32 baseOfCode;
	ui32 baseOfData; // for PE32+

	ui64 ImageBase;
	ui32 sectionAlign;
	ui32 fileAlign;
	ui16 majorOSVer;
	ui16 minorOSVer;
	ui16 majorImageVer;
	ui16 minorImageVer;
	ui16 majorSubVer;
	ui16 minorSubVer;
	ui32 win32VersionValue;
	ui32 sizeOfImage;
	ui32 sizeOfHeader;
	ui32 checkSum;
	ui16 subSystem;
	ui16 DLLChar;
	ui32 sizeOfStackReserve;
	ui32 sizeOfStackCommit;
	ui32 sizeOfHeapReserve;
	ui32 sizeOfHeapCommit;
	ui32 loaderFlag;
	ui32 numberOfRvaAndSize;
	data_directory DataDirectory[16];
};

#ifndef COMPILER_MSVC
typedef struct _IMAGE_IMPORT_DESCRIPTOR {
	DWORD   OriginalFirstThunk;
    DWORD   TimeDateStamp;
    DWORD   ForwarderChain;
    DWORD   Name;
    DWORD   FirstThunk;
} IMAGE_IMPORT_DESCRIPTOR;
typedef struct   __attribute__((packed)) _IMAGE_SECTION_HEADER {
    BYTE    Name[IMAGE_SIZEOF_SHORT_NAME];
    union {
            DWORD   PhysicalAddress;
            DWORD   VirtualSize;
    } Misc;
	
    DWORD   VirtualAddress;
    DWORD   SizeOfRawData;
    DWORD   PointerToRawData;
    DWORD   PointerToRelocations;
    DWORD   PointerToLinenumbers;
    WORD    NumberOfRelocations;
    WORD    NumberOfLinenumbers;
    DWORD   Characteristics;
} IMAGE_SECTION_HEADER, *PIMAGE_SECTION_HEADER;

typedef struct _IMAGE_IMPORT_BY_NAME {
	WORD    Hint;
	BYTE    Name[1];
} IMAGE_IMPORT_BY_NAME, *PIMAGE_IMPORT_BY_NAME;

typedef struct _IMAGE_THUNK_DATA {
	union {
		DWORD ForwarderString;              // RVA to forwarder string
		DWORD Ordinal;              // ordinal value of function
		DWORD AddressOfData;        // RVA of imported name
		DWORD  Function;             // address of imported function
	} u1;
} IMAGE_THUNK_DATA, *PIMAGE_THUNK_DATA;

typedef struct  _IMAGE_RESOURCE_DIRECTORY
{
	unsigned int Characteristics;
	unsigned int TimeDateStamp;
	unsigned short MajorVersion;
	unsigned short MinorVersion;
	unsigned short NumberOfNamedEntries;
	unsigned short NumberOfIdEntries;
} IMAGE_RESOURCE_DIRECTORY;

struct IMAGE_RESOURCE_DIRECTORY_ENTRY
{
	ui32 NameId;
	ui32 Data;
};

struct IMAGE_RESOURCE_DATA_ENTRY
{
        uint32 Data;
        uint32 Size;
        uint32 CodePage;
        uint32 Reserved;
};

#endif


const char DOS_STUB_CODE[] = "\x0E\x1F\xBA\x0E\x00\xB4\x09\xCD\x21\xB8\x01\x4C\xCD\x21\x54\x68\x69\x73\x20\x70\x72\x6F\x67\x72\x61\x6D\x20\x63\x61\x6E\x6E\x6F\x74\x20\x62\x65\x20\x72\x75\x6E\x20\x69\x6E\x20\x44\x4F\x53\x20\x6D\x6F\x64\x65\x2E\x0D\x0D\x0A\x24\x00\x00\x00\x00\x00\x00\x00";



#ifdef COMPILER_MSVC
BOOL EnableTokenPrivilege (LPTSTR pPrivilege);
#endif
ui32 FindRawAddress32(struct PE_HEADER *peHeader, ui32 rva, ui64 *opAddress);
ui32 FindRawAddress(struct PE_HEADER *peHeader, ui64 rva, ui64 *opAddress);
bool PEIs64Bit(ui8 *appBytes);
char *ExeEmbement(char *file, char *outfile, char *varName);
ui64 *CalcAddress(i32 size, ...);
void InitDebugWindow();
#endif
