using System;

namespace Ximu3
{
    public class CommandMessage
    {
        public required string Json { get; init; }
        public required string Key { get; init; }
        public required string Value { get; init; }
        public required string? Error { get; init; }

        public static CommandMessage? From(CApi.XIMU3_CommandMessage response)
        {
            if (Helpers.ToString(response.json).Length == 0)
            {
                return null;
            }

            return new CommandMessage
            {
                Json = Helpers.ToString(response.json),
                Key = Helpers.ToString(response.key),
                Value = Helpers.ToString(response.value),
                Error = Helpers.ToString(response.error).Length == 0 ? null : Helpers.ToString(response.error)
            };
        }
    }
}
