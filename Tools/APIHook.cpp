/******************************************************************************
Module:  APIHook.cpp
Notices: Copyright (c) 2008 Jeffrey Richter & Christophe Nasarre
******************************************************************************/
#include "StdAfx.h"
#include <ImageHlp.h>
#pragma comment(lib, "ImageHlp")

////////////////////////////// chINRANGE Macro ////////////////////////////////


// This macro returns TRUE if a number is between two others
#define chINRANGE(low, Num, High) (((low) <= (Num)) && ((Num) <= (High)))

#include "APIHook.h"
#include "Toolhelp.h"

/////////////////////////////////////////////////////////////////////////////////


// The head of the linked-list of CAPIHook objects
CAPIHook* CAPIHook::sm_pHead = NULL;

// By default, the module containing the CAPIHook() is not hooked
BOOL CAPIHook::ExcludeAPIHookMod = TRUE;


///////////////////////////////////////////////////////////////////////////////


CAPIHook::CAPIHook(PCSTR pszCalleeModName, PCSTR pszFuncName, PROC pfnHook) {

    // Note: the function can be hooked only if the exporting module 
    //       is already loaded. A solution could be to store the function
    //       name as a member; then, in the hooked LoadLibrary* handlers, parse
    //       the list of CAPIHook instances, check if pszCalleeModName
    //       is the name of the loaded module to hook its export table and 
    //       re-hook the import tables of all loaded modules.

    m_pNext = sm_pHead;    // The next node was at the head
    sm_pHead = this;        // This node is now at the head

    // Save information about this hooked function
    m_pszCalleeModName = pszCalleeModName;
    m_pszFuncName = pszFuncName;
    m_pfnHook = pfnHook;
    m_pfnOrig =
        GetProcAddressRaw(GetModuleHandleA(pszCalleeModName), m_pszFuncName);

    // If function does not exit,... bye bye
    // This happens when the module is not already loaded
    if (m_pfnOrig == NULL)
    {
        wchar_t szPathname[MAX_PATH];
        GetModuleFileNameW(NULL, szPathname, _countof(szPathname));
        wchar_t sz[1024];
        _snwprintf(sz, _countof(sz),
            TEXT("[%4u - %s] impossible to find %S\r\n"),
            GetCurrentProcessId(), szPathname, pszFuncName);
        OutputDebugString(sz);
        return;
    }

#ifdef _DEBUG
    // This section was used for debugging sessions when Explorer died as 
    // a folder content was requested
    // 
    //static BOOL s_bFirstTime = TRUE;
    //if (s_bFirstTime)
    //{
    //   s_bFirstTime = FALSE;

    //   wchar_t szPathname[MAX_PATH];
    //   GetModuleFileNameW(NULL, szPathname, _countof(szPathname));
    //   wchar_t* pszExeFile = wcsrchr(szPathname, L'\\') + 1;
    //   OutputDebugStringW(L"Injected in ");
    //   OutputDebugStringW(pszExeFile);
    //   if (_wcsicmp(pszExeFile, L"Explorer.EXE") == 0)
    //   {
    //      DebugBreak();
    //   }
    //   OutputDebugStringW(L"\n   --> ");
    //   StringCchPrintfW(szPathname, _countof(szPathname), L"%S", pszFuncName);
    //   OutputDebugStringW(szPathname);
    //   OutputDebugStringW(L"\n");
    //}
#endif

   // Hook this function in all currently loaded modules
    ReplaceIATEntryInAllMods(m_pszCalleeModName, m_pfnOrig, m_pfnHook);
}


///////////////////////////////////////////////////////////////////////////////


CAPIHook::~CAPIHook() {

    // Unhook this function from all modules
    ReplaceIATEntryInAllMods(m_pszCalleeModName, m_pfnHook, m_pfnOrig);

    // Remove this object from the linked list
    CAPIHook* p = sm_pHead;
    if (p == this) {     // Removing the head node
        sm_pHead = p->m_pNext;
    }
    else {

        BOOL bFound = FALSE;

        // Walk list from head and fix pointers
        for (; !bFound && (p->m_pNext != NULL); p = p->m_pNext) {
            if (p->m_pNext == this) {
                // Make the node that points to us point to our next node
                p->m_pNext = p->m_pNext->m_pNext;
                bFound = TRUE;
            }
        }
    }
}


