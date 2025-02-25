#include "EscapedStrings.h"
#include <iomanip>
#include <regex>
#include <sstream>

std::string EscapedStrings::escapedToBytes(const std::string& escaped)
{
    std::string bytes;

    for (size_t index = 0; index < escaped.length(); index++)
    {
        if (escaped[index] != '\\')
        {
            bytes += escaped[index];
            continue;
        }

        if (++index == escaped.length())
        {
            break;
        }

        switch (escaped[index])
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
                if ((index + 2) < escaped.length())
                {
                    try
                    {
                        bytes += static_cast<char>(std::stoi(escaped.substr(index + 1, 2), nullptr, 16));
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

std::string EscapedStrings::bytesToEscaped(const std::string& bytes)
{
    std::string escaped;

    for (char byte : bytes)
    {
        switch (byte)
        {
            case '\\':
                escaped += "\\\\";
                continue;

            case '\n':
                escaped += "\\n";
                continue;

            case '\r':
                escaped += "\\r";
                continue;

            default:
                break;
        }

        const auto byteUnsigned = (unsigned char) byte;
        if (byteUnsigned < 0x20 || byteUnsigned > 0x7E)
        {
            std::ostringstream stream;
            stream << "\\x" << std::setw(2) << std::setfill('0') << std::hex << std::uppercase << (unsigned int) byteUnsigned;
            escaped += stream.str();
            continue;
        }

        escaped += byte;
    }

    return escaped;
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

std::vector<std::string> EscapedStrings::splitEscaped(const std::string& escaped)
{
    static const std::regex regex(R"((\\\\|\\n|\\r|\\x[0-9A-Fa-f]{2}))");

    std::vector<std::string> output;

    size_t position = 0;
    for (std::sregex_iterator it(escaped.cbegin(), escaped.cend(), regex); it != std::sregex_iterator(); ++it)
    {
        if (it->position() != 0)
        {
            output.push_back(escaped.substr(position, (size_t) it->position() - position));
        }
        output.push_back(it->str());
        position = size_t(it->position() + it->length());
    }

    if (position < escaped.length())
    {
        output.push_back(escaped.substr(position));
    }

    return output;
}
