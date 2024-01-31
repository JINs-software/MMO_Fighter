#include "DialogBox.h"
#include "resource.h"
#include <string>

bool MyDialogClass::progressFlag = false;
bool MyDialogClass::loopBackMode = false;
TCHAR MyDialogClass::ipAddress[16] = { 0 };
TCHAR MyDialogClass::port[6] = { 0 };
unsigned short MyDialogClass::usPort = 0;

HFONT hNormalFont = CreateFont(
    -12,                    // 폰트 높이
    0,                      // 문자의 폭 (기본값)
    0,                      // 텍스트의 기울기 각도
    0,                      // 텍스트의 기울기 각도
    FW_NORMAL,              // 폰트의 굵기
    FALSE,                   // 이탤릭체 사용 여부
    FALSE,                  // 밑줄 사용 여부
    FALSE,                  // 취소선 사용 여부
    DEFAULT_CHARSET,        // 문자 집합
    OUT_OUTLINE_PRECIS,     // 출력 정밀도
    CLIP_DEFAULT_PRECIS,    // 클리핑 정밀도
    CLEARTYPE_QUALITY,      // 출력 품질
    DEFAULT_PITCH | FF_ROMAN,  // 기본 글꼴 및 글꼴 패밀리
    L"Arial"                // 폰트 이름
);
HFONT hCustomFont = CreateFont(
    -12,                    // 폰트 높이
    0,                      // 문자의 폭 (기본값)
    0,                      // 텍스트의 기울기 각도
    0,                      // 텍스트의 기울기 각도
    FW_THIN,              // 폰트의 굵기
    TRUE,                   // 이탤릭체 사용 여부
    FALSE,                  // 밑줄 사용 여부
    FALSE,                  // 취소선 사용 여부
    DEFAULT_CHARSET,        // 문자 집합
    OUT_OUTLINE_PRECIS,     // 출력 정밀도
    CLIP_DEFAULT_PRECIS,    // 클리핑 정밀도
    CLEARTYPE_QUALITY,      // 출력 품질
    DEFAULT_PITCH | FF_ROMAN,  // 기본 글꼴 및 글꼴 패밀리
    L"Arial"                // 폰트 이름
);

