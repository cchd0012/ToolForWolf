#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// GameDat_Edit_Dialog ��ȭ �����Դϴ�.

class GameDat_Edit_Dialog : public CDialogEx
{
	DECLARE_DYNAMIC(GameDat_Edit_Dialog)

public:
	GameDat_Edit_Dialog(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~GameDat_Edit_Dialog();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_EDIT_GAMEDAT_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:

	virtual BOOL OnInitDialog();
	virtual void OnOK();

	CEdit m_Game_Title;
	CEdit m_Font1;
	CEdit m_Font2;
	CEdit m_Font3;
	CEdit m_Font4;
	//*������ �ݿ��� �� ���� ����

	CString Game_Title;
	CString Font1;
	CString Font2;
	CString Font3;
	CString Font4;
	unsigned int type, code;
	//*�޾ƿ��ų� ������ �� ���� ����

	CButton m_GD_CPRadio1;
	CButton m_GD_CPRadio2;
};
