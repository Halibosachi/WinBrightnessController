#include <iostream>
#include <windows.h>
#include <highlevelmonitorconfigurationapi.h>

void ChangeBrightness(int step){
    //In progress
}

int main() {
    //Defining distinct IDs for each hotkey
    const int HOTKEY_UP_ID{1};
    const int HOTKEY_DOWN_ID{2};
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

    while(GetMessage(&msg, NULL, 0, 0) > 0){
        if(msg.message == WM_HOTKEY){
            if(msg.wParam == HOTKEY_UP_ID){
                std::cout <<"Increase brightness."<<'\n';
            }

            if(msg.wParam == HOTKEY_DOWN_ID){
                std::cout <<"Decrease brightness."<<'\n';
            }
        }
    }


    
    return 0;
}