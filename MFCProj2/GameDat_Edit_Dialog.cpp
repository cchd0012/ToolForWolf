// GameDat_Edit_Dialog.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "MFCProj2.h"
#include "GameDat_Edit_Dialog.h"
#include "afxdialogex.h"

#include "Wolf\Wolf_Functions.h"

extern TCHAR *CodePageMenu[];

// GameDat_Edit_Dialog ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(GameDat_Edit_Dialog, CDialogEx)

GameDat_Edit_Dialog::GameDat_Edit_Dialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(GameDat_Edit_Dialog::IDD, pParent)
{

}

GameDat_Edit_Dialog::~GameDat_Edit_Dialog()
{
}

void GameDat_Edit_Dialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_GD_EDIT1, m_Game_Title);
	DDX_Control(pDX, IDC_GD_EDIT2, m_Font1);
	DDX_Control(pDX, IDC_GD_EDIT3, m_Font2);
	DDX_Control(pDX, IDC_GD_EDIT4, m_Font3);
	DDX_Control(pDX, IDC_GD_EDIT5, m_Font4);
	DDX_Control(pDX, IDC_GD_RADIO1, m_GD_CPRadio1);
	DDX_Control(pDX, IDC_GD_RADIO2, m_GD_CPRadio2);
}


BEGIN_MESSAGE_MAP(GameDat_Edit_Dialog, CDialogEx)
END_MESSAGE_MAP()


// GameDat_Edit_Dialog �޽��� ó�����Դϴ�.


BOOL GameDat_Edit_Dialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetDlgItemText (IDC_GD_EDIT1, Game_Title);
	SetDlgItemText (IDC_GD_EDIT2, Font1);
	SetDlgItemText (IDC_GD_EDIT3, Font2);
	SetDlgItemText (IDC_GD_EDIT4, Font3);
	SetDlgItemText (IDC_GD_EDIT5, Font4);
	
	SetDlgItemText (IDC_GD_RADIO1, CodePageMenu[KOR_CODE]);
	SetDlgItemText (IDC_GD_RADIO2, CodePageMenu[JAP_CODE]);
	//*�ؽ�Ʈ ����
	
	m_GD_CPRadio1.SetCheck(BST_CHECKED);
	m_GD_CPRadio2.SetCheck(BST_UNCHECKED);
	//*���� ����

	if (type == DXA_330) { SetWindowText (_T("Edit 'Game.dat' (type 2/3)")); }
	else { SetWindowText (_T("Edit 'Game.dat' (type 1)")); }
	//*ĸ�� ����

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}


void GameDat_Edit_Dialog::OnOK()
{

	GetDlgItemText (IDC_GD_EDIT1, Game_Title);
	GetDlgItemText (IDC_GD_EDIT2, Font1);
	GetDlgItemText (IDC_GD_EDIT3, Font2);
	GetDlgItemText (IDC_GD_EDIT4, Font3);
	GetDlgItemText (IDC_GD_EDIT5, Font4);
	//*���ڿ� ���

	if (m_GD_CPRadio1.GetCheck() == BST_CHECKED) { code = KOR_CODE; }
	else if (m_GD_CPRadio2.GetCheck() == BST_CHECKED) { code = JAP_CODE; }
	//*��ư Ŭ������ ���

	if ((Game_Title.GetLength() == 0) || (Font1.GetLength() == 0)) {
		AfxMessageBox (_T("���� Ÿ��Ʋ Ȥ�� ù ��° ��Ʈ�� ��������� �� �˴ϴ�."));
		return;
	}
	//*���� Ÿ��Ʋ�̳� ù��° ��Ʈ�� ��������� �� �ȴ�.

	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

	CDialogEx::OnOK();
}
