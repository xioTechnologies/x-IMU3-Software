#pragma once

#include "../C/Ximu3.h"
#include <functional>
#include <string>
#include <type_traits>
#include <vector>

namespace ximu3
{
    class Helpers
    {
        friend class Connection;

        friend class DataLogger;

        friend class FileConverter;

        friend class NetworkAnnouncement;

        friend class PortScanner;

        template<typename... T, typename Callable>
        static auto wrapCallable(Callable const&) -> void (*)(T..., void*)
        {
            return +[](T... data, void* context)
            {
                (*static_cast<Callable*>(context))(data...);
            };
        }

        template<typename VectorType, typename ArgumentType>
        static auto toVector(const ArgumentType& argument)
        {
            std::vector<VectorType> vector;
            for (uint32_t index = 0; index < argument.length; index++)
            {
                vector.push_back(argument.array[index]);
            }
            return vector;
        }

        static auto toVectorAndFree(const XIMU3_CharArrays& charArrays)
        {
            const auto vector = toVector<std::string>(charArrays);
            XIMU3_char_arrays_free(charArrays);
            return vector;
        }
    };
} // namespace ximu3
