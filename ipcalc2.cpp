//#undef UNICODE
#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <tchar.h>
#include <stdio.h>
#include "resource.h"

union ip_u
{
	DWORD adr;
	BYTE oct[4];
};

struct net
{
	union ip_u ip;
	union ip_u mask;
};

typedef struct net NET;

BOOL CALLBACK DialogProc(HWND, UINT, WPARAM, LPARAM);
TCHAR* dec(TCHAR* Buf, DWORD n);
TCHAR* bin(TCHAR* Buf, DWORD n);
DWORD get_mask(DWORD hosts);

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpszCmdLine, int nCmdShow)
{
	return DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DialogProc);
}

BOOL CALLBACK DialogProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam)
{
	static HWND hOutText;
	static HWND hOutDec;
	static HWND hOutBin;
	static HWND hInMask;
	static HWND hInIP;
	static HWND hFullIP;
	static HWND hFullMask;
	TCHAR ip_buf[16];
	TCHAR out_buf[216];
	TCHAR out_tmp[64];
	TCHAR dstr[16];
	TCHAR bstr[36];
	int net_prefix;
	NET net0;
	union ip_u net_name;
	union ip_u net_broadcast;

	switch (uMessage)
	{
	case WM_INITDIALOG:
		hOutText = GetDlgItem(hWnd, IDC_OUT_TEXT);
		hOutDec = GetDlgItem(hWnd, IDC_OUT_DEC);
		hOutBin = GetDlgItem(hWnd, IDC_OUT_BIN);
		hInMask = GetDlgItem(hWnd, IDC_INFO_MASK);
		hInIP = GetDlgItem(hWnd, IDC_INFO_IP);
		hFullMask = GetDlgItem(hWnd, IDC_FULL_MASK);

		SetWindowText(hInIP, TEXT("192.168.0.1"));

		SendMessage(hInMask, CB_ADDSTRING, 0, LPARAM(TEXT(" /32 255.255.255.255")));
		SendMessage(hInMask, CB_ADDSTRING, 0, LPARAM(TEXT(" /31 255.255.255.254")));
		SendMessage(hInMask, CB_ADDSTRING, 0, LPARAM(TEXT(" /30 255.255.255.252")));
		SendMessage(hInMask, CB_ADDSTRING, 0, LPARAM(TEXT(" /29 255.255.255.248")));
		SendMessage(hInMask, CB_ADDSTRING, 0, LPARAM(TEXT(" /28 255.255.255.240")));
		SendMessage(hInMask, CB_ADDSTRING, 0, LPARAM(TEXT(" /27 255.255.255.224")));
		SendMessage(hInMask, CB_ADDSTRING, 0, LPARAM(TEXT(" /26 255.255.255.192")));
		SendMessage(hInMask, CB_ADDSTRING, 0, LPARAM(TEXT(" /25 255.255.255.128")));
		SendMessage(hInMask, CB_ADDSTRING, 0, LPARAM(TEXT(" /24 255.255.255.0")));
		SendMessage(hInMask, CB_ADDSTRING, 0, LPARAM(TEXT(" /23 255.255.254.0")));
		SendMessage(hInMask, CB_ADDSTRING, 0, LPARAM(TEXT(" /22 255.255.252.0")));
		SendMessage(hInMask, CB_ADDSTRING, 0, LPARAM(TEXT(" /21 255.255.248.0")));
		SendMessage(hInMask, CB_ADDSTRING, 0, LPARAM(TEXT(" /20 255.255.240.0")));
		SendMessage(hInMask, CB_ADDSTRING, 0, LPARAM(TEXT(" /19 255.255.224.0")));
		SendMessage(hInMask, CB_ADDSTRING, 0, LPARAM(TEXT(" /18 255.255.192.0")));
		SendMessage(hInMask, CB_ADDSTRING, 0, LPARAM(TEXT(" /17 255.255.128.0")));
		SendMessage(hInMask, CB_ADDSTRING, 0, LPARAM(TEXT(" /16 255.255.0.0")));
		SendMessage(hInMask, CB_ADDSTRING, 0, LPARAM(TEXT(" /15 255.254.0.0")));
		SendMessage(hInMask, CB_ADDSTRING, 0, LPARAM(TEXT(" /14 255.252.0.0")));
		SendMessage(hInMask, CB_ADDSTRING, 0, LPARAM(TEXT(" /13 255.248.0.0")));
		SendMessage(hInMask, CB_ADDSTRING, 0, LPARAM(TEXT(" /12 255.240.0.0")));
		SendMessage(hInMask, CB_ADDSTRING, 0, LPARAM(TEXT(" /11 255.224.0.0")));
		SendMessage(hInMask, CB_ADDSTRING, 0, LPARAM(TEXT(" /10 255.192.0.0")));
		SendMessage(hInMask, CB_ADDSTRING, 0, LPARAM(TEXT(" /9 255.128.0.0")));
		SendMessage(hInMask, CB_ADDSTRING, 0, LPARAM(TEXT(" /8 255.0.0.0")));
		SendMessage(hInMask, CB_ADDSTRING, 0, LPARAM(TEXT(" /7 254.0.0.0")));
		SendMessage(hInMask, CB_ADDSTRING, 0, LPARAM(TEXT(" /6 252.0.0.0")));
		SendMessage(hInMask, CB_ADDSTRING, 0, LPARAM(TEXT(" /5 248.0.0.0")));
		SendMessage(hInMask, CB_ADDSTRING, 0, LPARAM(TEXT(" /4 240.0.0.0")));
		SendMessage(hInMask, CB_ADDSTRING, 0, LPARAM(TEXT(" /3 224.0.0.0")));
		SendMessage(hInMask, CB_ADDSTRING, 0, LPARAM(TEXT(" /2 192.0.0.0")));
		SendMessage(hInMask, CB_ADDSTRING, 0, LPARAM(TEXT(" /1 128.0.0.0")));
		SendMessage(hInMask, CB_ADDSTRING, 0, LPARAM(TEXT(" /0 0.0.0.0")));

		SendMessage(hFullMask, CB_ADDSTRING, 0, LPARAM(TEXT(" /32 255.255.255.255")));
		SendMessage(hFullMask, CB_ADDSTRING, 0, LPARAM(TEXT(" /31 255.255.255.254")));
		SendMessage(hFullMask, CB_ADDSTRING, 0, LPARAM(TEXT(" /30 255.255.255.252")));
		SendMessage(hFullMask, CB_ADDSTRING, 0, LPARAM(TEXT(" /29 255.255.255.248")));
		SendMessage(hFullMask, CB_ADDSTRING, 0, LPARAM(TEXT(" /28 255.255.255.240")));
		SendMessage(hFullMask, CB_ADDSTRING, 0, LPARAM(TEXT(" /27 255.255.255.224")));
		SendMessage(hFullMask, CB_ADDSTRING, 0, LPARAM(TEXT(" /26 255.255.255.192")));
		SendMessage(hFullMask, CB_ADDSTRING, 0, LPARAM(TEXT(" /25 255.255.255.128")));
		SendMessage(hFullMask, CB_ADDSTRING, 0, LPARAM(TEXT(" /24 255.255.255.0")));
		SendMessage(hFullMask, CB_ADDSTRING, 0, LPARAM(TEXT(" /23 255.255.254.0")));
		SendMessage(hFullMask, CB_ADDSTRING, 0, LPARAM(TEXT(" /22 255.255.252.0")));
		SendMessage(hFullMask, CB_ADDSTRING, 0, LPARAM(TEXT(" /21 255.255.248.0")));
		SendMessage(hFullMask, CB_ADDSTRING, 0, LPARAM(TEXT(" /20 255.255.240.0")));
		SendMessage(hFullMask, CB_ADDSTRING, 0, LPARAM(TEXT(" /19 255.255.224.0")));
		SendMessage(hFullMask, CB_ADDSTRING, 0, LPARAM(TEXT(" /18 255.255.192.0")));
		SendMessage(hFullMask, CB_ADDSTRING, 0, LPARAM(TEXT(" /17 255.255.128.0")));
		SendMessage(hFullMask, CB_ADDSTRING, 0, LPARAM(TEXT(" /16 255.255.0.0")));
		SendMessage(hFullMask, CB_ADDSTRING, 0, LPARAM(TEXT(" /15 255.254.0.0")));
		SendMessage(hFullMask, CB_ADDSTRING, 0, LPARAM(TEXT(" /14 255.252.0.0")));
		SendMessage(hFullMask, CB_ADDSTRING, 0, LPARAM(TEXT(" /13 255.248.0.0")));
		SendMessage(hFullMask, CB_ADDSTRING, 0, LPARAM(TEXT(" /12 255.240.0.0")));
		SendMessage(hFullMask, CB_ADDSTRING, 0, LPARAM(TEXT(" /11 255.224.0.0")));
		SendMessage(hFullMask, CB_ADDSTRING, 0, LPARAM(TEXT(" /10 255.192.0.0")));
		SendMessage(hFullMask, CB_ADDSTRING, 0, LPARAM(TEXT(" /9 255.128.0.0")));
		SendMessage(hFullMask, CB_ADDSTRING, 0, LPARAM(TEXT(" /8 255.0.0.0")));
		SendMessage(hFullMask, CB_ADDSTRING, 0, LPARAM(TEXT(" /7 254.0.0.0")));
		SendMessage(hFullMask, CB_ADDSTRING, 0, LPARAM(TEXT(" /6 252.0.0.0")));
		SendMessage(hFullMask, CB_ADDSTRING, 0, LPARAM(TEXT(" /5 248.0.0.0")));
		SendMessage(hFullMask, CB_ADDSTRING, 0, LPARAM(TEXT(" /4 240.0.0.0")));
		SendMessage(hFullMask, CB_ADDSTRING, 0, LPARAM(TEXT(" /3 224.0.0.0")));
		SendMessage(hFullMask, CB_ADDSTRING, 0, LPARAM(TEXT(" /2 192.0.0.0")));
		SendMessage(hFullMask, CB_ADDSTRING, 0, LPARAM(TEXT(" /1 128.0.0.0")));
		SendMessage(hFullMask, CB_ADDSTRING, 0, LPARAM(TEXT(" /0 0.0.0.0")));

		SendMessage(hInMask, CB_SETCURSEL, 8, 0);

		SendMessage(hWnd, WM_NET_INFO, 0, 0);

		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_INFO_MASK:
			if (HIWORD(wParam) == CBN_SELCHANGE)
			SendMessage(hWnd, WM_NET_INFO, 0, 0);
			break;

		case IDC_INFO_IP:
			if (HIWORD(wParam) == EN_CHANGE)
				SendMessage(hWnd, WM_NET_INFO, 0, 0);
			break;
		}
		return TRUE;

	case WM_NET_INFO:
		GetWindowText(hInIP, ip_buf, 16);
		_stscanf(ip_buf, TEXT("%hhu.%hhu.%hhu.%hhu"), &net0.ip.oct[3], &net0.ip.oct[2], &net0.ip.oct[1], &net0.ip.oct[0]);

		net_prefix = SendMessage(hInMask, CB_GETCURSEL, 0, 0);
		net0.mask.adr = 0xFFFFFFFF << net_prefix;

		net_name.adr = net0.ip.adr & net0.mask.adr;
		net_broadcast.adr = net_name.adr + ~net0.mask.adr;

		wsprintf(out_buf, TEXT("IP адрес\nПрефикс\nМаска\nИмя сети\nПервый IP\nПоследний IP\nBroadcast\nЧисло хостов"));
		SetWindowText(hOutText, out_buf);

		wsprintf(out_buf, TEXT("%s\n"), dec(dstr, net0.ip.adr));
		wsprintf(out_tmp, TEXT("%d\n"), 32 - net_prefix);
		lstrcat(out_buf, out_tmp);
		wsprintf(out_tmp, TEXT("%s\n"), dec(dstr, net0.mask.adr));
		lstrcat(out_buf, out_tmp);
		wsprintf(out_tmp, TEXT("%s\n"), dec(dstr, net_name.adr));
		lstrcat(out_buf, out_tmp);
		wsprintf(out_tmp, TEXT("%s\n"), dec(dstr, net_name.adr + 1));
		lstrcat(out_buf, out_tmp);
		wsprintf(out_tmp, TEXT("%s\n"), dec(dstr, net_broadcast.adr - 1));
		lstrcat(out_buf, out_tmp);
		wsprintf(out_tmp, TEXT("%s\n"), dec(dstr, net_broadcast.adr));
		lstrcat(out_buf, out_tmp);
		wsprintf(out_tmp, TEXT("%lu\n"), ~net0.mask.adr - 1);
		lstrcat(out_buf, out_tmp);
		SetWindowText(hOutDec, out_buf);

		wsprintf(out_buf, TEXT("%s\n"), bin(bstr, net0.ip.adr));
		wsprintf(out_tmp, TEXT("\n"));
		lstrcat(out_buf, out_tmp);
		wsprintf(out_tmp, TEXT("%s\n"), bin(bstr, net0.mask.adr));
		lstrcat(out_buf, out_tmp);
		wsprintf(out_tmp, TEXT("%s\n"), bin(bstr, net_name.adr));
		lstrcat(out_buf, out_tmp);
		wsprintf(out_tmp, TEXT("%s\n"), bin(bstr, net_name.adr + 1));
		lstrcat(out_buf, out_tmp);
		wsprintf(out_tmp, TEXT("%s\n"), bin(bstr, net_broadcast.adr - 1));
		lstrcat(out_buf, out_tmp);
		wsprintf(out_tmp, TEXT("%s\n"), bin(bstr, net_broadcast.adr));
		lstrcat(out_buf, out_tmp);
		wsprintf(out_tmp, TEXT("\n"));
		lstrcat(out_buf, out_tmp);
		SetWindowText(hOutBin, out_buf);
		return TRUE;

	case WM_CLOSE:
		EndDialog(hWnd, 0);
		return TRUE;
	}
	return FALSE;
}

TCHAR* dec(TCHAR* Buf, DWORD n)
{
	union ip_u tmp;
	tmp.adr = n;
	wsprintf(Buf, TEXT("%d.%d.%d.%d"), tmp.oct[3], tmp.oct[2], tmp.oct[1], tmp.oct[0]);
	return Buf;
}

TCHAR* bin(TCHAR* Buf, DWORD n)
{
	int r = 0;
	for (DWORD i = 0x80000000; i > 0; i >>= 1)
	{
		Buf[r++] = (n & i) ? '1' : '0';
		if ((i == 0x01000000) | (i == 0x10000) | (i == 0x100))
			Buf[r++] = '.';
	}
	Buf[r] = 0;
	return Buf;
}

DWORD get_mask(DWORD hosts)
{
	int i = 0;
	hosts += 2;
	while (hosts != 1)
	{
		hosts >>= 1;
		i++;
	}
	while (i > 0)
	{
		hosts <<= 1;
		hosts |= 1;
		i--;
	}
	return ~hosts;
}