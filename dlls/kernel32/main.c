/*
 * Copyright 2017 Stefan Dösinger for CodeWeavers
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 */

/* NOTE: The guest side uses mingw's headers. The host side uses Wine's headers. */

#include <windows.h>
#include <stdio.h>

#include "windows-user-services.h"
#include "dll_list.h"
#include "kernel32.h"

#ifdef QEMU_DLL_GUEST

BOOL WINAPI DllMain(HMODULE mod, DWORD reason, void *reserved)
{
    return TRUE;
}

/* Copypyasted from Wine. */
WINBASEAPI INT WINAPI MulDiv( INT nMultiplicand, INT nMultiplier, INT nDivisor)
{
    LONGLONG ret;

    if (!nDivisor) return -1;

    /* We want to deal with a positive divisor to simplify the logic. */
    if (nDivisor < 0)
    {
        nMultiplicand = - nMultiplicand;
        nDivisor = -nDivisor;
    }

    /* If the result is positive, we "add" to round. else, we subtract to round. */
    if ( ( (nMultiplicand <  0) && (nMultiplier <  0) ) ||
         ( (nMultiplicand >= 0) && (nMultiplier >= 0) ) )
      ret = (((LONGLONG)nMultiplicand * nMultiplier) + (nDivisor/2)) / nDivisor;
    else
      ret = (((LONGLONG)nMultiplicand * nMultiplier) - (nDivisor/2)) / nDivisor;

    if ((ret > 2147483647) || (ret < -2147483647)) return -1;
    return ret;
}

#endif

#ifndef QEMU_DLL_GUEST
#include <wine/debug.h>
#include "va_helper_impl.h"
WINE_DEFAULT_DEBUG_CHANNEL(qemu_kernel32);

const struct qemu_ops *qemu_ops;

