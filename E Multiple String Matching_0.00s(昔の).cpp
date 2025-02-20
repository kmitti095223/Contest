//=============================================================================
// インクルード
//=============================================================================
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#ifndef WINDOWS_DEBUG
#include <unistd.h>
#else
#include <io.h>
#endif

#ifdef WINDOWS_DEBUG
#include <windows.h>
LARGE_INTEGER m_Freq, m_Start, m_End;
int nCount1 = 0, nCount2 = 0;
#endif

//=============================================================================
// 定数
//=============================================================================
#define MAX_LEN_T			1000000					// 文字列 Tの最大長
#define MAX_LEN_T_PLUS_1	(MAX_LEN_T+1)			// 文字列 Tの最大長+1

#define MAX_LEN_P			1000					// 文字列 Pの最大長
#define MAX_LEN_P_PLUS_1	(MAX_LEN_P+1)			// 文字列 Pの最大長+1

#define MAX_COUNT_Q			10000					// 文字列 Pの数(Q)の最大値

#define MAX_COUNT_Q         10000
#define MAX_COUNT_PLUS_1    (MAX_COUNT_Q+1)

// 標準入力読み込みの最大サイズ
#define INPUT_SIZE			(MAX_LEN_T_PLUS_1 + ( MAX_LEN_P_PLUS_1 *MAX_COUNT_Q ))

// 文字を数値に変換するためのテーブル('0'→0、'1'→1、'A'→10 など)
const unsigned char CHAR_TO_NO[] = {
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
	0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x31, 0x32,
	0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D
};
//=============================================================================
// グローバル変数
//=============================================================================
int			g_nLenP;			// 文字列 Pの長さ
char		*g_pT;				// 入力値 文字列 T
int			g_nLenT;			// 入力値 文字列 T の長さ
int			g_nQ;				// 入力値 整数 Q
char		*pP;				// 入力値 文字列 P
int			g_nLenPMin;			// 入力値 文字列 P の最小
int			i;					// ループカウンタ
char		*p, *p1, *p2;		// ポインタ(汎用)
int			len;				// 文字列長(汎用)

char		*g_Table_Sentou;	// 先頭X文字のテーブル

void proc_char_3();	// 文字種類 > 2 用処理
void proc_char_2(); // 文字種類 = 2 用処理
void proc_char_1();	// 文字種類 = 1 用処理

// ソート用比較関数
int compare(const void* a, const void* b)
{
	return memcmp(*((char**)a), *((char**)b), MAX_LEN_P);
}

// 2文探索用比較関数
int compare2(const void* a, const void* b)
{
	return memcmp((char*)a, *((char**)b), g_nLenP);
}

// vecもどき
char** MyVec_data;
int MyVec_size; // 現在使用中の領域サイズ
int MyVec_reserve_size; // 予約領域サイズ

void vec_create(int size)
{
	MyVec_reserve_size = size;
	MyVec_data = (char**)malloc(sizeof(char*) * MyVec_reserve_size);
	MyVec_size = 0;
}

#define vec_push( pWk)													\
{																		\
	if (MyVec_size == MyVec_reserve_size)								\
			{															\
		MyVec_reserve_size = MyVec_reserve_size * 2;					\
		char** wk = (char**)malloc(sizeof(char*) * MyVec_reserve_size);	\
		memcpy(wk, MyVec_data, sizeof(int) * MyVec_size);				\
		free(MyVec_data);												\
		MyVec_data = wk;												\
			}															\
	MyVec_data[MyVec_size] = pWk;										\
	MyVec_size++;														\
}


