
// MFCProj2Dlg.cpp : ���� ����
//
#include <errno.h>

#include "stdafx.h"
#include "MFCProj2.h"
#include "MFCProj2Dlg.h"
#include "afxdialogex.h"

#include "Autotrans_Papago_Dialog.h"

#include "Replace_Text_Dialog.h"
#include "File_Or_Text_Dialog.h"
#include "Find_Text_Dialog.h"

#include "Extract_Dialog.h"
#include "Pack_Dialog.h"

#include "Load_Text_Dialog.h"
#include "Save_Text_Dialog.h"
#include "Select_CP_Dialog.h"
#include "Edit_Text_Dialog.h"
#include "Check_FDName_Dialog.h"

#include "Import_List_Dialog.h"

#include "Wolf\Wolf_Functions.h"
#include "Wolf\Wolf_Text.h"
#include "Wolf\Wolf_ETC.h"
#include "Wolf\Wolf_Auto_Papago_Translate.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


TCHAR *CodePageMenu[] = {_T("CP932(jap)"), _T("CP949(kor)")};
//*�ڵ������� ���� �޴��� ���� ���ڿ��� ����ڵ�

TCHAR *Prefix_Text_Str[] = {_T(" �� (Jap)"), _T(" �� (Kor)")};
unsigned int Prefix_Length;
//*�ؽ�Ʈ Prefix�� ���ڿ��� ����

TCHAR *T_Head_Start = _T("[--Text in ");
TCHAR *T_Head_End = _T("--]");
//*��� �ؽ�Ʈ�� ���ڿ�

TCHAR *T_Head_Dirty = _T("(*)");
unsigned int Dirty_Prefix_Length;
//*Dirty �ؽ�Ʈ�� ���ڿ��� ����

TCHAR *Copy_Text_Str[] = {_T("Jap"), _T("Kor")};
unsigned int Copy_Length;
//*Copy�� ���ڿ��� ����

unsigned int Find_Direction;
CString Find_Str;
//*���ڿ��� ã�� �� �� �� ã�Ҵ� ����� ������ �����ϱ� ���� ����

unsigned int Clicked_Idx;

CString Wolf_File_ListName;
CString Wolf_File_List;
//*����Ʈ �̸�, ����Ʈ ������ ����ϴ� �� ���� CString
//*����Ʈ�� ������� Export List�� ��Ȱ��ȭ�� ��Ų��

CString Dir_That_Has_Text_Data;
//*�ؽ�Ʈ �����͸� ���� ���� �̸�

CListCtrl *m_G_ListCtrl_Files;
CListCtrl *m_G_ListCtrl_Text;
//*���� �̸�, �ؽ�Ʈ ����Ʈ

DIR_TXT_IDX m_Text_Idx_In_Dir;
//*�� ���丮 ���� �ִ� ��� �ؽ�Ʈ �����͸� �Ѱ��ϴ� ����

wchar_t N_table [0x3D] = {
	0x3002, 0x300C, 0x300D, 0x3001, 0x30FB, 0x30F2, 0x30A1, 0x30A3,
	0x30A5, 0x30A7, 0x30A9, 0x30E3, 0x30E5, 0x30E7, 0x30C3, 0x30FC,
	0x30A2, 0x30A4, 0x30A6, 0x30A8, 0x30AA, 0x30AB, 0x30AD, 0x30AF,
	0x30B1, 0x30B3, 0x30B5, 0x30B7, 0x30B9, 0x30BB, 0x30BD, 0x30BF,
	0x30C1, 0x30C4, 0x30C6, 0x30C8, 0x30CA, 0x30CB, 0x30CC, 0x30CD,
	0x30CE, 0x30CF, 0x30D2, 0x30D5, 0x30D8, 0x30DB, 0x30DE, 0x30DF,
	0x30E0, 0x30E1, 0x30E2, 0x30E4, 0x30E6, 0x30E8, 0x30E9, 0x30EA,
	0x30EB, 0x30EC, 0x30ED, 0x30EF, 0x30F3
};
//*�ݰ� �Ϻ��� ���̺�, ������ 2���� ���� ���� �����Ű�� �� ���� ���� �����̴�


// ���� ���α׷� ������ ���Ǵ� CAboutDlg ��ȭ �����Դϴ�.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

// �����Դϴ�.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMFCProj2Dlg ��ȭ ����




CMFCProj2Dlg::CMFCProj2Dlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMFCProj2Dlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCProj2Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LISTNAME, m_OnList);
	DDX_Control(pDX, IDC_LIST1, m_ListCtrl_Files);
	DDX_Control(pDX, IDC_LIST2, m_ListCtrl_Text);
}

BEGIN_MESSAGE_MAP(CMFCProj2Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CMFCProj2Dlg::OnBnClicked_Extract_Wolf)
	ON_BN_CLICKED(IDC_BUTTON2, &CMFCProj2Dlg::OnBnClicked_Make_Wolf)
	ON_LBN_SELCHANGE(IDC_LIST2, &CMFCProj2Dlg::OnLbnSelchangeList2)
	ON_BN_CLICKED(IDC_BUTTON6, &CMFCProj2Dlg::OnBnClicked_Export_List)
	ON_BN_CLICKED(IDC_BUTTON5, &CMFCProj2Dlg::OnBnClicked_Import_List)
	ON_BN_CLICKED(IDC_BUTTON3, &CMFCProj2Dlg::OnBnClicked_Load_Text)
	ON_BN_CLICKED(IDC_BUTTON4, &CMFCProj2Dlg::OnBnClicked_Save_Text)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST1, &CMFCProj2Dlg::On_Files_List_Ctrl_NMCustomdraw)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST2, &CMFCProj2Dlg::On_Text_List_NMCustomdrawList)
	ON_NOTIFY(NM_CLICK, IDC_LIST1, &CMFCProj2Dlg::On_Files_List_NMClickList)
	ON_NOTIFY(NM_CLICK, IDC_LIST2, &CMFCProj2Dlg::On_Text_List_NMClickList)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST2, &CMFCProj2Dlg::On_Text_List_NMDblclkList)
	ON_BN_CLICKED(IDC_BUTTON7, &CMFCProj2Dlg::OnBnClicked_Prepare_To_Translate)
	ON_BN_CLICKED(IDC_BUTTON8, &CMFCProj2Dlg::OnBnClicked_Auto_Trans_Papago)
//	ON_BN_CLICKED(IDC_BUTTON8, &CMFCProj2Dlg::OnBnClicked_Narrow_to_Em)
//	ON_BN_CLICKED(IDC_BUTTON9, &CMFCProj2Dlg::OnBnClicked_Check_Filename)
	ON_BN_CLICKED(IDC_BUTTON10, &CMFCProj2Dlg::OnBnClicked_Edit_GameDat_File)
	ON_BN_CLICKED(IDC_BUTTON11, &CMFCProj2Dlg::OnBnClicked_Change_Exe_File)
END_MESSAGE_MAP()


// CMFCProj2Dlg �޽��� ó����

BOOL CMFCProj2Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// �ý��� �޴��� "����..." �޴� �׸��� �߰��մϴ�.

	// IDM_ABOUTBOX�� �ý��� ��� ������ �־�� �մϴ�.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// �� ��ȭ ������ �������� �����մϴ�. ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	//  �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

	CRect cr;

	Wolf_File_ListName = _T("");
	Wolf_File_List.Empty();
	SetDlgItemText(IDC_LISTNAME, _T("--No List--"));
	//*���� ���� ����Ʈ �̸�, ���� �ʱ�ȭ
	
	GetDlgItem(IDC_BUTTON2)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON4)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON6)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON7)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON8)->EnableWindow(FALSE);
