// MMO_Figter_Mont.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "framework.h"
#include "MMO_Figter_Mont.h"

#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

// Window Draw 함수
void UpdateGridBackground(HWND hWnd) {
    //// 메모리 DC 클리어
    //PatBlt(grid_hMemDC, 0, 0, g_MemDC_Rect.right, g_MemDC_Rect.bottom, WHITENESS);
    //gGrid.DrawGrid(grid_hMemDC, g_MemDC_Rect.right, g_MemDC_Rect.bottom);// , & pMgr.players, & pMgr.playersMtx);
}
void WindowPaintPlayer(HWND hWnd, HDC hdc) {
    /*
    // 메모리 DC 클리어
    PatBlt(g_hMemDC, 0, 0, g_MemDC_Rect.right, g_MemDC_Rect.bottom, WHITENESS);
    //gGrid.DrawGrid(g_hMemDC, g_MemDC_Rect.right, g_MemDC_Rect.bottom, &pMgr.players, &pMgr.playersMtx);
    gGrid.DrawPlayer(g_hMemDC, g_MemDC_Rect.right, g_MemDC_Rect.bottom, &pMgr.players, &pMgr.playersMtx);
    */

    //PatBlt(g_hMemDC, 0, 0, g_MemDC_Rect.right, g_MemDC_Rect.bottom, WHITENESS);
    //gGrid.DrawGrid(g_hMemDC, g_MemDC_Rect.right, g_MemDC_Rect.bottom);// , & pMgr.players, & pMgr.playersMtx);

    //UpdateGridBackground(hWnd);
    //PAINTSTRUCT ps;
    //HDC hdc = BeginPaint(hWnd, &ps);
    //HDC hdc = GetWindowDC(hWnd);
    //BitBlt(hdc, 0, 0, g_MemDC_Rect.right, g_MemDC_Rect.bottom, grid_hMemDC, 0, 0, SRCCOPY);
    //BitBlt(hdc, 0, 0, g_MemDC_Rect.right, g_MemDC_Rect.bottom, g_hMemDC, 0, 0, SRCCOPY);

    //EndPaint(hWnd, &ps);
    //ReleaseDC(hWnd, hdc);
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_MMOFIGTERMONT, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MMOFIGTERMONT));

    MSG msg;

    // 기본 메시지 루프입니다:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  함수: MyRegisterClass()
