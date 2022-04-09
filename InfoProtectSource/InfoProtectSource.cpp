#include <stdio.h>
#include <tchar.h>
#include <windows.h>



DWORD Tag = 1837694739;


void embed(LPCWSTR szTextFileName, LPCWSTR szBMPFileName)
{
	BOOL res = CopyFile(szBMPFileName, L"enc.bmp", 0);
	if (res == 0)
	{
		printf("Error copy file! %d", GetLastError());
		return;
	}

	HANDLE hBMPFile = CreateFile(szBMPFileName, GENERIC_READ | GENERIC_WRITE | GENERIC_EXECUTE, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (hBMPFile == INVALID_HANDLE_VALUE)
	{
		printf("Error open bmp file! %d", GetLastError());
		return;
	}

	HANDLE hTextFile = CreateFile(szTextFileName, GENERIC_READ | GENERIC_WRITE | GENERIC_EXECUTE, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (hTextFile == INVALID_HANDLE_VALUE)
	{
		printf("Error open text file! %d", GetLastError());
		return;
	}


	HANDLE hEncFile = CreateFile(L"enc.bmp", GENERIC_WRITE | GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (hEncFile == INVALID_HANDLE_VALUE)
	{
		printf("Error open Enc file! %d", GetLastError());
		return;
	}



	DWORD dBytesBMP = 0;
	DWORD rastr_size = 0;
	BOOL bResult = 0;


	SetFilePointer(hBMPFile, 10, 0, FILE_BEGIN);
	ReadFile(hBMPFile, &rastr_size, 4, &dBytesBMP, NULL);
	SetFilePointer(hBMPFile, rastr_size, 0, FILE_BEGIN);

	dBytesBMP = 0;
	rastr_size = 0;

	SetFilePointer(hEncFile, 10, 0, FILE_BEGIN);
	ReadFile(hEncFile, &rastr_size, 4, &dBytesBMP, NULL);
	SetFilePointer(hEncFile, rastr_size, 0, FILE_BEGIN);

	DWORD TextSize = 0;
	TextSize = GetFileSize(hTextFile, NULL);
	DWORD dBytesText = 0;
	BYTE buf = 0;
	BYTE bufpix = 0;


	for (int i = 0; i < sizeof(DWORD) * 8; i++)
	{
		bResult = ReadFile(hBMPFile, &bufpix, 1, &dBytesBMP, NULL);
		bufpix &= 0xFE;
		bufpix |= (Tag & 1) << 0;
		Tag = Tag >> 1;
		WriteFile(hEncFile, &bufpix, sizeof(bufpix), &dBytesBMP, NULL);
	}


	for (int i = 0; i < sizeof(DWORD) * 8; i++)
	{
		bResult = ReadFile(hBMPFile, &bufpix, 1, &dBytesBMP, NULL);
		bufpix &= 0xFE;
		bufpix |= (TextSize & 1) << 0;
		TextSize = TextSize >> 1;
		WriteFile(hEncFile, &bufpix, sizeof(bufpix), &dBytesBMP, NULL);
	}

	while (1)
	{
		bResult = ReadFile(hTextFile, &buf, 1, &dBytesText, NULL);
		if (bResult && dBytesText == 0)
		{
			break;
		}


		for (int i = 0; i < 8; i++)
		{
			bResult = ReadFile(hBMPFile, &bufpix, 1, &dBytesBMP, NULL);
			bufpix &= 0xFE;
			bufpix |= (buf & 1) << 0;
			buf = buf >> 1;
			WriteFile(hEncFile, &bufpix, 1, &dBytesBMP, NULL);
		}
	}

	printf("Embed success!");
	CloseHandle(hBMPFile);
	CloseHandle(hTextFile);
	CloseHandle(hEncFile);
}

void retrieve(LPCWSTR szBMPFileName, LPCWSTR szDecTextFileName)
{
	HANDLE hBMPFile = CreateFile(szBMPFileName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (hBMPFile == INVALID_HANDLE_VALUE)
	{
		printf("Error open bmp file! %d", GetLastError());
		return;
	}

	HANDLE hTextFile = CreateFile(szDecTextFileName, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
	if (hTextFile == INVALID_HANDLE_VALUE)
	{
		printf("Error open text file! %d", GetLastError());
		return;
	}

	DWORD dBytesBMP = 0;
	DWORD rastr_size = 0;
	BOOL bResult = 0;

	SetFilePointer(hBMPFile, 10, 0, FILE_BEGIN);
	ReadFile(hBMPFile, &rastr_size, 4, &dBytesBMP, NULL);
	SetFilePointer(hBMPFile, rastr_size + sizeof(DWORD) * 8, 0, FILE_BEGIN);

	DWORD dBytesText = 0;
	BYTE byteText = 0;
	BYTE EncPix = 0;
	BOOL bEndFlag = 1;

	DWORD TextSize = 0;
	for (int i = 0; i < sizeof(DWORD) * 8; i++)
	{
		bResult = ReadFile(hBMPFile, &EncPix, 1, &dBytesBMP, NULL);
		TextSize |= (EncPix & 1) << i;
	}
	TextSize *= 8;

	dBytesBMP = 0;
	DWORD dBytesCount = 0;

	while (dBytesCount < TextSize)
	{

		for (int i = 0; i < 8; i++)
		{
			bResult = ReadFile(hBMPFile, &EncPix, 1, &dBytesBMP, NULL);
			if (bResult && dBytesBMP == 0)
			{
				return;
			}
			byteText |= (EncPix & 1) << i;
			dBytesCount += dBytesBMP;
		}

		WriteFile(hTextFile, &byteText, 1, &dBytesText, NULL);
		byteText = 0;
	}

	printf("retrieve success!");
	CloseHandle(hBMPFile);
	CloseHandle(hTextFile);
}



bool check(LPCWSTR szBMPFilename)
{
	HANDLE hBMPFile = CreateFile(szBMPFilename, GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0);
	if (hBMPFile == INVALID_HANDLE_VALUE)
	{
		printf("error open BMP file! %d", GetLastError());
		return 1;
	}

	DWORD dBytesBMP = 0;
	DWORD rastr_size = 0;
	BOOL bResult = 0;
	DWORD dFileTag = 0;
	BYTE EncPix = 0;

	SetFilePointer(hBMPFile, 10, 0, FILE_BEGIN);
	ReadFile(hBMPFile, &rastr_size, 4, &dBytesBMP, NULL);
	SetFilePointer(hBMPFile, rastr_size, 0, FILE_BEGIN);

	for (int i = 0; i < sizeof(DWORD) * 8; i++)
	{
		bResult = ReadFile(hBMPFile, &EncPix, 1, &dBytesBMP, NULL);
		dFileTag |= (EncPix & 1) << i;
	}

	if (dFileTag == Tag)
	{
		printf("Maybe have stego!");
	}
	else
	{
		printf("Don't have stego!");
	}


	CloseHandle(hBMPFile);
	return 0;
}



int _tmain(int argc, _TCHAR* argv[])
{
	if (argc < 3 || argc > 4)
	{
		printf("Error param count!");
		return 1;
	}

	if (wcscmp(argv[1], L"-embed") == 0)
	{
		embed(argv[2], argv[3]);
		return 0;
	}

	if (wcscmp(argv[1], L"-retrieve") == 0)
	{
		retrieve(argv[2], argv[3]);
		return 0;
	}

	if (wcscmp(argv[1], L"-check") == 0)
	{
		check(argv[2]);
		return 0;
	}

}