//	GetDlgItem(IDC_BUTTON9)->EnableWindow(FALSE);
	//*Make wolf file, Load text from data�� ������ �ؽ�Ʈ ����, export list ��ư ��Ȱ��ȭ.
	//*ó������ �翬�� �� ����� ����.

	m_ListCtrl_Files.GetWindowRect (&cr);
	m_ListCtrl_Files.SetExtendedStyle (LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_ListCtrl_Files.InsertColumn (0, _T("Files"), LVCFMT_CENTER, cr.Width() - GetSystemMetrics(SM_CXFRAME)/2, -1);
	m_ListCtrl_Files.ModifyStyle (0, LVS_NOCOLUMNHEADER | LVS_SHOWSELALWAYS, TRUE);
	//*���� ����Ʈ ��Ÿ�� ���� �� �ʱ�ȭ(����� ���� �巯�� �ʿ� ������ �����)
	//*���� ����Ʈ ���� ���� : ���� �־��� ���(��Ȯ�� ���߱�)

	m_ListCtrl_Text.GetWindowRect (&cr);
	m_ListCtrl_Text.SetExtendedStyle (LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_ListCtrl_Text.InsertColumn (0, _T("Text"), LVCFMT_CENTER, cr.Width() - GetSystemMetrics(SM_CXFRAME)/2, -1);
	m_ListCtrl_Text.ModifyStyle (0, LVS_NOCOLUMNHEADER | LVS_SHOWSELALWAYS, TRUE);
	//*�ؽ�Ʈ ��Ÿ�� ���� �� �ʱ�ȭ(����� ���� �巯�� �ʿ� ������ �����)
	//*�ؽ�Ʈ ����Ʈ ���� ���� : ���� �־��� ���(��Ȯ�� ���߱�)

	memset (&m_Text_Idx_In_Dir, 0, sizeof(DIR_TXT_IDX));
	//*�ؽ�Ʈ �Ѱ� ���� �ʱ�ȭ

	CFont* pFont = GetFont();
	if (pFont){
		LOGFONT logfont;
		pFont->GetLogFont(&logfont);
		logfont.lfWeight = FW_BOLD;
		m_fontBold.CreateFontIndirect(&logfont);
	}
//	else { m_fontBold = *pFont; }
	//*���� ��Ʈ ����

	Prefix_Length = (unsigned int)wcslen(Prefix_Text_Str[0]) + 8;
	Dirty_Prefix_Length = (unsigned int)wcslen(T_Head_Dirty);
	Copy_Length = (unsigned int)wcslen(Copy_Text_Str[0]);
	//Prefix_Length ����, +8�� ���� ������ "[%06d]"�� ���� �����̴�

	Load_Hanja();
	//*���� �ؽ�Ʈ �ҷ�����

	Find_Direction = NON_DIRECTION;
	Find_Str = _T("");
	Clicked_Idx = 0xFFFFFFFF;
	//*����Ű ����, ���� �ʱ�ȭ

	translateInit();
	//***�����غ� ���� �ʱ�ȭ

	// TODO: ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
}


void CMFCProj2Dlg::OnCancel()
{

	if (m_Text_Idx_In_Dir.Is_Dirty) {
		int ret = AfxMessageBox (_T("���������� ������� ���� �ؽ�Ʈ�� �ֽ��ϴ�.\n�̴�� �����ڽ��ϱ�?"), MB_YESNO);
		if (ret == IDNO) { return; }
	}
	//***Dirty�� �ؽ�Ʈ�� �ִٸ� �� �ų� �� �� �ų� �����

	All_Text_Data_Reset();
	//*����Ʈ ������ ����, �Ҵ��ߴ� m_Text_Idx_In_Dir ������ �ִ� ���� �����ϰ� ������ ��
	//*Load_Text_Dialog.h ���Ͽ� �����

	Release_Hanja();
	//*���� �ؽ�Ʈ �����ϱ�

	translateTerminate();
	//*���� ���� ��� ����

	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

	CDialogEx::OnCancel();
}


void CMFCProj2Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸�����
//  �Ʒ� �ڵ尡 �ʿ��մϴ�. ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
//  �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CMFCProj2Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ�Դϴ�.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �������� �׸��ϴ�.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}


// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ���
//  �� �Լ��� ȣ���մϴ�.
HCURSOR CMFCProj2Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


BOOL CMFCProj2Dlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

	if ((pMsg->message == WM_KEYDOWN) && 
		((pMsg->wParam == VK_ESCAPE) || (pMsg->wParam == VK_RETURN))) { return true; }
	//*���� �� ESC ������ �� ������ �� �� �ִ�.

	if ((pMsg->hwnd == m_ListCtrl_Text) && (m_Text_Idx_In_Dir.File_Num != 0)) {
		if (pMsg->message == WM_KEYDOWN) {

			if (GetKeyState(VK_CONTROL) && (pMsg->wParam == 0x43)) {
				Collect_Clipboard_String_From_List(); return true;
			}
			//*Ctrl + c(�ؽ�Ʈ ����)

			if (GetKeyState(VK_F3) && (pMsg->wParam == 0x72)) {
				Find_String (Find_Direction, Find_Str); return true;
			}
			else if (GetKeyState(VK_CONTROL) && (pMsg->wParam == 0x46)) {
				Find_String (NON_DIRECTION, _T("")); return true;
			}
			//*F3(Ctrl + f�� ����ϳ� ����� ������ �����Ǿ� �ִٸ� �״�� �����Ѵ�)
			//*Ctrl + f(Ư�� �ܾ ������ �ؽ�Ʈ ã��, ����, ���� �ʱ�ȭ)

			if (GetKeyState(VK_CONTROL) && (pMsg->wParam == 0x52)) {
				Replace_String(); return true;
			}
			//*Ctrl + r(Ư�� �ܾ �ٸ� �ܾ�� ��ü, ���� ���ڿ��� ����)

			if (GetKeyState(VK_CONTROL) && (pMsg->wParam == 0x56)) {
				Set_List_String_From_Clipboard(); return true;
			}
			//*Ctrl + v(�ؽ�Ʈ ����Ʈ�� �ٿ��ֱ�)

		}
	}
	//*(����/�ٿ��ֱ⸦ ���� ��) �ؽ�Ʈ ����Ʈ ��Ʈ�ѿ� ��Ŀ���� �־����� ����Ʈ�� �ε���� ��

	return CDialogEx::PreTranslateMessage(pMsg);
}








void CMFCProj2Dlg::OnBnClicked_Extract_Wolf()
{
	//***Extract Wolf File***

	Extract_Dialog dlg;
	dlg.DoModal();
	//*���� ���� ���̾�α� ȣ��
	//*�ش� ���̾�α׿��� Ȯ�� ��ư�� ������ IDOK ���� ���ƿ´�.

	if (!Wolf_File_List.IsEmpty()) { 
		GetDlgItem(IDC_BUTTON2)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON6)->EnableWindow(TRUE); 
		SetDlgItemText(IDC_LISTNAME, _T("List : ") + Wolf_File_ListName);
	}
	//*���� �� ������ ��ġ�� ����Ʈ ������ ����Ʈ���� ���Դٸ� export list ��ư Ȱ��ȭ

	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}


void CMFCProj2Dlg::OnBnClicked_Make_Wolf()
{
	//***Make Wolf File***

	if (Wolf_File_List.IsEmpty()) { 
		AfxMessageBox (_T("�ռ��� ������ ����Ʈ�� �����ϴ�."));
		return;
	}
	//*���� ����Ʈ ������ ����Ʈ���� �� ���Դٸ� ���� �Ұ��ϵ��� �Ѵ�.
	
	Pack_Dialog dlg;
	dlg.DoModal();
	//*���� ���� ���̾�α� ȣ��
	//***�ش� ���̾�α׿��� Ȯ�� ��ư�� ������ IDOK ���� ���ƿ´�.

	if (Wolf_File_List.IsEmpty()) { 
		GetDlgItem(IDC_BUTTON6)->EnableWindow(FALSE); 
		SetDlgItemText(IDC_LISTNAME, _T("--No List--"));
	}
	//*���� �� ������ ��ġ�� ����Ʈ ������ ����ٸ� export list ��ư ��Ȱ��ȭ

	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}


void CMFCProj2Dlg::OnBnClicked_Change_Exe_File()
{
	//***Modify '.exe' File***
	
	int ret = AfxMessageBox (_T("�ش� ����� ������ Type 1/2������ �����۵��մϴ�.\nType 3������ ���� �ʴ� ���� �����մϴ�.\n��� �����ϰڽ��ϱ�?"), MB_YESNO);
	//*�޽��� ����

	if (ret == IDYES) {

		TCHAR szFilter[] = _T("Wolf Game exe File (Game.exe) |Game.exe|");
		CFileDialog dlg (TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, szFilter);

		while (dlg.DoModal() != IDOK) { AfxMessageBox (_T("Game.exe ������ �����ϼ���.")); }
		//*���� ���� ���̾�α�

		if (dlg.GetPathName().GetBuffer() != NULL) {
			Exe_Change (dlg.GetPathName().GetBuffer());
			AfxMessageBox (_T("Game_New.exe ������ ��������ϴ�."));
			//*���� ���� �������� �ѱ� ǥ�⸦ �� �� �ֵ��� �ٲپ� ���� ������ Game_New.exe ������ �����.
		}

	}

	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}








void CMFCProj2Dlg::OnBnClicked_Load_Text()
{
	//***Load Text From Data***

	//***���� �� �� �ؽ�Ʈ�� �ִٸ� �����ϰ� �����ų�, ���ư� �ų� ����� �����Ѵ�
	//***������ �Ŷ� �ϸ� Save Text To Data ��ư�� ������ ��ó�� ���� �� �� �����Ѵ�

	GetDlgItem(IDC_BUTTON4)->EnableWindow(FALSE); 
	GetDlgItem(IDC_BUTTON7)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON8)->EnableWindow(FALSE);
//	GetDlgItem(IDC_BUTTON9)->EnableWindow(FALSE);
	//*��Ÿ ��� ��Ȱ��ȭ

	m_G_ListCtrl_Files = &m_ListCtrl_Files;
	m_G_ListCtrl_Text = &m_ListCtrl_Text;
	//*����Ʈ �̸� ����

	Load_Text_Dialog dlg;
	dlg.DoModal();
	if (m_ListCtrl_Text.GetItemCount() != 0) { 
		GetDlgItem(IDC_BUTTON4)->EnableWindow(TRUE); 
		GetDlgItem(IDC_BUTTON7)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON8)->EnableWindow(TRUE);
//		GetDlgItem(IDC_BUTTON9)->EnableWindow(TRUE);
		Dir_That_Has_Text_Data = dlg.m_FP_tmp;
	}
	//*�ؽ�Ʈ �ε� ���̾�α� ȣ��, ���� �ε�Ǹ� ������ �ؽ�Ʈ ��ư Ȱ��ȭ, ������� �޾Ƴ���

	//*���ڿ� �ϰ� ��ȯ�Լ�(�׷��ϱ� replace �Լ�)�� �ְ� ���̾�α׷� ������.
	//*�ؽ�Ʈ ����Ʈ���� ���� ������ Ŭ���ϸ� �� �ȿ� ���Ե� ��� �ؽ�Ʈ�� Ŭ���ǵ��� �Ѵ�.
	//*���� ����Ʈ���� �ؽ�Ʈ�� �ִ� ������ Ŭ���ϸ� �ؽ�Ʈ ����Ʈ���� �ڵ����� �� ���� ������� �̵���Ų��.

	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}


