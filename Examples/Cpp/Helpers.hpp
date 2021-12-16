#include <chrono>
#include <iostream>
#include <thread>

#pragma once

namespace helpers
{
    static char getKey()
    {
        char c;
        std::cin >> c;
        return toupper(c);
    }

    static bool yesOrNo(const char* question)
    {
        while (true)
        {
            std::cout << question << " [Y/N]" << std::endl;
            switch (getKey())
            {
                case 'Y':
                    return true;
                case 'N':
                    return false;
                default:
                    break;
            }
        }
    }

    static void wait(const int seconds)
    {
        if (seconds < 0)
        {
            int counter = 0;
            while (counter++ < std::numeric_limits<int>::max()) // 2^32 seconds = 136 years
            {
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        }
        else
        {
            std::this_thread::sleep_for(std::chrono::seconds(seconds));
        }
    }
} // namespace helpers
