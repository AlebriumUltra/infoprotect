#include <iostream>
#include <time.h>
#include <vector>
#include <algorithm>
#include <Windows.h>


class MidSquareRandom
{
private:
	time_t seed;
public:
	MidSquareRandom()
	{
		this->seed = time(NULL);
	}
	MidSquareRandom(time_t seed)
	{
		this->seed = seed;
	}

	uint8_t Rand()
	{
        uint8_t num = (uint8_t)this->seed;
        uint16_t sqr;
        uint8_t res;

        sqr = num * num;
        res = (sqr >> 4) & 0xFF;
		this->seed = (time_t)res;
		return res;
	}

    void ChangeSeed(time_t seed)
    {
        this->seed = seed;
    }
};



int main()
{
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

	MidSquareRandom rand = MidSquareRandom(1000);
    int count_random[10] = { 0 };
    int num = 1;
    std::vector<int> v;

    while (num != 0)
    {
        num = rand.Rand();
        printf("%d\n", num);
    }

    printf("\n");

    printf("Распределение чисел: ");
    for (int i = 1001; i < 10000; i++)
    {
        while (1)
        {
            if (num == 0)
            {
                count_random[0]++;
                break;
            }

            if (std::find(v.begin(), v.end(), num) != v.end())
            {
                break;
            }

            v.push_back(num);
            
            if (num < 25)
            {
                count_random[0]++;
            }
            if (num > 25 && num < 50)
            {
                count_random[1]++;

            }
            if (num > 50 && num < 75)
            {
                count_random[2]++;

            }
            if (num > 75 && num < 100)
            {
                count_random[3]++;

            }
            if (num > 100 && num < 125)
            {
                count_random[4]++;

            }
            if (num > 125 && num < 150)
            {
                count_random[5]++;

            }
            if (num > 150 && num < 175)
            {
                count_random[6]++;

            }
            if (num > 175 && num < 200)
            {
                count_random[7]++;

            }
            if (num > 200 && num < 225)
            {
                count_random[8]++;
            }
            if (num > 225 && num < 256)
            {
                count_random[9]++;
            }

            num = rand.Rand();
        }
        
        rand.ChangeSeed(i);
        num = rand.Rand();
        v.clear();
    }


	for (int i = 0; i < 10; i++)
	{
        std::cout << count_random[i] << " ";
	}
    printf("\n");
}