void CMFCProj2Dlg::OnBnClicked_Save_Text()
{
	//***Save Text To Data***

	if (m_Text_Idx_In_Dir.Is_Dirty) {

		m_G_ListCtrl_Files = &m_ListCtrl_Files;
		m_G_ListCtrl_Text = &m_ListCtrl_Text;
		//*����Ʈ �̸� ����
		
		GetDlgItem(IDC_BUTTON3)->EnableWindow(FALSE); 
		GetDlgItem(IDC_BUTTON4)->EnableWindow(FALSE); 
		GetDlgItem(IDC_BUTTON7)->EnableWindow(FALSE); 
		GetDlgItem(IDC_BUTTON8)->EnableWindow(FALSE); 
//		GetDlgItem(IDC_BUTTON9)->EnableWindow(FALSE); 
		//*�̸� ��ư ��Ȱ��ȭ

		Save_Text_Dialog dlg; dlg.m_FP_tmp = Dir_That_Has_Text_Data;
		dlg.DoModal();
		//*Dirty ������ ���� ȣ��, ������� ����

		m_ListCtrl_Files.Invalidate();
		m_ListCtrl_Text.Invalidate();
		//*�ؽ�Ʈâ �ٽ� �׸���
		
		GetDlgItem(IDC_BUTTON3)->EnableWindow(TRUE); 
		GetDlgItem(IDC_BUTTON4)->EnableWindow(TRUE); 
		GetDlgItem(IDC_BUTTON7)->EnableWindow(TRUE); 
		GetDlgItem(IDC_BUTTON8)->EnableWindow(TRUE); 
//		GetDlgItem(IDC_BUTTON9)->EnableWindow(TRUE); 
		//*�ٽ� ��ư Ȱ��ȭ

	}

	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}


void CMFCProj2Dlg::OnBnClicked_Prepare_To_Translate()
{
	//***Prepare to Translate***
	
	GetDlgItem(IDC_BUTTON3)->EnableWindow(FALSE); 
	GetDlgItem(IDC_BUTTON4)->EnableWindow(FALSE); 
	GetDlgItem(IDC_BUTTON7)->EnableWindow(FALSE); 
	GetDlgItem(IDC_BUTTON8)->EnableWindow(FALSE); 
//	GetDlgItem(IDC_BUTTON9)->EnableWindow(FALSE); 
	//*���� �� �ε�/����/���� �Ұ�

	Select_CP_Dialog sdlg;
	int T_List_Idx;
	CString tmpWStr, apply_Txt;

	while (sdlg.DoModal() != IDOK) { ; }
	//*�ڵ������� ����

	m_Text_Idx_In_Dir.Is_Dirty = true;
	//*Dirty ���� ����

	for (unsigned int i = 0;i < m_Text_Idx_In_Dir.File_Num;i++) {
		
		T_List_Idx = Get_List_Index (i, 0);
		if (!m_Text_Idx_In_Dir.File_Idx_Arr[i].Is_FileTxt_Dirty) {
			T_List_Idx--;
			apply_Txt = T_Head_Dirty; apply_Txt += m_ListCtrl_Text.GetItemText (T_List_Idx, 0);
			m_ListCtrl_Text.SetItemText (T_List_Idx, 0, apply_Txt);
			m_Text_Idx_In_Dir.File_Idx_Arr[i].Is_FileTxt_Dirty = true;
			T_List_Idx++;
		}
		//*���� ����� Dirty ���·� �ٲٱ�

		for (unsigned int j = 0;j < m_Text_Idx_In_Dir.File_Idx_Arr[i].Text_Cnt;j++) {

			tmpWStr = m_ListCtrl_Text.GetItemText ((T_List_Idx + j), 0);
			if (m_Text_Idx_In_Dir.File_Idx_Arr[i].Text_Idx_Arr[j].Is_Text_Dirty) {
				tmpWStr = tmpWStr.Right (tmpWStr.GetLength() - Prefix_Length - Dirty_Prefix_Length);
			}
			else {
				tmpWStr = tmpWStr.Right (tmpWStr.GetLength() - Prefix_Length);
			}
			//*Dirty ����� ������ ���� �ؽ�Ʈ ���

			m_Text_Idx_In_Dir.File_Idx_Arr[i].Text_Idx_Arr[j].Lang_Code = sdlg.CP_Code;
			m_Text_Idx_In_Dir.File_Idx_Arr[i].Text_Idx_Arr[j].Is_Text_Dirty = true;
			//*�ڵ� �ٲٱ�, Dirty ���� ����

			apply_Txt.Format (_T("%s[%06d]%s%s"), 
				Prefix_Text_Str[sdlg.CP_Code], j, T_Head_Dirty, tmpWStr);
			m_ListCtrl_Text.SetItemText ((T_List_Idx + j), 0, apply_Txt);
			//*Dirty ���, ����� �ڵ� ����� ����� �ؽ�Ʈ�� ����ֱ�

		}

	}
	//*��� �ؽ�Ʈ(���� �ؽ�Ʈ ����)�� �ڵ��������� �ϰ� ����, Dirty ���� ���
	//*�� �Լ��� ȣ��Ǹ� �ؽ�Ʈ�� �̹� �ε�Ǿ����Ƿ� ��������
	//*[set all text cp]

	Narrow_to_Em();
	//*�ݰ� �Ϻ��� �����ڵ带 ���� �Ϻ��� �����ڵ�� �ٲٱ�
	//*�� �Լ��� ȣ��Ǹ� �ؽ�Ʈ�� �̹� �ε�Ǿ����Ƿ� ��������
	//*[Narrow Text to Em]

	Check_FDName_Dialog chkdlg;
	chkdlg.code = sdlg.CP_Code;
	chkdlg.DoModal();
	//*����/���丮 �̸� üũ
	//*[Check File/Dir Name]

	m_ListCtrl_Files.Invalidate();
	m_ListCtrl_Text.Invalidate();
	//*�ؽ�Ʈâ �ٽ� �׸���
	
	GetDlgItem(IDC_BUTTON3)->EnableWindow(TRUE); 
	GetDlgItem(IDC_BUTTON4)->EnableWindow(TRUE); 
	GetDlgItem(IDC_BUTTON7)->EnableWindow(TRUE); 
	GetDlgItem(IDC_BUTTON8)->EnableWindow(TRUE); 
//	GetDlgItem(IDC_BUTTON9)->EnableWindow(TRUE); 
	//*���� ��

	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}


void CMFCProj2Dlg::OnBnClicked_Auto_Trans_Papago()
{
	//***Auto Trans [Papago]***
	
	GetDlgItem(IDC_BUTTON3)->EnableWindow(FALSE); 
	GetDlgItem(IDC_BUTTON4)->EnableWindow(FALSE); 
	GetDlgItem(IDC_BUTTON7)->EnableWindow(FALSE); 
	GetDlgItem(IDC_BUTTON8)->EnableWindow(FALSE); 
//	GetDlgItem(IDC_BUTTON9)->EnableWindow(FALSE); 
	//*���� �� �ε�/����/���� �Ұ�

	Autotrans_Papago_Dialog atdlg;
	atdlg.DoModal();
	m_Text_Idx_In_Dir.Is_Dirty = true;
	m_Text_Idx_In_Dir.File_Idx_Arr[atdlg.progressing_file_idx].Is_FileTxt_Dirty = true;
	//*���� ����

	m_ListCtrl_Files.Invalidate();
	m_ListCtrl_Text.Invalidate();
	//*�ؽ�Ʈâ �ٽ� �׸���
	
	GetDlgItem(IDC_BUTTON3)->EnableWindow(TRUE); 
	GetDlgItem(IDC_BUTTON4)->EnableWindow(TRUE); 
	GetDlgItem(IDC_BUTTON7)->EnableWindow(TRUE); 
	GetDlgItem(IDC_BUTTON8)->EnableWindow(TRUE); 
//	GetDlgItem(IDC_BUTTON9)->EnableWindow(TRUE); 
	//*���� ��
}


/*
void CMFCProj2Dlg::OnBnClicked_Narrow_to_Em()
{
	//***Narrow Text to Em***
	
	GetDlgItem(IDC_BUTTON3)->EnableWindow(FALSE); 
	GetDlgItem(IDC_BUTTON4)->EnableWindow(FALSE); 
	GetDlgItem(IDC_BUTTON7)->EnableWindow(FALSE); 
	GetDlgItem(IDC_BUTTON8)->EnableWindow(FALSE); 
//	GetDlgItem(IDC_BUTTON9)->EnableWindow(FALSE); 
	//*���� �� �ε�/����/���� �Ұ�

	Narrow_to_Em();
	//*�ݰ� �Ϻ��� �����ڵ带 ���� �Ϻ��� �����ڵ�� �ٲٱ�
	//*�� �Լ��� ȣ��Ǹ� �ؽ�Ʈ�� �̹� �ε�Ǿ����Ƿ� ��������
	
	m_ListCtrl_Files.Invalidate();
	m_ListCtrl_Text.Invalidate();
	//*�ؽ�Ʈâ �ٽ� �׸���
	
	GetDlgItem(IDC_BUTTON3)->EnableWindow(TRUE); 
	GetDlgItem(IDC_BUTTON4)->EnableWindow(TRUE); 
	GetDlgItem(IDC_BUTTON7)->EnableWindow(TRUE); 
	GetDlgItem(IDC_BUTTON8)->EnableWindow(TRUE); 
//	GetDlgItem(IDC_BUTTON9)->EnableWindow(TRUE); 
	//*���� ��

	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}


void CMFCProj2Dlg::OnBnClicked_Check_Filename()
{
	//***Check File/Dir Name***
	
	GetDlgItem(IDC_BUTTON3)->EnableWindow(FALSE); 
	GetDlgItem(IDC_BUTTON4)->EnableWindow(FALSE); 
	GetDlgItem(IDC_BUTTON7)->EnableWindow(FALSE); 
	GetDlgItem(IDC_BUTTON8)->EnableWindow(FALSE); 
//	GetDlgItem(IDC_BUTTON9)->EnableWindow(FALSE); 
	//*���� �� �ε�/����/���� �Ұ�

	Select_CP_Dialog sdlg;
	while (sdlg.DoModal() != IDOK) { ; }
	//*�ڵ������� ����

	Check_FDName_Dialog chkdlg;
	chkdlg.code = sdlg.CP_Code;
	chkdlg.DoModal();
	//*����/���丮 �̸� üũ

	m_ListCtrl_Files.Invalidate();
	m_ListCtrl_Text.Invalidate();
	//*�ؽ�Ʈâ �ٽ� �׸���
	
	GetDlgItem(IDC_BUTTON3)->EnableWindow(TRUE); 
	GetDlgItem(IDC_BUTTON4)->EnableWindow(TRUE); 
	GetDlgItem(IDC_BUTTON7)->EnableWindow(TRUE); 
	GetDlgItem(IDC_BUTTON8)->EnableWindow(TRUE); 
//	GetDlgItem(IDC_BUTTON9)->EnableWindow(TRUE); 
	//*���� ��

	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}
*/


