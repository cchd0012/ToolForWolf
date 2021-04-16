#include "stdafx.h"

#include "Wolf_Functions.h"


int My_mkdir (char *Dir_buffer, unsigned int code) {
	wchar_t *ts;

	if (code == KOR_CODE) { ts = K2U(Dir_buffer); }
	else { ts = J2U(Dir_buffer); }

	int ret = _wmkdir(ts);	//���丮 �����(���ڿ��� �����ڵ�� �ٲ�� �ҵ�)
	free(ts);
	return ret;
	//*s-jis���� �����ڵ�� ��ȯ�Ͽ� ������ ���鵵�� �Ѵ�.
	//*�̴� ���߿� �ٲ� ���� �ִ�
}

int FD_Rename (wchar_t *OldFileName, wchar_t *NewFileName)
{
	unsigned int ret;
	unsigned int OldSlashNum, NewSlashNum;
	unsigned int OldSlashPoint, NewSlashPoint;
	unsigned int OldLen, NewLen, OldAccu, NewAccu;
	TCHAR tmpBuff [MAX_PATH];
	memcpy (tmpBuff, OldFileName, sizeof(TCHAR)*MAX_PATH);

	OldLen = (unsigned int)wcslen (OldFileName);
	NewLen = (unsigned int)wcslen (NewFileName);

	OldSlashNum = NewSlashNum = 0;
	for (unsigned int i = 0;(i < OldLen) || (i < NewLen);i++) {
		if (i < OldLen) { if ((OldFileName[i] == '\\') || (OldFileName[i] == '/')) { OldSlashNum++; } }
		if (i < NewLen) { if ((NewFileName[i] == '\\') || (NewFileName[i] == '/')) { NewSlashNum++; } }
	}
	if (OldSlashNum != NewSlashNum) { return -1; }
	//�� �������� ���� �ľ��ϰ�, ���� ������ -1�� ��ȯ

	OldSlashPoint = NewSlashPoint = 0;
	OldAccu = NewAccu = 0;
	if (OldSlashNum > 0) {
		for (unsigned int i = 0;i < OldSlashNum;i++) {

			for (unsigned int j = OldAccu;j < OldLen;j++) {
				if ((tmpBuff[j] == '\\') || (tmpBuff[j] == '/')) { 
					OldSlashPoint = j; OldAccu = j+1;
					break;
				} 
			}
			for (unsigned int j = NewAccu;j < NewLen;j++) {
				if ((NewFileName[j] == '\\') || (NewFileName[j] == '/')) { 
					NewSlashPoint = j; NewAccu = j+1;
					break;
				} 
			}
			//������ ���� ����

			tmpBuff[OldSlashPoint] = NewFileName[NewSlashPoint] = 0;
			//������ ���ֱ�

			if (_tcscmp (tmpBuff, NewFileName) != 0) {
				_wrename(tmpBuff, NewFileName);
				memcpy (tmpBuff, NewFileName, sizeof(TCHAR)*_tcslen(NewFileName));
			}
			//rename���� ���丮 ���� �ٲٱ�

			tmpBuff[OldSlashPoint] = NewFileName[NewSlashPoint] = '/';
			//�ٽ� ȸ����Ŵ

		}
		//�����ú��� ���� �����, ���丮���� ������� �ٲ۴�
	}

	ret = _wrename (tmpBuff, NewFileName);


	return ret;
	//���丮���� ���Ž�Ű�� rename
	//���� '\'�� ���� ���� ���̰� �� ��� �����ϵ��� �Ѵ�
}




unsigned int Get_File_Count(unsigned char *filenam_tok, unsigned int filenam_tok_size){
	unsigned int cnt = 0;
	unsigned char *tok_offset = filenam_tok;
	unsigned short fl_len, chksum, chksum_tp;
	while(tok_offset < (filenam_tok+filenam_tok_size)){
		cnt++;
		fl_len = *(unsigned short*)tok_offset;tok_offset += sizeof(short);
		chksum_tp = *(unsigned short*)tok_offset;tok_offset += sizeof(short);
		chksum = 0;
		for(unsigned short i = 0;i < fl_len*4;i++){chksum += *(tok_offset+i);}
		if(chksum_tp != chksum){return 0xFFFFFFFF;}
		tok_offset += (fl_len*8);
		if(tok_offset > (filenam_tok+filenam_tok_size)){return 0xFFFFFFFF;}
	}
	return cnt;
}

