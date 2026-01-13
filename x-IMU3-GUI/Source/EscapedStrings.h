#pragma once

#include <string>
#include <vector>

// Methods for converting between three string representations: bytes,
// printable, and JSON.
//
// Bytes: Each character is stored as its raw byte between 0x00 and 0xFF. Some
// characters may not be printable.
//
// Printable: Strings used by the UI, either for display or user input. ASCII
// characters 0x20 to 0x5B and 0x5D to 0x7F are stored as their raw byte
// values. All other characters, except 0x0A, 0x0D, and 0x5C, are stored as the
// 4-byte escape sequence "\\xHH" where HH is the 2-digit hexadecimal value.
// Characters 0x0A, 0x0D, and 0x5C are stored as the 2-byte sequences "\\n",
// "\\r", "\\\\" respectively.
//
// JSON: Strings follow RFC 8259 with the following clarifications: UTF-8
// characters are not supported so that all characters are limited to the range
// 0x00 to 0xFF. The escape sequences "\b", "\f", "\n", "\r", and "\t" are
// preferred over their 6-byte escape sequence "\\uHHHH".
//
// https://datatracker.ietf.org/doc/html/rfc8259

namespace EscapedStrings {
    std::string printableToBytes(const std::string &printable);

    std::string bytesToPrintable(const std::string &bytes);

    std::string jsonToBytes(std::string json);

    std::string bytesToJson(const std::string &bytes);

    std::vector<std::string> splitPrintable(const std::string &printable);
}