void CMFCProj2Dlg::OnBnClicked_Edit_GameDat_File()
{
	//***Edit 'Game.dat'***

	Edit_Game_Dat();
	//*BasicData/Game.dat ���� ���� (���� ����)

	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}








void CMFCProj2Dlg::OnBnClicked_Import_List()
{
	//***Import List***

	if (!Wolf_File_List.IsEmpty()) {
		int ret = AfxMessageBox (_T("������ �ִ� ����Ʈ�� ��������ϴ�.\n��� �����ϰڽ��ϱ�?"), MB_YESNO);
		if (ret == IDNO) { return; }
	}
	//*����Ʈ�� ��� �ҷ��� �ǰ� Ȯ��

	Wolf_File_ListName = _T("");
	Wolf_File_List.Empty();
	//*����Ʈ �ʱ�ȭ
	
	Import_List_Dialog dlg;
	if (dlg.DoModal() == IDOK) {
		AfxMessageBox (Wolf_File_ListName + _T(" : ����Ʈ�� �ҷ��Խ��ϴ�."));
		//*����Ʈ Ȯ�� â
	}
	//*���丮 ���ο��� ����Ʈ�� ���� ���ΰ�, �ؽ�Ʈ ������ �ҷ��� ���ΰ� �����ϴ� ���̾�α� â ����

	if (Wolf_File_ListName.Compare(_T("")) != 0) {
		SetDlgItemText(IDC_LISTNAME, _T("List : ") + Wolf_File_ListName);
		GetDlgItem(IDC_BUTTON2)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON6)->EnableWindow(TRUE); 
	}
	//*���� ���� �� ��ư Ȱ��ȭ (���� ����� ������� ����)

	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}


void CMFCProj2Dlg::OnBnClicked_Export_List()
{
	//***Export List***

	FILE *stream;
	unsigned short uni_code = 0xFEFF;

	_wfopen_s (&stream, Wolf_File_ListName.GetBuffer(), L"w+b");
	fwrite (&uni_code, sizeof(short), 1, stream);
	fwrite (Wolf_File_List.GetBuffer(), sizeof(wchar_t), Wolf_File_List.GetLength(), stream);
	fclose (stream);
	//*����Ʈ ������ ����� ���� �������� (�̸� : (wolf �̸�).txt)
	//*����Ʈ ���� : �����ڵ� ����)

	AfxMessageBox (Wolf_File_ListName + _T(" : ����Ʈ�� �����½��ϴ�."));
	//*��� Ȯ��â

	Wolf_File_ListName = _T("");
	Wolf_File_List.Empty();
	GetDlgItem(IDC_BUTTON6)->EnableWindow(FALSE);
	SetDlgItemText(IDC_LISTNAME, _T("--No List--"));
	//*�������� ���� ����Ʈ �̸��� ������ ���� Export List ��ư�� ��Ȱ��ȭ��Ų��

	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}








void CMFCProj2Dlg::On_Files_List_Ctrl_NMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	LPNMLVCUSTOMDRAW pLVCD = reinterpret_cast<LPNMLVCUSTOMDRAW>(pNMHDR);

	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	*pResult = 0;

	if (pNMCD->dwDrawStage == CDDS_PREPAINT) {
		*pResult = CDRF_NOTIFYITEMDRAW;
	}
	else if (pNMCD->dwDrawStage == CDDS_ITEMPREPAINT) {
		if (m_Text_Idx_In_Dir.File_Num != 0) {
			if (m_Text_Idx_In_Dir.File_Idx_Arr[pLVCD->nmcd.dwItemSpec].Text_Cnt != 0) {
				if (!m_Text_Idx_In_Dir.File_Idx_Arr[pLVCD->nmcd.dwItemSpec].Is_FileTxt_Dirty) {
					pLVCD->clrText = RGB (20, 20, 75);
					pLVCD->clrTextBk = RGB (200, 200, 250);
				}
				else {
					pLVCD->clrText = RGB (20, 75, 75);
					pLVCD->clrTextBk = RGB (200, 250, 250);
				}
			}
			//*�ؽ�Ʈ�� �ִ� ���Ͽ� ���ؼ��� ���� �����ϱ�
			//*Clean�� ���� �� �ؽ�Ʈ ���� = RGB (20, 20, 75), ���� ���� = RGB (200, 200, 250)
			//*Dirty�� ���� �� �ؽ�Ʈ ���� = RGB (20, 75, 75), ���� ���� = RGB (200, 250, 250)
		}
		*pResult = CDRF_DODEFAULT;
	}
	//*���� ����Ʈ ���� ������ ������ �ľ��ϰ� ������ �����ϱ� �����̴�
}


void CMFCProj2Dlg::On_Text_List_NMCustomdrawList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	LPNMLVCUSTOMDRAW pLVCD = reinterpret_cast<LPNMLVCUSTOMDRAW>(pNMHDR);

	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	*pResult = 0;

	if (pNMCD->dwDrawStage == CDDS_PREPAINT) {
		*pResult = CDRF_NOTIFYITEMDRAW;
	}
	else if (pNMCD->dwDrawStage == CDDS_ITEMPREPAINT) {
		if (m_Text_Idx_In_Dir.File_Num != 0) {

			*pResult = CDRF_DODEFAULT;

			unsigned int f_idx, t_idx;
			Get_Fidx_Tidx ((unsigned int)pLVCD->nmcd.dwItemSpec, &f_idx, &t_idx);
			//*�ؽ�Ʈ �ε��� ������

			if ((t_idx == 0) || 
				((m_Text_Idx_In_Dir.File_Idx_Arr[f_idx].Text_Idx_Arr[t_idx - 1].File_Str_Idx != LOADING_FILE) 
				&& (m_Text_Idx_In_Dir.File_Idx_Arr[f_idx].Text_Idx_Arr[t_idx - 1].File_Str_Idx != NONE_FILE))) {
				if (t_idx != 0) {
					pLVCD->clrText = RGB (20, 75, 20);
					pLVCD->clrTextBk = RGB (200, 250, 200);
				}
				//*��¥ �ؽ�Ʈ ������ ��
				else {
					SelectObject(pNMCD->hdc, m_fontBold.GetSafeHandle());
					*pResult = CDRF_NEWFONT;
				}
				//*�ؽ�Ʈ �տ� �ִ� ����� �� -> ���� ��Ʈ�� ����
			}
			else if (m_Text_Idx_In_Dir.File_Idx_Arr[f_idx].Text_Idx_Arr[t_idx - 1].Is_Text_Dirty) {
				pLVCD->clrText = RGB (75, 75, 20);
				pLVCD->clrTextBk = RGB (250, 250, 200);
			}
			//*�ؽ�Ʈ �ε带 ��ģ ���, �ؽ�Ʈ ������ ���ϸ��� ��쿡 ���ؼ��� ���� �����ϱ�
			//*�ؽ�Ʈ ���� = RGB (20, 75, 20), ���� ���� = RGB (200, 250, 200)
			//*t_idx�� - 1�� ���� ������ �տ� ��� ������
			//*�׳� ���ڿ� �� Dirty�� ���� �� �ؽ�Ʈ ���� = RGB (75, 75, 20), ���� ���� = RGB (250, 250, 200)
		}
		else { *pResult = CDRF_DODEFAULT; }
	}
	//*�ؽ�Ʈ ����Ʈ ���� ������ ������ �ľ��ϰ� ������ �����ϱ� �����̴�
}




void CMFCProj2Dlg::On_Files_List_NMClickList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	*pResult = 0;
	
	GetDlgItem(IDC_BUTTON3)->EnableWindow(FALSE); 
	GetDlgItem(IDC_BUTTON4)->EnableWindow(FALSE); 
	GetDlgItem(IDC_BUTTON7)->EnableWindow(FALSE); 
	GetDlgItem(IDC_BUTTON8)->EnableWindow(FALSE); 
