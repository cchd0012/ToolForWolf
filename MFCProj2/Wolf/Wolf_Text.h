#include "../resource.h"
#include "../Load_Text_Dialog.h"
#include "../Save_Text_Dialog.h"
#include "../Change_FDName_Dialog.h"
#include "../Check_FDName_Dialog.h"

#include "Wolf_Functions.h"
#include "Wolf_Arclib.h"


int Load_Tmptxt (Load_Text_Dialog *LoadTtxDlg, CString DirPath);
//*�ش� ��ο��� tmptxt ������ �ҷ��� ����ϴ� �Լ� (���� : ���α׷����� ǥ���� ���̾�α�, �ҷ��� ������ ���)
//*���� �ε� : 0 ��ȯ, �ε� ���� : -1 ��ȯ

void Save_Tmptxt (CString DirPath);
//*�ش� ��ο� tmptxt ������ �����ϴ� �Լ� (���� : �ҷ��� ���� �ڵ�, DIR_TXT_IDX ������), üũ�� �ʿ� ����

void Load_Text_From_Directory_Data (Load_Text_Dialog *LoadTtxDlg, CString DirPath, unsigned int code, bool IsRoot);
//*���丮 ���� �ؽ�Ʈ�� ������ ������ ���� �ε����ִ� �Լ�

int Save_Text_To_Directory_Data (Save_Text_Dialog *SaveTxtDlg, CString DirPath);
//*������ �ؽ�Ʈ�� ���丮 ���� �����Ϳ� �������ִ� �Լ�

void Narrow_to_Em();
//*����Ʈ�� �ִ� �ؽ�Ʈ �� �ݰ��� ���� �������� �ٲ��ִ� �Լ�

void Check_Filename_For_Codepage (Check_FDName_Dialog *ChkDlg, unsigned int code);
//*�ش� �ڵ����������� ���� �̸��� ������ �ʴ°�, ������ � �̸����� �ٲ� �ǰ� ���ϵ��� �ϴ� �Լ�

void Load_Hanja ();
//*Hanja.txt ������ �ε��ϴ� �Լ�

void Release_Hanja ();
//*Hanja.txt ������ �����ϴ� �Լ�

void JPHan_2_KRHan (CString *str);
//*�ٲ� �� �ִ� �Ϻ����ڸ� ��� �ѱ����ڷ� �ٲٴ� �Լ