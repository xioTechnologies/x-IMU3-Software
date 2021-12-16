#pragma once

#include "../../C/Ximu3.h"
#include <string>

using namespace System;
using namespace System::Runtime::InteropServices;

namespace Ximu3
{
    private ref class Helpers
    {
    public:
        static array<String^>^ ToArrayAndFree(const ximu3::XIMU3_CharArrays charArrays) {
            auto strings = gcnew array<String^>(charArrays.length);

            for (uint32_t index = 0; index < charArrays.length; index++)
            {
                strings[index] = gcnew String(charArrays.array[index]);
            }

            ximu3::XIMU3_char_arrays_free(charArrays);
            return strings;
        }

        static const char* ToCharPtr(String^ string)
        {
            return (const char*)(Marshal::StringToHGlobalAnsi(string).ToPointer());
        }

        static void StringCopy(char* destination, String^ string, const size_t destinationSize)
        {
            strncpy_s(destination, destinationSize, ToCharPtr(string), destinationSize - 1);
        }
    };
}
