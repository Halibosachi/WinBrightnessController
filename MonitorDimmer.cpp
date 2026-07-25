#include <iostream>
#include <windows.h>
#include <highlevelmonitorconfigurationapi.h>
#include <string>

int ChangeBrightness(int step) {
    HWND hDesktop = GetDesktopWindow();
    HMONITOR hMonitor = MonitorFromWindow(hDesktop, MONITOR_DEFAULTTOPRIMARY);
    DWORD monitorCount;

    if(!GetNumberOfPhysicalMonitorsFromHMONITOR(hMonitor, &monitorCount)){
        return -1;
    }

    PHYSICAL_MONITOR* pMonitors = new PHYSICAL_MONITOR[monitorCount];

    if( !GetPhysicalMonitorsFromHMONITOR(hMonitor, monitorCount, pMonitors) ){
        delete[] pMonitors;
        return -1;
    }
    
    HANDLE hPhysical = pMonitors[0].hPhysicalMonitor;
    DWORD minB, currB, maxB;
    int newB = -1;


    if(GetMonitorBrightness(hPhysical, &minB, &currB, &maxB)){
        newB = currB + step;
        if(newB > maxB)  newB = maxB;
        if(newB < minB)  newB = minB;
        SetMonitorBrightness(hPhysical, newB); 
    }
    
    DestroyPhysicalMonitors(monitorCount, pMonitors);
    delete[] pMonitors;

    return newB;

}

void DrawBrightnessOnScreen(int brightness) {
    if (brightness == -1) return; // If the brightness wasn't read, it shouldn't draw anything.

    HDC hdc = GetDC(NULL); // Get drawing permission for the entire screen.

    // Set text color.
    SetTextColor(hdc, RGB(0, 255, 0));
    
    // Background color: black.
    SetBkColor(hdc, RGB(0, 0, 0));

    // Making background of text.
    SetBkMode(hdc, OPAQUE); 

    // Ekrana basılacak metni hazırla
    std::string text = "Brightness: %" + std::to_string(brightness) + "    ";

    // Drawing terxt to this coordinates.
    TextOutA(hdc, 50, 50, text.c_str(), text.length());

    // To prevent memory leaks in the operating system, release the canvas.
    ReleaseDC(NULL, hdc); 
}

void AddToStartup() {
    HKEY hKey;
    const char* appName = "BrightnessDimmerApp"; // Name to appear in the registry
    char exePath[MAX_PATH];
    
    // 1. Find .exe path.
    GetModuleFileNameA(NULL, exePath, MAX_PATH);

    // 2. Open the Startup folder in the Registry.
    if (RegOpenKeyExA(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS) {
        // 3. Save file path.
        RegSetValueExA(hKey, appName, 0, REG_SZ, (const BYTE*)exePath, strlen(exePath) + 1);
        RegCloseKey(hKey);
    }
}

int main() {
    FreeConsole();
    AddToStartup();
    
    //Defining distinct IDs for each hotkey
    const int HOTKEY_UP_ID{1};
    const int HOTKEY_DOWN_ID{2};
    const int HOTKEY_QUIT_ID{3};
    
    int currentLevel{};
    UINT_PTR timerId = 0;  //Variable for tracking the timer.

    MSG msg = {0};

    //If for registerhotkey for increasing brightness
    if( !RegisterHotKey(NULL,HOTKEY_UP_ID,MOD_CONTROL | MOD_ALT,VK_UP) ){
        std::cout<<"FAIL! 1"<<'\n';
        return 1;
    } 


    //If for registerhotkey for decreasing brightness
    if( !RegisterHotKey(NULL,HOTKEY_DOWN_ID,MOD_CONTROL | MOD_ALT,VK_DOWN) ){
        std::cout<<"FAIL! 2"<<'\n';
        return 1;
    }

    //If for registerhotkey for quiting the program
    if( !RegisterHotKey(NULL,HOTKEY_QUIT_ID,MOD_CONTROL | MOD_ALT,0x51) ){
        std::cout<<"FAIL! 3"<<'\n';
        return 1;
    }

    while(GetMessage(&msg, NULL, 0, 0) > 0){
        if(msg.message == WM_HOTKEY){
            if(msg.wParam == HOTKEY_UP_ID){
                currentLevel = ChangeBrightness(10);
                DrawBrightnessOnScreen(currentLevel);
            }

            if(msg.wParam == HOTKEY_DOWN_ID){
                currentLevel = ChangeBrightness(-10);
                DrawBrightnessOnScreen(currentLevel);
            }

            if (timerId != 0) {                 // If program already have a countdown timer, cancel it.
                    KillTimer(NULL, timerId);
                }

            timerId = SetTimer(NULL, 0, 3000, NULL);    // Start new timer for 3000 millisecond.

            if(msg.wParam == HOTKEY_QUIT_ID){
                break;
            }
        }

        // If the incoming message indicates that the timer has expired:
        else if (msg.message == WM_TIMER) {
            if (msg.wParam == timerId) {
                KillTimer(NULL, timerId); // We are done with the timer quit it.
                timerId = 0;
                
                RedrawWindow(NULL, NULL, NULL, RDW_INVALIDATE | RDW_ALLCHILDREN | RDW_UPDATENOW | RDW_ERASENOW);
            }
        }

    }


    
    return 0;
}