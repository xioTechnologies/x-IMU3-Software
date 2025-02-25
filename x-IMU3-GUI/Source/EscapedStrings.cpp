#include "EscapedStrings.h"
#include <iomanip>
#include <regex>
#include <sstream>

std::string EscapedStrings::printableToBytes(const std::string& printable)
{
    std::string bytes;

    for (size_t index = 0; index < printable.length(); index++)
    {
        if (printable[index] != '\\')
        {
            bytes += printable[index];
            continue;
        }

        if (++index == printable.length())
        {
            break;
        }

        switch (printable[index])
        {
            case '\\':
                bytes += '\\';
                continue;

            case 'n':
                bytes += '\n';
                continue;

            case 'r':
                bytes += '\r';
                continue;

            case 'x':
                if ((index + 2) < printable.length())
                {
                    try
                    {
                        bytes += static_cast<char>(std::stoi(printable.substr(index + 1, 2), nullptr, 16));
                    }
                    catch (...)
                    {
                    }

                    index += 2;
                }

            default:
                break;
        }
    }

    return bytes;
}

std::string EscapedStrings::bytesToPrintable(const std::string& bytes)
{
    std::string printable;

    for (char byte : bytes)
    {
        switch (byte)
        {
            case '\\':
                printable += "\\\\";
                continue;

            case '\n':
                printable += "\\n";
                continue;

            case '\r':
                printable += "\\r";
                continue;

            default:
                break;
        }

        const auto byteUnsigned = (unsigned char) byte;
        if (byteUnsigned < 0x20 || byteUnsigned > 0x7E)
        {
            std::ostringstream stream;
            stream << "\\x" << std::setw(2) << std::setfill('0') << std::hex << std::uppercase << (unsigned int) byteUnsigned;
            printable += stream.str();
            continue;
        }

        printable += byte;
    }

    return printable;
}

std::string EscapedStrings::jsonToBytes(std::string json)
{
    if ((json.length()) < 2 || (json.front() != '"') || (json.back() != '"'))
    {
        return {};
    }
    json = json.substr(1, json.length() - 2);

    std::string bytes;

    for (size_t index = 0; index < json.length(); index++)
    {
        if (json[index] != '\\')
        {
            bytes += json[index];
            continue;
        }

        if (++index == json.length())
        {
            break;
        }

        switch (json[index])
        {
            case '"':
                bytes += '"';
                continue;

            case '\\':
                bytes += '\\';
                continue;

            case 'b':
                bytes += '\b';
                continue;

            case 'f':
                bytes += '\f';
                continue;

            case 'n':
                bytes += '\n';
                continue;

            case 'r':
                bytes += '\r';
                continue;

            case 't':
                bytes += '\t';
                continue;

            case 'u':
                if ((index + 4) < json.length())
                {
                    try
                    {
                        bytes += static_cast<char>(std::stoi(json.substr(index + 1, 4), nullptr, 16) & 0xFF);
                    }
                    catch (...)
                    {
                    }

                    index += 4;
                }

            default:
                break;
        }
    }

    return bytes;
}

std::string EscapedStrings::bytesToJson(const std::string& bytes)
{
    std::string json;

    for (char byte : bytes)
    {
        switch (byte)
        {
            case '"':
                json += "\\\"";
                continue;

            case '\\':
                json += "\\\\";
                continue;

            case '\b':
                json += "\\b";
                continue;

            case '\f':
                json += "\\f";
                continue;

            case '\n':
                json += "\\n";
                continue;

            case '\r':
                json += "\\r";
                continue;

            case '\t':
                json += "\\t";
                continue;

            default:
                break;
        }

        const auto byteUnsigned = (unsigned char) byte;
        if (byteUnsigned < 0x20)
        {
            std::ostringstream stream;
            stream << "\\u" << std::setw(4) << std::setfill('0') << std::hex << std::uppercase << (unsigned int) byteUnsigned;
            json += stream.str();
            continue;
        }

        json += byte;
    }

    return "\"" + json + "\"";
}

std::vector<std::string> EscapedStrings::splitPrintable(const std::string& printable)
{
    static const std::regex regex(R"((\\\\|\\n|\\r|\\x[0-9A-Fa-f]{2}))");

    std::vector<std::string> output;

    size_t position = 0;
    for (std::sregex_iterator it(printable.cbegin(), printable.cend(), regex); it != std::sregex_iterator(); ++it)
    {
        if (it->position() != 0)
        {
            output.push_back(printable.substr(position, (size_t) it->position() - position));
        }
        output.push_back(it->str());
        position = size_t(it->position() + it->length());
    }

    if (position < printable.length())
    {
        output.push_back(printable.substr(position));
    }

    return output;
}
