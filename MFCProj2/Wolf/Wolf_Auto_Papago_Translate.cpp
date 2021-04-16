#include "stdafx.h"

#include <map>
#include "Wolf_Auto_Papago_Translate.h"

#pragma warning (disable : 4996)

using namespace std;

struct MemoryStruct {
	char *memory;
	size_t size;
	size_t capacity;
};

#define BAD	 -1
#define DECODE64(c)  (isascii(c) ? base64val[c] : BAD)
#define UNIT_LENGTH 0x400

#define FORMDATA_KEY_VALUE_COUNT 10

static const char base64digits[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static const char uuidCharacters[] = "abcdefghijklmnopqrstuvwxyz0123456789";
char base64val[128];
static CkCrypt2 crypt;
static const char *papago_domain = "https://papago.naver.com/apis/n2mt/translate";
//*�ʿ��� ���� �� ���ǵ�

static const char* postParams [FORMDATA_KEY_VALUE_COUNT * 2] = {
	"deviceId", NULL,
	"locale", "ko",
	"dict", "false",
	"dictDisplay", "30",
	"honorific", "false",
	"instant", "false",
	"paging", "false",
	"source", NULL,
	"target", NULL,
	"text", NULL
};
//*x-www-form-urlencoded �������� �־��� key-value����
//*ù ��° NULL���� ���� ���� UUID, �� ��°�� ���� �ؽ�Ʈ�� ���(���⼭�� "ja")
//*�� ��° NULL���� ������ �ؽ�Ʈ�� ���(���⼭�� "ko"), �� ��° NULL�� UTF-8�� ���ڵ��� �ؽ�Ʈ��

static CURL *curl;
//*libcurl�� �̿��ϱ� ���� ����

static struct MemoryStruct m_chunk;
//*�����͸� ���� ����


extern CListCtrl *m_G_ListCtrl_Files;
extern CListCtrl *m_G_ListCtrl_Text;
extern DIR_TXT_IDX m_Text_Idx_In_Dir;
//*extern���� ����Ʈ ������ ���� 

extern TCHAR *Prefix_Text_Str[];
extern unsigned int Prefix_Length;
//*Prefix�� ���ڿ�

extern TCHAR *T_Head_Start;
extern TCHAR *T_Head_End;
//*�ؽ�Ʈ ����� ���ڿ�

extern TCHAR *T_Head_Dirty;
extern unsigned int Dirty_Prefix_Length;
//*Dirty Prefix ���ڿ�

extern int Get_List_Index (unsigned int fidx, unsigned int tidx);


TCHAR *GetTranslatedText (TCHAR *srcLang, TCHAR *destLang, TCHAR *originText);
//*���� ���� �Լ�


void encode_base64(unsigned char *out, unsigned char *in, int inlen)
{
	for (; inlen >= 3; inlen -= 3)										//3����Ʈ�� 6��Ʈ ������ ��� 4����Ʈ�� ����.
	{
		*out++ = base64digits[in[0] >> 2];								//0��° ��6�ڸ�
		*out++ = base64digits[((in[0] << 4) & 0x30) | (in[1] >> 4)];	//0��° ��2�ڸ� | 1��° ��4�ڸ�
		*out++ = base64digits[((in[1] << 2) & 0x3c) | (in[2] >> 6)];	//1��° ��4�ڸ� | 2��° ��2�ڸ�
		*out++ = base64digits[in[2] & 0x3f];							//2��° ��6�ڸ�
		in += 3;
	}

	if (inlen > 0)
	{
		unsigned char fragment;
		*out++ = base64digits[in[0] >> 2];
		fragment = (in[0] << 4) & 0x30;
		if (inlen > 1)  fragment |= in[1] >> 4;
		*out++ = base64digits[fragment];
		*out++ = (inlen < 2) ? '=' : base64digits[(in[1] << 2) & 0x3c];
		*out++ = '=';
	}
	*out = 0;
}
//*base64 ���ڵ�


static size_t WriteMemoryCallback (void *contents, size_t size, size_t nmemb, void *userp)
{
	size_t realsize = size * nmemb;
	struct MemoryStruct *mem = (struct MemoryStruct*)userp;
	if (mem->capacity < (realsize + mem->size)) {
		while (mem->capacity < (realsize + mem->size)) {
			mem->capacity += UNIT_LENGTH;
		}
		char *new_m = (char*)malloc (mem->capacity);
		memcpy (new_m, mem->memory, mem->size);
		free (mem->memory); mem->memory = new_m;
	}
	// �޸� ũ�⸦ �������� �����Ѵ�
	if (mem->memory == NULL) {
		printf ("Memory allocate Error\n"); return 0;
	}
	memcpy (&(mem->memory[mem->size]), contents, realsize);
	mem->size += realsize;
	mem->memory[mem->size] = 0;
	return realsize;
}
//*libcurl���� ��� �� �ݹ� �Լ�


int char2int (char input)
{
	if(input >= '0' && input <= '9'){ return input - '0'; }
	if(input >= 'A' && input <= 'F'){ return input - 'A' + 10; }
	if(input >= 'a' && input <= 'f'){ return input - 'a' + 10; }
	throw invalid_argument ("Invalid input string");
}

// This function assumes src to be a zero terminated sanitized string with
// an even number of [0-9a-f] characters, and target to be sufficiently large
void hex2bin (const char* src, char* target)
{
	while(*src && src[1])
	{
		*(target++) = char2int(*src)*16 + char2int(src[1]);
		src += 2;
	}
}

string hexStr(unsigned char* data, int len)
{
	stringstream ss;
	ss << hex;
	for(int i=0;i<len;++i) { ss << setw(2) << setfill('0') << (int)data[i]; }
	return ss.str();
}

char* stristr( const char* str1, const char* str2 )
{
	const char* p1 = str1;
	const char* p2 = str2;
	const char* r = (p2 == NULL) ? str1 : NULL;
	
	while( *p1 != 0 && *p2 != 0 )
	{
		if (tolower((unsigned char)*p1) == tolower((unsigned char)*p2))
		{
			if( r == 0 ) { r = p1; }
			p2++ ;
		}
		else
		{
			p2 = str2 ;
			if ( r != 0 ) { p1 = r + 1 ; }
			if (tolower((unsigned char)*p1) == tolower((unsigned char)*p2)) { r = p1 ; p2++ ; }
			else { r = 0 ; }
		}
		p1++ ;
	}
	return *p2 == 0 ? (char*)r : 0 ;
}
//*��Ÿ ��ɵ�


void translateInit()
{
	curl_global_init(CURL_GLOBAL_DEFAULT);
	curl = curl_easy_init();

	crypt.put_EncodingMode("hex");
	crypt.put_HashAlgorithm("md5");
	crypt.SetHmacKeyEncoded("v1.5.2_0d13cb6cf4", "ansi");

	curl_easy_setopt(curl, CURLOPT_URL, "https://papago.naver.com/apis/n2mt/translate");
	curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
	// url ���� �� ���� ��� ����
		
	curl_easy_setopt (curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
	curl_easy_setopt (curl, CURLOPT_WRITEDATA, (void*)&m_chunk);
	// ��Ͻ� �ݹ� �Լ��� ����� ���� ����

	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
	// ���� ����
}
//*�ʱ�ȭ �Լ�

void translateTerminate()
{
	curl_easy_cleanup (curl);
	curl_global_cleanup();
}
//*���� �Լ�



string getRandomUUID()
{
	ostringstream postBuf;
	for (unsigned int i = 0;i < 8;i++) { postBuf << uuidCharacters[rand() % 36]; }
	postBuf << '-';
	for (unsigned int i = 0;i < 4;i++) { postBuf << uuidCharacters[rand() % 36]; }
	postBuf << '-';
	for (unsigned int i = 0;i < 4;i++) { postBuf << uuidCharacters[rand() % 36]; }
	postBuf << '-';
	for (unsigned int i = 0;i < 4;i++) { postBuf << uuidCharacters[rand() % 36]; }
	postBuf << '-';
	for (unsigned int i = 0;i < 12;i++) { postBuf << uuidCharacters[rand() % 36]; }
	string ret_s = postBuf.str();
	postBuf << flush;
	return ret_s;
	// 8����Ʈ + '-' + 4����Ʈ + '-' + 4����Ʈ + '-' + 4����Ʈ + '-' + 12����Ʈ
	// ��� �ε��� : 26 ~ 61 (36��)
}
//*UUID ���� �Լ�


ULONGLONG getCurrentUCTTimeAsMillisecond()
{
	SYSTEMTIME systemTime;
	GetSystemTime (&systemTime);
	FILETIME fileTime;
	SystemTimeToFileTime (&systemTime, &fileTime);
	ULONGLONG fileTimeNano100;
	fileTimeNano100 = (((ULONGLONG) fileTime.dwHighDateTime) << 32) + fileTime.dwLowDateTime;
	ULONGLONG posixTime = fileTimeNano100/10000 - 11644473600000;
	return posixTime;
}
//*���� UTC �ð� �и��� ��ȯ �Լ�


void getEncodedToken (char *uuid, ULONGLONG millisecond, char *token_sav)
{
	char _t[100]; memset (_t, 0, 100); sprintf(_t, "%llu", millisecond);
	string cc = uuid; cc += "\n"; cc += papago_domain; cc += "\n"; cc += _t;
	const char *mac = crypt.hmacStringENC(cc.c_str());
	char hex[16]; memset (hex, 0, 16); hex2bin (mac, hex);
	char *st = (char*)malloc(24 + 1); memset (st, 0, 24 + 1);
	encode_base64 ((unsigned char*)token_sav, (unsigned char*)hex, 16);
}
//*��ȣȭ ��ū ��ȯ �Լ�


void Papago_Translate (Autotrans_Papago_Dialog *TransDlg, TCHAR *Translating_File, TCHAR *srcLang, TCHAR *destLang, unsigned int start_idx, unsigned int end_idx)
{
	// �ε����� �̿��� ���� ������ �����غ���
	map<CString, CString> cachier;

	TransDlg->SetDlgItemText (IDC_TRANS_PROG_TEXT, _T("Translating Progress : "));
	CString fstr, tstr, apply_Txt;
	
	unsigned int total_idx = end_idx - start_idx + 1;

	unsigned int File_Idx = 0;
	for (unsigned int i = 0;i < m_Text_Idx_In_Dir.File_Num;i++) {
		if (_tcsstr (m_Text_Idx_In_Dir.File_Idx_Arr[i].File_FullPath, Translating_File) != NULL) {
			File_Idx = i; break;
		}
	}
	//*������ ������ �ε��� ���ϱ�
		
	int T_List_Idx = Get_List_Index (File_Idx, 0);
	TransDlg->progressing_file_idx = File_Idx;
	//*���� ����

	for (unsigned int j = start_idx;j <= end_idx;j++) {

		CString tmpCStr; 
		tmpCStr.Format(_T("Text in File %s(%d/%d)"), Translating_File, j, m_Text_Idx_In_Dir.File_Idx_Arr[File_Idx].Text_Cnt);
		TransDlg->SetDlgItemText (IDC_TRANS_PROG_TEXT, tmpCStr);
		//*���� ���� ���� ���� ����

		TransDlg->m_Translate_Progress.SetPos (PROGRESSVALUE((j-start_idx), total_idx));
		//*���α׷��� �� ����

		tstr = m_G_ListCtrl_Text->GetItemText (j + m_Text_Idx_In_Dir.File_Idx_Arr[File_Idx].Idx_of_Text_Start + 1, 0);
		//*�ؽ�Ʈ

		if (m_Text_Idx_In_Dir.File_Idx_Arr[File_Idx].Text_Idx_Arr[j].File_Str_Idx == NONE_FILE) {
			CString srcLang, destLang;
			CString orgStr = _T(""), transStr = _T("");

			if (m_Text_Idx_In_Dir.File_Idx_Arr[File_Idx].Text_Idx_Arr[j].Is_Text_Dirty) {
				orgStr = tstr.Right (tstr.GetLength() - Prefix_Length - Dirty_Prefix_Length);
			}
			else {
				orgStr = tstr.Right (tstr.GetLength() - Prefix_Length);
			}
			//*��Ƽ ���ڿ�, prefix ���ڿ� ���� �� ����

			for (unsigned int t_rep = 0;t_rep < 3;t_rep++) {
				if (cachier.find(orgStr) == cachier.end()) {
					TransDlg->m_Src_Language_List.GetLBText (TransDlg->m_Src_Language_List.GetCurSel(), srcLang);
					TransDlg->m_Dst_Language_List.GetLBText (TransDlg->m_Dst_Language_List.GetCurSel(), destLang);
					TCHAR *trans_text = GetTranslatedText (srcLang.GetBuffer(), destLang.GetBuffer(), orgStr.GetBuffer());
					transStr = trans_text;
					free (trans_text);
					//*���ڰ� �ʿ� ������ ���� (������ �ؽ�Ʈ ���� �����Ǿ� ������ �̷��� �� ���� �� ����)
					cachier.insert(pair<CString, CString>(orgStr, transStr));
					//*���� �� �ʿ� �߰�
				}
				// �ʿ��� �� ã�� ��
				else { transStr = cachier[orgStr]; }
				// �ʿ��� ã�� ��
				if (transStr.GetLength() != 0) { break; }
			}

			if (transStr.GetLength() == 0) {
				tmpCStr.Format (_T("%s:[%06d] in %s:���� ����!"), orgStr, j, Translating_File);
				AfxMessageBox (tmpCStr);
			}
			//*���� ���� �����ϸ� �޽���â ���� ����������
			//*Ȥ�� ���� ��ַ� �ȵ��� ���� ������ 3������ ��õ��غ��� �͵� ������ �� ����
			//*���� ������ �ؽ�Ʈ �ε����� ����༭ �� ���� �����ؾ� ���� �˷��ش�

			else {
				m_Text_Idx_In_Dir.File_Idx_Arr[File_Idx].Text_Idx_Arr[j].Is_Text_Dirty = true;	
				apply_Txt.Format (_T("%s[%06d]%s%s"), 
					Prefix_Text_Str[m_Text_Idx_In_Dir.File_Idx_Arr[File_Idx].Text_Idx_Arr[j].Lang_Code], 
					j, T_Head_Dirty, transStr);
				m_G_ListCtrl_Text->SetItemText ((T_List_Idx + j), 0, apply_Txt);
				//*���� �Ŀ��� ������ ��Ƽ�� ������Ű�� ��Ƽ ���ڿ�, prefix �� �ٿ��� ����Ʈ�� �ݿ��Ѵ�
			}
		}
		//*���� ���ϸ��� �ƴ� ���� �����Ѵ�
	}

	if (!m_Text_Idx_In_Dir.File_Idx_Arr[File_Idx].Is_FileTxt_Dirty) {
		apply_Txt = T_Head_Dirty; apply_Txt += m_G_ListCtrl_Text->GetItemText ((T_List_Idx - 1), 0);
		m_G_ListCtrl_Text->SetItemText ((T_List_Idx - 1), 0, apply_Txt);
		m_Text_Idx_In_Dir.File_Idx_Arr[File_Idx].Is_FileTxt_Dirty = true;
	}
	//*�ݰ� ���ڰ� �ִٸ� ���� ����� Dirty ���·� �ٲٱ�
	//*ĳ�ÿ����� map�� ����� ������ �ű⼭ ��� ó���ϵ��� �Ѵ�

	if (!m_Text_Idx_In_Dir.Is_Dirty) {
		m_Text_Idx_In_Dir.Is_Dirty = true;
	}
	//*����, �������� Dirty ���� ����

	cachier.clear();
	TransDlg->m_Translate_Progress.SetPos (100);
	TransDlg->SetDlgItemText (IDC_TRANS_PROG_TEXT, _T("Translating Finished"));
	//*���α׷��� �� ����
}
//*���� ���� ���� �Լ�


TCHAR *GetTranslatedText (TCHAR *srcLang, TCHAR *destLang, TCHAR *originText)
{
	char *utf8txt = UTF16_To_UTF8 (originText);
	// ������ �ؽ�Ʈ UTF-8�� ��ȯ

	struct curl_slist *chunk = NULL;
	// ����Ʈ �ʵ�� ����

	m_chunk.memory = (char*)malloc (UNIT_LENGTH);
	memset (m_chunk.memory, 0, UNIT_LENGTH);
	m_chunk.capacity = UNIT_LENGTH;
	m_chunk.size = 0;
	// �̸� �����ϰ� �Ҵ�

	curl_easy_setopt (curl, CURLOPT_WRITEDATA, (void*)&m_chunk);
	// ��Ͻ� �ݹ� �Լ��� ����� ���� ����

	ULONGLONG t_set = getCurrentUCTTimeAsMillisecond();
	// ����̽� ���̵�(uuid)�� ����ð�
	// ����̽� ���̵�� ������ ���������ϹǷ� ������ �� �� �� �����ϰ� ��� ��Դ´�

	string deviceIdStr, millisecondStr, tokenStr;

	char _t[100];
	deviceIdStr = getRandomUUID();
	memset (_t, 0, 100); sprintf (_t, "%llu", t_set); millisecondStr = _t;
	memset (_t, 0, 100); getEncodedToken ((char*)deviceIdStr.c_str(), t_set, _t); tokenStr = _t;
	// ����̽� ���̵�(uuid)�� ���� �и��� ���صΰ� ��ū ���
	// string ��ü�� �����صα�

	postParams[1] = deviceIdStr.c_str();
	char src_lang[3], dest_lang[3];
	src_lang[0] = (char)srcLang[0]; src_lang[1] = (char)srcLang[1]; src_lang[2] = 0;
	dest_lang[0] = (char)destLang[0]; dest_lang[1] = (char)destLang[1]; dest_lang[2] = 0;
	postParams[15] = src_lang;
	postParams[17] = dest_lang;
	postParams[19] = utf8txt;
	// ����̽� ���̵�, ���� ���, ������ ���, �ؽ�Ʈ�� �޾ƿ� ���ڵ�� ����
		
	ostringstream postBuf;
	for (unsigned int i = 0;i < FORMDATA_KEY_VALUE_COUNT;i++) {
		char* key = curl_escape (postParams[i*2], 0);
		char* val = curl_escape (postParams[i*2+1], 0);
		postBuf << key << "=" << val;
		if (i < (FORMDATA_KEY_VALUE_COUNT-1)) { postBuf << "&"; }
		curl_free (key); curl_free (val);
		// ��� �ƴ� ���ڵ��� x-www-form-urlencoded �������� �ٲ�ġ��
		// �̰͵� ������ �Ҵ��̹Ƿ� �������� ���������ش�
	}
	// x-www-form-urlencoded �������� ����ϱ�

	string postData; postData = postBuf.str();
	postBuf << flush;
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.c_str());
	curl_easy_setopt(curl, CURLOPT_POST, 1L);
	// form ������ ����

	string author_Str = "PPG "; author_Str += deviceIdStr; author_Str += ":"; author_Str += tokenStr;
	// authorization�� ���� �̾Ƶα�

	string __t;
	chunk = curl_slist_append(chunk, "Accept: application/json");
	chunk = curl_slist_append(chunk, "Accept-Language: ko");
	__t = "Authorization: "; __t += author_Str;
	chunk = curl_slist_append(chunk, __t.c_str());
	chunk = curl_slist_append(chunk, "Connection: keep-alive");
	memset (_t, 0, 100); sprintf (_t, "%d", postData.length());
	__t = "Content-Length: "; __t += _t;
	chunk = curl_slist_append(chunk, __t.c_str());
	chunk = curl_slist_append(chunk, "Content-Type: application/x-www-form-urlencoded; charset=UTF-8");
	chunk = curl_slist_append(chunk, "device-type: pc");
	chunk = curl_slist_append(chunk, "Host: papago.naver.com");
	chunk = curl_slist_append(chunk, "Origin: https://papago.naver.com");
	chunk = curl_slist_append(chunk, "Referer: https://papago.naver.com/");
	chunk = curl_slist_append(chunk, "TE: Trailers");
	__t = "Timestamp: "; __t += millisecondStr;
	chunk = curl_slist_append(chunk, __t.c_str());
	chunk = curl_slist_append(chunk, "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:79.0) Gecko/20100101 Firefox/79.0");
	chunk = curl_slist_append(chunk, "x-apigw-partnerid: papago");
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
	// �ʿ��� ��� �߰� �� ����

	CURLcode rc = curl_easy_perform(curl);
	free (utf8txt);
	// ������Ʈ ����
		
	if(CURLE_OK != rc){
//		cerr << "Error from cURL: " << curl_easy_strerror (rc) << endl;
		free (m_chunk.memory);
		return NULL;
	}
	// ������ ������ �޽��� ���� ���ư�

	char *translatedText_h;
	if ((translatedText_h = stristr((const char*)m_chunk.memory, "\"translatedText\"")) != NULL) {
		char *translatedText = translatedText_h + strlen("\"translatedText\":\"");
		char *p = translatedText;
		while ((size_t)(p - m_chunk.memory) < m_chunk.size) { 
			if (*p == '"') {
				if (*(p-1) == '\\') { p++; continue; }
				else { break; }
			}
			else { p++; continue; }
		}
		// JSON������ ū����ǥ�� '\"'�� ���Ƿ� �װ� ����Ͽ� �ɷ�����
		// \r\n Ȥ�� \n���� �����Ǵ� ��찡 �ִµ� �̷����� �׳� ����(0xD, 0xA / 0xA)���� ó���ؾ� �� �� ����
		// �ű�� ���� \�� �ϳ��� �־��µ� 2���� �þ�� �����Ǵ� ��찡 �����
		// �ϴ� �̰͵��� �����ؼ� �����ϱ�� ����

		*p = 0;
		TCHAR *utf16txt = UTF8_To_UTF16 (translatedText);
		CString cs = utf16txt;
		cs.Replace (_T("\\r"), _T("\r"));
		cs.Replace (_T("\\n"), _T("\n"));
		cs.Replace (_T("\\\\"), _T("\\"));
		free (utf16txt);
		utf16txt = (TCHAR*)malloc (sizeof(TCHAR) * (cs.GetLength()+1));
		memcpy (utf16txt, cs.GetBuffer(), sizeof(TCHAR) * cs.GetLength());
		utf16txt[cs.GetLength()] = 0;

		curl_slist_free_all (chunk);
		free (m_chunk.memory);
		// �޸� ����

		return utf16txt;
		// (�� �� translatedText�� �ٷ� ������ �ؽ�Ʈ�� �ȴ�)
	}
	else {
		// �޽����� ����

		curl_slist_free_all (chunk);
		free (m_chunk.memory);
		// �޸� ����

		return NULL;
	}
	// translatedText�� �ִ� ���� ������ utf-8 -> utf-16���� ���ڵ��Ѵ�
	// ū����ǥ �ȿ� �ѷ����� ���� ������ �ɵ�
}
//*�������� ���� �Լ