INT_PTR CALLBACK MyDialogClass::DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        // 메시지 처리
    case WM_INITDIALOG:
    {
        MyDialogClass::loopBackMode = false;
        //hPrevFont = (HFONT)SendDlgItemMessage(hwndDlg, IDC_EDIT1, WM_GETFONT, 0, 0);
        //hNormalFont = CreateFont(
        //    -12,                    // 폰트 높이
        //    0,                      // 문자의 폭 (기본값)
        //    0,                      // 텍스트의 기울기 각도
        //    0,                      // 텍스트의 기울기 각도
        //    FW_NORMAL,              // 폰트의 굵기
        //    FALSE,                   // 이탤릭체 사용 여부
        //    FALSE,                  // 밑줄 사용 여부
        //    FALSE,                  // 취소선 사용 여부
        //    DEFAULT_CHARSET,        // 문자 집합
        //    OUT_OUTLINE_PRECIS,     // 출력 정밀도
        //    CLIP_DEFAULT_PRECIS,    // 클리핑 정밀도
        //    CLEARTYPE_QUALITY,      // 출력 품질
        //    DEFAULT_PITCH | FF_ROMAN,  // 기본 글꼴 및 글꼴 패밀리
        //    L"Arial"                // 폰트 이름
        //);
        //hCustomFont = CreateFont(
        //    -12,                    // 폰트 높이
        //    0,                      // 문자의 폭 (기본값)
        //    0,                      // 텍스트의 기울기 각도
        //    0,                      // 텍스트의 기울기 각도
        //    FW_THIN,              // 폰트의 굵기
        //    TRUE,                   // 이탤릭체 사용 여부
        //    FALSE,                  // 밑줄 사용 여부
        //    FALSE,                  // 취소선 사용 여부
        //    DEFAULT_CHARSET,        // 문자 집합
        //    OUT_OUTLINE_PRECIS,     // 출력 정밀도
        //    CLIP_DEFAULT_PRECIS,    // 클리핑 정밀도
        //    CLEARTYPE_QUALITY,      // 출력 품질
        //    DEFAULT_PITCH | FF_ROMAN,  // 기본 글꼴 및 글꼴 패밀리
        //    L"Arial"                // 폰트 이름
        //);
        // 에디트 컨트롤에 이탤릭 폰트를 설정합니다.
        SendDlgItemMessage(hwndDlg, IDC_EDIT1, WM_SETFONT, (WPARAM)hCustomFont, TRUE);
        SetDlgItemText(hwndDlg, IDC_EDIT1, L"ex) 192.168.30.150");

        SendDlgItemMessage(hwndDlg, IDC_EDIT2, WM_SETFONT, (WPARAM)hCustomFont, TRUE);
        SetDlgItemText(hwndDlg, IDC_EDIT2, L"ex) 20000");
    }
        return TRUE; // 포커스를 컨트롤에 설정하지 않음
    case WM_DESTROY:
    {
        if (hCustomFont != NULL) {
            DeleteObject(hCustomFont);
        }
        if (hNormalFont != NULL) {
            DeleteObject(hNormalFont);
        }
    }
    break;
    case WM_COMMAND:
        if (LOWORD(wParam) == IDC_EDIT1 && HIWORD(wParam) == EN_SETFOCUS)
        {
            // 에디트 컨트롤이 포커스를 받았을 때, 텍스트를 삭제합니다.
            SendDlgItemMessage(hwndDlg, IDC_EDIT1, WM_SETFONT, (WPARAM)hNormalFont, TRUE);
            SetDlgItemText(hwndDlg, IDC_EDIT1, L"");
            break;
        }
        if (LOWORD(wParam) == IDC_EDIT2 && HIWORD(wParam) == EN_SETFOCUS)
        {
            // 에디트 컨트롤이 포커스를 받았을 때, 텍스트를 삭제합니다.
            SendDlgItemMessage(hwndDlg, IDC_EDIT2, WM_SETFONT, (WPARAM)hNormalFont, TRUE);
            SetDlgItemText(hwndDlg, IDC_EDIT2, L"");
            break;
        }

        int wmId = LOWORD(wParam);
        switch (wmId) {
        case IDOK:
        {
            MyDialogClass::progressFlag = true;
            if (!MyDialogClass::loopBackMode) {
                GetDlgItemText(hwndDlg, IDC_EDIT1, MyDialogClass::ipAddress, 16);
            }
            GetDlgItemText(hwndDlg, IDC_EDIT2, MyDialogClass::port, 6);
            std::wstring str(MyDialogClass::port);
            MyDialogClass::usPort = static_cast<unsigned short>(std::stoi(str));
            EndDialog(hwndDlg, LOWORD(wParam));
        }
            return true;
        case IDCANCEL:
            MyDialogClass::progressFlag = false;
            EndDialog(hwndDlg, LOWORD(wParam));
            return true;
        case IDC_CHECK1: {
            // 체크박스 상태 변경 확인
            if (IsDlgButtonChecked(hwndDlg, IDC_CHECK1) == BST_CHECKED) {
                MyDialogClass::loopBackMode = true;
                // 체크된 경우, 에디트 창에 "127.0.0.1" 입력 및 비활성화
                SetDlgItemText(hwndDlg, IDC_EDIT1, L"127.0.0.1");
                EnableWindow(GetDlgItem(hwndDlg, IDC_EDIT1), FALSE);
            }
            else {
                MyDialogClass::loopBackMode = false;
                // 체크가 해제된 경우, 에디트 창 활성화
                SetDlgItemText(hwndDlg, IDC_EDIT1, L"");
                //SendDlgItemMessage(hwndDlg, IDC_EDIT1, WM_SETFONT, (WPARAM)hCustomFont, TRUE);
                //SetDlgItemText(hwndDlg, IDC_EDIT1, L"ex) 192.168.30.150");
                EnableWindow(GetDlgItem(hwndDlg, IDC_EDIT1), TRUE);
            }
            return TRUE;
        }
                       // 여기서 다른 컨트롤의 처리를 할 수 있습니다.
        }
        break;
    }
    return FALSE;
}
