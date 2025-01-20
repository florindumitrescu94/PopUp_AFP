using System.Runtime.InteropServices;

namespace ASCOM.LocalServer
{
    /// <summary>
    /// Class to keep track of COM object instanciations and destructions
    /// </summary>
    [ComVisible(false)]
    public class ReferenceCountedObjectBase
    {
        /// <summary>
        /// Called every time a driver instance is created
        /// </summary>
        public ReferenceCountedObjectBase()
        {
            // We increment the global count of objects.
            Server2.IncrementObjectCount();
        }

        /// <summary>
        /// Called automatically by the garbage collection mechanic every time a driver instance is finalised (destroyed).
        /// </summary>
        ~ReferenceCountedObjectBase()
        {
            // We decrement the global count of objects.
            Server2.DecrementObjectCount();

            // We then immediately test to see if we the conditions
            // are right to attempt to terminate this server application.
            Server2.ExitIf();
        }
    }
}