void Set_FileNames(FILESTR_DATA *filenam_coll, unsigned int filenam_coll_size, unsigned char *filenam_tok){
	unsigned int i;
	unsigned char *tok_offset = filenam_tok;
	for(i = 0;i < filenam_coll_size;i++){
		filenam_coll[i].start = (unsigned int)(tok_offset-filenam_tok);
		filenam_coll[i].len = *(unsigned short*)tok_offset;tok_offset += sizeof(short);

		filenam_coll[i].chksum = *(unsigned short*)tok_offset;tok_offset += sizeof(short);

		if(filenam_coll[i].len == 0){
			filenam_coll[i].big_str = (char*)malloc(sizeof(char));
			filenam_coll[i].big_str[0] = 0;
		}
		else{
			filenam_coll[i].big_str = (char*)malloc(sizeof(char)*filenam_coll[i].len*4);
			for(unsigned short j = 0;j < (filenam_coll[i].len*4);j++){filenam_coll[i].big_str[j] = (char)(tok_offset[j]);}
			tok_offset += (sizeof(char)*filenam_coll[i].len*4);
		}

		if(filenam_coll[i].len == 0){
			filenam_coll[i].real_str = (char*)malloc(sizeof(char));
			filenam_coll[i].real_str[0] = 0;
		}
		else{
			filenam_coll[i].real_str = (char*)malloc(sizeof(char)*filenam_coll[i].len*4);
			for(unsigned short j = 0;j < (filenam_coll[i].len*4);j++){filenam_coll[i].real_str[j] = (char)(tok_offset[j]);}
			tok_offset += (sizeof(char)*filenam_coll[i].len*4);
		}
	}
}

int Get_Last_SlashPos (char *pathname, unsigned int code){
	
	if(strlen(pathname) == 0){return -1;}

	if (code == KOR_CODE) {
		wchar_t *t = K2U (pathname);
		for(int i = (int)wcslen(t)-1;i >= 0;i--){
			if(t[i] == '\\'){ t[i] = ':'; break; }
		}

		char *ut = U2K (t);
		for(int i = (int)strlen(ut)-1;i >= 0;i--){
			if(ut[i] == ':'){ free (t); free (ut); return i; }
		}
		free (ut);
	}
	else {
		wchar_t *t = J2U (pathname);
		for(int i = (int)wcslen(t)-1;i >= 0;i--){
			if(t[i] == '\\'){ t[i] = ':'; break; }
		}

		char *ut = U2J (t);
		for(int i = (int)strlen(ut)-1;i >= 0;i--){
			if(ut[i] == ':'){ free (t); free (ut); return i; }
		}
		free (ut);
	}

	return 0;
}
//�̰� ����� ��ٷӴ�. �׷��Ƿ� �����ڵ� ��ȯ �� '?'�� �ٲٰ� �ٽ� ��Ƽ�ڵ�� ��ȯ�ϴ� �� ���� ����� �� �ʹ�

int Is_File_Or_Dir (wchar_t* ss) {
	_wfinddatai64_t c_file;
	intptr_t hFile;
	int result;

	if((hFile = _wfindfirsti64(ss, &c_file)) == -1L){result = -1;} // ���� �Ǵ� ���丮�� ������ -1 ��ȯ
	else{
		if (c_file.attrib & _A_SUBDIR){result = 0;}// ���丮�� 0 ��ȯ
		else{result = 1;}// �׹��� ���� "�����ϴ� ����"�̱⿡ 1 ��ȯ
	}
	_findclose(hFile);
	return result;
}

unsigned int Find_FileOrDirName_Idx (FILESTR_DATA *fsd, unsigned int fsd_size, unsigned int pos)
{
	unsigned int i, accm_val;

	accm_val = 0;
	for(i = 0;i < fsd_size;i++){
		if(pos == accm_val){ return i; }
		accm_val += (sizeof(short)*2);
		accm_val += (fsd[i].len*4*2);
	}
	return i;
}

