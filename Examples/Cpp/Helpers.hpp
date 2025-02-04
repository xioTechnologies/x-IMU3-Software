#pragma once

#include <iostream>

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
} // namespace helpers