//=============================================================================
// メイン関数
//=============================================================================
int main(void) {

#ifdef WINDOWS_DEBUG // デバッグ用処理時間計測
	QueryPerformanceFrequency(&m_Freq);
	QueryPerformanceCounter(&m_Start);
#endif

	//----------------------------------------------------------------------------
	// 標準入力読み込み
	//----------------------------------------------------------------------------
	// 標準入力をすべて読み込む。
	g_pT = (char*)malloc(sizeof(char*) * INPUT_SIZE);
	i = read(0, g_pT, INPUT_SIZE);
	g_pT[i] = 0x0A;
	
	// 文字列 T のアドレスを取得する。
	// 同時に文字の数をカウントする。
	char CharList['z' + 1] = { 0 };
	p1 = g_pT;
	while (*p1 != '\n')
	{
		CharList[*p1++] = 1;
	}
	*p1 = 0;
	g_nLenT = p1 - g_pT;
	p1++;

	int char_count = 0;
	for (i = '0'; i <= 'z'; i++)
	{
		char_count += CharList[i];
	}

	// 整数 Q (文字列 Pの数) を取得する。
	p2 = strchr(p1, '\n');
	*p2 = 0;
	g_nQ = atoi(p1);
	p2++;

	// 文字列 P の先頭アドレスを保持する。
	pP = p2;

	//----------------------------------------------------------------------------
	// 文字列 T に含まれる文字種類で処理分岐
	//----------------------------------------------------------------------------
	if (char_count  > 2)
	{
		// 文字列 T に含まれる文字種類 > 2 の場合
		proc_char_3();
	}
	else if (char_count == 2)
	{
		// 文字の種類が少ない(＝2)の場合のための特殊処理 ※ Case #11 対策
		proc_char_2();
	}
	else { // char_count == 1
		// 文字列 T に含まれる文字種類 ＝ 1 の場合
		proc_char_1();
	}

#ifdef WINDOWS_DEBUG // デバッグ用処理時間計測
	QueryPerformanceCounter(&m_End);
	printf("実行時間:%dms\n", (int)((double)(m_End.QuadPart - m_Start.QuadPart) * 1000.0 / m_Freq.QuadPart));
	printf("nCount1=%d  nCount2=%d\n", nCount1, nCount2);
	while (1) getchar();
#endif
}