//	GetDlgItem(IDC_BUTTON9)->EnableWindow(FALSE); 
	//*���� �� �ε�/����/���� �Ұ�

	int index = pNMItemActivate->iItem; 
	//*Ŭ���� �������� �ε��� ���

	if (index >= 0 && index < m_ListCtrl_Files.GetItemCount()){

		if (m_Text_Idx_In_Dir.File_Idx_Arr[index].Text_Cnt != 0) {
			POINT point;
			m_ListCtrl_Text.GetItemPosition (m_Text_Idx_In_Dir.File_Idx_Arr[index].Idx_of_Text_Start, &point);
			CSize sz(0, point.y);
			m_ListCtrl_Text.Scroll(sz);
		}
		//*�ؽ�Ʈ�� �ִ� ���� �׸��� Ŭ���ϸ� �ؽ�Ʈ ����Ʈ�� �������� �̵���Ų��.

	}
	//*Ŭ���� ������ �ε����� ��µ� ����Ʈ �ȿ� ���� �� 

	//*���� ����Ʈ�� ���Ҹ� Ŭ������ �� �ؽ�Ʈ ����Ʈ�� �̵����� Ŭ�� ���·� ����� �����̴�.
	
	GetDlgItem(IDC_BUTTON3)->EnableWindow(TRUE); 
	GetDlgItem(IDC_BUTTON4)->EnableWindow(TRUE); 
	GetDlgItem(IDC_BUTTON7)->EnableWindow(TRUE); 
	GetDlgItem(IDC_BUTTON8)->EnableWindow(TRUE); 
//	GetDlgItem(IDC_BUTTON9)->EnableWindow(TRUE); 
	//*���� ����
}





void CMFCProj2Dlg::On_Text_List_NMClickList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	*pResult = 0;

	int index = pNMItemActivate->iItem; 
	//*Ŭ���� �������� �ε��� ���

	if (index >= 0 && index < m_ListCtrl_Text.GetItemCount()){
		Clicked_Idx = (unsigned int)index;
	}
	//*���� �����ϱ�?
}




void CMFCProj2Dlg::On_Text_List_NMDblclkList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	*pResult = 0;
	
	GetDlgItem(IDC_BUTTON3)->EnableWindow(FALSE); 
	GetDlgItem(IDC_BUTTON4)->EnableWindow(FALSE); 
	GetDlgItem(IDC_BUTTON7)->EnableWindow(FALSE); 
	GetDlgItem(IDC_BUTTON8)->EnableWindow(FALSE); 
//	GetDlgItem(IDC_BUTTON9)->EnableWindow(FALSE); 
	//*���� �� �ε�/����/���� �Ұ�

	int index = pNMItemActivate->iItem; 
	//*����Ŭ���� �������� �ε��� ���

	if (index >= 0 && index < m_ListCtrl_Text.GetItemCount()){

		unsigned int fidx, tidx, FH_idx;
		CString tmptt, tmptt2, tt;

		Get_Fidx_Tidx ((unsigned int)index, &fidx, &tidx); 
		FH_idx = (unsigned int)index - tidx;
		//*���� �ε���, �ؽ�Ʈ �ε���, �ؽ�Ʈ ��� �ε��� ���

		if (tidx == 0)
		{ 
			GetDlgItem(IDC_BUTTON3)->EnableWindow(TRUE); 
			GetDlgItem(IDC_BUTTON4)->EnableWindow(TRUE); 
			GetDlgItem(IDC_BUTTON7)->EnableWindow(TRUE); 
			GetDlgItem(IDC_BUTTON8)->EnableWindow(TRUE); 
//			GetDlgItem(IDC_BUTTON9)->EnableWindow(TRUE); 
			return; 
		}
		//*�ؽ�Ʈ ����� ���� �۾����� �ʴ´�

		else if (m_Text_Idx_In_Dir.File_Idx_Arr[fidx].Text_Idx_Arr[tidx - 1].File_Str_Idx != NONE_FILE)
		{

			File_Or_Text_Dialog FDd;
			if (FDd.DoModal() != IDOK) { goto DBCL_END; }
			if (FDd.Type == 2) { goto TEXT_EDIT; }
			//*���ϸ��� ���� üũâ �ϳ� �� ����� ���� �̸����� ������ ����,
			//*�ƴϸ� �Ϲ����� �ؽ�Ʈ�� ������ ���� ������ �� �ֵ��� �Ѵ�

			Change_FDName_Dialog CFDdlg;
			unsigned int File_Idx;
			CString FileName, FileName_Chg;
			CString FullPath, FullPath_Chg;
			CString Root_Dir;
			CString apl_txt;

			File_Idx = m_Text_Idx_In_Dir.File_Idx_Arr[fidx].Text_Idx_Arr[tidx - 1].File_Str_Idx;
			FileName = CFDdlg.Input_WStr = m_Text_Idx_In_Dir.File_Idx_Arr[File_Idx].File_Name;
			CFDdlg.Output_WStr = m_Text_Idx_In_Dir.File_Idx_Arr[File_Idx].File_Name;
			CFDdlg.code = m_Text_Idx_In_Dir.File_Idx_Arr[fidx].Text_Idx_Arr[tidx - 1].Lang_Code;
			CFDdlg.Meta_type = FILE_DB;
			CFDdlg.ShouldChange = false;
			//*���� �ε����� ��� �׿� �ش��ϴ� ���ϸ��� �־� �ٲٵ��� �Ѵ�

			if (CFDdlg.DoModal() == IDOK) {

				FileName_Chg = CFDdlg.Output_WStr;
				if (Root_Dir == _T("")) {
					for (unsigned int i = 0;i < m_Text_Idx_In_Dir.File_Num;i++) {
						tt = m_Text_Idx_In_Dir.File_Idx_Arr[i].File_FullPath;
						tt.Replace (_T("\\"), _T("/"));
						if (Root_Dir.Compare(_T("")) == 0) { Root_Dir = tt; }
						else {
							while (wcsstr(tt, Root_Dir.GetBuffer()) == 0) {
								Root_Dir = Root_Dir.Left (Root_Dir.GetLength() - 1);
							}
						}
					}
				}
				//*��Ʈ ���丮 �߷����� �����ϰ� �ٲ� ��� �߷�����.

				FullPath = m_Text_Idx_In_Dir.File_Idx_Arr[File_Idx].File_FullPath;
				FullPath_Chg = FullPath.Left (FullPath.GetLength() - FileName.GetLength());
				FullPath_Chg += FileName_Chg;
				//*���ϸ�, ��ü��� �ٲ�ġ��

				if (FD_Rename (FullPath.GetBuffer(), FullPath_Chg.GetBuffer()) == -1) {
					AfxMessageBox (_T("���ϸ��� �ٲ��� ���߽��ϴ�.\n���� ������̰ų� �ߺ��� ���ϸ��Դϴ�."));
					GetDlgItem(IDC_BUTTON3)->EnableWindow(TRUE); 
					GetDlgItem(IDC_BUTTON4)->EnableWindow(TRUE); 
					GetDlgItem(IDC_BUTTON7)->EnableWindow(TRUE); 
					GetDlgItem(IDC_BUTTON8)->EnableWindow(TRUE); 
//					GetDlgItem(IDC_BUTTON9)->EnableWindow(TRUE); 
					return; 
				}
				//*���ϸ� �ٲٱ�, �� �ٲٸ� ��� ���̰ų� �ߺ����� ó��

				memset (m_Text_Idx_In_Dir.File_Idx_Arr[File_Idx].File_Name, 0, (sizeof(wchar_t)*MAX_PATH));
				memcpy (m_Text_Idx_In_Dir.File_Idx_Arr[File_Idx].File_Name, 
					FileName_Chg.GetBuffer(), (sizeof(wchar_t)*FileName_Chg.GetLength()));
				memset (m_Text_Idx_In_Dir.File_Idx_Arr[File_Idx].File_FullPath, 0, (sizeof(wchar_t)*MAX_PATH));
				memcpy (m_Text_Idx_In_Dir.File_Idx_Arr[File_Idx].File_FullPath, 
					FullPath_Chg.GetBuffer(), (sizeof(wchar_t)*FullPath_Chg.GetLength()));
				//*�ش� ���ϸ��� ������ File_Idx_Arr �ٲٱ�

				tmptt = FullPath.Right (FullPath.GetLength() - Root_Dir.GetLength());
				tmptt2 = FullPath_Chg.Right (FullPath_Chg.GetLength() - Root_Dir.GetLength());
				//*�ٲ� ���ڿ� ����

				for (unsigned int i = 0;i < (unsigned int)m_ListCtrl_Text.GetItemCount();i++) {

					Get_Fidx_Tidx ((unsigned int)i, &fidx, &tidx); 
					if (m_Text_Idx_In_Dir.File_Idx_Arr[fidx].Text_Idx_Arr[tidx - 1].File_Str_Idx == File_Idx) {
						
						tt = m_ListCtrl_Text.GetItemText (i, 0);
						if (m_Text_Idx_In_Dir.File_Idx_Arr[fidx].Text_Idx_Arr[tidx - 1].Is_Text_Dirty) {
							tt = tt.Right (tt.GetLength() - Prefix_Length - Dirty_Prefix_Length);
							//* Dirty�� ���� '*'�� ����ؾ� �Ѵ�
						}
						else {
							tt = tt.Right (tt.GetLength() - Prefix_Length);
						}
						//*���� �ؽ�Ʈ ���ϱ�

						tt.Replace (tmptt, tmptt2);
						apl_txt.Format (_T("%s[%06d]%s%s"), Prefix_Text_Str[CFDdlg.code], (tidx - 1), T_Head_Dirty, tt);
						m_ListCtrl_Text.SetItemText (i, 0, apl_txt);
						//*�ؽ�Ʈ �ٲ�ġ�� Dirty ���� ����

						if (!m_Text_Idx_In_Dir.File_Idx_Arr[fidx].Text_Idx_Arr[tidx - 1].Is_Text_Dirty) {
							m_Text_Idx_In_Dir.File_Idx_Arr[fidx].Text_Idx_Arr[tidx - 1].Is_Text_Dirty = true;
						}
						//*�ؽ�Ʈ Dirty ����
						
						if (!m_Text_Idx_In_Dir.File_Idx_Arr[fidx].Is_FileTxt_Dirty) {
							FH_idx = Get_List_Index (fidx, 0) - 1;
							apl_txt = T_Head_Dirty; apl_txt += m_ListCtrl_Text.GetItemText (FH_idx, 0);
							m_ListCtrl_Text.SetItemText (FH_idx, 0, apl_txt);
							m_Text_Idx_In_Dir.File_Idx_Arr[fidx].Is_FileTxt_Dirty = true;
						}
						//*���� ��Ƽ ����
						
						if (!m_Text_Idx_In_Dir.Is_Dirty) { m_Text_Idx_In_Dir.Is_Dirty = true; }
						//*��ü ��Ƽ ����

					}
				}
				//*��� ���� �ؽ�Ʈ�� �� �ش� File_Idx�� ���� �ε����� ���� ���ڿ��� �� �ٲٱ�, Dirty ���

			}
			//*���ϸ��� �޶��� ���� �ٲٸ� ��

		}
		//*���� ����� ���� ���� �̸��� �ٲ� �� �ִ� ���̾�α� â�� ���� �����ϸ� �ٲٰ� �Ѵ�
		//*����ڵ�� ���� �ִ� Lang_Code�� �ľ��Ѵ�, ������ ���� ������ �Ұ����ϴ� ���ϵǾ� ���� ���̴�

		else {

TEXT_EDIT:

			Edit_Text_Dialog ETdlg;

			ETdlg.m_Input_CP = m_Text_Idx_In_Dir.File_Idx_Arr[fidx].Text_Idx_Arr[tidx - 1].Lang_Code;
			ETdlg.m_Output_CP = KOR_CODE;						//*�Ϲ������� �ѱ۹����� �״� �̷��� �� ����
			tmptt = m_ListCtrl_Text.GetItemText (index, 0);

			if (m_Text_Idx_In_Dir.File_Idx_Arr[fidx].Text_Idx_Arr[tidx - 1].Is_Text_Dirty) {
				tmptt = tmptt.Right (tmptt.GetLength() - Prefix_Length - Dirty_Prefix_Length);
				//* Dirty�� ���� '*'�� ����ؾ� �Ѵ�
			}
			else {
				tmptt = tmptt.Right (tmptt.GetLength() - Prefix_Length);
			}
			//*Dirty �� ��Ÿ ��� �����ֱ�

			if (m_Text_Idx_In_Dir.File_Idx_Arr[fidx].Text_Idx_Arr[tidx - 1].Is_OneChar_Enter) {
				tmptt.Replace(_T("\n"), _T("\r\n"));
				tmptt.Replace(_T("\r\r"), _T("$$\r\n"));
			}
			//*0xA������ ������ ��� �ʿ� ��ġ�� ����
			ETdlg.m_Input_tmp = ETdlg.m_Output_tmp = tmptt;
			//*���̾�α׸� ������Ű�� ���� �ʿ��� ���� �־��ش�(Dirty ���� ���)

			if (ETdlg.DoModal() == IDOK) {

				m_Text_Idx_In_Dir.File_Idx_Arr[fidx].Text_Idx_Arr[tidx - 1].Lang_Code = ETdlg.m_Output_CP;

				CString apl_txt;
				apl_txt.Format (_T("%s[%06d]%s%s"), Prefix_Text_Str[ETdlg.m_Output_CP], (tidx - 1), T_Head_Dirty, ETdlg.m_Output_tmp);
				if (m_Text_Idx_In_Dir.File_Idx_Arr[fidx].Text_Idx_Arr[tidx - 1].Is_OneChar_Enter) {
					apl_txt.Replace(_T("$$\r\n"), _T("\r\r"));
					apl_txt.Replace(_T("\r\n"), _T("\n"));
				}
				m_ListCtrl_Text.SetItemText (index, 0, apl_txt);
				//*0xA������ ������ ��� �ʿ� ��ġ�� ���ؼ� �ٽ� �ݿ��Ѵ�

				if (!m_Text_Idx_In_Dir.File_Idx_Arr[fidx].Text_Idx_Arr[tidx - 1].Is_Text_Dirty) {
					m_Text_Idx_In_Dir.File_Idx_Arr[fidx].Text_Idx_Arr[tidx - 1].Is_Text_Dirty = true;
				}
				if (!m_Text_Idx_In_Dir.File_Idx_Arr[fidx].Is_FileTxt_Dirty) {
					apl_txt = T_Head_Dirty; apl_txt += m_ListCtrl_Text.GetItemText (FH_idx, 0);
					m_ListCtrl_Text.SetItemText (FH_idx, 0, apl_txt);
					m_Text_Idx_In_Dir.File_Idx_Arr[fidx].Is_FileTxt_Dirty = true;
				}
				if (!m_Text_Idx_In_Dir.Is_Dirty) {
					m_Text_Idx_In_Dir.Is_Dirty = true;
				}
				//*�ٲ� �ڵ������� ����, dirty ���� ����, ���� ��� �ؽ�Ʈ Dirty ����, ����Ʈ�� �ٲ� ���ڿ� �ݿ�

				m_ListCtrl_Files.Invalidate();
				m_ListCtrl_Text.Invalidate();
				//*�ؽ�Ʈâ �ٽ� �׸���
			}

		}
		//*�ؽ�Ʈ��� �ؽ�Ʈ ���� â�� ��쵵�� �Ѵ�

	}
	//*Ŭ���� ������ �ε����� ��µ� ����Ʈ �ȿ� ���� �� 

	m_ListCtrl_Files.Invalidate();
	m_ListCtrl_Text.Invalidate();
	//*�ؽ�Ʈâ �ٽ� �׸���

