// Load_Text_Dialog.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "MFCProj2.h"
#include "Load_Text_Dialog.h"
#include "Select_CP_Dialog.h"
#include "afxdialogex.h"

#include "Wolf\Wolf_Text.h"

extern CListCtrl *m_G_ListCtrl_Files;
extern CListCtrl *m_G_ListCtrl_Text;
extern DIR_TXT_IDX m_Text_Idx_In_Dir;
//*extern���� ����Ʈ ������ ���� 



void Get_Fidx_Tidx (unsigned int Idx, unsigned int *fidx, unsigned int *tidx)
{
	unsigned int f_idx, t_idx = Idx;
	if (m_Text_Idx_In_Dir.File_Num == 0) { (*fidx) = (*tidx) = 0xFFFFFFFF; return; }
	for (f_idx = 0;f_idx < m_Text_Idx_In_Dir.File_Num;f_idx++) {
		if (m_Text_Idx_In_Dir.File_Idx_Arr[f_idx].Text_Cnt != 0) {
			if (t_idx >= (m_Text_Idx_In_Dir.File_Idx_Arr[f_idx].Text_Cnt + 1)) 
			{ t_idx -= (m_Text_Idx_In_Dir.File_Idx_Arr[f_idx].Text_Cnt + 1); }
			else { break; }
		}
	}
	(*fidx) = f_idx; (*tidx) = t_idx; return;
}
//*���� ���ڰ�(���� idx)�� �������� ���� �ε����� �ؽ�Ʈ �ε����� ��������ִ� �Լ�


int Get_List_Index (unsigned int fidx, unsigned int tidx)
{
	if (m_Text_Idx_In_Dir.File_Num <= fidx) { return -1; }
	if (m_Text_Idx_In_Dir.File_Idx_Arr[fidx].Text_Cnt <= tidx) { return -1; }

	int ret = 0;
	for (unsigned int i = 0;i < fidx;i++) {
		if (m_Text_Idx_In_Dir.File_Idx_Arr[i].Text_Cnt != 0) {
			ret += ((int)m_Text_Idx_In_Dir.File_Idx_Arr[i].Text_Cnt + 1);
		}
	}
	if (m_Text_Idx_In_Dir.File_Idx_Arr[fidx].Text_Cnt != 0) { ret += ((int)tidx + 1); }
	//+1�� �ٴ� ������ ���� �ؽ�Ʈ ��� ������

	return ret;
}
//*���� ���ڰ�(���� �ε���, �ؽ�Ʈ �ε���)�� �������� ����Ʈ �ε����� ��ȯ�ϴ� �Լ�


void Dir_Idx_Reset (DIR_TXT_IDX *dti)
{
	for (unsigned int i = 0;i < dti->File_Num;i++) {
		if (dti->File_Idx_Arr[i].Text_Cnt != 0) { free (dti->File_Idx_Arr[i].Text_Idx_Arr); }
	}
	if (dti->File_Num != 0) { free (dti->File_Idx_Arr); }
	//*�ؽ�Ʈ �Ѱ� ����ü ����

	dti->File_Num = 0;
	dti->File_Idx_Arr = NULL;
	//*�ؽ�Ʈ �Ѱ� ����ü �ʱ�ȭ
}


void All_List_Item_Delete()
{
	if (m_G_ListCtrl_Files->GetItemCount() != 0) { m_G_ListCtrl_Files->DeleteAllItems(); }
	if (m_G_ListCtrl_Text->GetItemCount() != 0) { m_G_ListCtrl_Text->DeleteAllItems(); }
	//*����Ʈ ���� ���� ����
}


void All_Text_Data_Reset ()
{
	Dir_Idx_Reset (&m_Text_Idx_In_Dir);
	//*m_Text_Idx_In_Dir ���� �ʱ�ȭ
}



// Load_Text_Dialog ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(Load_Text_Dialog, CDialogEx)

Load_Text_Dialog::Load_Text_Dialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(Load_Text_Dialog::IDD, pParent)
	, Load_Text_Thread(NULL)
{

}

Load_Text_Dialog::~Load_Text_Dialog()
{
}

void Load_Text_Dialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SCP_EDIT, m_FolderPath);
	DDX_Control(pDX, IDC_LT_PROGRESS1, m_Load_Text_Progress);
	DDX_Control(pDX, IDC_LT_TEXT, m_Load_Text_Print);
}


BEGIN_MESSAGE_MAP(Load_Text_Dialog, CDialogEx)
	ON_BN_CLICKED(IDOK, &Load_Text_Dialog::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON1, &Load_Text_Dialog::OnBnClicked_SelectFolder)
END_MESSAGE_MAP()


// Load_Text_Dialog �޽��� ó�����Դϴ�.


