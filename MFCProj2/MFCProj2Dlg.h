
// MFCProj2Dlg.h : ��� ����
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CMFCProj2Dlg ��ȭ ����
class CMFCProj2Dlg : public CDialogEx
{
// �����Դϴ�.
public:
	CMFCProj2Dlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_TFW_MAIN_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.


// �����Դϴ�.
protected:
	HICON m_hIcon;

	// ������ �޽��� �� �Լ�
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	DECLARE_MESSAGE_MAP()

public:

	CStatic m_OnList;
	CListCtrl m_ListCtrl_Files;
	CListCtrl m_ListCtrl_Text;
	CFont m_fontBold;

	afx_msg void OnBnClicked_Extract_Wolf();
	afx_msg void OnBnClicked_Make_Wolf();
	afx_msg void OnBnClicked_Change_Exe_File();

	afx_msg void OnBnClicked_Load_Text();
	afx_msg void OnBnClicked_Save_Text();
	afx_msg void OnBnClicked_Prepare_To_Translate();
	afx_msg void OnBnClicked_Auto_Trans_Papago();
//	afx_msg void OnBnClicked_Narrow_to_Em();
//	afx_msg void OnBnClicked_Check_Filename();
	afx_msg void OnBnClicked_Edit_GameDat_File();

	afx_msg void OnBnClicked_Import_List();
	afx_msg void OnBnClicked_Export_List();
	//*�� ��ư�� ���� �Լ�

	afx_msg void On_Files_List_Ctrl_NMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void On_Text_List_NMCustomdrawList(NMHDR *pNMHDR, LRESULT *pResult);
	//*�� �ٸ��� ����Ʈ �� ���� �Լ�

	afx_msg void On_Files_List_NMClickList(NMHDR *pNMHDR, LRESULT *pResult);
	//*���� ����Ʈ ���Ҹ� Ŭ�� �� ������ ������ �Լ�
	
	afx_msg void On_Text_List_NMClickList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void On_Text_List_NMDblclkList(NMHDR *pNMHDR, LRESULT *pResult);
	//*�ؽ�Ʈ ����Ʈ�� ���Ҹ� Ŭ��, ����Ŭ�� �� ������ ������ �Լ�

	void Copy_String_To_Clipboard (CString *str);
	void Get_String_From_Clipboard (CString *str);
	//*Ŭ�����忡�� ���ڿ��� �����ϴ� �Լ�, Ŭ�����忡�� ���ڿ��� ������ �Լ�

	void Replace_String ();
	//*����Ʈ ���ڿ��� �ٲ��ִ� �Լ�. ���� ���ڿ�, �ؽ�Ʈ ��� ����

	void Find_String (unsigned int Direction, CString Word);
	//*Ư�� ���ڿ��� ã���ִ� �Լ�. ���� ���ڿ�, �ؽ�Ʈ ��� ����

	void Collect_Clipboard_String_From_List ();
	void Set_List_String_From_Clipboard ();
	//*����Ʈ���� Ŭ���� ���ڿ� �����ؼ� Ŭ�����忡 �ٿ��ֱ�, Ŭ�����忡�� ���� ����Ʈ�� �ٿ��ֱ�

	void Rid_Space (CString *str);
	//*�����Ǿ� �� �ؽ�Ʈ�� ���ǹ��� �� ������ �� �����ִ� �Լ�

	afx_msg void OnLbnSelchangeList2();
	afx_msg void OnNMThemeChangedScrollbar1(NMHDR *pNMHDR, LRESULT *pResult);
};