DBCL_END:
	
	GetDlgItem(IDC_BUTTON3)->EnableWindow(TRUE); 
	GetDlgItem(IDC_BUTTON4)->EnableWindow(TRUE); 
	GetDlgItem(IDC_BUTTON7)->EnableWindow(TRUE); 
	GetDlgItem(IDC_BUTTON8)->EnableWindow(TRUE); 
//	GetDlgItem(IDC_BUTTON9)->EnableWindow(TRUE); 
	//*���� ��

	//*���� ���� ���ڿ��� �ƴϸ� �ؽ�Ʈ ����Ʈ ���̾�α׸� ��� �����ϰ�
	//*����� IDOK�� ���� �ؽ�Ʈ�� ����Ʈ�� �����Ѵ�
}




void CMFCProj2Dlg::Copy_String_To_Clipboard (CString *str)
{
    
	if (OpenClipboard() == FALSE) { AfxMessageBox (_T("Ŭ�����带 ���� ���߽��ϴ�!")); return; }
    EmptyClipboard();
	
    HGLOBAL hGlobal = GlobalAlloc (GHND | GMEM_SHARE, (str->GetLength() + 1) * sizeof(TCHAR));
    LPTSTR pGlobal = (LPTSTR)GlobalLock(hGlobal);
	if (pGlobal == NULL) { AfxMessageBox (_T("�� �ּҸ� ���� ���߽��ϴ�!")); return; }
	lstrcpy (pGlobal, str->GetBuffer());
    GlobalUnlock (hGlobal);
    SetClipboardData (CF_UNICODETEXT, hGlobal);
	//*������ ���� �� �Ҵ� �� �������� ����

    CloseClipboard();
	//*������ ���� ���� �ִ� �����͸� Ŭ������� ����. Ŭ������� �����̹Ƿ� �� �Ҵ��� �����ϸ� �� �ȴ�.
}
//*���ڿ��� Ŭ������� �������ִ� �Լ�


void CMFCProj2Dlg::Get_String_From_Clipboard (CString *str)
{
	CString ret = _T("");

	if (OpenClipboard() == FALSE) { AfxMessageBox (_T("Ŭ�����带 ���� ���߽��ϴ�!")); return; }
	HANDLE hClipboardData = GetClipboardData (CF_UNICODETEXT);
	//*Ŭ�����忡�� �����ڵ� �ؽ�Ʈ ������ ���

	if (hClipboardData == NULL) { AfxMessageBox (_T("�ؽ�Ʈ�� ������ ���߽��ϴ�!")); return; }
	TCHAR *pchData = (TCHAR*)GlobalLock (hClipboardData);
	if (pchData == NULL) { AfxMessageBox (_T("�� �ּҸ� ���� ���߽��ϴ�!")); return; }
	ret = pchData;
	GlobalUnlock (hClipboardData);
    CloseClipboard();
	//*�����ڵ� �ؽ�Ʈ �ּҸ� ��� ����

	if (str != NULL) { (*str) = ret; }

}
//*Ŭ�����忡�� ���ڿ��� ������ �Լ�. �������� ���ϸ� �� ���ڿ��� ��ȯ�Ѵ�




