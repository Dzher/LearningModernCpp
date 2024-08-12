#include <iostream>
#include "sequencegenerator.h"

Generator sequencegenerator()
{
    int value = 0;
    while (true)
    {
        co_await value++;
    }
}

Generator fibonacci()
{
    co_yield 0;
    co_yield 1;

    int a = 0;
    int b = 1;
    int sum;
    while (true)
    {
        sum = a + b;
        co_yield sum;
        a = b;
        b = sum;
    }
}

int main()
{
    auto gen = sequencegenerator();
    for (int index = 0; index < 15; ++index)
    {
        if (gen.hasNext())
        {
            std::cout << gen.next() << std::endl;
        }
        else
        {
            break;
        }
    }

    std::cout << std::endl;

    auto gen1 = fibonacci();
    for (int index = 0; index < 15; ++index)
    {
        if (gen1.hasNext())
        {
            std::cout << gen1.next() << std::endl;
        }
        else
        {
            break;
        }
    }
    return 0;
}