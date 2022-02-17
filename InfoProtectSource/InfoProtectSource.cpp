#include <iostream>
#include <fstream>
#include <string>
using namespace std;

void CopyArray(int* source, int* dest, int size)
{
	for (size_t i = 0; i < size; i++)
	{
		dest[i] = source[i];
	}
}

void FilesCompare(string text_filename, string decode_filename)
{
	ifstream text_file(text_filename, ios::binary);
	ifstream decode_file(decode_filename, ios::binary);

	if (text_file.is_open() && decode_file.is_open())
	{
		int size_text;
		int size_decode;

		text_file.seekg(0, text_file.end);
		decode_file.seekg(0, decode_file.end);

		size_text = text_file.tellg();
		size_decode = decode_file.tellg();

		text_file.seekg(0, text_file.beg);
		decode_file.seekg(0, decode_file.beg);

		bool result = true;
		if (size_text != size_decode)
		{
			result = false;
		}
		else
		{
			char ch1, ch2;
			do
			{
				decode_file.get(ch2);
				text_file.get(ch1);
				if (ch1 != ch2)
				{
					result = false;
					break;
				}
			} while (!text_file.eof() && !decode_file.eof());
		}
		if (result)
			cout << "Equal" << endl;
		else
			cout << "Unequal" << endl;
	}
	else
		cout << "Error opening file!" << endl;

}

class Encoder {
private:
	int* key;
	int size_key;
public:
	Encoder()
	{
		this->size_key = 10;
		this->key = new int[this->size_key] {3, 6, 5, 2, 4, 1, 7, 9, 8, 10};
	}
	Encoder(int* key, int size_key)
	{
		this->size_key = size_key;
		this->key = new int[this->size_key];
		CopyArray(key, this->key, size_key);
	}
	
	string Encode(string text)
	{
		string encode_text;
		while (text.size() % 10 != 0)
			text += ' ';
		size_t size_text = text.size();
		int shift = 0;
		for (size_t i = 0; i < size_text; i++)
		{
			if (i % size_key == 0 && i != 0)
			{
				shift++;
			}
			encode_text += text[this->key[i % size_key] + (size_key * shift) - 1];
		}
		return encode_text;
	}

	string Decode(string text)
	{
		char* symbols = new char[size_key];
		string decode_text;
		size_t size_text = text.size();
		int shift = 0;
		for (size_t i = 0; i < size_text; i++)
		{
			if (i % size_key == 0 && i != 0)
			{
				symbols[size_key] = '\0';
				decode_text += symbols;
				shift++;
			}
			symbols[this->key[i % size_key] - 1] = text[i];
		}
		symbols[size_key] = '\0';
		decode_text += symbols;
		return decode_text;
	}
	
	bool FileEncode(string text_filename, string encode_filename)
	{
		fstream text_file(text_filename);
		ofstream encode_file(encode_filename);
		char* text = new char[size_key];
		text[size_key] = '\0';
		string encode_text;
		int count_ch;
		if (text_file.is_open() && encode_file.is_open())
		{
			text_file.seekg(0, text_file.end);
			count_ch = text_file.tellg();
			if (count_ch % size_key != 0)
			{
				for (int i = 0; i < size_key - (count_ch % size_key); i++)
				{
					text_file << ' ';
				}
			}
			text_file.seekg(0, text_file.beg);

			cout << "Files is open. Start Encoding\n";
			while (!text_file.eof())
			{
				text_file.read(text, this->size_key);
				if (text_file.eof())
					break;
				encode_text = Encode(text);
				encode_file << encode_text;
			}
			text_file.close();
			encode_file.close();
		}
		else
		{
			cout << "Files opened error!\n";
			return 1;
		}
	}

	bool FileDecode(string text_filename, string decode_filename)
	{
		ifstream text_file(text_filename);
		ofstream decode_file(decode_filename);
		char* text = new char[size_key];
		text[size_key] = '\0';
		string decode_text;
		if (text_file.is_open() && decode_file.is_open())
		{
			cout << "Files is open. Start Decoding\n";
			while (!text_file.eof())
			{
				text_file.read(text, this->size_key);
				if (text_file.eof())
					break;
				decode_text = Decode(text);
				decode_file << decode_text;
			} 
			text_file.close();
			decode_file.close();
		}
		else
		{
			cout << "Files opened error!\n";
			return 1;
		}
	}
};



int main()
{
	string text = "Hola, my name is Alex";
	Encoder enc = Encoder();
	/*string enc_text = enc.Encode(text);
	string dec_text = enc.Decode(enc_text);*/


	enc.FileEncode("text.txt", "encode_text.txt");
	enc.FileDecode("encode_text.txt", "decode_text.txt");
	FilesCompare("text.txt", "decode_text.txt");
}