//
//  용도: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MMOFIGTERMONT));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_MMOFIGTERMONT);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   용도: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   주석:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   // Open Console
   AllocConsole();
   freopen("CONOUT$", "wt", stdout);

   /////////////////////
   // My Init
   /////////////////////
   
   // Draw
   HDC hdc = GetDC(hWnd);
   //GetClientRect(hWnd, &g_MemDC_Rect);
   GetClientRect(hWnd, &grid_MemDC_Rect);

   //g_hMemDC_Bitmap = CreateCompatibleBitmap(hdc, g_MemDC_Rect.right, g_MemDC_Rect.bottom);
   //g_hMemDC = CreateCompatibleDC(hdc);
   //g_hMemDC_BitmapOld = (HBITMAP)SelectObject(g_hMemDC, g_hMemDC_Bitmap);

   grid_hMemDC_Bitmap = CreateCompatibleBitmap(hdc, grid_MemDC_Rect.right, grid_MemDC_Rect.bottom);
   grid_hMemDC = CreateCompatibleDC(hdc);
   grid_hMemDC_BitmapOld = (HBITMAP)SelectObject(grid_hMemDC, grid_hMemDC_Bitmap);

   ReleaseDC(hWnd, hdc);

   // Grid
   gGrid.SetGridCell(64, 100, 100);
   //UpdateGridBackground(hWnd);

   // Capture
   servCapture.Init(serverIP);// , true);
   servCapture.RunServerCapture(serverIP, listenPort);
   pMgr.SetCapture(&servCapture);
   pMgr.SetServerIP(serverIP);
   pMgr.RunProcCapture();
   pMgr.RunProcFrameMove(WaitMS);

   // 타이머
   SetTimer(hWnd, timer120ms, 120, NULL);
   SetTimer(hWnd, timer40ms, 40, NULL);

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  용도: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 애플리케이션 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 메뉴 선택을 구문 분석합니다:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_TIMER:
    {
        //PAINTSTRUCT ps;
        //HDC hdc = BeginPaint(hWnd, &ps);
        //RECT rect;
        //GetClientRect(hWnd, &rect);
        //gGrid.Draw(hdc, rect.right, rect.bottom, &pMgr.players, &pMgr.playersMtx);
        //EndPaint(hWnd, &ps);
        switch (wParam)
        {
        //case timer40ms:
        //    pMgr.FrameMove()
        //    break;
        //case timer40ms:
        //    pMgr.RunProcFrameMove(1);
        
        //case timer120ms: {
        //    //pMgr.FrameMove(3);
        //    //InvalidateRect(hWnd, NULL, TRUE);
        //    SetThreadPriority(GetCurrentThread(), +1);
        //    //InvalidateRect(hWnd, NULL, TRUE);
        //    HDC hdc = GetWindowDC(hWnd);
        //    WindowPaintPlayer(hWnd, hdc);  // 윈도우를 다시 그림
        //    ReleaseDC(hWnd, hdc);
        //    SetThreadPriority(GetCurrentThread(), 0);
        //}
        //    break;

        default:
            break;
        }
    }
    break;
    case WM_PAINT:
        {
            // 메모리 DC 클리어
            //PatBlt(g_hMemDC, 0, 0, g_MemDC_Rect.right, g_MemDC_Rect.bottom, WHITENESS);
            //gGrid.DrawGrid(g_hMemDC, g_MemDC_Rect.right, g_MemDC_Rect.bottom);
            PatBlt(grid_hMemDC, 0, 0, grid_MemDC_Rect.right, grid_MemDC_Rect.bottom, WHITENESS);
            gGrid.DrawGrid(grid_hMemDC, grid_MemDC_Rect.right, grid_MemDC_Rect.bottom);

            //gGrid.Draw(g_hMemDC, g_MemDC_Rect.right, g_MemDC_Rect.bottom, &pMgr.players, &pMgr.playersMtx);

            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            //BitBlt(hdc, 0, 0, g_MemDC_Rect.right, g_MemDC_Rect.bottom, g_hMemDC, 0, 0, SRCCOPY);
            BitBlt(hdc, 0, 0, grid_MemDC_Rect.right, grid_MemDC_Rect.bottom, grid_hMemDC, 0, 0, SRCCOPY);
            EndPaint(hWnd, &ps);

        }
        break;
    case WM_LBUTTONDOWN:
    {
        int x = LOWORD(lParam);
        int y = HIWORD(lParam);
        gGrid.SelectPlayer(x, y, pMgr.players);
    }
    break;
    case WM_MOUSEWHEEL:
    {
        POINT cursorPos;
        GetCursorPos(&cursorPos);
        ScreenToClient(hWnd, &cursorPos);

        int delta = GET_WHEEL_DELTA_WPARAM(wParam);
        // 휠을 위로 굴리면 격자 확대
        if (delta > 0) { gGrid.cellSize = 100 < gGrid.cellSize + 5 ? 100 : gGrid.cellSize + 5; }
        // 휠을 아래로 굴리면 격자 축소 (최소 크기는 5)
        else { gGrid.cellSize = 5 > gGrid.cellSize - 5 ? 5 : gGrid.cellSize - 5; }

        // 윈도우를 다시 그리도록 강제
        //UpdateGridBackground(hWnd);
        InvalidateRect(hWnd, NULL, TRUE);
    }
    break;
    case WM_KEYDOWN:
    {
        bool flag = false;
        switch (wParam) {
        case 'W':
            flag = true;
            gGrid.MoveUp();
            break;
        case 'w':
            flag = true;
            gGrid.MoveUp();
            break;
        case 'A':
            flag = true;
            gGrid.MoveLeft();
            break;
        case 'a':
            flag = true;
            gGrid.MoveLeft();
            break;
        case 'S':
            flag = true;
            gGrid.MoveDown();
            break;
        case 's':
            flag = true;
            gGrid.MoveDown();
            break;
        case 'D':
            flag = true;
            gGrid.MoveRight();
            break;
        case 'd':
            flag = true;
            gGrid.MoveRight();
            break;
        }

        if (flag) {
            // 윈도우를 다시 그리도록 강제
            //UpdateGridBackground(hWnd);
            InvalidateRect(hWnd, NULL, TRUE);
        }
    }
    break;
    case WM_SIZE:
    {
        //if (g_hMemDC_Bitmap) {
        //    DeleteObject(g_hMemDC_Bitmap);
        //}
        //if (g_hMemDC) {
        //    DeleteObject(g_hMemDC);
        //}
        if (grid_hMemDC_Bitmap) {
            DeleteObject(grid_hMemDC_Bitmap);
        }
        if (grid_hMemDC) {
            DeleteObject(grid_hMemDC);
        }

        HDC hdc = GetDC(hWnd);
        //GetClientRect(hWnd, &g_MemDC_Rect);
        GetClientRect(hWnd, &grid_MemDC_Rect);

        //g_hMemDC_Bitmap = CreateCompatibleBitmap(hdc, g_MemDC_Rect.right, g_MemDC_Rect.bottom);
        //g_hMemDC = CreateCompatibleDC(hdc);
        //g_hMemDC_BitmapOld = (HBITMAP)SelectObject(g_hMemDC, g_hMemDC_Bitmap);

        grid_hMemDC_Bitmap = CreateCompatibleBitmap(hdc, grid_MemDC_Rect.right, grid_MemDC_Rect.bottom);
        grid_hMemDC = CreateCompatibleDC(hdc);
        grid_hMemDC_BitmapOld = (HBITMAP)SelectObject(grid_hMemDC, grid_hMemDC_BitmapOld);

        ReleaseDC(hWnd, hdc);

        //UpdateGridBackground(hWnd);
        InvalidateRect(hWnd, NULL, TRUE);
        return TRUE;
    }
    break;
    case WM_DESTROY:
        servCapture.stopCapture();
        pMgr.StopCapture();

        //if (g_hMemDC_Bitmap) {
        //    DeleteObject(g_hMemDC_Bitmap);
        //}
        //if (g_hMemDC) {
        //    DeleteObject(g_hMemDC);
        //}
        //if (grid_hMemDC_Bitmap) {
        //    DeleteObject(grid_hMemDC_Bitmap);
        //}
        //if (grid_hMemDC_Bitmap) {
        //    DeleteObject(grid_hMemDC_Bitmap);
        //}

        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
