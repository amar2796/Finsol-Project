#pragma once
#include <windows.h>

namespace FIN {
    inline void setConsoleColor(WORD color) 
    {
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(hConsole, color);

        /*
        Colors code : 
        
        0 = Black
        1 = Blue
        2 = Green
        3 = Cyan
        4 = Red
        5 = Magenta
        6 = Brown
        7 = Light Gray(Default)
        8 = Dark Gray
        9 = Light Blue
        10 = Light Green
        11 = Light Cyan
        12 = Light Red
        13 = Light Magenta
        14 = Yellow
        15 = White*/
    }
}