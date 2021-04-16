// Save_Text_Dialog.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "MFCProj2.h"
#include "Save_Text_Dialog.h"
#include "afxdialogex.h"

#include "Wolf\Wolf_Text.h"


// Save_Text_Dialog ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(Save_Text_Dialog, CDialogEx)

Save_Text_Dialog::Save_Text_Dialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(Save_Text_Dialog::IDD, pParent)
{

}

Save_Text_Dialog::~Save_Text_Dialog()
{
}

void Save_Text_Dialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS1, m_Save_Text_Progress);
}


BEGIN_MESSAGE_MAP(Save_Text_Dialog, CDialogEx)
END_MESSAGE_MAP()


// Save_Text_Dialog �޽��� ó�����Դϴ�.


BOOL Save_Text_Dialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

	m_Save_Text_Progress.SetRange32 (0, 100);
	//*���α׷��� �ʱ�ȭ

	Save_Text_Thread = AfxBeginThread (Save_Text_Func, this);
	//*���α׷��� ������ ����

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}


BOOL Save_Text_Dialog::PreTranslateMessage(MSG* pMsg)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	
	if ((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_ESCAPE)) { return true; }
	if ((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_RETURN)) { return true; }
	//*Enter, ESC�� ����ȭ��Ų��

	if (m_Save_Text_Progress.GetPos() == 100) { ::SendMessage (m_hWnd, WM_CLOSE, NULL, NULL); }

	return CDialogEx::PreTranslateMessage(pMsg);
}


UINT Save_Text_Dialog::Save_Text_Func (LPVOID lParam)
{
	Save_Text_Dialog *dlg = (Save_Text_Dialog*)lParam;
	//*��ü ���

	CString Dir_Path = dlg->m_FP_tmp;
	//*���丮�� ���

	if (Save_Text_To_Directory_Data (dlg, Dir_Path)== -1) { 
		dlg->m_Save_Text_Progress.SetPos (100);
	}
	//*���� �Լ� ȣ��

//	AfxMessageBox (_T("�ؽ�Ʈ ���� �Ϸ�!"));

	return 0;
}