//=============================================================================
// 文字列 T に含まれる文字種類 > 2 の処理
// ※先頭4文字(3バイト)をテーブルに格納して検索
//=============================================================================
void proc_char_3()
{
	//-----------------------------------------------------
	// 初期処理
	//-----------------------------------------------------
	// 変数準備など
	char* result = (char*)malloc((g_nQ + 1) * 2 + 1);
	char* p_result = result;
	memset(result, '\n', (g_nQ + 1) * 2 + 1);

	int x;
	p = p2;

	char** List_PtrP = (char**)malloc(sizeof(char*) * g_nQ);
	int* List_LenP = (int*)malloc(sizeof(int) * g_nQ);

	g_Table_Sentou = (char*)malloc(256 * 256 * 256);
	memset(g_Table_Sentou, 0, 256 * 256 * 256);

	// 配列初期化
	// ※初期値大きめで確保。不足したら追加。
	vec_create(10000);

	//-----------------------------------------------------
	// 文字列Qを元にList_PtrPを作成
	//-----------------------------------------------------
	for (i = 0; i < g_nQ; i++) {

		List_PtrP[i] = p;
		List_LenP[i] = strchr(p, '\n') - p;

		x = CHAR_TO_NO[*(p++)];
		if (List_LenP[i] > 1)
		{
			x += (CHAR_TO_NO[*(p++)] << 6);
			if (List_LenP[i] > 2)
			{
				x += (CHAR_TO_NO[*(p++)] << 12);
				if (List_LenP[i] > 3)
				{
					x += (CHAR_TO_NO[*(p++)] << 18);
					if (List_LenP[i] > 4)
					{
						g_Table_Sentou[x] = 10; 
						p = List_PtrP[i] + List_LenP[i] + 1;
						continue;
					}
				}
			}
		}

		g_Table_Sentou[x] = (char)List_LenP[i] << 1;
		p = List_PtrP[i] + List_LenP[i] + 1;
	}

	//-----------------------------------------------------
	// 文字列Tの1文字目、2文字目・・・を順に配列に格納
	// 同時にg_Table_Sentouを更新
	//-----------------------------------------------------
	p = g_pT;
	len = g_nLenT - 3;
	if (len>0)
	{
		do
		{
			x = CHAR_TO_NO[*p];
			if (g_Table_Sentou[x] == 2) // 長さ=1
				g_Table_Sentou[x] |= 1;	// 先頭1バイトが一致していればフラグを立てる

			x += (CHAR_TO_NO[*(p + 1)] << 6);
			if (g_Table_Sentou[x] == 4) // 長さ=2
				g_Table_Sentou[x] |= 1;

			x += (CHAR_TO_NO[*(p + 2)] << 12);
			if (g_Table_Sentou[x] == 6) // 長さ=3
				g_Table_Sentou[x] |= 1;

			x += (CHAR_TO_NO[*(p + 3)] << 18);
			if (g_Table_Sentou[x] == 8) // 長さ=4
				g_Table_Sentou[x] |= 1;

			// 長さ5以上
			if (g_Table_Sentou[x] && (g_Table_Sentou[x] != 9)){
				g_Table_Sentou[x] |= 1;
				vec_push(p);
			}
		} while (*(++p + 3));

	}
	// ループ内の判定を減らすため最後の3文字はループの外で処理
	// (本関数は3文字以上でしか呼ばれない)
	
	// 後ろから3文字目
	x = CHAR_TO_NO[*p];
	if (g_Table_Sentou[x] == 2)
		g_Table_Sentou[x] |= 1;

	x += (CHAR_TO_NO[*(p + 1)] << 6);
	if (g_Table_Sentou[x] == 4)
		g_Table_Sentou[x] |= 1;

	x += (CHAR_TO_NO[*(p + 2)] << 12);
	if (g_Table_Sentou[x] == 6)
		g_Table_Sentou[x] |= 1;
	p++;

	//---------------------------
	// 後ろから2文字目
	x = CHAR_TO_NO[*p];
	if (g_Table_Sentou[x] == 2)
		g_Table_Sentou[x] |= 1;

	x += (CHAR_TO_NO[*(p + 1)] << 6);
	if (g_Table_Sentou[x] == 4)
		g_Table_Sentou[x] |= 1;
	p++;

	//---------------------------
	// 一番後ろの文字
	x = CHAR_TO_NO[*p];
	if (g_Table_Sentou[x] == 2)
		g_Table_Sentou[x] |= 1;

	//-----------------------------------------------------
	// 文字列 Tの配列をソート
	//-----------------------------------------------------
	qsort((void*)MyVec_data, MyVec_size, sizeof(const char*), compare);

	//-----------------------------------------------------
	// 文字列 T から、文字列 P を検索
	//-----------------------------------------------------
	for (i = 0; i < g_nQ; i++) {

		if (List_LenP[i] > 4)
		{
			// 文字列Pが5文字以上の場合
			if (!(g_Table_Sentou[CHAR_TO_NO[*List_PtrP[i]]
				+ (CHAR_TO_NO[*(List_PtrP[i] + 1)] << 6)
				+ (CHAR_TO_NO[*(List_PtrP[i] + 2)] << 12)
				+ (CHAR_TO_NO[*(List_PtrP[i] + 3)] << 18)] & 1))
			{
				// g_Table_Sentouにフラグが立ってない場合、
				// 検索するまでもなく「無し」。
				*p_result = '0';
			}
			else
			{
				// 2分探索
				g_nLenP = List_LenP[i];
				*p_result = '0' + (bsearch(List_PtrP[i], MyVec_data, MyVec_size, sizeof(const char*), compare2) > 0);
			}
		}
		else {

			if (List_LenP[i] == 1) {
				// 文字列Pが1文字の場合
				*p_result = '0' + (g_Table_Sentou[CHAR_TO_NO[*List_PtrP[i]]] & 1);
			}
			else if (List_LenP[i] == 2) {
				// 文字列Pが2文字の場合
				*p_result = '0' + (g_Table_Sentou[CHAR_TO_NO[*List_PtrP[i]]
					+ (CHAR_TO_NO[*(List_PtrP[i] + 1)] << 6)] & 1);
			}
			else if (List_LenP[i] == 3) {
				// 文字列Pが3文字の場合
				*p_result = '0' + (g_Table_Sentou[CHAR_TO_NO[*List_PtrP[i]]
					+ (CHAR_TO_NO[*(List_PtrP[i] + 1)] << 6)
					+ (CHAR_TO_NO[*(List_PtrP[i] + 2)] << 12)] & 1);
			}
			else {
				// 文字列Pが3文字の場合
				*p_result = '0' + (g_Table_Sentou[CHAR_TO_NO[*List_PtrP[i]]
					+ (CHAR_TO_NO[*(List_PtrP[i] + 1)] << 6)
					+ (CHAR_TO_NO[*(List_PtrP[i] + 2)] << 12)
					+ (CHAR_TO_NO[*(List_PtrP[i] + 3)] << 18)] & 1);
			}
		}

		p_result += 2;
	}

	// 結果出力用文字列の末尾にNULLを入れて出力。
	*(p_result - 1) = 0;
	puts(result);
}