void Converting_Big (char *dest, char *source, unsigned int length, unsigned int code)
{
	if (code == KOR_CODE) {
		wchar_t *t = K2U(source);
		for(unsigned int i = 0;i < length;i++){
			if((t[i] >= 'a') && (t[i] <= 'z')){ t[i] = t[i]-('a'-'A'); }
		}

		char *s = U2K (t); free (t);
		for(unsigned int i = 0;i < length;i++){ dest[i] = s[i]; } free (s);
	}
	else {
		wchar_t *t = J2U(source);
		for(unsigned int i = 0;i < length;i++){
			if((t[i] >= 'a') && (t[i] <= 'z')){ t[i] = t[i]-('a'-'A'); }
		}

		char *s = U2J (t); free (t);
		for(unsigned int i = 0;i < length;i++){ dest[i] = s[i]; } free (s);
	}
}




void Low_GameDat_Xor (unsigned char *Buff, unsigned int BuffSize)
{
	unsigned int Start_Idx = 0xA;
	unsigned char FirstChar = Buff[0];
	unsigned char SecondChar = Buff[8];
	unsigned char ThirdChar = Buff[6];
	unsigned int tempint;
	
	tempint = FirstChar;
	for (unsigned int i = Start_Idx;i < BuffSize;i++) {
		tempint = (tempint*0x343FD) + 0x269EC3;
		Buff[i] ^= (((tempint>>0x10) & 0x7000) >> 0xC);
	}
	
	tempint = SecondChar;
	for (unsigned int i = Start_Idx;i < BuffSize;i += 2) {
		tempint = (tempint*0x343FD) + 0x269EC3;
		Buff[i] ^= (((tempint>>0x10) & 0x7000) >> 0xC);
	}
	
	tempint = ThirdChar;
	for (unsigned int i = Start_Idx;i < BuffSize;i += 5) {
		tempint = (tempint*0x343FD) + 0x269EC3;
		Buff[i] ^= (((tempint>>0x10) & 0x7000) >> 0xC);
	}
}
//���� ������ ���� �� �۾�ü�� ���������� �ٲ� �� �ְ� �Ѵ�







wchar_t* J2U (char *string)
{
	wchar_t *bstr;
	int nLen = MultiByteToWideChar(932, 0, string, -1, NULL, NULL);
	bstr = (wchar_t*)malloc(sizeof(wchar_t)*(nLen+1));
	bstr[nLen] = 0;
	MultiByteToWideChar(932, 0, string, -1, bstr, nLen);
	return bstr;
}

wchar_t* K2U (char *string)
{
	wchar_t *bstr;
	int nLen = MultiByteToWideChar(949, 0, string, -1, NULL, NULL);
	bstr = (wchar_t*)malloc(sizeof(wchar_t)*(nLen+1));
	bstr[nLen] = 0;
	MultiByteToWideChar(949, 0, string, -1, bstr, nLen);
	return bstr;
}

char* U2K (wchar_t *string)
{
	char *lstr;
	int nLen = WideCharToMultiByte(949, 0, string, -1, NULL, NULL, NULL, NULL);
	lstr = (char*)malloc(sizeof(char)*(nLen+1));
	lstr[nLen] = 0;
	WideCharToMultiByte(949, 0, string, -1, lstr, nLen, NULL, NULL);
	return lstr;
}

char* U2J (wchar_t *string)
{
	char *lstr;
	int nLen = WideCharToMultiByte(932, 0, string, -1, NULL, NULL, NULL, NULL);
	lstr = (char*)malloc(sizeof(char)*(nLen+1));
	lstr[nLen] = 0;
	WideCharToMultiByte(932, 0, string, -1, lstr, nLen, NULL, NULL);
	return lstr;
}

wchar_t* UTF8_To_UTF16 (char *str)
{
	wchar_t *bstr;
	int nLen = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, NULL);
	bstr = (wchar_t*)malloc(sizeof(wchar_t)*(nLen+1));
	bstr[nLen] = 0;
	MultiByteToWideChar(CP_UTF8, 0, str, -1, bstr, nLen);
	return bstr;
}

