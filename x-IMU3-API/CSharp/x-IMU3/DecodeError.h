#pragma once

#include "../../C/Ximu3.h"

namespace Ximu3
{
    public enum class DecodeError
    {
        BufferOverrun = ximu3::XIMU3_DecodeErrorBufferOverrun,
        InvalidMessageIdentifier = ximu3::XIMU3_DecodeErrorInvalidMessageIdentifier,
        InvalidUtf8 = ximu3::XIMU3_DecodeErrorInvalidUtf8,
        InvalidJson = ximu3::XIMU3_DecodeErrorInvalidJson,
        JsonIsNotAnObject = ximu3::XIMU3_DecodeErrorJsonIsNotAnObject,
        JsonObjectIsNotASingleKeyValuePair = ximu3::XIMU3_DecodeErrorJsonObjectIsNotASingleKeyValuePair,
        InvalidEscapeSequence = ximu3::XIMU3_DecodeErrorInvalidEscapeSequence,
        InvalidBinaryMessageLength = ximu3::XIMU3_DecodeErrorInvalidBinaryMessageLength,
        UnableToParseAsciiMessage = ximu3::XIMU3_DecodeErrorUnableToParseAsciiMessage,
    };
}