///////////////////////////////////////////////////////////////////////////////


// NOTE: This function must NOT be inlined
FARPROC CAPIHook::GetProcAddressRaw(HMODULE hmod, PCSTR pszProcName) {

    return(::GetProcAddress(hmod, pszProcName));
}


///////////////////////////////////////////////////////////////////////////////


// Returns the HMODULE that contains the specified memory address
static HMODULE ModuleFromAddress(PVOID pv) {

    MEMORY_BASIC_INFORMATION mbi;
    return((VirtualQuery(pv, &mbi, sizeof(mbi)) != 0)
        ? (HMODULE)mbi.AllocationBase : NULL);
}


///////////////////////////////////////////////////////////////////////////////


void CAPIHook::ReplaceIATEntryInAllMods(PCSTR pszCalleeModName,
    PROC pfnCurrent, PROC pfnNew) {

    HMODULE hmodThisMod = ExcludeAPIHookMod
        ? ModuleFromAddress(ReplaceIATEntryInAllMods) : NULL;

    // Get the list of modules in this process
    CToolhelp th(TH32CS_SNAPMODULE, GetCurrentProcessId());

    MODULEENTRY32 me = { sizeof(me) };
    for (BOOL bOk = th.ModuleFirst(&me); bOk; bOk = th.ModuleNext(&me)) {

        // NOTE: We don't hook functions in our own module
        if (me.hModule != hmodThisMod) {

            // Hook this function in this module
            ReplaceIATEntryInOneMod(
                pszCalleeModName, pfnCurrent, pfnNew, me.hModule);
        }
    }
}


///////////////////////////////////////////////////////////////////////////////


// Handle unexpected exceptions if the module is unloaded
LONG WINAPI InvalidReadExceptionFilter(PEXCEPTION_POINTERS pep) {

    // handle all unexpected exceptions because we simply don't patch
    // any module in that case
    LONG lDisposition = EXCEPTION_EXECUTE_HANDLER;

    // Note: pep->ExceptionRecord->ExceptionCode has 0xc0000005 as a value

    return(lDisposition);
}


void CAPIHook::ReplaceIATEntryInOneMod(PCSTR pszCalleeModName,
    PROC pfnCurrent, PROC pfnNew, HMODULE hmodCaller) {

    // Get the address of the module's import section
    ULONG ulSize;

    // An exception was triggered by Explorer (when browsing the content of 
    // a folder) into imagehlp.dll. It looks like one module was unloaded...
    // Maybe some threading problem: the list of modules from Toolhelp might 
    // not be accurate if FreeLibrary is called during the enumeration.
    PIMAGE_IMPORT_DESCRIPTOR pImportDesc = NULL;
    __try {
        pImportDesc = (PIMAGE_IMPORT_DESCRIPTOR)ImageDirectoryEntryToData(
            hmodCaller, TRUE, IMAGE_DIRECTORY_ENTRY_IMPORT, &ulSize);
    }
    __except (InvalidReadExceptionFilter(GetExceptionInformation())) {
        // Nothing to do in here, thread continues to run normally
        // with NULL for pImportDesc 
    }

    if (pImportDesc == NULL)
        return;  // This module has no import section or is no longer loaded


    // Find the import descriptor containing references to callee's functions
    for (; pImportDesc->Name; pImportDesc++) {
        PSTR pszModName = (PSTR)((PBYTE)hmodCaller + pImportDesc->Name);
        if (lstrcmpiA(pszModName, pszCalleeModName) == 0) {

            // Get caller's import address table (IAT) for the callee's functions
            PIMAGE_THUNK_DATA pThunk = (PIMAGE_THUNK_DATA)
                ((PBYTE)hmodCaller + pImportDesc->FirstThunk);

            // Replace current function address with new function address
            for (; pThunk->u1.Function; pThunk++) {

                // Get the address of the function address
                PROC* ppfn = (PROC*)&pThunk->u1.Function;

                // Is this the function we're looking for?
                BOOL bFound = (*ppfn == pfnCurrent);
                if (bFound) {
                    if (!WriteProcessMemory(GetCurrentProcess(), ppfn, &pfnNew,
                        sizeof(pfnNew), NULL) && (ERROR_NOACCESS == GetLastError())) {
                        DWORD dwOldProtect;
                        if (VirtualProtect(ppfn, sizeof(pfnNew), PAGE_WRITECOPY,
                            &dwOldProtect)) {

                            WriteProcessMemory(GetCurrentProcess(), ppfn, &pfnNew,
                                sizeof(pfnNew), NULL);
                            VirtualProtect(ppfn, sizeof(pfnNew), dwOldProtect,
                                &dwOldProtect);
                        }
                    }
                    return;  // We did it, get out
                }
            }
        }  // Each import section is parsed until the right entry is found and patched
    }
}


