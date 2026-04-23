using System;

namespace Ximu3
{
    public class Device : IDisposable
    {
        public Device(CApi.XIMU3_Device device)
        {
            wrapped = CApi.XIMU3_device_clone(device);
        }

        ~Device() => Dispose();

        public void Dispose()
        {
            if (!disposed)
            {
                CApi.XIMU3_device_free(wrapped);
                disposed = true;
            }

            GC.SuppressFinalize(this);
        }

        public override string ToString()
        {
            return Helpers.ToString(CApi.XIMU3_device_to_string(wrapped));
        }

        public CApi.XIMU3_Device wrapped;
        private bool disposed = false;
    }
}