//=============================================================================
// 文字列 T に含まれる文字種類 ＝ 2 の処理
// ※先頭10文字(10ビット)をテーブルに格納して検索
//=============================================================================
void proc_char_2()
{
	int x;
	g_nLenPMin = MAX_LEN_P;

	vec_create(50000);

	g_Table_Sentou = (char*)malloc(sizeof(char) * 2048);
	memset(g_Table_Sentou, 0, 1024 * sizeof(char));

	// 存在する文字の1文字目を取得
	// ※ 例えば "a" と "b"の場合 "a" の文字コードを取得する。
	char c = g_pT[0];

	for (i = 0; i < g_nQ; i++) {
		p1 = strchr(p2, '\n');

		len = p1 - p2;

		if (len < g_nLenPMin)
		{
			g_nLenPMin = len;
		}

		x = (*(p2) == c);
		if (len > 1) x += (((*(p2 + 1)) == c) << 1);
		if (len > 2) x += (((*(p2 + 2)) == c) << 2);
		if (len > 3) x += (((*(p2 + 3)) == c) << 3);
		if (len > 4) x += (((*(p2 + 4)) == c) << 4);
		if (len > 5) x += (((*(p2 + 5)) == c) << 5);
		if (len > 6) x += (((*(p2 + 6)) == c) << 6);
		if (len > 7) x += (((*(p2 + 7)) == c) << 7);
		if (len > 8) x += (((*(p2 + 8)) == c) << 8);
		if (len > 9) x += (((*(p2 + 9)) == c) << 9);
		if (len > 10) x += (((*(p2 + 10)) == c) << 10);

		g_Table_Sentou[x] = 1;

		p1++;
		p2 = p1;
	}

	//  文字列 T の重複した部分を除去する。
	char* pNewT = (char*)malloc(MAX_LEN_T_PLUS_1);
	memcpy(pNewT, g_pT, g_nLenT);
	pNewT[g_nLenT] = 0;

	// p2 = pNewT;
	char SameChar = '?';
	int SameCount = 0;
	int k = 0;
	int nSitchCount = 0;

	p2 = g_pT;
	for (i = 0; i < g_nLenT; i++)
	{
		if (*p2 == SameChar)
		{
			if (SameCount <= MAX_LEN_P)
			{
				pNewT[k++] = *p2;
				SameCount++;
			}
		}
		else
		{
			pNewT[k++] = *p2;
			SameChar = *p2;
			SameCount = 0;
			nSitchCount++;
			if (nSitchCount > 2)
			{
				nSitchCount = -1;
				break;
			}
		}
		p2++;
	}

	if (nSitchCount >= 0)
	{
		pNewT[k++] = 0;
		g_nLenT = k;
		memcpy(g_pT, pNewT, g_nLenT);
	}

	//  文字列 T を 0 or 1 に置換する。
	char* pZeroOne = (char*)malloc(MAX_LEN_T_PLUS_1);
	memcpy(pZeroOne, g_pT, g_nLenT);
	pZeroOne[g_nLenT] = 0;

	p2 = pZeroOne;
	for (i = 0; i < g_nLenT; i++)
	{
		*p2 = *p2 == c;
		p2++;
	}

	p = g_pT;
	p2 = pZeroOne;
	len = g_nLenT - 10;
	for (i = 0; i < len; i++)
	{
		x = 0;

		while (1)
		{
			// 1つ前と同じ場合は追加しない。
			if (strcmp(p, p - 1) == 0)
			{
				break;
			}

			x = *(p2);
			x += *(p2 + 1) << 1;
			x += (*(p2 + 2) << 2);
			x += (*(p2 + 3) << 3);
			x += (*(p2 + 4) << 4);
			x += (*(p2 + 5) << 5);
			x += (*(p2 + 6) << 6);
			x += (*(p2 + 7) << 7);
			x += (*(p2 + 8) << 8);
			x += (*(p2 + 9) << 9);
			x += (*(p2 + 10) << 10);

			if (g_nLenPMin <= 10)	if (g_Table_Sentou[x]) { vec_push(p); break; }

			break;
		}

		p++;
		p2++;
	}

	for (i; i < g_nLenT; i++)
	{
		// i文字目以降の文字列のポインタ
		vec_push(p);
		p++;
		p2++;
	}

	// 文字列 T をソート
	qsort((void*)MyVec_data, MyVec_size, sizeof(const char*), compare);

	// 文字列 T から、文字列 P を検索
	p = pP;

	for (i = 0; i < g_nQ; i++) {
		p1 = strchr(p, '\n');
		g_nLenP = p1 - p;

		if (bsearch(p, MyVec_data, MyVec_size, sizeof(const char*), compare2))
			printf("1\n");
		else
			printf("0\n");

		p = p1 + 1;
	}
}

//=============================================================================
// 文字列 T に含まれる文字種類 ＝ 1 の処理
// ※文字列1つを配列に格納して検索
//=============================================================================
void proc_char_1()
{
	// 配列に追加 ※全て同じなので1つだけ
	vec_create(1);
	vec_push(g_pT);

	// 文字列 T をソート
	qsort((void*)MyVec_data, MyVec_size, sizeof(const char*), compare);

	// 検索
	p = pP;
	for (i = 0; i < g_nQ; i++) {

		p1 = strchr(p, '\n');
		g_nLenP = p1 - p;

		if (bsearch(p, MyVec_data, MyVec_size, sizeof(const char*), compare2))
			printf("1\n");
		else
			printf("0\n");

		p = p1 + 1;
	}
}