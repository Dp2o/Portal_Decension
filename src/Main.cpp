#include <windows.h>
#include <string>
#include <vector>
#include <ctime>
#include <cmath>  // <--- ADDED THIS TO FIX THE ERROR

#define BOOT_LINES 12

int SCREEN_W, SCREEN_H;
bool g_Running = true;
float g_BootTimer = 0.0f;
float g_LoadPercent = 0.0f;

struct LogLine { std::string text; float time; };
std::vector<LogLine> g_BootLogs;

void InitLogs() {
    g_BootLogs = {
        {"[ System ] INITIALIZING PORTAL_DECENSION_V1.00", 0.5f},
        {"[ System ] ENCRYPTING FILES...", 1.2f},
        {"[ System ] DECRYPTING CORE_ASSETS.PAK...", 2.0f},
        {"[ System ] LOADING CORD_GRABBER.EXE...", 3.1f},
        {"[ System ] GRABBING PLAYER COORDINATES...", 3.8f},
        {"[ System ] SETTING NUCLEAR COORDINATES...", 4.5f},
        {"[ System ] LOADING PHYSICS_ENGINE...", 5.2f},
        {"[ System ] PREPARING FOR BOOT...", 5.8f},
        {"[ System ] ALL SYSTEMS NOMINAL.", 6.2f},
        {"", 6.5f},
        {">> ENTERING SIMULATION...", 7.0f}
    };
}

void DrawTerminal(HDC hdc) {
    // Background Dark Green Tint
    RECT r = {0, 0, SCREEN_W, SCREEN_H};
    HBRUSH hBack = CreateSolidBrush(RGB(5, 15, 5));
    FillRect(hdc, &r, hBack);
    DeleteObject(hBack);

    HFONT hFont = CreateFontA(24, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH | FF_MODERN, "Consolas");
    SelectObject(hdc, hFont);
    SetBkMode(hdc, TRANSPARENT);

    // Typing Logs
    for (int i = 0; i < g_BootLogs.size(); i++) {
        if (g_BootTimer > g_BootLogs[i].time) {
            SetTextColor(hdc, (i == 3) ? RGB(255, 200, 0) : RGB(50, 255, 50));
            TextOutA(hdc, 50, 50 + (i * 30), g_BootLogs[i].text.c_str(), g_BootLogs[i].text.length());
        }
    }

    // Flicker Effect
    if ((int)(g_BootTimer * 10) % 7 == 0) {
        HBRUSH hFlicker = CreateSolidBrush(RGB(20, 40, 20));
        RECT rf = {0, 0, SCREEN_W, SCREEN_H};
        // Use a light blend or just skip drawing to simulate flicker
        DeleteObject(hFlicker);
    }

    DeleteObject(hFont);
}

void DrawLoadingScreen(HDC hdc) {
    // Background Black
    RECT r = {0, 0, SCREEN_W, SCREEN_H};
    HBRUSH hBack = CreateSolidBrush(RGB(0, 0, 0));
    FillRect(hdc, &r, hBack);
    DeleteObject(hBack);

    // Title
    HFONT hFont = CreateFontA(60, 0, 0, 0, FW_HEAVY, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH, "Impact");
    SelectObject(hdc, hFont);
    SetTextColor(hdc, RGB(0, 255, 255));
    std::string title = "PORTAL DESCENT";
    TextOutA(hdc, (SCREEN_W / 2) - 200, (SCREEN_H / 2) - 100, title.c_str(), title.length());
    DeleteObject(hFont);

    // Progress Bar Outline
    int bw = 400, bh = 30;
    int bx = (SCREEN_W / 2) - (bw / 2);
    int by = (SCREEN_H / 2) + 20;
    
    HPEN hPen = CreatePen(PS_SOLID, 2, RGB(100, 100, 100));
    SelectObject(hdc, hPen);
    Rectangle(hdc, bx, by, bx + bw, by + bh);
    
    // Fill Bar
    HBRUSH hBar = CreateSolidBrush(RGB(0, 255, 255));
    RECT fill = {bx + 4, by + 4, bx + 4 + (int)((bw - 8) * (g_LoadPercent / 100.0f)), by + bh - 4};
    FillRect(hdc, &fill, hBar);
    
    DeleteObject(hBar);
    DeleteObject(hPen);

    // Multi-step subtext sequence based on progress
    static const std::vector<std::string> steps = {
        "DOWNLOADING NUCLEAR LAUNCH CODES...",
        "VERIFYING ARCHIVE...",
        "DECRYPTING PAYLOAD...",
        "INPUTTING NUCLEAR LAUNCH CODES...",
        "FINALIZING..."
    };
    
    // Determine which step to show by mapping progress to step index
    // CHANGED: used 'floor' instead of 'floorf' and ensured proper casting
    size_t idx = (size_t)floor((g_LoadPercent / 100.0f) * (float)steps.size());
    if (idx >= steps.size()) idx = steps.size() - 1;

    SetTextColor(hdc, RGB(200, 200, 200));
    std::string loadText = steps[idx] + " " + std::to_string((int)g_LoadPercent) + "%";
    TextOutA(hdc, bx, by + 45, loadText.c_str(), loadText.length());
}

LRESULT CALLBACK WndProc(HWND h, UINT m, WPARAM w, LPARAM l) {
    if (m == WM_DESTROY) PostQuitMessage(0);
    if (m == WM_KEYDOWN && w == VK_ESCAPE) g_Running = false;
    return DefWindowProc(h, m, w, l);
}

int WINAPI WinMain(HINSTANCE hi, HINSTANCE hp, LPSTR lp, int ns) {
    SCREEN_W = GetSystemMetrics(SM_CXSCREEN); SCREEN_H = GetSystemMetrics(SM_CYSCREEN);
    WNDCLASS wc = {0}; wc.lpfnWndProc = WndProc; wc.hInstance = hi; wc.lpszClassName = "Terminal";
    RegisterClass(&wc);
    HWND hw = CreateWindowEx(0, "Terminal", "System Boot", WS_POPUP | WS_VISIBLE, 0, 0, SCREEN_W, SCREEN_H, NULL, NULL, hi, NULL);
    
    InitLogs();
    ShowCursor(FALSE);

    while (g_Running) {
        MSG msg = {};
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) { 
            if (msg.message == WM_QUIT) g_Running = false; 
            TranslateMessage(&msg); DispatchMessage(&msg); 
        }

        g_BootTimer += 0.016f; 

        HDC hdc = GetDC(hw);
        HDC mdc = CreateCompatibleDC(hdc);
        HBITMAP mbm = CreateCompatibleBitmap(hdc, SCREEN_W, SCREEN_H);
        SelectObject(mdc, mbm);

        if (g_BootTimer < 8.0f) {
            DrawTerminal(mdc);
        } else {
            if (g_LoadPercent < 100.0f) g_LoadPercent += 0.3f;
            DrawLoadingScreen(mdc);
        }

        BitBlt(hdc, 0, 0, SCREEN_W, SCREEN_H, mdc, 0, 0, SRCCOPY);
        DeleteObject(mbm);
        DeleteDC(mdc);
        ReleaseDC(hw, hdc);
        
        Sleep(16);
    }

    ShowCursor(TRUE);
    return 0;
}
