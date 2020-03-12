#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <tchar.h>
#include <stdio.h>
#include "resource.h"

union unIP
{
	DWORD adr;
	BYTE oct[4];
};

struct net_s
{
	TCHAR name[32];
	union unIP ip;
	union unIP mask;
};

typedef struct net_s NET;

NET* stNets = NULL;
INT cNets = 0;
DWORD dwAvIP;

BOOL CALLBACK DialogProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK AddNetProc(HWND, UINT, WPARAM, LPARAM);
TCHAR* DecIP(TCHAR* Buf, DWORD n);
TCHAR* BinIP(TCHAR* szBuf, DWORD dwIP);
DWORD GetMaskOfHosts(DWORD cHosts);
INT GetMaskPrefix(DWORD dwMask);
BOOL AddNet(NET stNet0, NET stNet, BOOL fSort);
VOID SortNets(VOID);
VOID ClrNets(VOID);

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpszCmdLine, INT nCmdShow)
{
	return DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DialogProc);
}

BOOL CALLBACK DialogProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam)
{
	static HWND hInMask;
	static HWND hFullMask;
	static HWND hNetList;
	TCHAR szDecIP[16] = { 0 };

	switch (uMessage)
	{
	case WM_INITDIALOG:
	{
		HICON hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(APPICON));
		SendMessage(hWnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);

		hInMask = GetDlgItem(hWnd, IDC_INFO_MASK);
		hFullMask = GetDlgItem(hWnd, IDC_FULL_MASK);
		hNetList = GetDlgItem(hWnd, IDC_NET_LIST);

		SetDlgItemText(hWnd, IDC_INFO_IP, TEXT("192.168.0.1"));

		TCHAR szMask[][21] =
		{
		TEXT(" /30 255.255.255.252"),
		TEXT(" /29 255.255.255.248"),
		TEXT(" /28 255.255.255.240"),
		TEXT(" /27 255.255.255.224"),
		TEXT(" /26 255.255.255.192"),
		TEXT(" /25 255.255.255.128"),
		TEXT(" /24 255.255.255.0"),
		TEXT(" /23 255.255.254.0"),
		TEXT(" /22 255.255.252.0"),
		TEXT(" /21 255.255.248.0"),
		TEXT(" /20 255.255.240.0"),
		TEXT(" /19 255.255.224.0"),
		TEXT(" /18 255.255.192.0"),
		TEXT(" /17 255.255.128.0"),
		TEXT(" /16 255.255.0.0"),
		TEXT(" /15 255.254.0.0"),
		TEXT(" /14 255.252.0.0"),
		TEXT(" /13 255.248.0.0"),
		TEXT(" /12 255.240.0.0"),
		TEXT(" /11 255.224.0.0"),
		TEXT(" /10 255.192.0.0"),
		TEXT(" /9 255.128.0.0"),
		TEXT(" /8 255.0.0.0"),
		TEXT(" /7 254.0.0.0"),
		TEXT(" /6 252.0.0.0"),
		TEXT(" /5 248.0.0.0"),
		TEXT(" /4 240.0.0.0"),
		TEXT(" /3 224.0.0.0"),
		TEXT(" /2 192.0.0.0"),
		TEXT(" /1 128.0.0.0")
		};

		for (INT i = 0; i < 30; i++)
		{
			SendMessage(hInMask, CB_ADDSTRING, 0, (LPARAM)szMask[i]);
			SendMessage(hFullMask, CB_ADDSTRING, 0, (LPARAM)szMask[i]);
		}

		SendMessage(hInMask, CB_SETCURSEL, 6, 0);

		const INT nTabs[3] = { 10, 100, 75 };
		SendMessage(hNetList, LB_SETTABSTOPS, 3, (LPARAM)nTabs);
		SendMessage(hNetList, LB_SETCOLUMNWIDTH, 400, 0);

		return TRUE;
	}
	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
		case IDC_ADD_BUTTON:
		{
			GetDlgItemText(hWnd, IDC_FULL_IP, szDecIP, sizeof(szDecIP));
			if (lstrcmp(szDecIP, TEXT("0.0.0.0")) == 0 || SendMessage(hFullMask, CB_GETCURSEL, 0, 0) == -1)
				MessageBox(hWnd, TEXT("Не выбрана сеть"), 0, MB_OK);
			else
				DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_ADD_NET_DIALOG), hWnd, AddNetProc);
			InvalidateRect(hWnd, NULL, FALSE);
			break;
		}
		case IDC_CLR_BUTTON:
		{
			ClrNets();
			dwAvIP = ~(0xFFFFFFFF << (SendMessage(hFullMask, CB_GETCURSEL, 0, 0) + 2)) + 1;
			InvalidateRect(hWnd, NULL, FALSE);
			break;
		}
		case IDC_INFO_MASK:
		{
			if (HIWORD(wParam) == CBN_SELCHANGE)
				InvalidateRect(hWnd, NULL, FALSE);
			break;
		}
		case IDC_INFO_IP:
		{
			if (HIWORD(wParam) == EN_CHANGE)
				InvalidateRect(hWnd, NULL, FALSE);
			break;
		}
		case IDC_FULL_IP:
		{
			if (HIWORD(wParam) == EN_CHANGE)
			{
				ClrNets();
				dwAvIP = ~(0xFFFFFFFF << (SendMessage(hFullMask, CB_GETCURSEL, 0, 0) + 2)) + 1;
				InvalidateRect(hWnd, NULL, FALSE);
			}
			break;
		}
		case IDC_FULL_MASK:
		{
			if (HIWORD(wParam) == CBN_SELCHANGE)
			{
				ClrNets();
				dwAvIP = ~(0xFFFFFFFF << (SendMessage(hFullMask, CB_GETCURSEL, 0, 0) + 2)) + 1;
				InvalidateRect(hWnd, NULL, FALSE);
			}
			break;
		}
		case IDC_NET_LIST:
		{
			if (HIWORD(wParam) == LBN_SELCHANGE)
			{
				INT index = SendMessage(hNetList, LB_GETCURSEL, 0, 0);
				SetDlgItemText(hWnd, IDC_INFO_IP, DecIP(szDecIP, stNets[index].ip.adr));
				SendMessage(hInMask, CB_SETCURSEL, 30 - GetMaskPrefix(stNets[index].mask.adr), 0);
				InvalidateRect(hWnd, NULL, FALSE);
			}
			break;
		}
		}
		return TRUE;
	}
	case WM_PAINT:
	{
		union unIP dwIP;
		DWORD dwMask;
		DWORD dwNetName;
		DWORD dwNetMin;
		DWORD dwNetMax;
		DWORD dwNetBroadcast;
		INT iMaskPref;
		DWORD dwNetHosts;
		TCHAR szBinIP[36] = { 0 };
		TCHAR szOutText[216] = { 0 };
		TCHAR szOutTmp[64] = { 0 };

		GetDlgItemText(hWnd, IDC_INFO_IP, szDecIP, sizeof(szDecIP));
		_stscanf(szDecIP, TEXT("%hhu.%hhu.%hhu.%hhu"), &dwIP.oct[3], &dwIP.oct[2], &dwIP.oct[1], &dwIP.oct[0]);

		iMaskPref = 30 - SendMessage(hInMask, CB_GETCURSEL, 0, 0);
		dwMask = 0xFFFFFFFF << (32 - iMaskPref);

		dwNetName = dwIP.adr & dwMask;
		dwNetMin = dwNetName + 1;
		dwNetBroadcast = dwNetName + ~dwMask;
		dwNetMax = dwNetBroadcast - 1;
		dwNetHosts = ~dwMask - 1;

		wsprintf(szOutText, TEXT("IP адрес\nПрефикс\nМаска\nИмя сети\nПервый IP\nПоследний IP\nBroadcast\nЧисло хостов"));
		SetDlgItemText(hWnd, IDC_OUT_TEXT, szOutText);

		wsprintf(szOutText, TEXT("%s\n"), DecIP(szDecIP, dwIP.adr));
		wsprintf(szOutTmp, TEXT("%d\n"), iMaskPref);
		lstrcat(szOutText, szOutTmp);
		wsprintf(szOutTmp, TEXT("%s\n"), DecIP(szDecIP, dwMask));
		lstrcat(szOutText, szOutTmp);
		wsprintf(szOutTmp, TEXT("%s\n"), DecIP(szDecIP, dwNetName));
		lstrcat(szOutText, szOutTmp);
		wsprintf(szOutTmp, TEXT("%s\n"), DecIP(szDecIP, dwNetMin));
		lstrcat(szOutText, szOutTmp);
		wsprintf(szOutTmp, TEXT("%s\n"), DecIP(szDecIP, dwNetMax));
		lstrcat(szOutText, szOutTmp);
		wsprintf(szOutTmp, TEXT("%s\n"), DecIP(szDecIP, dwNetBroadcast));
		lstrcat(szOutText, szOutTmp);
		wsprintf(szOutTmp, TEXT("%lu\n"), dwNetHosts);
		lstrcat(szOutText, szOutTmp);
		SetDlgItemText(hWnd, IDC_OUT_DEC, szOutText);

		wsprintf(szOutText, TEXT("%s\n"), BinIP(szBinIP, dwIP.adr));
		wsprintf(szOutTmp, TEXT("\n"));
		lstrcat(szOutText, szOutTmp);
		wsprintf(szOutTmp, TEXT("%s\n"), BinIP(szBinIP, dwMask));
		lstrcat(szOutText, szOutTmp);
		wsprintf(szOutTmp, TEXT("%s\n"), BinIP(szBinIP, dwNetName));
		lstrcat(szOutText, szOutTmp);
		wsprintf(szOutTmp, TEXT("%s\n"), BinIP(szBinIP, dwNetMin));
		lstrcat(szOutText, szOutTmp);
		wsprintf(szOutTmp, TEXT("%s\n"), BinIP(szBinIP, dwNetMax));
		lstrcat(szOutText, szOutTmp);
		wsprintf(szOutTmp, TEXT("%s\n"), BinIP(szBinIP, dwNetBroadcast));
		lstrcat(szOutText, szOutTmp);
		wsprintf(szOutTmp, TEXT("\n"));
		lstrcat(szOutText, szOutTmp);
		SetDlgItemText(hWnd, IDC_OUT_BIN, szOutText);

		SendMessage(hNetList, LB_RESETCONTENT, 0, 0);
		for (INT i = 0; i < cNets; i++)
		{
			wsprintf(szOutTmp, TEXT("%d\t%s\t%s/%d"), i + 1, stNets[i].name, DecIP(szDecIP, stNets[i].ip.adr), GetMaskPrefix(stNets[i].mask.adr));
			SendMessage(hNetList, LB_ADDSTRING, 0, (LPARAM)szOutTmp);
		}

		wsprintf(szOutTmp, TEXT("Доступно IP: %lu"), dwAvIP);
		SetDlgItemText(hWnd, IDC_ALL_IP, szOutTmp);

		return FALSE;
	}
	case WM_CLOSE:
	{
		free(stNets);
		EndDialog(hWnd, 0);
		return TRUE;
	}
	}
	return FALSE;
}

