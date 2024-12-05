#pragma once

#include "../../C/Ximu3.h"
#include "Helpers.h"

namespace Ximu3
{
    public ref class CommandMessage
    {
    internal:
        CommandMessage(ximu3::XIMU3_CommandMessage commandMessage) : commandMessage{ new ximu3::XIMU3_CommandMessage{commandMessage} }
        {
        }

    public:
        ~CommandMessage()
        {
            delete commandMessage;
        }

        property String^ Json
        {
            String^ get()
            {
                return gcnew String(commandMessage->json);
            }
        }

        property String^ Key
        {
            String^ get()
            {
                return gcnew String(commandMessage->key);
            }
        }

        property String^ Value
        {
            String^ get()
            {
                return gcnew String(commandMessage->value);
            }
        }

        property String^ Error
        {
            String^ get()
            {
                return gcnew String(commandMessage->error);
            }
        }

        static CommandMessage^ Parse(String^ json)
        {
            return gcnew CommandMessage(ximu3::XIMU3_command_message_parse(Helpers::ToCharPtr(json)));
        }

    private:
        ximu3::XIMU3_CommandMessage* commandMessage;
    };
}