///////////////////////////////////////////////////////////////////////////////


void CAPIHook::ReplaceEATEntryInOneMod(HMODULE hmod, PCSTR pszFunctionName,
    PROC pfnNew) {

    // Get the address of the module's export section
    ULONG ulSize;

    PIMAGE_EXPORT_DIRECTORY pExportDir = NULL;
    __try {
        pExportDir = (PIMAGE_EXPORT_DIRECTORY)ImageDirectoryEntryToData(
            hmod, TRUE, IMAGE_DIRECTORY_ENTRY_EXPORT, &ulSize);
    }
    __except (InvalidReadExceptionFilter(GetExceptionInformation())) {
        // Nothing to do in here, thread continues to run normally
        // with NULL for pExportDir 
    }

    if (pExportDir == NULL)
        return;  // This module has no export section or is unloaded

    PDWORD pdwNamesRvas = (PDWORD)((PBYTE)hmod + pExportDir->AddressOfNames);
    PWORD pdwNameOrdinals = (PWORD)
        ((PBYTE)hmod + pExportDir->AddressOfNameOrdinals);
    PDWORD pdwFunctionAddresses = (PDWORD)
        ((PBYTE)hmod + pExportDir->AddressOfFunctions);

    // Walk the array of this module's function names 
    for (DWORD n = 0; n < pExportDir->NumberOfNames; n++) {
        // Get the function name
        PSTR pszFuncName = (PSTR)((PBYTE)hmod + pdwNamesRvas[n]);

        // If not the specified function, try the next function
        if (lstrcmpiA(pszFuncName, pszFunctionName) != 0) continue;

        // We found the specified function
        // --> Get this function's ordinal value
        WORD ordinal = pdwNameOrdinals[n];

        // Get the address of this function's address
        PROC* ppfn = (PROC*)&pdwFunctionAddresses[ordinal];

        // Turn the new address into an RVA
        pfnNew = (PROC)((PBYTE)pfnNew - (PBYTE)hmod);

        // Replace current function address with new function address
        if (!WriteProcessMemory(GetCurrentProcess(), ppfn, &pfnNew,
            sizeof(pfnNew), NULL) && (ERROR_NOACCESS == GetLastError())) {
            DWORD dwOldProtect;
            if (VirtualProtect(ppfn, sizeof(pfnNew), PAGE_WRITECOPY,
                &dwOldProtect)) {

                WriteProcessMemory(GetCurrentProcess(), ppfn, &pfnNew,
                    sizeof(pfnNew), NULL);
                VirtualProtect(ppfn, sizeof(pfnNew), dwOldProtect, &dwOldProtect);
            }
        }
        break;  // We did it, get out
    }
}


///////////////////////////////////////////////////////////////////////////////
// Hook LoadLibrary functions and GetProcAddress so that hooked functions
// are handled correctly if these functions are called.

CAPIHook CAPIHook::sm_LoadLibraryA("Kernel32.dll", "LoadLibraryA",
    (PROC)CAPIHook::LoadLibraryA);

CAPIHook CAPIHook::sm_LoadLibraryW("Kernel32.dll", "LoadLibraryW",
    (PROC)CAPIHook::LoadLibraryW);

