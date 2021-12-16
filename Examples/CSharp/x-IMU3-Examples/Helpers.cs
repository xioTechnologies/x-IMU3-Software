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

        public static void Wait(int seconds)
        {
            if (seconds < 0)
            {
                for (int counter = 0; counter < Int32.MaxValue; counter++)
                {
                    System.Threading.Thread.Sleep(1000);
                }
            }
            else
            {
                System.Threading.Thread.Sleep(seconds * 1000);
            }
        }
    }
}