BOOL CALLBACK AddNetProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam)
{
	static HWND hNet0Mask;
	static HWND hPrev;
	NET stNet0;
	NET stNet;
	TCHAR szNet0IP[16];
	TCHAR szNetName[32];
	DWORD nNetHosts = 0;
	static INT cNetName = 1;

	switch (uMessage)
	{
	case WM_INITDIALOG:
	{
		hPrev = GetParent(hWnd);
		hNet0Mask = GetDlgItem(hPrev, IDC_FULL_MASK);

		wsprintf(szNetName, TEXT("Сеть %d"), cNetName);
		SetDlgItemText(hWnd, IDC_ADD_NAME, szNetName);
		return TRUE;
	}
	case WM_COMMAND:
	{
		if (LOWORD(wParam) == IDC_ADD_OK)
		{
			GetDlgItemText(hWnd, IDC_ADD_NAME, szNetName, sizeof(szNetName));
			nNetHosts = GetDlgItemInt(hWnd, IDC_ADD_HOSTS, NULL, FALSE);
			if (lstrlen(szNetName) == 0)
				MessageBox(hWnd, TEXT("Введите нвзвание сети"), 0, MB_OK);
			else
				if (nNetHosts == 0)
					MessageBox(hWnd, TEXT("Введите количество хостов в сети"), 0, MB_OK);
				else
				{
					GetDlgItemText(hPrev, IDC_FULL_IP, szNet0IP, sizeof(szNet0IP));
					_stscanf(szNet0IP, TEXT("%hhu.%hhu.%hhu.%hhu"), &stNet0.ip.oct[3], &stNet0.ip.oct[2], &stNet0.ip.oct[1], &stNet0.ip.oct[0]);
					stNet0.mask.adr = 0xFFFFFFFF << (SendMessage(hNet0Mask, CB_GETCURSEL, 0, 0) + 2);

					lstrcpy(stNet.name, szNetName);
					stNet.mask.adr = GetMaskOfHosts(nNetHosts);

					switch (AddNet(stNet0, stNet, TRUE))
					{
					case -1:
						MessageBox(hWnd, TEXT("Не хватает доступных адресов"), NULL, MB_ICONHAND | MB_OK);
						break;
					case 0:
						MessageBox(hWnd, TEXT("Ошибка выделения памяти"), NULL, MB_ICONHAND | MB_OK);
						SendMessage(hWnd, WM_CLOSE, 0, 0);
					case 1:
						cNetName++;
						SendMessage(hWnd, WM_CLOSE, 0, 0);
					}
				}
		}
		return TRUE;
	}
	case WM_CLOSE:
	{
		EndDialog(hWnd, 0);
		return TRUE;
	}
	}
	return FALSE;
}

