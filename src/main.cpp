#define OEMRESOURCE 
#include <windows.h>

#include <array>
#include <vector>

#include "core/PianoKey.h"
#include "core/PianoKeyboard.h"
#include "ui/ObjectGrid.h"
#include "resource.h"

HWND hBtnOptimise, hBtnTransposeDown, hBtnTransposeUp, hBtnReset, hBtnImport;
HWND hTextClick, hTextImport, hTextOptimise, hTextReset, hTextFile, hTextResult;

ObjectGrid grid;
PianoKeyboard keyboard;

const wchar_t g_szClassName[] = L"myWindowClass";
int g_default_width = GetSystemMetrics(SM_CXSCREEN) * 3 / 5;
int g_default_height = GetSystemMetrics(SM_CYSCREEN) * 3 / 5;

// Window procedure
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg) {
    case WM_CREATE:
    {
        hBtnOptimise = CreateWindowW(L"BUTTON", L"Optimise",
            WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | WS_TABSTOP,
            0, 0, 0, 0, hwnd, (HMENU)ID_BTN_OPTIMISE, NULL, NULL);

        hBtnTransposeDown = CreateWindowW(L"BUTTON", L"Transpose down",
            WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | WS_TABSTOP,
            0, 0, 0, 0, hwnd, (HMENU)ID_BTN_TRANSPOSE_DOWN, NULL, NULL);

        hBtnTransposeUp = CreateWindowW(L"BUTTON", L"Transpose up",
            WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | WS_TABSTOP,
            0, 0, 0, 0, hwnd, (HMENU)ID_BTN_TRANSPOSE_UP, NULL, NULL);

        hBtnReset = CreateWindowW(L"BUTTON", L"Reset",
            WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | WS_TABSTOP,
            0, 0, 0, 0, hwnd, (HMENU)ID_BTN_RESET, NULL, NULL);

        hBtnImport = CreateWindowW(L"BUTTON", L"Import",
            WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | WS_TABSTOP,
            0, 0, 0, 0, hwnd, (HMENU)ID_BTN_IMPORT, NULL, NULL);

        hTextClick = CreateWindowW(L"STATIC", L"1. Left click to select played keys and right click to select broken keys.",
            WS_VISIBLE | WS_CHILD | SS_SIMPLE,
            0, 0, 0, 0, hwnd, (HMENU)ID_TEXT_CLICK, NULL, NULL);

        hTextImport = CreateWindowW(L"STATIC", L"You can also import played keys from MIDI file.",
            WS_VISIBLE | WS_CHILD | SS_SIMPLE,
            0, 0, 0, 0, hwnd, (HMENU)ID_TEXT_IMPORT, NULL, NULL);

        hTextOptimise = CreateWindowW(L"STATIC", L"2. Find the transpose which avoids most broken keys by hitting 'Optimise'",
            WS_VISIBLE | WS_CHILD | SS_SIMPLE,
            0, 0, 0, 0, hwnd, (HMENU)ID_TEXT_OPTIMISE, NULL, NULL);

        hTextReset = CreateWindowW(L"STATIC", L"3. Hit 'Reset' to release all keys and allow editing again.",
            WS_VISIBLE | WS_CHILD | SS_SIMPLE,
            0, 0, 0, 0, hwnd, (HMENU)ID_TEXT_RESET, NULL, NULL);

        hTextFile = CreateWindowW(L"STATIC", L"File:",
            WS_VISIBLE | WS_CHILD | SS_SIMPLE,
            0, 0, 0, 0, hwnd, (HMENU)ID_TEXT_FILE, NULL, NULL);

        hTextResult = CreateWindowW(L"STATIC", L"Best transpose:",
            WS_VISIBLE | WS_CHILD | SS_SIMPLE,
            0, 0, 0, 0, hwnd, (HMENU)ID_TEXT_RESULT, NULL, NULL);
    }
    break;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        keyboard.DrawKeys(hdc);
        EndPaint(hwnd, &ps);
    }
    break;

    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
    {
        POINT pt = { LOWORD(lParam), HIWORD(lParam) };
        bool is_right_click = (msg == WM_RBUTTONDOWN);
        keyboard.HandleClickAtPoint(pt, is_right_click, hwnd);
    }
    break;

    case WM_SIZE:
    {
        int window_width = LOWORD(lParam);
        int window_height = HIWORD(lParam);

        // Calculate grid to place controls
        grid.CalculateGrid(hwnd, window_width, window_height, 20, 30, 10);

        // Resize keyboard
        keyboard.LayoutKeys(window_width, window_height);

        // Place buttons and text
        grid.PlaceObject(hTextClick, 4, 4, 10, 2);

        grid.PlaceObject(hTextImport, 4, 6, 10, 2);
        grid.PlaceObject(hBtnImport, 4, 8, 2, 2);
        grid.PlaceObject(hTextFile, 6, 8, 10, 2);

        grid.PlaceObject(hTextOptimise, 4, 12, 10, 2);
        grid.PlaceObject(hBtnOptimise, 4, 14, 2, 2);
        grid.PlaceObject(hTextResult, 6, 14, 10, 2);

        grid.PlaceObject(hBtnTransposeDown, 4, 16, 3, 2);
        grid.PlaceObject(hBtnTransposeUp, 7, 16, 3, 2);

        grid.PlaceObject(hTextReset, 4, 20, 10, 2);
        grid.PlaceObject(hBtnReset, 4, 22, 2, 2);

        InvalidateRect(hwnd, NULL, TRUE); // Redraw whole window

    }
    break;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
            case ID_BTN_OPTIMISE:
                keyboard.OptimiseKeys();
                // Optimising and editing keys again should be disabled until reset
                if(keyboard.GetEditLockStatus()) EnableWindow(hBtnOptimise, FALSE);
                InvalidateRect(hwnd, NULL, TRUE);
                break;

            case ID_BTN_TRANSPOSE_DOWN:
                keyboard.TransposeDown(1);
                InvalidateRect(hwnd, NULL, TRUE);
                break;

            case ID_BTN_TRANSPOSE_UP:
                keyboard.TransposeUp(1);
                InvalidateRect(hwnd, NULL, TRUE);
                break;

            case ID_BTN_RESET:
                keyboard.ResetKeys();
                if(!keyboard.GetEditLockStatus()) EnableWindow(hBtnOptimise, TRUE);
                InvalidateRect(hwnd, NULL, TRUE);
                break;
        }
        break;

    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProcW(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    PWSTR pCmdLine, int nCmdShow)
{
    WNDCLASSEXW wc{};
    HWND hwnd;
    MSG Msg;

    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = 0;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = NULL;
    wc.hCursor = (HCURSOR) LoadImageW(NULL, MAKEINTRESOURCEW(OCR_NORMAL), IMAGE_CURSOR, 0, 0, LR_SHARED);
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = g_szClassName;
    wc.hIconSm = NULL;

    if (!RegisterClassExW(&wc))
    {
        MessageBoxW(NULL, L"Window Registration Failed!", L"Error!",
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    SetProcessDPIAware();

    hwnd = CreateWindowExW(
        WS_EX_CLIENTEDGE,
        g_szClassName,
        L"Broken Keys",
        WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX|WS_MAXIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, g_default_width, g_default_height,
        NULL, NULL, hInstance, NULL);

    if (hwnd == NULL)
    {
        MessageBoxW(NULL, L"Window Creation Failed!", L"Error!",
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    while (GetMessageW(&Msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&Msg);
        DispatchMessageW(&Msg);
    }
    return Msg.wParam;
}
