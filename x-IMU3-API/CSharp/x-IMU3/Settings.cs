using System;

namespace Ximu3
{
    public class Settings
    {
        public static CApi.XIMU3_Result Backup(string filePath, Connection connection)
        {
            return CApi.XIMU3_settings_backup(Helpers.ToPointer(filePath), connection.wrapped);
        }

        public static CApi.XIMU3_Result Restore(string filePath, Connection connection)
        {
            return CApi.XIMU3_settings_restore(Helpers.ToPointer(filePath), connection.wrapped);
        }
    }
}
