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
	int count_file;

	
public:
	Encoder()
	{
		this->size_key = 10;
		this->key = new int[this->size_key] {3, 6, 5, 2, 4, 1, 7, 9, 8, 10};
		this->count_file = 0;
	}
	Encoder(int* key, int size_key)
	{
		this->size_key = size_key;
		this->key = new int[this->size_key];
		this->count_file = 0;
		CopyArray(key, this->key, size_key);
	}
	
	void Encode(char* source_text, char* encode_text, int size_source)
	{
		if (size_source % this->size_key != 0)
		{
			memset(&source_text[size_source], ' ', this->size_key - size_source);
			source_text[this->size_key] = '\0';
		}

		for (size_t i = 0; i < this->size_key; i++)
		{
			encode_text[i] = source_text[this->key[i] - 1];
		}
	}

	void Decode(char* source_text, char* decode_text)
	{
		for (size_t i = 0; i < this->size_key; i++)
		{
			decode_text[this->key[i] - 1] = source_text[i];
		}
	}

	
	bool FileEncode(string text_filename, string encode_filename)
	{
		ifstream text_file(text_filename, ios::binary);
		ofstream encode_file(encode_filename, ios::binary);
		char* text = new char[size_key];
		char* encode_text = new char[size_key];
		text[size_key] = '\0';
		encode_text[size_key] = '\0';
		int count_ch;
		if (text_file.is_open() && encode_file.is_open())
		{
			cout << "Files is open. Start Encoding: " << text_filename << "\n";
			while (!text_file.eof())
			{
				text_file.read(text, this->size_key);
				if (text_file.eof() && text_file.gcount() == 0)
					break;
				Encode(text, encode_text, text_file.gcount());
				encode_file.write(encode_text, this->size_key);
			}
			text_file.close();
			encode_file.close();
			cout << "Success! Encode file: " << encode_filename << "\n";
		}
		else
		{
			cout << "Files opened error!\n";
			return 1;
		}
	}

	bool FileDecode(string text_filename, string encode_filename, string decode_filename)
	{
		ifstream text_file(text_filename, ios::binary);
		int lenght_file = 0;
		if (text_file.is_open())
		{
			text_file.seekg(0, text_file.end);
			lenght_file = text_file.tellg();
			text_file.seekg(0, text_file.beg);
			text_file.close();
		}

		ifstream encode_file(encode_filename, ios::binary);
		ofstream decode_file(decode_filename, ios::binary);
		char* encode_text = new char[size_key];
		encode_text[size_key] = '\0';
		char* decode_text = new char[size_key];
		decode_text[size_key] = '\0';
		if (encode_file.is_open() && decode_file.is_open())
		{
			cout << "Files is open. Start Decoding " << encode_filename << "\n";
			while (!encode_file.eof())
			{
				encode_file.read(encode_text, this->size_key);
				if (encode_file.eof() && encode_file.gcount() == 0)
					break;
				Decode(encode_text, decode_text);
				int length_encode = encode_file.tellg();
				if (length_encode > lenght_file)
				{
					decode_text[lenght_file % size_key] = '\0';
					decode_file.write(decode_text, lenght_file % size_key);
					break;
				}
				decode_file.write(decode_text, this->size_key);
			} 
			encode_file.close();
			decode_file.close();
			cout << "Success! Decode file: " << decode_filename << "\n";
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
	Encoder enc = Encoder();
	enc.FileEncode("text.txt", "enc_text.txt");
	enc.FileDecode("text.txt", "enc_text.txt", "dec_text.txt");

	enc.FileEncode("img.jpg", "enc_img.jpg");
	enc.FileDecode("img.jpg", "enc_img.jpg", "dec_img.jpg");
	
	/*enc.FileEncode("video.mp4", "enc_vid.mp4");
	enc.FileDecode("video.mp4", "enc_vid.mp4", "dec_vid.mp4");*/
}