char* UTF16_To_UTF8 (wchar_t *str)
{
	char *lstr;
	int nLen = WideCharToMultiByte (CP_UTF8, 0, str, -1, NULL, NULL, NULL, NULL);
	lstr = (char*)malloc(sizeof(char)*(nLen+1));
	lstr[nLen] = 0;
	WideCharToMultiByte (CP_UTF8, 0, str, -1, lstr, nLen, NULL, NULL);
	return lstr;
}






int GetWolfFileVersion (CString Wolf_Full_Path) 
{
	wchar_t *tp_path = (wchar_t*)(LPCTSTR)(Wolf_Full_Path);
	HANDLE hPath = CreateFileW(tp_path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hPath == INVALID_HANDLE_VALUE) { return -2; }
	//*������ ������ -2�� ��ȯ�Ѵ�

	if (GetFileSize (hPath, NULL) < sizeof(WOLF_BIG_HEADER)) { CloseHandle(hPath); return -1; }
	//*������ �������� -1�� ��ȯ�Ѵ�

	unsigned int Magic_Num, Magic_bak;
	unsigned char *pnt = (unsigned char*)(&Magic_Num);
	ReadFile (hPath, &Magic_Num, (DWORD)sizeof(unsigned int), NULL, NULL);
	Magic_bak = Magic_Num;
	CloseHandle(hPath);
	//*���� �ѹ� �����ŭ�� �б�

	for (unsigned int i = 0;i < sizeof(unsigned int);i++) { pnt[i] ^= WOLF_KEY[i%KEY_SIZE]; }
	if (Magic_Num == WOLF_MAGIC) { return DXA_LOW; }
	Magic_Num = Magic_bak;

	for (unsigned int i = 0;i < sizeof(unsigned int);i++) { pnt[i] ^= WOLF_KEY2[i%KEY_SIZE]; }
	if (Magic_Num == WOLF_MAGIC) { return DXA_220; }
	Magic_Num = Magic_bak;

	for (unsigned int i = 0;i < sizeof(unsigned int);i++) { pnt[i] ^= WOLF_KEY3[i%KEY_SIZE]; }
	if (Magic_Num == WOLF_MAGIC_64) { return DXA_330; }
	Magic_Num = Magic_bak;

	for (unsigned int i = 0;i < sizeof(unsigned int);i++) { pnt[i] ^= THMK2_KEY[i%KEY_SIZE]; }
	if (Magic_Num == DXA_MAGIC) { return DXA_THMK; }
	else if (Magic_Num == WOLF_MAGIC_64) { return DXA_THMK_64; }
	Magic_Num = Magic_bak;

	return -1;

	//*������ ���� DXA_LOW, DXA_220, DXA_330 �� �ϳ��� ��ȯ�Ѵ�. ��� �������� �ش���� ������ -1�� ��ȯ�Ѵ�
}




bool CanBeFile (char *FileName, int code) 
{				
	int lp = Get_Last_SlashPos (FileName, code);
	int sl = (int)strlen (FileName);
	for (int ll = sl - 1;ll >= lp;ll--) {
		if (FileName[ll] == '?') { return false; }
	}
	return true;
	//*������ �� �� �ִ��� Ȯ���Ͽ� ��ȯ�Ѵ�. ����ǥ ������ �ȵ�.
}




unsigned int Get_Total_FileCount (CString DirPath, CString Except) {
	WIN32_FIND_DATAW FData;
	HANDLE hFind;
	CString File_FullPath;
	unsigned int ret = 0;

	CString DirPath_ast = DirPath + _T("\\*");
	hFind = FindFirstFileW (DirPath_ast.GetBuffer(), &FData);
	do {
		File_FullPath = DirPath + _T("\\") + FData.cFileName;
		if (lstrcmp(FData.cFileName, Except.GetBuffer()) == 0){;}
		else if(!(FData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) 
			&& (lstrcmp(FData.cFileName, L".") != 0) 
			&& (lstrcmp(FData.cFileName, L"..") != 0))
		{ ret++; }
		else if((lstrcmp(FData.cFileName, L".") != 0) && (lstrcmp(FData.cFileName, L"..") != 0)) {
			ret += Get_Total_FileCount(File_FullPath, Except);
		}
	}while (FindNextFileW (hFind, &FData));
	FindClose (hFind);

	return ret;
	//*��������� ���ϱ�, �����̸� �ϳ� ���ϰ�, ���丮�� �� ���丮 �� �� ���� �� ���ؼ� ���ϱ�
}




