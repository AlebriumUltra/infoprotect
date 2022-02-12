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
	
	bool Encode(string text)
	{
		char* symbols = new char[size_key];
		int pos = 0;
		while (pos < text.size())
		{
			pos += text.copy(symbols, size_key, pos);
			if (pos % size_key != 0)
			{
				for (size_t i = pos % size_key; i < size_key; i++)
				{
					symbols[i] = ' ';
				}
				cout << symbols;
			}
			symbols[size_key] = '\0';

			string result;


		}
		return 0;
	}


};



int main()
{
	string text = "Hello, my name is Alex";

	Encoder enc = Encoder();
	enc.Encode(text);
	// enc.Decode(text);



}