TCHAR* DecIP(TCHAR* szBuf, DWORD dwIP)
{
	union unIP unTmp;
	unTmp.adr = dwIP;
	wsprintf(szBuf, TEXT("%d.%d.%d.%d"), unTmp.oct[3], unTmp.oct[2], unTmp.oct[1], unTmp.oct[0]);
	return szBuf;
}

TCHAR* BinIP(TCHAR* szBuf, DWORD dwIP)
{
	INT iIndex = 0;
	for (DWORD nBit = 0x80000000; nBit > 0; nBit >>= 1)
	{
		szBuf[iIndex++] = (dwIP & nBit) ? '1' : '0';
		if ((nBit == 0x01000000) | (nBit == 0x00010000) | (nBit == 0x00000100))
			szBuf[iIndex++] = '.';
	}
	szBuf[iIndex] = 0;
	return szBuf;
}

DWORD GetMaskOfHosts(DWORD cHosts)
{
	INT i = 0;
	cHosts++;
	while (cHosts != 1)
	{
		cHosts >>= 1;
		i++;
	}
	while (i > 0)
	{
		cHosts <<= 1;
		cHosts |= 1;
		i--;
	}
	return ~cHosts;
}

INT GetMaskPrefix(DWORD dwMask)
{
	INT nPrefix = 0;
	while ((dwMask << nPrefix) != 0)
		nPrefix++;
	return nPrefix;
}