int Get_Text_Count_Or_Set_Text (unsigned char *buff, unsigned int buffsize, unsigned int code, FILE_TXT_ELEM *fte)
{
	unsigned char *BuffPnt = buff;
	unsigned int tp_strlen;
	int ret = 0;
	bool IsZero;
	bool IsTextofLang;
	unsigned int IsOneEnter;				//*0xA������ ������ ��Ÿ�� �� ���̴� ����
	unsigned int i;

	while (BuffPnt < (buff + buffsize - sizeof(int))) {

		tp_strlen = *(unsigned int*)(BuffPnt);
		//*���� ���ڿ� ���̰� ���

		if ((tp_strlen >= ((unsigned int)(buff + buffsize) - (unsigned int)(BuffPnt + sizeof(int))))
			|| !((tp_strlen > 2) && (tp_strlen <= MAX_STRLEN))) {
			BuffPnt++; continue;
		}
		//*���ڿ� ���̰� ���� ���̺��� ������, �׸��� �ִ� ���̺��� ������
		//*���� �ʴٸ� �ϳ� ���ϰ� �ٽ� �ø���. 1~2����Ʈ(0~1����Ʈ + NULL)�� �ǹ̾���

		unsigned char *BuffPnt_sub = BuffPnt + sizeof(int);
		//*���ڿ��� �ִ� ��ġ ����

		IsZero = false; IsTextofLang = true;
		i = 0; IsOneEnter = 1;
		//*�⺻������ 0xA�� ����ó���� �Ѵ� ���� (1)
		while (i < tp_strlen) {

			if (BuffPnt_sub[i] == 0) { 
				if (i != (tp_strlen-1)) { IsZero = true; }
				break; 
			}
			//*�� ������ ����Ʈ���� 0�� �ƴ϶�� �ٷ� ������
			
			if (IS_ASCII (&BuffPnt_sub[i])) { i++; }
			else if (IS_ENTER (&BuffPnt_sub[i])) { 
				if (BuffPnt_sub[i+1] == 0xD) { IsOneEnter = 2; }
				//*0xD, 0xD�� 2�� �����Ѵ�
				else if (BuffPnt_sub[i+1] == 0xA) { IsOneEnter = 0; }
				//*0xD, 0xA�� ���̾� ���� ���� 0���� �����Ѵ�
				i += 2; 
			}
			else {

				if (code == JAP_CODE) {
					if (IS_SJIS (&BuffPnt_sub[i])) { i += 2; }
					else if (IS_NARROW_SJIS (&BuffPnt_sub[i])) { i++; }
					else { IsTextofLang = false; break; }
				}
				else if (code == KOR_CODE) {
					if (IS_CP949 (&BuffPnt_sub[i])) { i += 2; }
					else { IsTextofLang = false; break; }
				}

			}
			//***����ڵ忡 ���� �ش� ����ڵ� �ؽ�Ʈ���� �ƴ��� ���θ� �Ǵ��Ѵ�***

			if (i >= tp_strlen) { IsTextofLang = false; break; }
			//*i�� tp_strlen�� �Ѿ�� �ٷ� ������

		}

		if (IsZero || !IsTextofLang) { BuffPnt++; continue; }
		//*�� ������ ����Ʈ �� ���� üũ �߿� 0�� �־�� �� �ǰ�, �ش� ����� �ؽ�Ʈ�� �ƴ϶� �� �ȴ�.

		if (BuffPnt_sub[tp_strlen-1] != 0) { BuffPnt++; continue; }
		//*�� ������ ����Ʈ�� �ݵ�� 0�̾�� �Ѵ�.

		if (tp_strlen == 4) {
			if ((BuffPnt_sub[1] == 0x54) && (BuffPnt_sub[2] == 0x89)) { BuffPnt++; continue; }
			else if ((BuffPnt_sub[1] == 0xC6) && (BuffPnt_sub[2] == 0x2D)) { BuffPnt++; continue; }
			else if ((BuffPnt_sub[0] == 0x92) && (BuffPnt_sub[1] == 0x8E)) { BuffPnt++; continue; }
			else if ((BuffPnt_sub[0] == 0x8C) && (BuffPnt_sub[1] == 0xB9)) { BuffPnt++; continue; }
			else if ((BuffPnt_sub[0] == 0xA1) && (BuffPnt_sub[1] == 0x25)) { BuffPnt++; continue; }
		}
		else if (tp_strlen == 3) {
			if ((BuffPnt_sub[0] == 0xD) && (BuffPnt_sub[1] == 0xA)) { BuffPnt++; continue; }
			else if ((BuffPnt_sub[0] == 0x6A) && (BuffPnt_sub[1] == 0x42)) { BuffPnt++; continue; }
			else if ((BuffPnt_sub[0] == 0x84) && (BuffPnt_sub[1] == 0xAC)) { BuffPnt++; continue; }
			else if ((BuffPnt_sub[0] == 0xC8) && (BuffPnt_sub[1] == 0x32)) { BuffPnt++; continue; }
			else if ((BuffPnt_sub[0] == 0xCC) && (BuffPnt_sub[1] == 0x29)) { BuffPnt++; continue; }
			else if ((BuffPnt_sub[0] == 0xFA) && (BuffPnt_sub[1] == 0x63)) { BuffPnt++; continue; }
		}
		//***������ ���͸��� �����Ѵ�. ���Ǳ��� �� ��ġ������ �̰� �ɸ��� �ȵǴ� �ؽ�Ʈ���̴�.

		if (fte != NULL) {
			fte->Text_Idx_Arr[ret].Offset = (unsigned int)(BuffPnt - buff);
			fte->Text_Idx_Arr[ret].Length = tp_strlen;
			fte->Text_Idx_Arr[ret].Lang_Code = code;
			fte->Text_Idx_Arr[ret].File_Str_Idx = LOADING_FILE;
			fte->Text_Idx_Arr[ret].Is_OneChar_Enter = IsOneEnter;
		}
		//*FILE_TXT_ELEM �������� ���� NULL�� �ƴ϶�� ������ ���� ���ڿ� ����, �ڵ带 �״�� �����Ѵ�
		//*���� ���ڿ� �ε������� ���� ���� ���ڿ����� ���� �𸣹Ƿ� �ε� ���� �ǹ��� ������ �ο��Ѵ�

		ret++;
		BuffPnt += (sizeof(int) + tp_strlen);
		//*������ ����ߴٸ� �ؽ�Ʈ�� �����ϰ� ��� �ѱ��
	}
	//*������ ��� �����鼭 ������ ����(��ũ�� �̿�)
	//*���ڿ� ���� : [���̰�] + ���ڿ�(null ����)

	return ret;
}




