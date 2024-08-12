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

int main()
{
    auto gen = sequencegenerator();
    for (int index = 0; index < 5; ++index)
    {
        std::cout << gen.next() << std::endl;
    }
}