VOID SortNets(VOID)
{
	NET stTemp;
	for (INT i = 0; i < cNets - 1; i++)
		for (INT j = 0; j < cNets - 1; j++)
			if (~stNets[j].mask.adr < ~stNets[j + 1].mask.adr)
			{
				stTemp = stNets[j];
				stNets[j] = stNets[j + 1];
				stNets[j + 1] = stTemp;
			}
}

INT AddNet(NET stNet0, NET stNet, BOOL fSort)
{
	if (~stNet.mask.adr + 1 > dwAvIP)
		return -1;
	else
	{
		stNets = (NET*)realloc(stNets, (cNets + 1) * sizeof(NET));
		if (stNets == NULL)
			return 0;
		else
		{
			stNets[cNets] = stNet;

			cNets++;
			dwAvIP -= ~stNet.mask.adr + 1;

			if (fSort)
				SortNets();

			stNets[0].ip.adr = stNet0.ip.adr & stNet0.mask.adr;
			for (INT i = 1; i < cNets; i++)
				stNets[i].ip.adr = stNets[i - 1].ip.adr + ~stNets[i - 1].mask.adr + 1;

			return 1;
		}
	}
}

VOID ClrNets(VOID)
{
	if (stNets)
		free(stNets);
	stNets = NULL;
	cNets = 0;
}
