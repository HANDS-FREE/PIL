#include <windows.h>


#if !defined(PIL_NO_UNWINDOWS)
// A list of annoying macros to #undef.
// Feel free to extend as required.
#undef GetBinaryType
#undef GetShortPathName
#undef GetLongPathName
#undef GetEnvironmentStrings
#undef SetEnvironmentStrings
#undef FreeEnvironmentStrings
#undef FormatMessage
#undef EncryptFile
#undef DecryptFile
#undef CreateMutex
#undef OpenMutex
#undef CreateEvent
#undef OpenEvent
#undef CreateSemaphore
#undef OpenSemaphore
#undef LoadLibrary
#undef GetModuleFileName
#undef CreateProcess
#undef GetCommandLine
#undef GetEnvironmentVariable
#undef SetEnvironmentVariable
#undef ExpandEnvironmentStrings
#undef OutputDebugString
#undef FindResource
#undef UpdateResource
#undef FindAtom
#undef AddAtom
#undef GetSystemDirector
#undef GetTempPath
#undef GetTempFileName
#undef SetCurrentDirectory
#undef GetCurrentDirectory
#undef CreateDirectory
#undef RemoveDirectory
#undef CreateFile
#undef DeleteFile
#undef SearchPath
#undef CopyFile
#undef MoveFile
#undef ReplaceFile
#undef GetComputerName
#undef SetComputerName
#undef GetUserName
#undef LogonUser
#undef GetVersion
#undef GetObject
#endif // PIL_NO_UNWINDOWS
