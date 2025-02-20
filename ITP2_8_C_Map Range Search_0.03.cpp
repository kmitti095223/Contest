//=============================================================================
// インクルード
//=============================================================================
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <set>

#ifndef WINDOWS_DEBUG
#include <unistd.h>
#else
#include <io.h>
#pragma warning(disable : 4996)
#endif

#ifdef WINDOWS_DEBUG
#include <windows.h>
LARGE_INTEGER m_Freq, m_Start, m_End;
int nCount1 = 0, nCount2 = 0;
#endif

//=============================================================================
// 定義
//=============================================================================
// 文字コードを数値に変換
#define CHAR_TO_INDEX(c)	(c - 'a' + 1)

typedef struct
{
	char	*pPtr;
	int		nTotalLen;
	int		nValueLen;
} INF;


// setの定義
typedef std::set<INF*, bool(*)(const INF*, const INF*)> MYSET;

//=============================================================================
// 文字列長取得 (半角スペース、改行、NULLまでの長さ)
//=============================================================================
inline int StrCmp(const char* p1, const char* p2) {

	for (int i = 0;; i++) {
		if (p1[i] < '0') {
			if (p2[i] < '0')
				return 0;
			else
				return -1;
		}

		if (p2[i] < '0') {
			return 1;
		}

		if (p1[i] != p2[i])
		{
			if (p1[i] > p2[i])
				return 1;
			else
				return -1;
		}
	}
}

inline int StrCmp3(const char* p1, const char* p2) {

	for (int i = 3;; i++) {
		if (p1[i] < '0') {
			if (p2[i] < '0')
				return 0;
			else
				return -1;
		}

		if (p2[i] < '0') {
			return 1;
		}

		if (p1[i] != p2[i])
		{
			if (p1[i] > p2[i])
				return 1;
			else
				return -1;
		}
	}
}

//=============================================================================
// 文字列の先頭3文字から配列インデックスを決定
//=============================================================================
inline int GetIndex(char* mac_p) {

	int nIndex =  (CHAR_TO_INDEX(*mac_p) << 10);
	if (*(mac_p + 1) >= 'a') {
		nIndex += (CHAR_TO_INDEX(*(mac_p + 1)) << 5);
		if (*(mac_p + 2) >= 'a')
			nIndex += (CHAR_TO_INDEX(*(mac_p + 2)));
	}

	return nIndex;
}

class InfCompare{
public:
	bool operator()(const INF  * p1, const INF  * p2){
		return StrCmp3(p1->pPtr, p2->pPtr) < 0;
	}
};

// setの定義
typedef std::set<INF*, InfCompare> MYSET2;
MYSET2 set3[20000];
int set2_count = 0;

//std::set<INF*, bool(*)(const INF*, const INF*)> set2([](const INF* p1, const INF* p2)
//{
//	return StrCmp3(p1->pPtr, p2->pPtr) < 0;
//}
//);