void CMFCProj2Dlg::Replace_String ()
{
	unsigned int fidx, tidx, FH_idx;
	CString tmpWStr, cc;

	CString bef_str, aft_str;
	//*���̾�α׸� ȣ���ؼ� �����Ѵ�

	if (m_ListCtrl_Text.GetItemCount() > 0) {

		Replace_Text_Dialog dlg;
		if (dlg.DoModal() != IDOK) { return; }
		bef_str = dlg.strtofind;
		aft_str = dlg.strtochg;

		for (unsigned int i = 0;i < (unsigned int)m_ListCtrl_Text.GetItemCount();i++) {

			Get_Fidx_Tidx (i, &fidx, &tidx);
			//*�ε��� ���

			if ((tidx == 0) || 
				((!dlg.FCH) && (m_Text_Idx_In_Dir.File_Idx_Arr[fidx].Text_Idx_Arr[tidx - 1].File_Str_Idx != NONE_FILE))) 
			{ continue; }
			//*��� ���ڿ��� ���� ���ڿ��� �ƴ� ���� �ٲ۴�

			tmpWStr = m_ListCtrl_Text.GetItemText (i, 0);
			if (m_Text_Idx_In_Dir.File_Idx_Arr[fidx].Text_Idx_Arr[tidx - 1].Is_Text_Dirty) {
				tmpWStr = tmpWStr.Right (tmpWStr.GetLength() - Prefix_Length - Dirty_Prefix_Length);
			}
			else {
				tmpWStr = tmpWStr.Right (tmpWStr.GetLength() - Prefix_Length);
			}
			//*Dirty ����� ������ ���� �ؽ�Ʈ ���

			cc = tmpWStr; cc.Replace (bef_str, aft_str);
			if (cc != tmpWStr) {

				if (dlg.code == NON_CODE) {
					tmpWStr.Format (_T("%s[%06d]%s%s"), 
						Prefix_Text_Str[m_Text_Idx_In_Dir.File_Idx_Arr[fidx].Text_Idx_Arr[tidx - 1].Lang_Code], 
						(tidx - 1), T_Head_Dirty, cc.GetBuffer());
					m_ListCtrl_Text.SetItemText (i, 0, tmpWStr);
					//*����Ʈ�� �ݿ�
				}
				else {
					tmpWStr.Format (_T("%s[%06d]%s%s"), 
						Prefix_Text_Str[dlg.code], 
						(tidx - 1), T_Head_Dirty, cc.GetBuffer());
					m_ListCtrl_Text.SetItemText (i, 0, tmpWStr);
					//*����Ʈ�� �ݿ�
				}

				if (!m_Text_Idx_In_Dir.File_Idx_Arr[fidx].Text_Idx_Arr[tidx - 1].Is_Text_Dirty) {
					m_Text_Idx_In_Dir.File_Idx_Arr[fidx].Text_Idx_Arr[tidx - 1].Is_Text_Dirty = true;
				}
				if (!m_Text_Idx_In_Dir.File_Idx_Arr[fidx].Is_FileTxt_Dirty) {
					FH_idx = Get_List_Index (fidx, 0) - 1;
					tmpWStr = T_Head_Dirty; tmpWStr += m_ListCtrl_Text.GetItemText (FH_idx, 0);
					m_ListCtrl_Text.SetItemText (FH_idx, 0, tmpWStr);
					m_Text_Idx_In_Dir.File_Idx_Arr[fidx].Is_FileTxt_Dirty = true;
				}
				if (!m_Text_Idx_In_Dir.Is_Dirty) { m_Text_Idx_In_Dir.Is_Dirty = true; }
				//*Dirty ���� ����

			}
			//*�ٲٰ� �� ���ڿ��� ��ȭ�� ���� ���� ����־��ָ� �ȴ�

		}
		

		m_ListCtrl_Files.Invalidate();
		m_ListCtrl_Text.Invalidate();
		//*�ؽ�Ʈâ �ٽ� �׸���

	}

}
//*�ؽ�Ʈ ����Ʈ �� ���ڿ��� �ٲ��ִ� �Լ�. ���� ���ڿ�, �ؽ�Ʈ ��� ����




void CMFCProj2Dlg::Find_String (unsigned int _Direction, CString _Word)
{

	unsigned int Direction;
	CString Word, ListStr;
	//*����� ����

	int nItem = -1;
	unsigned int T_List_Idx, fidx, tidx;
	//*�ؽ�Ʈ�� ����(��� Ȥ�� ���� �ؽ�Ʈ)�� ��� ���� ����
	
	POINT point;
	CSize sz;
	//*����Ʈ�� �̵���ų �� �ʿ��� ����

	if (m_ListCtrl_Text.GetItemCount() > 0) {

		nItem = m_ListCtrl_Text.GetNextItem (nItem, LVNI_SELECTED);
		//*Ŭ���� �ε��� ã�� -> ���⼭ ���� ���̳� ��.

		if ((_Direction == NON_DIRECTION) || (_Word.GetLength() == 0)) {
			Find_Text_Dialog dlg;
			if (dlg.DoModal() != IDOK) { Find_Direction = NON_DIRECTION; Find_Str = _T(""); return; }
			Direction = Find_Direction = dlg.Direction;
			Word = Find_Str = dlg.Find_Str;
		}
		else {
			Direction = _Direction;
			Word = _Word;
		}
		//*���� ã�� �����̳� ã�� ������ �����Ǿ� ���� �ʴٸ� ���̾�α׸� ȣ���ؼ� �����Ѵ�
		//*���� Find_Direction�̳� Find_Str�� �����Ѵ�
		//*�����Ǿ� �ִٸ� �״�� �޾Ƶ��̰�, ã�⸦ ����ϸ� ���� �ʱ�ȭ�Ѵ�

		T_List_Idx = (unsigned int)nItem;

		for (;((T_List_Idx > 0) && (Direction == UP_DIRECTION)) || 
			((T_List_Idx < (unsigned int)m_ListCtrl_Text.GetItemCount()) && (Direction == DOWN_DIRECTION));) {

			ListStr = m_ListCtrl_Text.GetItemText (T_List_Idx, 0);
			Get_Fidx_Tidx (T_List_Idx, &fidx, &tidx);
			if (tidx == 0) { goto MOV_LIST; }
			//*����� �� �ʿ䵵 ����

			if (m_Text_Idx_In_Dir.File_Idx_Arr[fidx].Text_Idx_Arr[tidx - 1].Is_Text_Dirty) {
				ListStr = ListStr.Right (ListStr.GetLength() - Prefix_Length - Dirty_Prefix_Length);
			}
			else {
				ListStr = ListStr.Right (ListStr.GetLength() - Prefix_Length);
			}
			//*�翬�� ���ڿ��� Dirty�� �� �� ���� �ؾ� �Ѵ�

			if (ListStr.Find(Word) != -1) { 
				if (T_List_Idx != (unsigned int)nItem) { break; }
			}
			//*���ڿ� ã��, ã���� �״�� ����������
			//*�� ó���� ���ڿ��� �ִ� �ؽ�Ʈ�� Ŭ������ ���� �ִµ�, �� ���� ��� �����̸� �ȴ�

MOV_LIST:
			if (Direction == UP_DIRECTION) { T_List_Idx--; }
			else { T_List_Idx++; }
			//*���⿡ ���� �ε��� ����

		}

		if (((T_List_Idx == 0) && (Direction == UP_DIRECTION)) || 
			((T_List_Idx == (unsigned int)m_ListCtrl_Text.GetItemCount()) && (Direction == DOWN_DIRECTION))) {
			AfxMessageBox (_T("�ش� �������� �ش� �ؽ�Ʈ�� ���� ���ڿ���\n�� �̻� �����ϴ�."));
			return;
		}
		//*���� �ش� �������� �ؽ�Ʈ�� ������ �ش� ���ڿ��� ������ �ؽ�Ʈ�� ���ٰ� ����
		
		m_ListCtrl_Text.GetItemPosition ((int)T_List_Idx, &point);
		sz.cx = 0; sz.cy = point.y;
		m_ListCtrl_Text.Scroll(sz);
		Get_Fidx_Tidx ((unsigned int)T_List_Idx, &fidx, &tidx);
		m_ListCtrl_Text.SetItemState (-1, 0, LVIS_SELECTED);
		m_ListCtrl_Text.SetItemState ((int)T_List_Idx, LVIS_SELECTED, LVIS_SELECTED);
		//*�ش� �ؽ�Ʈ�� Ŭ�� ���·� ���� �̵���Ű��

	}

}
//*�ؽ�Ʈ ����Ʈ ������ Ư�� ���ڿ��� ã���ִ� �Լ�. ���� ���ڿ�, �ؽ�Ʈ ��� ����




