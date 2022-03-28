#include <stdio.h>
#include <tchar.h>
#include <windows.h>


void CreateTestBMP(LPCWSTR szFileName, PRGBTRIPLE pColor)
{
	// Объявим нужные структуры
	BITMAPFILEHEADER bfh;
	memset(&bfh, 0, sizeof(bfh));
	BITMAPINFOHEADER bih;
	BYTE Palette[1024]; // Палитра
	memset(Palette, 0, 1024);
	// Пусть у нас будет картинка размером 35 x 50 пикселей
	int Width = 500;
	int Height = 500;
	bfh.bfType = 0x4D42; // Обозначим, что это bmp 'BM'
	bfh.bfOffBits = sizeof(bfh) + sizeof(bih); // Палитра занимает 1Kb, но мы его использовать не будем
	bfh.bfSize = bfh.bfOffBits +
		sizeof(*pColor) * Width * Height +
		Height * ((sizeof(*pColor) * Width) % 4); // Посчитаем размер конечного файла
	memset(&bih, 0, sizeof(bih));
	bih.biSize = sizeof(bih);
	bih.biBitCount = 24;
	bih.biClrUsed = 0;
	bih.biCompression = BI_RGB; // Без сжатия
	bih.biHeight = Height;
	bih.biWidth = Width;
	bih.biPlanes = 1; // Должно быть 1
	// Остальные поля остаются 0
	HANDLE hFile = CreateFile(szFileName, GENERIC_WRITE, 0, NULL,
		CREATE_ALWAYS, 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return;
	DWORD RW;
	// Запишем заголовки
	WriteFile(hFile, &bfh, sizeof(bfh), &RW, NULL);
	WriteFile(hFile, &bih, sizeof(bih), &RW, NULL);
	// Запишем растр
	for (int i = 0; i < Height; ++i)
	{
		for (int j = 0; j < Width; ++j)
		{
			WriteFile(hFile, pColor, sizeof(*pColor), &RW, NULL);
		}
		// Обеспечим выравнивание строки
		WriteFile(hFile, 0, (sizeof(*pColor) * Width) % 4, &RW, NULL);
	}
	CloseHandle(hFile);
}

void embed(LPCWSTR szTextFileName, LPCWSTR szBMPFileName)
{
	HANDLE hBMPFile = CreateFile(szBMPFileName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (hBMPFile == INVALID_HANDLE_VALUE)
	{
		printf("Error open bmp file!");
		return;
	}

	HANDLE hTextFile = CreateFile(szTextFileName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (hTextFile == INVALID_HANDLE_VALUE)
	{
		printf("Error open text file!");
		return;
	}

	HANDLE hEncFile = CreateFile(L"Enc.bmp", GENERIC_WRITE | GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (hEncFile == INVALID_HANDLE_VALUE)
	{
		printf("Error open Enc file!");
		return;
	}



	DWORD dBytesBMP;
	DWORD rastr_size;
	BOOL bResult;


	SetFilePointer(hBMPFile, 10, 0, FILE_BEGIN);
	ReadFile(hBMPFile, &rastr_size, 4, &dBytesBMP, NULL);
	SetFilePointer(hBMPFile, rastr_size, 0, FILE_BEGIN);
	
	SetFilePointer(hEncFile, 10, 0, FILE_BEGIN);
	ReadFile(hEncFile, &rastr_size, 4, &dBytesBMP, NULL);
	SetFilePointer(hEncFile, rastr_size, 0, FILE_BEGIN);

	DWORD TextSize = 0; 
	TextSize = GetFileSize(hTextFile, NULL);
	DWORD dBytesText;
	BYTE buf;
	BYTE bufpix;

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
			WriteFile(hEncFile, &bufpix, sizeof(bufpix), &dBytesBMP, NULL);
		}
	}
	
	CloseHandle(hBMPFile);
	CloseHandle(hTextFile);
	CloseHandle(hEncFile);
}

void retrieve(LPCWSTR szBMPFileName, LPCWSTR szDecTextFileName)
{
	HANDLE hBMPFile = CreateFile(szBMPFileName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (hBMPFile == INVALID_HANDLE_VALUE)
	{
		printf("Error open bmp file!");
		return;
	}

	HANDLE hTextFile = CreateFile(szDecTextFileName, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
	if (hTextFile == INVALID_HANDLE_VALUE)
	{
		printf("Error open text file!");
		return;
	}

	DWORD dBytesBMP;
	DWORD rastr_size;
	BOOL bResult;
	
	SetFilePointer(hBMPFile, 10, 0, FILE_BEGIN);
	ReadFile(hBMPFile, &rastr_size, 4, &dBytesBMP, NULL);
	SetFilePointer(hBMPFile, rastr_size, 0, FILE_BEGIN);

	DWORD dBytesText;
	BYTE byteText = 0;
	BYTE EncPix;
	BOOL bEndFlag = 1;

	DWORD TextSize = 0;
	for (int i = 0; i < sizeof(DWORD) * 8; i++)
	{
		bResult = ReadFile(hBMPFile, &EncPix, sizeof(BYTE), &dBytesBMP, NULL);
		TextSize |= (EncPix & 1) << i;
	}
	TextSize *= 8;

	dBytesBMP = 0;
	DWORD dBytesCount = 0;



	while (dBytesCount < TextSize)
	{

		for (int i = 0; i < 8; i++)
		{
			bResult = ReadFile(hBMPFile, &EncPix, sizeof(BYTE), &dBytesBMP, NULL);
			if (bResult && dBytesBMP == 0)
			{
				return;
			}
			byteText |= (EncPix & 1) << i;
			dBytesCount += dBytesBMP;
		}

		WriteFile(hTextFile, &byteText, sizeof(BYTE), &dBytesText, NULL);
		byteText = 0;
	}

	CloseHandle(hBMPFile);
	CloseHandle(hTextFile);
}


bool check(LPCWSTR szBMPFilename)
{
	HANDLE hBMPFile = CreateFile(szBMPFilename, GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0);
	if (hBMPFile == INVALID_HANDLE_VALUE)
	{
		printf("error open BMP file!");
		return 1;
	}

	HANDLE hOutFile = CreateFile(L"Out.bmp", GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);
	if (hOutFile == INVALID_HANDLE_VALUE)
	{
		printf("error open out BMP file!");
		return 1;
	}

	DWORD dBytesBMP;
	DWORD rastr_size;
	BOOL bResult;

	SetFilePointer(hBMPFile, 10, 0, FILE_BEGIN);
	ReadFile(hBMPFile, &rastr_size, 4, &dBytesBMP, NULL);
	SetFilePointer(hBMPFile, rastr_size, 0, FILE_BEGIN);

	SetFilePointer(hOutFile, 10, 0, FILE_BEGIN);
	ReadFile(hOutFile, &rastr_size, 4, &dBytesBMP, NULL);
	SetFilePointer(hOutFile, rastr_size, 0, FILE_BEGIN);

	BYTE pix;
	BYTE black = 0;


	while (1)
	{
		bResult = ReadFile(hBMPFile, &pix, sizeof(BYTE), &dBytesBMP, NULL);
		if (bResult && dBytesBMP == 0)
		{
			break;
		}
		if ((pix & 1) == 0)
		{
			WriteFile(hOutFile, &black, sizeof(BYTE), &dBytesBMP, NULL);
		}
	}

	CloseHandle(hBMPFile);
	CloseHandle(hOutFile);
	return 0;
}



int _tmain(int argc, _TCHAR* argv[])
{
	//embed(L"text.txt", L"test.bmp");
	retrieve(L"Enc.bmp", L"DecText.txt");
	return 0;
}
