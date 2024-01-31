#include "DialogBox.h"
#include "resource.h"
#include <string>

bool MyDialogClass::progressFlag = false;
bool MyDialogClass::loopBackMode = false;
TCHAR MyDialogClass::ipAddress[16] = { 0 };
TCHAR MyDialogClass::port[6] = { 0 };
unsigned short MyDialogClass::usPort = 0;

HFONT hNormalFont = CreateFont(
    -12,                    // ��Ʈ ����
    0,                      // ������ �� (�⺻��)
    0,                      // �ؽ�Ʈ�� ���� ����
    0,                      // �ؽ�Ʈ�� ���� ����
    FW_NORMAL,              // ��Ʈ�� ����
    FALSE,                   // ���Ÿ�ü ��� ����
    FALSE,                  // ���� ��� ����
    FALSE,                  // ��Ҽ� ��� ����
    DEFAULT_CHARSET,        // ���� ����
    OUT_OUTLINE_PRECIS,     // ��� ���е�
    CLIP_DEFAULT_PRECIS,    // Ŭ���� ���е�
    CLEARTYPE_QUALITY,      // ��� ǰ��
    DEFAULT_PITCH | FF_ROMAN,  // �⺻ �۲� �� �۲� �йи�
    L"Arial"                // ��Ʈ �̸�
);
HFONT hCustomFont = CreateFont(
    -12,                    // ��Ʈ ����
    0,                      // ������ �� (�⺻��)
    0,                      // �ؽ�Ʈ�� ���� ����
    0,                      // �ؽ�Ʈ�� ���� ����
    FW_THIN,              // ��Ʈ�� ����
    TRUE,                   // ���Ÿ�ü ��� ����
    FALSE,                  // ���� ��� ����
    FALSE,                  // ��Ҽ� ��� ����
    DEFAULT_CHARSET,        // ���� ����
    OUT_OUTLINE_PRECIS,     // ��� ���е�
    CLIP_DEFAULT_PRECIS,    // Ŭ���� ���е�
    CLEARTYPE_QUALITY,      // ��� ǰ��
    DEFAULT_PITCH | FF_ROMAN,  // �⺻ �۲� �� �۲� �йи�
    L"Arial"                // ��Ʈ �̸�
);

INT_PTR CALLBACK MyDialogClass::DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        // �޽��� ó��
    case WM_INITDIALOG:
    {
        MyDialogClass::loopBackMode = false;
        //hPrevFont = (HFONT)SendDlgItemMessage(hwndDlg, IDC_EDIT1, WM_GETFONT, 0, 0);
        //hNormalFont = CreateFont(
        //    -12,                    // ��Ʈ ����
        //    0,                      // ������ �� (�⺻��)
        //    0,                      // �ؽ�Ʈ�� ���� ����
        //    0,                      // �ؽ�Ʈ�� ���� ����
        //    FW_NORMAL,              // ��Ʈ�� ����
        //    FALSE,                   // ���Ÿ�ü ��� ����
        //    FALSE,                  // ���� ��� ����
        //    FALSE,                  // ��Ҽ� ��� ����
        //    DEFAULT_CHARSET,        // ���� ����
        //    OUT_OUTLINE_PRECIS,     // ��� ���е�
        //    CLIP_DEFAULT_PRECIS,    // Ŭ���� ���е�
        //    CLEARTYPE_QUALITY,      // ��� ǰ��
        //    DEFAULT_PITCH | FF_ROMAN,  // �⺻ �۲� �� �۲� �йи�
        //    L"Arial"                // ��Ʈ �̸�
        //);
        //hCustomFont = CreateFont(
        //    -12,                    // ��Ʈ ����
        //    0,                      // ������ �� (�⺻��)
        //    0,                      // �ؽ�Ʈ�� ���� ����
        //    0,                      // �ؽ�Ʈ�� ���� ����
        //    FW_THIN,              // ��Ʈ�� ����
        //    TRUE,                   // ���Ÿ�ü ��� ����
        //    FALSE,                  // ���� ��� ����
        //    FALSE,                  // ��Ҽ� ��� ����
        //    DEFAULT_CHARSET,        // ���� ����
        //    OUT_OUTLINE_PRECIS,     // ��� ���е�
        //    CLIP_DEFAULT_PRECIS,    // Ŭ���� ���е�
        //    CLEARTYPE_QUALITY,      // ��� ǰ��
        //    DEFAULT_PITCH | FF_ROMAN,  // �⺻ �۲� �� �۲� �йи�
        //    L"Arial"                // ��Ʈ �̸�
        //);
        // ����Ʈ ��Ʈ�ѿ� ���Ÿ� ��Ʈ�� �����մϴ�.
        SendDlgItemMessage(hwndDlg, IDC_EDIT1, WM_SETFONT, (WPARAM)hCustomFont, TRUE);
        SetDlgItemText(hwndDlg, IDC_EDIT1, L"ex) 192.168.30.150");

        SendDlgItemMessage(hwndDlg, IDC_EDIT2, WM_SETFONT, (WPARAM)hCustomFont, TRUE);
        SetDlgItemText(hwndDlg, IDC_EDIT2, L"ex) 20000");
    }
        return TRUE; // ��Ŀ���� ��Ʈ�ѿ� �������� ����
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
            // ����Ʈ ��Ʈ���� ��Ŀ���� �޾��� ��, �ؽ�Ʈ�� �����մϴ�.
            SendDlgItemMessage(hwndDlg, IDC_EDIT1, WM_SETFONT, (WPARAM)hNormalFont, TRUE);
            SetDlgItemText(hwndDlg, IDC_EDIT1, L"");
            break;
        }
        if (LOWORD(wParam) == IDC_EDIT2 && HIWORD(wParam) == EN_SETFOCUS)
        {
            // ����Ʈ ��Ʈ���� ��Ŀ���� �޾��� ��, �ؽ�Ʈ�� �����մϴ�.
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
            // üũ�ڽ� ���� ���� Ȯ��
            if (IsDlgButtonChecked(hwndDlg, IDC_CHECK1) == BST_CHECKED) {
                MyDialogClass::loopBackMode = true;
                // üũ�� ���, ����Ʈ â�� "127.0.0.1" �Է� �� ��Ȱ��ȭ
                SetDlgItemText(hwndDlg, IDC_EDIT1, L"127.0.0.1");
                EnableWindow(GetDlgItem(hwndDlg, IDC_EDIT1), FALSE);
            }
            else {
                MyDialogClass::loopBackMode = false;
                // üũ�� ������ ���, ����Ʈ â Ȱ��ȭ
                SetDlgItemText(hwndDlg, IDC_EDIT1, L"");
                //SendDlgItemMessage(hwndDlg, IDC_EDIT1, WM_SETFONT, (WPARAM)hCustomFont, TRUE);
                //SetDlgItemText(hwndDlg, IDC_EDIT1, L"ex) 192.168.30.150");
                EnableWindow(GetDlgItem(hwndDlg, IDC_EDIT1), TRUE);
            }
            return TRUE;
        }
                       // ���⼭ �ٸ� ��Ʈ���� ó���� �� �� �ֽ��ϴ�.
        }
        break;
    }
    return FALSE;
}
