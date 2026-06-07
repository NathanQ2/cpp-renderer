#pragma once

#ifdef _WIN32
    #ifdef PT_BUILD
        #define PT_API __declspec(dllexport)
    #else
        #define PT_API __declspec(dllimport)
    #endif
#else
    #define PT_API
#endif