CAPIHook CAPIHook::sm_LoadLibraryExA("Kernel32.dll", "LoadLibraryExA",
    (PROC)CAPIHook::LoadLibraryExA);

CAPIHook CAPIHook::sm_LoadLibraryExW("Kernel32.dll", "LoadLibraryExW",
    (PROC)CAPIHook::LoadLibraryExW);

CAPIHook CAPIHook::sm_GetProcAddress("Kernel32.dll", "GetProcAddress",
    (PROC)CAPIHook::GetProcAddress);


///////////////////////////////////////////////////////////////////////////////


void CAPIHook::FixupNewlyLoadedModule(HMODULE hmod, DWORD dwFlags) {

    // If a new module is loaded, hook the hooked functions
    if ((hmod != NULL) &&   // Do not hook our own module
        (hmod != ModuleFromAddress(FixupNewlyLoadedModule)) &&
        ((dwFlags & LOAD_LIBRARY_AS_DATAFILE) == 0) &&
        ((dwFlags & LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE) == 0) &&
        ((dwFlags & LOAD_LIBRARY_AS_IMAGE_RESOURCE) == 0)
        ) {

        for (CAPIHook* p = sm_pHead; p != NULL; p = p->m_pNext) {
            if (p->m_pfnOrig != NULL) {
                ReplaceIATEntryInAllMods(p->m_pszCalleeModName,
                    p->m_pfnOrig, p->m_pfnHook);
            }
            else {
#ifdef _DEBUG
                // We should never end up here 
                wchar_t szPathname[MAX_PATH];
                GetModuleFileNameW(NULL, szPathname, _countof(szPathname));
                wchar_t sz[1024];
                _snwprintf(sz, _countof(sz),
                    TEXT("[%4u - %s] impossible to find %S\r\n"),
                    GetCurrentProcessId(), szPathname, p->m_pszCalleeModName);
                OutputDebugString(sz);
#endif
            }
        }
    }
}


///////////////////////////////////////////////////////////////////////////////


HMODULE WINAPI CAPIHook::LoadLibraryA(PCSTR pszModulePath) {

    HMODULE hmod = ::LoadLibraryA(pszModulePath);
    FixupNewlyLoadedModule(hmod, 0);
    return(hmod);
}


///////////////////////////////////////////////////////////////////////////////


HMODULE WINAPI CAPIHook::LoadLibraryW(PCWSTR pszModulePath) {

    HMODULE hmod = ::LoadLibraryW(pszModulePath);
    FixupNewlyLoadedModule(hmod, 0);
    return(hmod);
}


///////////////////////////////////////////////////////////////////////////////


HMODULE WINAPI CAPIHook::LoadLibraryExA(PCSTR pszModulePath,
    HANDLE hFile, DWORD dwFlags) {

    HMODULE hmod = ::LoadLibraryExA(pszModulePath, hFile, dwFlags);
    FixupNewlyLoadedModule(hmod, dwFlags);
    return(hmod);
}


///////////////////////////////////////////////////////////////////////////////


HMODULE WINAPI CAPIHook::LoadLibraryExW(PCWSTR pszModulePath,
    HANDLE hFile, DWORD dwFlags) {

    HMODULE hmod = ::LoadLibraryExW(pszModulePath, hFile, dwFlags);
    FixupNewlyLoadedModule(hmod, dwFlags);
    return(hmod);
}


///////////////////////////////////////////////////////////////////////////////


FARPROC WINAPI CAPIHook::GetProcAddress(HMODULE hmod, PCSTR pszProcName) {

    // Get the true address of the function
    FARPROC pfn = GetProcAddressRaw(hmod, pszProcName);

    // Is it one of the functions that we want hooked?
    CAPIHook* p = sm_pHead;
    for (; (pfn != NULL) && (p != NULL); p = p->m_pNext) {

        if (pfn == p->m_pfnOrig) {

            // The address to return matches an address we want to hook
            // Return the hook function address instead
            pfn = p->m_pfnHook;
            break;
        }
    }

    return(pfn);
}
