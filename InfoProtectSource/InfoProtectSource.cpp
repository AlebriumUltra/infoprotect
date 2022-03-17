#define _CRT_SECURE_NO_WARNINGS
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
	int size_block;
public:
	Encoder()
	{
		this->size_key = 32;
		this->key = new int[this->size_key]{ 2, 22, 14, 1, 29, 0, 28, 23, 21, 15, 11, 17, 24, 19, 5, 25, 3, 30, 26, 6, 31, 16, 4, 13, 12, 18, 9, 8, 27, 20, 10, 7 };
		this->size_block = 4;
	}
	Encoder(int* key, int size_key)
	{
		this->size_key = size_key;
		this->key = new int[this->size_key];
		this->size_block = 4;
		CopyArray(key, this->key, size_key);
	}

	void Encode(void* source_text, void* encode_text, int size_source)
	{

		if (size_source % this->size_block != 0)
		{
			memset((char*)source_text + size_source, '0', this->size_block - size_source);
		}

		int text_num = *(int*)source_text;
		int enc_num = 0;

		for (int i = 0; i < this->size_key; i++)
		{
			enc_num |= ((text_num >> this->key[i]) & 1) << i;
		}

		/**((int*)encode_text) = enc_num;*/             // <-- два рабочих варианта по пустому указателю внести зашифрованное число
		memcpy(encode_text, &enc_num, sizeof(int));
	}

	void Decode(void* source_text, void* decode_text)
	{
		int text_num = *(int*)source_text;
		int dec_num = 0;

		for (int i = 0; i < this->size_key; i++)
		{
			dec_num |= ((text_num >> i) & 1) << this->key[i];
		}

		/**((int*)decode_text) = dec_num;*/          // <-- и тут))
		memcpy(decode_text, &dec_num, sizeof(int));
	}

	
	bool FileEncode(string text_filename, string encode_filename)
	{
		ifstream text_file(text_filename, ios::binary);
		ofstream encode_file(encode_filename, ios::binary);
		void* text = malloc(size_block);
		void* encode_text = malloc(size_block);
		int count_ch;

		if (text_file.is_open() && encode_file.is_open())
		{
			cout << "Files is open. Start Encoding: " << text_filename << "\n";
			text_file.seekg(0, text_file.end);
			count_ch = text_file.tellg();
			text_file.seekg(0, text_file.beg);
			encode_file.write((char*)&count_ch, sizeof(int));
			while (!text_file.eof())
			{
				text_file.read((char*)text, size_block);
				if (text_file.eof() && text_file.gcount() == 0)
					break;
				Encode(text, encode_text, text_file.gcount());
				encode_file.write((char*)encode_text, size_block);
			}
			text_file.close();
			encode_file.close();
			cout << "Success! Encode file: " << encode_filename << "\n";
			return 0;
		}
		else
		{
			cout << "Files opened error! " << text_filename << "\n";
			return 1;
		}
	}

	bool FileDecode(string encode_filename, string decode_filename)
	{
		ifstream encode_file(encode_filename, ios::binary);
		ofstream decode_file(decode_filename, ios::binary);
		void* encode_text = malloc(size_block);
		void* decode_text = malloc(size_block);
		int length_file = 0;
		if (encode_file.is_open() && decode_file.is_open())
		{
			cout << "Files is open. Start Decoding " << encode_filename << "\n";
			encode_file.read((char*)&length_file, sizeof(int));
			while (!encode_file.eof())
			{
				encode_file.read((char*)encode_text, size_block);
				if (encode_file.eof() && encode_file.gcount() == 0)
					break;
				Decode(encode_text, decode_text);
				int length_encode = encode_file.tellg();
				length_encode -= 4;
				if (length_encode > length_file)
				{
					decode_file.write((char*)decode_text, length_file % size_block);
					break;
				}
				decode_file.write((char*)decode_text, size_block);
			} 
			encode_file.close();
			decode_file.close();
			cout << "Success! Decode file: " << decode_filename << "\n";
			return 0;
		}
		else
		{
			cout << "Files opened error! " << encode_filename << "\n";
			return 1;
		}
	}
};



int main()
{
	Encoder enc = Encoder();
	enc.FileEncode("text.txt", "enc_text.txt");
	enc.FileDecode("enc_text.txt", "dec_text.txt");

	enc.FileEncode("img.jpg", "enc_img.jpg");
	enc.FileDecode("enc_img.jpg", "dec_img.jpg");
	
	/*enc.FileEncode("video.mp4", "enc_vid.mp4");
	enc.FileDecode("enc_vid.mp4", "dec_vid.mp4");*/
}