//=============================================================================
// メイン処理
//=============================================================================
int main(void) {

#ifdef WINDOWS_DEBUG // デバッグ用処理時間計測
	QueryPerformanceFrequency(&m_Freq);
	QueryPerformanceCounter(&m_Start);
#endif

	//--------------------------------------------------------
	// 変数定義、初期処理
	//--------------------------------------------------------
	// 標準入力読み込み
	const int MAX_SIZE = 10 * 1000 * 1000; // 10MBで足りるはず
	char* pAll = (char*)malloc(MAX_SIZE);
	int i = read(0, pAll, MAX_SIZE);
	pAll[i] = '\n';

	// 標準入力のアドレス範囲を取得
	char* pAllS = pAll;
	char* pAllE = pAll + MAX_SIZE;

	// データ数を取得
	int nCount = atoi(pAll);
	char* p = strchr(pAll, '\n') + 1;

	// 配列を作成
	const int TABLE_SIZE = 32768;
	INF* ppTable = new INF[TABLE_SIZE];
	memset(ppTable, 0, sizeof(INF) * TABLE_SIZE);

	// 出力バッファ作成 (10MBで足りるはず)
	char* szResult = new char[10 * 1000 * 1000];
	char* pResult = szResult;

	// set用比較関数定義
	bool(*Compare)(const INF*, const INF*) = [](const INF* p1, const INF* p2) {

		return StrCmp3(p1->pPtr, p2->pPtr) < 0;
		//for (int i = 0;; i++) {
		//	if (p1->pPtr[i] < '0') {
		//		if (p2->pPtr[i] < '0')
		//			return false;
		//		else
		//			return true;
		//	}

		//	if (p2->pPtr[i] < '0') {
		//		return false;
		//	}

		//	if (p1->pPtr[i] != p2->pPtr[i])
		//	{
		//		if (p1->pPtr[i] > p2->pPtr[i])
		//			return false;
		//		else
		//			return true;
		//	}
		//}
	};

	INF *pInf = new INF[200000];
	memset(pInf,0, sizeof(INF) * 200000);
	int nInfNo = 0;

	char* p2;
	const INF* pItr;

	//--------------------------------------------------------
	// メイン処理 (データ数分ループ)
	//--------------------------------------------------------
	for (i = 0; i < nCount; i++) {

		switch (*p) {
		case '0': // insert
		{
			p += 2;
			int nIndex = GetIndex(p);
			p2 = strchr(p, ' ')+1;

			if (ppTable[nIndex].pPtr == 0) {
				// 配列にデータが無い場合
				ppTable[nIndex].pPtr = p;
				
				p = strchr(p2, '\n') + 1;
				ppTable[nIndex].nValueLen = p - p2;
				ppTable[nIndex].nTotalLen = (p2 - ppTable[nIndex].pPtr) + ppTable[nIndex].nValueLen;
			}
			else if ((ppTable[nIndex].pPtr >= pAllS) && (ppTable[nIndex].pPtr <= pAllE)) {

				// 配列にchar*が入っている場合
				if (StrCmp(p, ppTable[nIndex].pPtr) == 0) {
					// 同じものがあれば上書き
					ppTable[nIndex].pPtr = p;

					p = strchr(p2, '\n') + 1;
					ppTable[nIndex].nValueLen = p - p2;
					ppTable[nIndex].nTotalLen = (p2 - ppTable[nIndex].pPtr) + ppTable[nIndex].nValueLen;
				}
				else {
					// 違うものが入っていればset化
					MYSET2* pSetWk = &set3[set2_count++];

					memcpy(&pInf[nInfNo], &ppTable[nIndex], sizeof(INF));
					pSetWk->insert(&pInf[nInfNo]);
					nInfNo++;

					pInf[nInfNo].pPtr = p;
					p = strchr(p2, '\n') + 1;
					pInf[nInfNo].nValueLen = p - p2;
					pInf[nInfNo].nTotalLen = (p2 - pInf[nInfNo].pPtr) + pInf[nInfNo].nValueLen;

					pSetWk->insert(&pInf[nInfNo]);
					nInfNo++;

					ppTable[nIndex].pPtr = (char*)pSetWk;


					ppTable[nIndex].nValueLen = 0;
					ppTable[nIndex].nTotalLen = 0;
				}
			}
			else {
				// 配列にset*が入っている場合、上書きする。(消してから入れる)
				pInf[nInfNo].pPtr = p;
				p = strchr(p2, '\n') + 1;
				pInf[nInfNo].nValueLen = p - p2;
				pInf[nInfNo].nTotalLen = (p2 - pInf[nInfNo].pPtr) + pInf[nInfNo].nValueLen;

				((MYSET2*)ppTable[nIndex].pPtr)->erase(&pInf[nInfNo]);
				((MYSET2*)ppTable[nIndex].pPtr)->insert(&pInf[nInfNo]);
				// https://zenn.dev/nariakiiwatani/articles/33152127379b3e

				nInfNo++;

				ppTable[nIndex].nValueLen = 0;
				ppTable[nIndex].nTotalLen = 0;
			}

			
			break;
		}
		case '1': // get
		{
			p += 2;
			int nIndex = GetIndex(p);
			p2 = strchr(p, '\n') + 1;
			*(p2 - 1) = 0;

			if (ppTable[nIndex].pPtr == 0) {
				// 配列にデータが無い場合、0を出力
				memcpy(pResult, "0\n", 2);
				pResult += 2;
			}
			else if ((ppTable[nIndex].pPtr >= pAllS) && (ppTable[nIndex].pPtr <= pAllE))
			{
				// 配列にchar*が入っている場合、出力対象であれば出力
				if (StrCmp(p, ppTable[nIndex].pPtr) == 0) {
					memcpy(pResult, ppTable[nIndex].pPtr + ppTable[nIndex].nTotalLen - ppTable[nIndex].nValueLen, ppTable[nIndex].nValueLen);
					pResult += ppTable[nIndex].nValueLen;
				}
				else {
					memcpy(pResult, "0\n", 2);
					pResult += 2;
				}
			}
			else
			{
				// 配列にset*が入っている場合、出力対象をSetから取得して出力
				pInf[nInfNo].pPtr = p;

				MYSET2* pSet = ((MYSET2*)ppTable[nIndex].pPtr);
				auto itr = pSet->find(&pInf[nInfNo]);
				if (itr == pSet->end()) {

					memcpy(pResult, "0\n", 2);
					pResult += 2;
				}
				else {
					memcpy(pResult, (*itr)->pPtr + (*itr)->nTotalLen - (*itr)->nValueLen, (*itr)->nValueLen);
					pResult += (*itr)->nValueLen;
				}
			}

			p = p2;
			break;
		}
		case '2': // delete
		{
			p += 2;
			int nIndex = GetIndex(p);
			p2 = strchr(p, '\n')+1;
			*(p2 - 1) = 0;

			if (ppTable[nIndex].pPtr == 0)
			{
				// 配列にデータが無い場合、何もしない
			}
			else if (ppTable[nIndex].nTotalLen) {

				// 配列にchar*が入っている場合、削除対象と同じであればクリア
				if (StrCmp(p, ppTable[nIndex].pPtr) == 0) {
					ppTable[nIndex].pPtr = 0;
				}
			}
			else
			{
				// 配列にset*が入っている場合、setから削除
				pInf[nInfNo].pPtr = p;
				((MYSET2*)ppTable[nIndex].pPtr)->erase(&pInf[nInfNo]);
				nInfNo++;

				/*if (((MYSET*)ppTable[nIndex].pPtr)->size() == 1) {
					memcpy(&ppTable[nIndex], *((MYSET*)ppTable[nIndex].pPtr)->begin(), sizeof(INF));
				}
				else if (((MYSET*)ppTable[nIndex].pPtr)->size() == 0) {
					ppTable[nIndex].pPtr = 0;
				}*/
			}

			p = p2;
			break;
		}
		case '3': // dump
		{
			p += 2;
			int nIndex = GetIndex(p);
			char* pS = p;
			char* pE = strchr(pS, ' ');
			*pE = 0;
			pE++;
			p = strchr(pE, '\n');
			*p = 0;
			p++;

			int nIndexS = nIndex;
			int nIndexE = GetIndex(pE);

			//-------------------------------------------------------------------------------------
			while (1)
			{
				int j = nIndexS;
				if (ppTable[j].pPtr == 0) {
					// 配列にデータが無い場合、何もしない
					break;
				}
				else if (ppTable[j].nTotalLen) {

					// 配列にchar*が入っている場合、文字列が範囲内であれば出力
					if ((StrCmp(ppTable[j].pPtr, pS) >= 0)
						&& (StrCmp(ppTable[j].pPtr, pE) <= 0)) {
						memcpy(pResult, ppTable[j].pPtr, ppTable[j].nTotalLen);
						pResult += ppTable[j].nTotalLen;
					}
				}
				else {
					// 配列にset*が入っている場合、文字列が範囲内であれば出力
					MYSET2* pSet = ((MYSET2*)ppTable[j].pPtr);
					for (auto itr = pSet->begin(); itr != pSet->end(); ++itr) {

						pItr = *itr;
						if (StrCmp(pItr->pPtr, pS) >= 0) {
							if (StrCmp(pItr->pPtr, pE) <= 0) {
								memcpy(pResult, pItr->pPtr, pItr->nTotalLen);
								pResult += (pItr->nTotalLen);
							}
							else {
								break;
							}
						}
					}
				}
				break;
			}

			//-------------------------------------------------------------------------------------
			for (int j = nIndexS+1; j <= nIndexE-1; j++)
			{
				if (ppTable[j].pPtr == 0) {
					// 配列にデータが無い場合、何もしない
					continue;
				}
				else if (ppTable[j].nTotalLen) {

					// 配列にchar*が入っている場合、文字列が範囲内であれば出力
					memcpy(pResult, ppTable[j].pPtr, ppTable[j].nTotalLen);
					pResult += ppTable[j].nTotalLen;
				}
				else {
					// 配列にset*が入っている場合、文字列が範囲内であれば出力
					MYSET2* pSet = ((MYSET2*)ppTable[j].pPtr);
					for (auto itr = pSet->begin(); itr != pSet->end(); ++itr) {

						pItr = *itr;
						memcpy(pResult, pItr->pPtr, pItr->nTotalLen);
						pResult += (pItr->nTotalLen);
					}
				}
			}

			//-------------------------------------------------------------------------------------
			while (nIndexS != nIndexE)
			{
				int j = nIndexE;

				if (ppTable[j].pPtr == 0) {
					// 配列にデータが無い場合、何もしない
					break;
				}
				else if (ppTable[j].nTotalLen) {

					// 配列にchar*が入っている場合、文字列が範囲内であれば出力
					if ((StrCmp(ppTable[j].pPtr, pS) >= 0)
						&& (StrCmp(ppTable[j].pPtr, pE) <= 0)) {
						memcpy(pResult, ppTable[j].pPtr, ppTable[j].nTotalLen);
						pResult += ppTable[j].nTotalLen;
					}
				}
				else {
					// 配列にset*が入っている場合、文字列が範囲内であれば出力
					MYSET2* pSet = ((MYSET2*)ppTable[j].pPtr);
					for (auto itr = pSet->begin(); itr != pSet->end(); ++itr) {

						pItr = *itr;
						if (StrCmp(pItr->pPtr, pS) >= 0) {
							if (StrCmp(pItr->pPtr, pE) <= 0) {
								memcpy(pResult, pItr->pPtr, pItr->nTotalLen);
								pResult += (pItr->nTotalLen);
							}
							else {
								break;
							}
						}
					}
				}
				break;
			}

			//-------------------------------------------------------------------------------------

			// 結果を出力(バッファをはみ出さないようdumpごとに出力)
			if (pResult != szResult) {
				*(pResult - 1) = 0;
				puts(szResult);
				pResult = szResult;
			}

			break;
		}
		default:
			assert(0);
			break;
		}
	}

	// 結果を出力
	if (pResult != szResult) {
		*(pResult - 1) = 0;
		puts(szResult);
	}

#ifdef WINDOWS_DEBUG // デバッグ用処理時間計測
	QueryPerformanceCounter(&m_End);
	printf("実行時間:%dms\n", (int)((double)(m_End.QuadPart - m_Start.QuadPart) * 1000.0 / m_Freq.QuadPart));
	printf("nCount1=%d  nCount2=%d\n", nCount1, nCount2);
	while (1) getchar();
#endif
}