void CMFCProj2Dlg::Collect_Clipboard_String_From_List ()
{
	unsigned int uSelectedCount;
	unsigned int fidx, tidx;
	int nItem;
	CString totalWStr, tmpWStr, cc;
	//*����Ʈ�� �����ϴ� �� �ʿ��� ������
	//*fidx, tidx�� Dirty�� �������� ��� ���� �ʿ��ϴ�

	if (m_ListCtrl_Text.GetItemCount() > 0) {

		uSelectedCount = m_ListCtrl_Text.GetSelectedCount(); //*���õ� ���� ���
		if (uSelectedCount != 0) {

			nItem = -1; totalWStr = _T("");

			for (unsigned int i = 0;i < uSelectedCount;i++) {
				nItem = m_ListCtrl_Text.GetNextItem (nItem, LVNI_SELECTED);
				Get_Fidx_Tidx (nItem, &fidx, &tidx);

				if ((tidx == 0) 
					|| (m_Text_Idx_In_Dir.File_Idx_Arr[fidx].Text_Idx_Arr[tidx - 1].File_Str_Idx != NONE_FILE)) { continue; }
				//*���� ���ڿ��� ����� �ƴ� ���� ������

				tmpWStr = m_ListCtrl_Text.GetItemText (nItem, 0);
				if (m_Text_Idx_In_Dir.File_Idx_Arr[fidx].Text_Idx_Arr[tidx - 1].Is_Text_Dirty) {
					tmpWStr = tmpWStr.Right (tmpWStr.GetLength() - Prefix_Length - Dirty_Prefix_Length);
				}
				else {
					tmpWStr = tmpWStr.Right (tmpWStr.GetLength() - Prefix_Length);
				}
				//*Dirty ����� ������ ���� �ؽ�Ʈ ���

				if (m_Text_Idx_In_Dir.File_Idx_Arr[fidx].Text_Idx_Arr[tidx - 1].Is_OneChar_Enter) {
					tmpWStr.Replace(_T("\n"), _T("||"));
					tmpWStr.Replace(_T("\r\r"), _T("$$$"));
				}
				else { tmpWStr.Replace(_T("\r\n"), _T("||")); }
				//*0xA �ϳ��� �����ϴ� ��� ���� ����Ͽ� ���� ���ڿ��� Replace�Ѵ�

				cc.Format(_T("[::][%s][%06d][%06d][::]%s[::]\r\n"), 
					Copy_Text_Str[m_Text_Idx_In_Dir.File_Idx_Arr[fidx].Text_Idx_Arr[tidx - 1].Lang_Code], 
					(fidx + 1), (tidx), tmpWStr.GetBuffer());
				totalWStr += cc;
				//*�ؽ�Ʈ ������
			}

			Copy_String_To_Clipboard (&totalWStr);
			//*������ ���ڿ��� ��� Ŭ�����忡 �ٿ��ֱ�

		}
		else {
			AfxMessageBox (_T("�ؽ�Ʈ�� ������ �Ŀ� �����ϼ���."));
			//*�ؽ�Ʈ ����Ʈ���� �ƹ��͵� Ŭ������ ������ Ŭ���϶�� �޽����� ����
		}

	}
}
//*����Ʈ���� ���õ� ���ڿ� �����Ͽ� Ŭ�����忡 �ٿ��ֱ�, ���� ���ڿ��� ����


void CMFCProj2Dlg::Set_List_String_From_Clipboard ()
{
	CString str;
	CString tp_str, lang_str, txt_str, list_str;
	unsigned int Total_strNum = 0, fidx, tidx, Start_Pos, End_Pos;
	unsigned int T_List_Idx;
	unsigned int code;

	Get_String_From_Clipboard (&str);
	//*Ŭ�����忡�� ���ڿ� ���

	str.Replace (_T("] ["), _T("]["));
	str.Replace (_T(" ::"), _T("::"));
	str.Replace (_T(":: "), _T("::"));
	str.Replace (_T("[: :]"), _T("[::]"));
	str.Replace (_T("[::] "), _T("[::]"));
	str.Replace (_T(" [::]"), _T("[::]"));
	str.Replace (_T(" : "), _T(":"));
	str.Replace (_T("@ "), _T("@"));
	str.Replace (_T("[:]"), _T("[::]"));
	Rid_Space (&str);
	//*�ǹ̾��� �� ���� ���� ���ֱ�, ���İ��� ����ǵ��� ����

	for (unsigned int i = 0;i < (unsigned int)str.GetLength() - 1;i++) {
		if ((str[i] == 0xD) && (str[i+1] == 0xA)) { Total_strNum++; }
	}
	if (!((str[str.GetLength() - 2] == 0xD) && (str[str.GetLength() - 1] == 0xA))) 
	{ Total_strNum++; }
	//*�� �� : ���� �� + 1(�����ϸ鼭 �������µ�)

	Start_Pos = End_Pos = 0; 
	for (unsigned int i = 0;i < Total_strNum;i++) {

		End_Pos = Start_Pos;
		while ((str.Mid(End_Pos, 6) != _T("[::]\r\n")) && (End_Pos < (unsigned int)str.GetLength())) 
		{ End_Pos++; }
		tp_str = str.Mid (Start_Pos, (End_Pos - Start_Pos));
		if (tp_str.Right(4) == _T("[::]")) { tp_str = tp_str.Left(tp_str.GetLength() - 4); }
		//*�� ���ڿ��� ���� ��ġ, �� ��ġ ��� ���ڿ� ���

		lang_str = tp_str.Mid (5, 3);
		fidx = _ttoi(tp_str.Mid (10, 6));
		tidx = _ttoi(tp_str.Mid (18, 6));
		txt_str = tp_str.Right (tp_str.GetLength() - 29);
		//*���ڿ��� �ϳ��ϳ� �Ű��ֱ�

		if ((fidx == 0) || (tidx == 0)) {
			AfxMessageBox (_T("error : �ùٸ��� ���� ���ڿ��� �ٿ��־����ϴ�."));
			return;
		}
		fidx--; tidx--;
		//***���� fidx�� tidx �� 0�� ���Դٸ� �߸��� ���ڿ��� ã�� ������ ���̴� ������ ó���Ѵ�

		if (m_Text_Idx_In_Dir.File_Idx_Arr[fidx].Text_Idx_Arr[tidx].Is_OneChar_Enter) { 
			txt_str.Replace (_T("||"), _T("\n")); 
			txt_str.Replace (_T("$$$"), _T("\r\r")); 
		}
		else  { txt_str.Replace (_T("||"), _T("\r\n")); }
		//*0xD, 0xA�� �����ΰ�, 0xA�� ������ �����̳Ŀ� ���� ������ ��ü���ֱ�
		
		if (lang_str == Copy_Text_Str[KOR_CODE]) { code = KOR_CODE; }
		else if (lang_str == Copy_Text_Str[JAP_CODE]) { code = JAP_CODE; }
		//*�ڵ� ���߱�

		T_List_Idx = Get_List_Index (fidx, tidx);
		list_str.Format (_T("%s[%06d]%s%s"), Prefix_Text_Str[code], tidx, T_Head_Dirty, txt_str);
		m_ListCtrl_Text.SetItemText (T_List_Idx, 0, list_str);
		//*�ش� ����Ʈ ��ġ�� ���
		
		if (!m_Text_Idx_In_Dir.File_Idx_Arr[fidx].Text_Idx_Arr[tidx].Is_Text_Dirty) {
			m_Text_Idx_In_Dir.File_Idx_Arr[fidx].Text_Idx_Arr[tidx].Is_Text_Dirty = true;
		}
		if (!m_Text_Idx_In_Dir.File_Idx_Arr[fidx].Is_FileTxt_Dirty) {
			m_Text_Idx_In_Dir.File_Idx_Arr[fidx].Is_FileTxt_Dirty = true;
		}
		if (!m_Text_Idx_In_Dir.Is_Dirty) { m_Text_Idx_In_Dir.Is_Dirty = true; }
		//*Dirty ���� ����
		
		Start_Pos = End_Pos + 6;
		//*���� ������ ���� ����

	}

	m_ListCtrl_Files.Invalidate();
	m_ListCtrl_Text.Invalidate();
	//*�ؽ�Ʈâ �ٽ� �׸���

}
//*Ŭ�����忡�� ���ڿ��� ���� ����Ʈ�� �ݿ��ϱ�, ���� ���ڿ��� ����




void CMFCProj2Dlg::Rid_Space (CString *str)
{
	bool NoTrivialSpace = false;
	unsigned int i, Mid_start, Mid_end;
	CString Midstr, bfr_chg, aft_chg;
	
	while (!NoTrivialSpace) {

		i = 0;
		while (i < ((unsigned int)str->GetLength() - 3)) {

			if ((str->GetBuffer()[i] == '\\') && (str->GetBuffer()[i+1] == ' ')){

				Mid_start = i + 2; i += 2;
				//*%s�� ���۵Ǵ� ��ġ ��� i ����

				while ((i < ((unsigned int)str->GetLength() - 1)) && 
					!((str->GetBuffer()[i] == ' ') && (str->GetBuffer()[i+1] == '['))) { i++; }
				if (i == (str->GetLength() - 1)) { NoTrivialSpace = true; break; }
				Mid_end = i; i += 2;
				//*%s�� ������ ��ġ ��� i ����. ������ ������ ��ġ�� ���� ������ ��� �׳� ������
				//*������ ù ��°, �� ��° while ���������� �ڵ������� ������

				Midstr = str->Mid (Mid_start, (Mid_end - Mid_start));
				bfr_chg = str->Mid (Mid_start - 2, (Mid_end - Mid_start + 4));
				aft_chg = _T("\\"); aft_chg += Midstr; aft_chg += _T("[");
				str->Replace (bfr_chg, aft_chg);
				break;
				//*%s�� �ش��ϴ� ���ڿ� ���. %s�� ���� ��ġ�� ������ ��ġ�� ���� ������
				//*���̰� �ִٴ� ���̹Ƿ� ���ڿ��� �ٲ۴�

			}
			else { 
				i++; 
				if (i == (str->GetLength() - 3)) { NoTrivialSpace = true; }
			}
			//'\ '�� �ִ� ��ġ ���, ������ �׳� �ѱ��
		}

	}

	//*��Ȯ���� '\ ' + %s + ' ['�� ���¸� ���� ���ڿ��� �ִٸ� '\' + %s + '['�� �ٲ��ִ� ��.
	//*%s�� �� ���ڿ��� ���̰� 0�̵� ���� ��� ����
}
//*�����Ǿ� �� �ؽ�Ʈ�� ���ǹ��� �� ������ �� �����ִ� �Լ�




void CMFCProj2Dlg::OnLbnSelchangeList2()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}


void CMFCProj2Dlg::OnNMThemeChangedScrollbar1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// �� ����� ����Ϸ��� Windows XP �̻��� �ʿ��մϴ�.
	// _WIN32_WINNT ��ȣ�� 0x0501���� ũ�ų� ���ƾ� �մϴ�.
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	*pResult = 0;
}