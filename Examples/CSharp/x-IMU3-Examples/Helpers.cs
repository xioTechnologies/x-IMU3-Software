using System;

namespace Ximu3Examples
{
    class Helpers
    {
        public static Char GetKey()
        {
            Char key = Char.ToUpper(Console.ReadKey().KeyChar);
            Console.WriteLine();
            return key;
        }

        public static bool YesOrNo(string question)
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