BOOL Load_Text_Dialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetDlgItemText (IDC_SCP_EDIT, _T(""));
	//*�ؽ�Ʈ ����

	m_Load_Text_Progress.SetRange32 (0, 100);
	//*���α׷��� �� ����

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}


void Load_Text_Dialog::OnBnClickedOk()
{
	CString Dir_Path;
	GetDlgItemText (IDC_SCP_EDIT, Dir_Path);
	if (Dir_Path.Compare(_T("")) == 0) { AfxMessageBox (_T("������ �����ϼ���.")); return; }
	//*���丮�� Ȯ��

	Load_Text_Thread = AfxBeginThread (Load_Text_Func, this);
	GetDlgItem(IDOK)->EnableWindow(FALSE);
	//*�ؽ�Ʈ �ε� ������ ����, Load ��ư ��Ȱ��ȭ
}


UINT Load_Text_Dialog::Load_Text_Func (LPVOID lParam) 
{
	Load_Text_Dialog *dlg = (Load_Text_Dialog*)lParam;
	//*��ü ���

	CString Dir_Path;
	dlg->GetDlgItemText (IDC_SCP_EDIT, Dir_Path);
	//*���丮�� ���

	if (Load_Tmptxt (dlg, Dir_Path) != 0) {
		
		Select_CP_Dialog sdlg;
		while (sdlg.DoModal() != IDOK) { ; }
		Load_Text_From_Directory_Data (dlg, Dir_Path, sdlg.CP_Code, true);
		//***�ڵ������� �����ϰ� ���丮���� ���� �ε�***

	}
	//*�ؽ�Ʈ �ε� �Լ� ȣ��. tmptxt ������ �ε��ߴٰ� �����ϸ� ���̾�α� â�� ��� �ڵ带 �����ϵ��� �Ѵ�

	m_G_ListCtrl_Files->Invalidate();
	m_G_ListCtrl_Text->Invalidate();
	//*�ؽ�Ʈâ �ٽ� �׸���

	dlg->GetDlgItemText (IDC_SCP_EDIT, dlg->m_FP_tmp);
	//*������ ����

	AfxMessageBox (_T("�ؽ�Ʈ �ε� �Ϸ�!"));
	dlg->GetDlgItem(IDOK)->EnableWindow(TRUE);
	//*�Ϸ� �޽��� ����, Load ��ư Ȱ��ȭ

	return 0;
}


// Load_Text_Dialog.cpp : ���� �����Դϴ�.
//

void Load_Text_Dialog::OnBnClicked_SelectFolder()
{

	BROWSEINFO BrInfo;
	TCHAR szBuffer[512];

	ZeroMemory (&BrInfo, sizeof(BROWSEINFO));
	ZeroMemory (szBuffer, 512);

	BrInfo.hwndOwner = m_hWnd;
	BrInfo.lpszTitle = _T("Select Folder to Load Text");
	BrInfo.ulFlags = BIF_NEWDIALOGSTYLE | BIF_RETURNONLYFSDIRS;
	BrInfo.lParam = (LPARAM)(_T("C:\\"));
	
	LPITEMIDLIST pItemIdList = SHBrowseForFolder (&BrInfo);
	SHGetPathFromIDList (pItemIdList, szBuffer);

	SetDlgItemText (IDC_SCP_EDIT, szBuffer);
	//*data�� ���� ���丮 ���� �� �ݿ�

	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}


void Load_Text_Dialog::OnCancel()
{
	DWORD ExitCode;

	if (Load_Text_Thread != NULL) {
		GetExitCodeThread (Load_Text_Thread->m_hThread, &ExitCode);
		if (ExitCode == STILL_ACTIVE) {
			
			Load_Text_Thread->SuspendThread();
			int ret = AfxMessageBox(_T("�ؽ�Ʈ �ε带 ���߰ڽ��ϱ�?"), MB_YESNO);
			//*���� ���� ���� �����尡 ���ư��� �ִٸ� �Ͻ�������Ų �� ����� �ų� ���´�.

			if (ret == IDYES) {
				TerminateThread (Load_Text_Thread->m_hThread, 0);
				CloseHandle (Load_Text_Thread->m_hThread);
				All_List_Item_Delete();
				All_Text_Data_Reset();
				//*����� ���� ����Ʈ �����͸� ����� �ؽ�Ʈ ����Ʈ�� �����ؾ� �Ѵ�
			}
			else {
				Load_Text_Thread->ResumeThread();
				return;
			}
			//*����Ѵٸ� �ٽ� �簳 �� ���ư���, �׸��дٸ� �״�� �����带 �����Ų �� OnCancel�� ȣ���Ѵ�

		}
	}

	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

	CDialogEx::OnCancel();
}
