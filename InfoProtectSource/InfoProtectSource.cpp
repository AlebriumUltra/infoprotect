#include <iostream>

using namespace std;

void copy_array(int* source, int* dest, int size)
{
	for (size_t i = 0; i < size; i++)
	{
		dest[i] = source[i];
	}
}


 


class Encoder {
private:
	int* key;
	int size_key;
	int* reverse_key;
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
		copy_array(key, this->key, size_key);
	}
	
	string Encode(string text)
	{
		string encode_text;
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
		cout << encode_text;
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
		cout << decode_text;
		return decode_text;
	}
	



};



int main()
{
	string text = "Hola, my name is Alp";
	Encoder enc = Encoder();
	string enc_text = enc.Encode(text);
	string dec_text = enc.Decode(enc_text);
}
