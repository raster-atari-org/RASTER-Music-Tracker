#include "Shell.h"

#include "GuiHelpers.h"

BOOL CShell::OpenFile(const CString& filePath) {
    /* "HINSTANCE ShellExecuteA(
       [in, optional] HWND   hwnd,
       [in, optional] LPCSTR lpOperation,
       [in]           LPCSTR lpFile,
       [in, optional] LPCSTR lpParameters,
       [in, optional] LPCSTR lpDirectory,
       [in]           INT    nShowCmd
       ); "*/
    const auto result = ShellExecute(NULL, TEXT("open"), filePath, TEXT(""), NULL, SW_SHOWNORMAL);
    if ((INT_PTR)result < 32) {
        LPTSTR lpMsgBuf;
        DWORD dw = GetLastError();

        if (FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            dw,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPTSTR)&lpMsgBuf, // What a cray API definition
            0, NULL) == 0) {
            SendErrorMessage(lpMsgBuf);
            return FALSE;
        }
        CString message;
        message.Format("Error while opening \"%s\": %s", filePath, lpMsgBuf);
        LocalFree(lpMsgBuf);

        SendErrorMessage(message);
        return FALSE;
    }
    return TRUE;
}
