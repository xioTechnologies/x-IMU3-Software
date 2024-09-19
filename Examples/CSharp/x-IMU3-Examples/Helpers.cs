﻿using System;

namespace Ximu3Examples
{
    class Helpers
    {
        public static Char? GetKey()
        {
            string buffer = Console.ReadLine();
            if (string.IsNullOrEmpty(buffer))
            {
                return null;
            }
            return Char.ToUpper(buffer[0]);
        }

        public static bool AskQuestion(string question)
        {
            while (true)
            {
                Console.WriteLine(question + " [Y/N]");
                switch (GetKey())
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
    }
}
