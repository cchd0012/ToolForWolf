// Check_FDName_Dialog.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "MFCProj2.h"
#include "Check_FDName_Dialog.h"
#include "afxdialogex.h"

#include "Wolf\Wolf_Text.h"


// Check_FDName_Dialog ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(Check_FDName_Dialog, CDialogEx)

Check_FDName_Dialog::Check_FDName_Dialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(Check_FDName_Dialog::IDD, pParent)
{

}

Check_FDName_Dialog::~Check_FDName_Dialog()
{
}

void Check_FDName_Dialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHK_FD_TEXT, m_Check_FDName_Print);
	DDX_Control(pDX, IDC_CHK_FD_PROGRESS, m_Check_FDName_Progress);
}


BEGIN_MESSAGE_MAP(Check_FDName_Dialog, CDialogEx)
END_MESSAGE_MAP()


// Check_FDName_Dialog �޽��� ó�����Դϴ�.


BOOL Check_FDName_Dialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

	m_Check_FDName_Progress.SetRange32 (0, 100);
	//*���α׷��� �ʱ�ȭ

	Save_Text_Thread = AfxBeginThread (Check_FDName_Func, this);
	//*���α׷��� ������ ����

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}


BOOL Check_FDName_Dialog::PreTranslateMessage(MSG* pMsg)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	
	if ((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_ESCAPE)) { return true; }
	if ((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_RETURN)) { return true; }
	//*Enter, ESC�� ����ȭ��Ų��

	if (m_Check_FDName_Progress.GetPos() == 100) { ::SendMessage (m_hWnd, WM_CLOSE, NULL, NULL); }

	return CDialogEx::PreTranslateMessage(pMsg);
}


UINT Check_FDName_Dialog::Check_FDName_Func (LPVOID lParam)
{
	Check_FDName_Dialog *dlg = (Check_FDName_Dialog*)lParam;
	//*��ü ���

	Check_Filename_For_Codepage (dlg, dlg->code);
	//*üũ �Լ� ȣ��

	AfxMessageBox (_T("����/������ üũ �Ϸ�!"));

	return 0;
}