unsigned int Get_Dir_Num_Or_DirList (CString Root_Dir, TCHAR **Dir_List) 
{
	WIN32_FIND_DATAW FData;
	HANDLE hFind;
	CString File_FullPath;
	unsigned int ret = 0;

	CString DirPath_ast = Root_Dir + _T("\\*");
	hFind = FindFirstFileW (DirPath_ast.GetBuffer(), &FData);
	do {
		File_FullPath = Root_Dir + _T("\\") + FData.cFileName;
		if((FData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) 
			&& (lstrcmp(FData.cFileName, L".") != 0) 
			&& (lstrcmp(FData.cFileName, L"..") != 0))
		{ 
			ret++; 
			if (Dir_List != NULL) { 
				for (unsigned int i = 0;;i++){ 
					if (wcslen(Dir_List[i]) == 0) { 
						memcpy (Dir_List[i], File_FullPath.GetBuffer(), (sizeof(TCHAR)*File_FullPath.GetLength())); 
						for (unsigned int j = 0;j < wcslen(Dir_List[i]);j++) { if (Dir_List[i][j] == '\\') { Dir_List[i][j] = '/'; } }
						Dir_List[i][wcslen(Dir_List[i])] = '/';
						break; 
					}
				}
			}
			//*���ڷ� ���� �� ������ ���丮���� �־��ش�, ������ �߰��� '/' �ϳ� �� �޾��ش�. �׷��� �̻��� ���ڿ��� �� �ٲ�
			ret += Get_Dir_Num_Or_DirList (File_FullPath, Dir_List); 
		}
	}while (FindNextFileW (hFind, &FData));
	FindClose (hFind);

	return ret;
}