static const syscall_handler dll_functions[] =
{
    qemu__hread,
    qemu__hwrite,
    qemu__lclose,
    qemu__lcreat,
    qemu__llseek,
    qemu__lopen,
    qemu__lread,
    qemu__lwrite,
    qemu_ActivateActCtx,
    qemu_AddAtomA,
    qemu_AddAtomW,
    qemu_AddRefActCtx,
    qemu_AllocateUserPhysicalPages,
    qemu_ApplicationRecoveryFinished,
    qemu_ApplicationRecoveryInProgress,
    qemu_AreFileApisANSI,
    qemu_AssignProcessToJobObject,
    qemu_BackupRead,
    qemu_BackupSeek,
    qemu_BackupWrite,
    qemu_BaseFlushAppcompatCache,
    qemu_BeginUpdateResourceA,
    qemu_BeginUpdateResourceW,
    qemu_BindIoCompletionCallback,
    qemu_BuildCommDCBA,
    qemu_BuildCommDCBAndTimeoutsA,
    qemu_BuildCommDCBAndTimeoutsW,
    qemu_BuildCommDCBW,
    qemu_CallbackMayRunLong,
    qemu_CallNamedPipeA,
    qemu_CallNamedPipeW,
    qemu_CancelIo,
    qemu_CancelIoEx,
    qemu_CancelSynchronousIo,
    qemu_CancelTimerQueueTimer,
    qemu_CancelWaitableTimer,
    qemu_ChangeTimerQueueTimer,
    qemu_CheckNameLegalDOS8Dot3A,
    qemu_CheckNameLegalDOS8Dot3W,
    qemu_CheckRemoteDebuggerPresent,
    qemu_ClearCommBreak,
    qemu_ClearCommError,
    qemu_CloseHandle,
    qemu_CloseProfileUserMapping,
    qemu_CmdBatNotification,
    qemu_CommConfigDialogA,
    qemu_CommConfigDialogW,
    qemu_CompareStringA,
    qemu_CompareStringEx,
    qemu_CompareStringOrdinal,
    qemu_CompareStringW,
    qemu_ConnectNamedPipe,
    qemu_ContinueDebugEvent,
    qemu_ConvertDefaultLocale,
    qemu_ConvertFiberToThread,
    qemu_ConvertThreadToFiber,
    qemu_ConvertThreadToFiberEx,
    qemu_ConvertToGlobalHandle,
    qemu_CopyFileA,
    qemu_CopyFileExA,
    qemu_CopyFileExW,
    qemu_CopyFileW,
    qemu_CreateActCtxA,
    qemu_CreateActCtxW,
    qemu_CreateDirectoryA,
    qemu_CreateDirectoryExA,
    qemu_CreateDirectoryExW,
    qemu_CreateDirectoryW,
    qemu_CreateEventA,
    qemu_CreateEventExA,
    qemu_CreateEventExW,
    qemu_CreateEventW,
    qemu_CreateFiber,
    qemu_CreateFiberEx,
    qemu_CreateFile2,
    qemu_CreateFileA,
    qemu_CreateFileMappingA,
    qemu_CreateFileMappingW,
    qemu_CreateFileW,
    qemu_CreateHardLinkA,
    qemu_CreateHardLinkTransactedA,
    qemu_CreateHardLinkTransactedW,
    qemu_CreateHardLinkW,
    qemu_CreateIoCompletionPort,
    qemu_CreateJobObjectA,
    qemu_CreateJobObjectW,
    qemu_CreateMailslotA,
    qemu_CreateMailslotW,
    qemu_CreateMemoryResourceNotification,
    qemu_CreateMutexA,
    qemu_CreateMutexExA,
    qemu_CreateMutexExW,
    qemu_CreateMutexW,
    qemu_CreateNamedPipeA,
    qemu_CreateNamedPipeW,
    qemu_CreatePipe,
    qemu_CreateProcessA,
    qemu_CreateProcessW,
    qemu_CreateRemoteThread,
    qemu_CreateSemaphoreA,
    qemu_CreateSemaphoreExA,
    qemu_CreateSemaphoreExW,
    qemu_CreateSemaphoreW,
    qemu_CreateSocketHandle,
    qemu_CreateSymbolicLinkA,
    qemu_CreateSymbolicLinkW,
    qemu_CreateTapePartition,
    qemu_CreateThread,
    qemu_CreateThreadpool,
    qemu_CreateThreadpoolCleanupGroup,
    qemu_CreateThreadpoolTimer,
    qemu_CreateThreadpoolWait,
    qemu_CreateThreadpoolWork,
    qemu_CreateTimerQueue,
    qemu_CreateTimerQueueTimer,
    qemu_CreateWaitableTimerA,
    qemu_CreateWaitableTimerExA,
    qemu_CreateWaitableTimerExW,
    qemu_CreateWaitableTimerW,
    qemu_DeactivateActCtx,
    qemu_DebugActiveProcess,
    qemu_DebugActiveProcessStop,
    qemu_DebugBreak,
    qemu_DebugBreakProcess,
    qemu_DebugSetProcessKillOnExit,
    qemu_DelayLoadFailureHook,
    qemu_DeleteAtom,
    qemu_DeleteFiber,
    qemu_DeleteFileA,
    qemu_DeleteFileW,
    qemu_DeleteProcThreadAttributeList,
    qemu_DeleteTimerQueue,
    qemu_DeleteTimerQueueEx,
    qemu_DeleteTimerQueueTimer,
    qemu_DeviceIoControl,
    qemu_DisableThreadLibraryCalls,
    qemu_DisconnectNamedPipe,
    qemu_DnsHostnameToComputerNameA,
    qemu_DnsHostnameToComputerNameW,
    qemu_DuplicateHandle,
    qemu_EndUpdateResourceA,
    qemu_EndUpdateResourceW,
    qemu_EnumLanguageGroupLocalesA,
    qemu_EnumLanguageGroupLocalesW,
    qemu_EnumResourceLanguagesA,
    qemu_EnumResourceLanguagesExA,
    qemu_EnumResourceLanguagesExW,
    qemu_EnumResourceLanguagesW,
    qemu_EnumResourceNamesA,
    qemu_EnumResourceNamesW,
    qemu_EnumResourceTypesA,
    qemu_EnumResourceTypesW,
    qemu_EnumSystemCodePagesA,
    qemu_EnumSystemCodePagesW,
    qemu_EnumSystemGeoID,
    qemu_EnumSystemLanguageGroupsA,
    qemu_EnumSystemLanguageGroupsW,
    qemu_EnumSystemLocalesA,
    qemu_EnumSystemLocalesEx,
    qemu_EnumSystemLocalesW,
    qemu_EnumUILanguagesA,
    qemu_EnumUILanguagesW,
    qemu_EraseTape,
    qemu_EscapeCommFunction,
    qemu_ExitProcess,
    qemu_ExitThread,
    qemu_ExpandEnvironmentStringsA,
    qemu_ExpandEnvironmentStringsW,
    qemu_FindActCtxSectionGuid,
    qemu_FindActCtxSectionStringA,
    qemu_FindActCtxSectionStringW,
    qemu_FindAtomA,
    qemu_FindAtomW,
    qemu_FindClose,
    qemu_FindCloseChangeNotification,
    qemu_FindFirstChangeNotificationA,
    qemu_FindFirstChangeNotificationW,
    qemu_FindFirstFileA,
    qemu_FindFirstFileExA,
    qemu_FindFirstFileExW,
    qemu_FindFirstFileW,
    qemu_FindNextChangeNotification,
    qemu_FindNextFileA,
    qemu_FindNextFileW,
    qemu_FindResourceA,
    qemu_FindResourceExA,
    qemu_FindResourceExW,
    qemu_FindResourceW,
    qemu_FlsAlloc,
    qemu_FlsFree,
    qemu_FlsGetValue,
    qemu_FlsSetValue,
    qemu_FlushFileBuffers,
    qemu_FlushInstructionCache,
    qemu_FlushProcessWriteBuffers,
    qemu_FlushViewOfFile,
    qemu_FoldStringA,
    qemu_FoldStringW,
    qemu_FormatMessage,
    qemu_FormatMessage,
    qemu_FreeEnvironmentStringsA,
    qemu_FreeEnvironmentStringsW,
    qemu_FreeLibrary,
    qemu_FreeLibraryAndExitThread,
    qemu_FreeResource,
    qemu_FreeUserPhysicalPages,
    qemu_GetACP,
    qemu_GetActiveProcessorCount,
    qemu_GetActiveProcessorGroupCount,
    qemu_GetAtomNameA,
    qemu_GetAtomNameW,
    qemu_GetBinaryTypeA,
    qemu_GetBinaryTypeW,
    qemu_GetCommandLineA,
    qemu_GetCommandLineW,
    qemu_GetCommConfig,
    qemu_GetCommMask,
    qemu_GetCommModemStatus,
    qemu_GetCommProperties,
    qemu_GetCommState,
    qemu_GetCommTimeouts,
    qemu_GetCompressedFileSizeA,
    qemu_GetCompressedFileSizeW,
    qemu_GetComputerNameA,
    qemu_GetComputerNameExA,
    qemu_GetComputerNameExW,
    qemu_GetComputerNameW,
    qemu_GetCPInfo,
    qemu_GetCPInfoExA,
    qemu_GetCPInfoExW,
    qemu_GetCurrentActCtx,
    qemu_GetCurrentDirectoryA,
    qemu_GetCurrentDirectoryW,
    qemu_GetCurrentProcess,
    qemu_GetCurrentProcessId,
    qemu_GetCurrentThread,
    qemu_GetCurrentThreadId,
    qemu_GetDateFormatA,
    qemu_GetDateFormatW,
    qemu_GetDefaultCommConfigA,
    qemu_GetDefaultCommConfigW,
    qemu_GetDevicePowerState,
    qemu_GetDllDirectoryA,
    qemu_GetDllDirectoryW,
    qemu_GetEnvironmentStringsA,
    qemu_GetEnvironmentStringsW,
    qemu_GetEnvironmentVariableA,
    qemu_GetEnvironmentVariableW,
    qemu_GetErrorMode,
    qemu_GetExitCodeProcess,
    qemu_GetExitCodeThread,
    qemu_GetFileAttributesA,
    qemu_GetFileAttributesExA,
    qemu_GetFileAttributesExW,
    qemu_GetFileAttributesW,
    qemu_GetFileInformationByHandle,
    qemu_GetFileInformationByHandleEx,
    qemu_GetFileMUIInfo,
    qemu_GetFileMUIPath,
    qemu_GetFileSize,
    qemu_GetFileSizeEx,
    qemu_GetFileTime,
    qemu_GetFileType,
    qemu_GetFinalPathNameByHandleA,
    qemu_GetFinalPathNameByHandleW,
    qemu_GetFirmwareEnvironmentVariableA,
    qemu_GetFirmwareEnvironmentVariableW,
    qemu_GetFullPathNameA,
    qemu_GetFullPathNameW,
    qemu_GetGeoInfoA,
    qemu_GetGeoInfoW,
    qemu_GetHandleContext,
    qemu_GetHandleInformation,
    qemu_GetLargePageMinimum,
    qemu_GetLastError,
    qemu_GetLocaleInfoA,
    qemu_GetLocaleInfoEx,
    qemu_GetLocaleInfoW,
    qemu_GetLocalTime,
    qemu_GetLogicalProcessorInformation,
    qemu_GetLogicalProcessorInformationEx,
    qemu_GetLongPathNameA,
    qemu_GetLongPathNameW,
    qemu_GetMailslotInfo,
    qemu_GetModuleFileNameA,
    qemu_GetModuleFileNameW,
    qemu_GetModuleHandleA,
    qemu_GetModuleHandleExA,
    qemu_GetModuleHandleW,
    qemu_GetNamedPipeHandleStateA,
    qemu_GetNamedPipeHandleStateW,
    qemu_GetNamedPipeInfo,
    qemu_GetNativeSystemInfo,
    qemu_GetNumaAvailableMemoryNode,
    qemu_GetNumaHighestNodeNumber,
    qemu_GetNumaNodeProcessorMask,
    qemu_GetNumaProcessorNode,
    qemu_GetOEMCP,
    qemu_GetOverlappedResult,
    qemu_GetPhysicallyInstalledSystemMemory,
    qemu_GetPriorityClass,
    qemu_GetPrivateProfileIntA,
    qemu_GetPrivateProfileIntW,
    qemu_GetPrivateProfileSectionA,
    qemu_GetPrivateProfileSectionNamesA,
    qemu_GetPrivateProfileSectionNamesW,
    qemu_GetPrivateProfileSectionW,
    qemu_GetPrivateProfileStringA,
    qemu_GetPrivateProfileStringW,
    qemu_GetPrivateProfileStructA,
    qemu_GetPrivateProfileStructW,
    qemu_GetProcAddress,
    qemu_GetProcessAffinityMask,
    qemu_GetProcessDEPPolicy,
    qemu_GetProcessFlags,
    qemu_GetProcessHandleCount,
    qemu_GetProcessHeap,
    qemu_GetProcessHeaps,
    qemu_GetProcessId,
    qemu_GetProcessIdOfThread,
    qemu_GetProcessIoCounters,
    qemu_GetProcessPriorityBoost,
    qemu_GetProcessShutdownParameters,
    qemu_GetProcessVersion,
    qemu_GetProcessWorkingSetSize,
    qemu_GetProfileIntA,
    qemu_GetProfileIntW,
    qemu_GetProfileSectionA,
    qemu_GetProfileSectionW,
    qemu_GetProfileStringA,
    qemu_GetProfileStringW,
    qemu_GetQueuedCompletionStatus,
    qemu_GetShortPathNameA,
    qemu_GetShortPathNameW,
    qemu_GetStartupInfoA,
    qemu_GetStartupInfoW,
    qemu_GetStdHandle,
    qemu_GetStringTypeA,
    qemu_GetStringTypeExA,
    qemu_GetStringTypeExW,
    qemu_GetStringTypeW,
    qemu_GetSystemDefaultLangID,
    qemu_GetSystemDefaultLCID,
    qemu_GetSystemDefaultLocaleName,
    qemu_GetSystemDefaultUILanguage,
    qemu_GetSystemDEPPolicy,
    qemu_GetSystemDirectoryA,
    qemu_GetSystemDirectoryW,
    qemu_GetSystemFileCacheSize,
    qemu_GetSystemFirmwareTable,
    qemu_GetSystemInfo,
    qemu_GetSystemPowerStatus,
    qemu_GetSystemPreferredUILanguages,
    qemu_GetSystemTimeAsFileTime,
    qemu_GetSystemWindowsDirectoryA,
    qemu_GetSystemWindowsDirectoryW,
    qemu_GetSystemWow64DirectoryA,
    qemu_GetSystemWow64DirectoryW,
    qemu_GetTapeParameters,
    qemu_GetTapePosition,
    qemu_GetTapeStatus,
    qemu_GetTempFileNameA,
    qemu_GetTempFileNameW,
    qemu_GetTempPathA,
    qemu_GetTempPathW,
    qemu_GetThreadContext,
    qemu_GetThreadErrorMode,
    qemu_GetThreadGroupAffinity,
    qemu_GetThreadId,
    qemu_GetThreadIOPendingFlag,
    qemu_GetThreadLocale,
    qemu_GetThreadPreferredUILanguages,
    qemu_GetThreadPriority,
    qemu_GetThreadPriorityBoost,
    qemu_GetThreadSelectorEntry,
    qemu_GetThreadTimes,
    qemu_GetThreadUILanguage,
    qemu_GetTickCount,
    qemu_GetTimeFormatA,
    qemu_GetTimeFormatW,
    qemu_GetUserDefaultLangID,
    qemu_GetUserDefaultLCID,
    qemu_GetUserDefaultLocaleName,
    qemu_GetUserDefaultUILanguage,
    qemu_GetUserGeoID,
    qemu_GetUserPreferredUILanguages,
    qemu_GetVersion,
    qemu_GetVersionExA,
    qemu_GetVersionExW,
    qemu_GetWindowsDirectoryA,
    qemu_GetWindowsDirectoryW,
    qemu_GetWriteWatch,
    qemu_GlobalAddAtomA,
    qemu_GlobalAddAtomW,
    qemu_GlobalAlloc,
    qemu_GlobalCompact,
    qemu_GlobalDeleteAtom,
    qemu_GlobalFindAtomA,
    qemu_GlobalFindAtomW,
    qemu_GlobalFix,
    qemu_GlobalFlags,
    qemu_GlobalFree,
    qemu_GlobalGetAtomNameA,
    qemu_GlobalGetAtomNameW,
    qemu_GlobalHandle,
    qemu_GlobalLock,
    qemu_GlobalMemoryStatus,
    qemu_GlobalMemoryStatusEx,
    qemu_GlobalReAlloc,
    qemu_GlobalSize,
    qemu_GlobalUnfix,
    qemu_GlobalUnlock,
    qemu_GlobalUnWire,
    qemu_GlobalWire,
    qemu_HeapAlloc,
    qemu_HeapCompact,
    qemu_HeapCreate,
    qemu_HeapDestroy,
    qemu_HeapFree,
    qemu_HeapLock,
    qemu_HeapQueryInformation,
    qemu_HeapReAlloc,
    qemu_HeapSetInformation,
    qemu_HeapSize,
    qemu_HeapUnlock,
    qemu_HeapValidate,
    qemu_HeapWalk,
    qemu_IdnToAscii,
    qemu_IdnToNameprepUnicode,
    qemu_IdnToUnicode,
    qemu_InitAtomTable,
    qemu_InitializeCriticalSection,
    qemu_InitializeCriticalSectionAndSpinCount,
    qemu_InitializeProcThreadAttributeList,
    qemu_InitOnceBeginInitialize,
    qemu_InitOnceComplete,
    qemu_InitOnceExecuteOnce,
    qemu_InvalidateNLSCache,
    qemu_IsBadCodePtr,
    qemu_IsBadHugeReadPtr,
    qemu_IsBadHugeWritePtr,
    qemu_IsBadReadPtr,
    qemu_IsBadStringPtrA,
    qemu_IsBadStringPtrW,
    qemu_IsBadWritePtr,
    qemu_IsDBCSLeadByte,
    qemu_IsDBCSLeadByteEx,
    qemu_IsDebuggerPresent,
    qemu_IsNormalizedString,
    qemu_IsProcessInJob,
    qemu_IsProcessorFeaturePresent,
    qemu_IsSystemResumeAutomatic,
    qemu_IsThreadAFiber,
    qemu_IsValidCodePage,
    qemu_IsValidLanguageGroup,
    qemu_IsValidLocale,
    qemu_IsValidLocaleName,
    qemu_IsWow64Process,
    qemu_K32EmptyWorkingSet,
    qemu_K32EnumDeviceDrivers,
    qemu_K32EnumPageFilesA,
    qemu_K32EnumPageFilesW,
    qemu_K32EnumProcesses,
    qemu_K32EnumProcessModules,
    qemu_K32EnumProcessModulesEx,
    qemu_K32GetDeviceDriverBaseNameA,
    qemu_K32GetDeviceDriverBaseNameW,
    qemu_K32GetDeviceDriverFileNameA,
    qemu_K32GetDeviceDriverFileNameW,
    qemu_K32GetMappedFileNameA,
    qemu_K32GetMappedFileNameW,
    qemu_K32GetModuleBaseNameA,
    qemu_K32GetModuleBaseNameW,
    qemu_K32GetModuleFileNameExA,
    qemu_K32GetModuleFileNameExW,
    qemu_K32GetModuleInformation,
    qemu_K32GetPerformanceInfo,
    qemu_K32GetProcessImageFileNameA,
    qemu_K32GetProcessImageFileNameW,
    qemu_K32GetProcessMemoryInfo,
    qemu_K32GetWsChanges,
    qemu_K32InitializeProcessForWsWatch,
    qemu_K32QueryWorkingSet,
    qemu_K32QueryWorkingSetEx,
    qemu_LCIDToLocaleName,
    qemu_LCMapStringA,
    qemu_LCMapStringEx,
    qemu_LCMapStringW,
    qemu_LoadLibraryA,
    qemu_LoadLibraryExA,
    qemu_LoadLibraryExW,
    qemu_LoadLibraryW,
    qemu_LoadModule,
    qemu_LoadResource,
    qemu_LocalAlloc,
    qemu_LocalCompact,
    qemu_LocaleNameToLCID,
    qemu_LocalFlags,
    qemu_LocalFree,
    qemu_LocalHandle,
    qemu_LocalLock,
    qemu_LocalReAlloc,
    qemu_LocalShrink,
    qemu_LocalSize,
    qemu_LocalUnlock,
    qemu_LockFile,
    qemu_LockFileEx,
    qemu_LockResource,
    qemu_lstrcatA,
    qemu_lstrcatW,
    qemu_lstrcmpA,
    qemu_lstrcmpiA,
    qemu_lstrcmpiW,
    qemu_lstrcmpW,
    qemu_lstrcpyA,
    qemu_lstrcpynA,
    qemu_lstrcpynW,
    qemu_lstrcpyW,
    qemu_lstrlenA,
    qemu_lstrlenW,
    qemu_MakeCriticalSectionGlobal,
    qemu_MapViewOfFile,
    qemu_MapViewOfFileEx,
    qemu_MoveFileA,
    qemu_MoveFileExA,
    qemu_MoveFileExW,
    qemu_MoveFileW,
    qemu_MoveFileWithProgressA,
    qemu_MoveFileWithProgressW,
    qemu_MultiByteToWideChar,
    qemu_NeedCurrentDirectoryForExePathA,
    qemu_NeedCurrentDirectoryForExePathW,
    qemu_NormalizeString,
    qemu_OpenEventA,
    qemu_OpenEventW,
    qemu_OpenFile,
    qemu_OpenFileById,
    qemu_OpenFileMappingA,
    qemu_OpenFileMappingW,
    qemu_OpenJobObjectA,
    qemu_OpenJobObjectW,
    qemu_OpenMutexA,
    qemu_OpenMutexW,
    qemu_OpenProcess,
    qemu_OpenProfileUserMapping,
    qemu_OpenSemaphoreA,
    qemu_OpenSemaphoreW,
    qemu_OpenThread,
    qemu_OpenVxDHandle,
    qemu_OpenWaitableTimerA,
    qemu_OpenWaitableTimerW,
    qemu_OutputDebugStringA,
    qemu_OutputDebugStringW,
    qemu_PeekNamedPipe,
    qemu_PostQueuedCompletionStatus,
    qemu_PowerClearRequest,
    qemu_PowerCreateRequest,
    qemu_PowerSetRequest,
    qemu_PrepareTape,
    qemu_ProcessIdToSessionId,
    qemu_PulseEvent,
    qemu_PurgeComm,
    qemu_QueryActCtxW,
    qemu_QueryFullProcessImageNameA,
    qemu_QueryFullProcessImageNameW,
    qemu_QueryInformationJobObject,
    qemu_QueryMemoryResourceNotification,
    qemu_QueryPerformanceCounter,
    qemu_QueryPerformanceFrequency,
    qemu_QueueUserAPC,
    qemu_QueueUserWorkItem,
    qemu_ReadDirectoryChangesW,
    qemu_ReadFile,
    qemu_ReadFileEx,
    qemu_ReadFileScatter,
    qemu_ReadProcessMemory,
    qemu_RegisterApplicationRecoveryCallback,
    qemu_RegisterApplicationRestart,
    qemu_RegisterServiceProcess,
    qemu_RegisterWaitForSingleObject,
    qemu_RegisterWaitForSingleObjectEx,
    qemu_ReinitializeCriticalSection,
    qemu_ReleaseActCtx,
    qemu_ReleaseMutex,
    qemu_ReleaseSemaphore,
    qemu_RemoveDirectoryA,
    qemu_RemoveDirectoryW,
    qemu_ReplaceFileA,
    qemu_ReplaceFileW,
    qemu_RequestWakeupLatency,
    qemu_ResetEvent,
    qemu_ResetWriteWatch,
    qemu_ResumeThread,
    qemu_SearchPathA,
    qemu_SearchPathW,
    qemu_SetCommBreak,
    qemu_SetCommConfig,
    qemu_SetCommMask,
    qemu_SetCommState,
    qemu_SetCommTimeouts,
    qemu_SetComputerNameA,
    qemu_SetComputerNameExA,
    qemu_SetComputerNameExW,
    qemu_SetComputerNameW,
    qemu_SetConsoleCtrlHandler,
    qemu_SetCPGlobal,
    qemu_SetCurrentDirectoryA,
    qemu_SetCurrentDirectoryW,
    qemu_SetDefaultCommConfigA,
    qemu_SetDefaultCommConfigW,
    qemu_SetDefaultDllDirectories,
    qemu_SetDllDirectoryA,
    qemu_SetDllDirectoryW,
    qemu_SetEndOfFile,
    qemu_SetEnvironmentVariableA,
    qemu_SetEnvironmentVariableW,
    qemu_SetErrorMode,
    qemu_SetEvent,
    qemu_SetFileApisToANSI,
    qemu_SetFileApisToOEM,
    qemu_SetFileAttributesA,
    qemu_SetFileAttributesW,
    qemu_SetFileCompletionNotificationModes,
    qemu_SetFileInformationByHandle,
    qemu_SetFilePointer,
    qemu_SetFilePointerEx,
    qemu_SetFileTime,
    qemu_SetFileValidData,
    qemu_SetHandleContext,
    qemu_SetHandleCount,
    qemu_SetHandleInformation,
    qemu_SetInformationJobObject,
    qemu_SetLastError,
    qemu_SetLocaleInfoA,
    qemu_SetLocaleInfoW,
    qemu_SetMailslotInfo,
    qemu_SetNamedPipeHandleState,
    qemu_SetPriorityClass,
    qemu_SetProcessAffinityMask,
    qemu_SetProcessDEPPolicy,
    qemu_SetProcessPriorityBoost,
    qemu_SetProcessShutdownParameters,
    qemu_SetProcessWorkingSetSize,
    qemu_SetSearchPathMode,
    qemu_SetStdHandle,
    qemu_SetSystemFileCacheSize,
    qemu_SetSystemPowerState,
    qemu_SetTapeParameters,
    qemu_SetTapePosition,
    qemu_SetThreadAffinityMask,
    qemu_SetThreadContext,
    qemu_SetThreadErrorMode,
    qemu_SetThreadExecutionState,
    qemu_SetThreadGroupAffinity,
    qemu_SetThreadIdealProcessor,
    qemu_SetThreadIdealProcessorEx,
    qemu_SetThreadLocale,
    qemu_SetThreadpoolTimer,
    qemu_SetThreadpoolWait,
    qemu_SetThreadPreferredUILanguages,
    qemu_SetThreadPriority,
    qemu_SetThreadPriorityBoost,
    qemu_SetThreadStackGuarantee,
    qemu_SetThreadUILanguage,
    qemu_SetUnhandledExceptionFilter,
    qemu_SetupComm,
    qemu_SetUserGeoID,
    qemu_SetWaitableTimer,
    qemu_SetWaitableTimerEx,
    qemu_SignalObjectAndWait,
    qemu_SizeofResource,
    qemu_Sleep,
    qemu_SleepConditionVariableCS,
    qemu_SleepConditionVariableSRW,
    qemu_SleepEx,
    qemu_SuspendThread,
    qemu_SwitchToFiber,
    qemu_SwitchToThread,
    qemu_TerminateJobObject,
    qemu_TerminateProcess,
    qemu_TerminateThread,
    qemu_TlsAlloc,
    qemu_TlsFree,
    qemu_TlsGetValue,
    qemu_TlsSetValue,
    qemu_TransactNamedPipe,
    qemu_TransmitCommChar,
    qemu_TrySubmitThreadpoolCallback,
    qemu_UnhandledExceptionFilter,
    qemu_UninitializeCriticalSection,
    qemu_UnlockFile,
    qemu_UnlockFileEx,
    qemu_UnmapViewOfFile,
    qemu_UnregisterApplicationRestart,
    qemu_UnregisterWait,
    qemu_UnregisterWaitEx,
    qemu_UpdateProcThreadAttribute,
    qemu_UpdateResourceA,
    qemu_UpdateResourceW,
    qemu_VerifyVersionInfoA,
    qemu_VerifyVersionInfoW,
    qemu_VerLanguageNameA,
    qemu_VerLanguageNameW,
    qemu_VirtualAlloc,
    qemu_VirtualAllocEx,
    qemu_VirtualFree,
    qemu_VirtualFreeEx,
    qemu_VirtualLock,
    qemu_VirtualProtect,
    qemu_VirtualProtectEx,
    qemu_VirtualQuery,
    qemu_VirtualQueryEx,
    qemu_VirtualUnlock,
    qemu_WaitCommEvent,
    qemu_WaitForDebugEvent,
    qemu_WaitForMultipleObjects,
    qemu_WaitForMultipleObjectsEx,
    qemu_WaitForSingleObject,
    qemu_WaitForSingleObjectEx,
    qemu_WaitNamedPipeA,
    qemu_WaitNamedPipeW,
    qemu_WideCharToMultiByte,
    qemu_WinExec,
    qemu_Wow64DisableWow64FsRedirection,
    qemu_Wow64EnableWow64FsRedirection,
    qemu_Wow64RevertWow64FsRedirection,
    qemu_WriteFile,
    qemu_WriteFileEx,
    qemu_WriteFileGather,
    qemu_WritePrivateProfileSectionA,
    qemu_WritePrivateProfileSectionW,
    qemu_WritePrivateProfileStringA,
    qemu_WritePrivateProfileStringW,
    qemu_WritePrivateProfileStructA,
    qemu_WritePrivateProfileStructW,
    qemu_WriteProcessMemory,
    qemu_WriteProfileSectionA,
    qemu_WriteProfileSectionW,
    qemu_WriteProfileStringA,
    qemu_WriteProfileStringW,
    qemu_WriteTapemark,
    qemu_WTSGetActiveConsoleSessionId,
    qemu_ZombifyActCtx,
};

const WINAPI syscall_handler *qemu_dll_register(const struct qemu_ops *ops, uint32_t *dll_num)
{
    WINE_TRACE("Loading host-side kernel32 wrapper.\n");
    qemu_ops = ops;
    *dll_num = QEMU_CURRENT_DLL;

    return dll_functions;
}

#endif
