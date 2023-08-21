/* Copyright 2023 kmwebnet
 * SPDX-License-Identifier: Apache-2.0
 */

#include <windows.h>
#include <stdio.h>

#ifndef FLOW_SILENT

void CreateConsole(void) {
    FILE* fp;
    AllocConsole();
    freopen_s(&fp, "CONOUT$", "w", stdout);
    freopen_s(&fp, "CONOUT$", "w", stderr);
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    switch (fdwReason) {
    case DLL_PROCESS_ATTACH:
        CreateConsole();
        break;
    }
    return TRUE;
}
#endif