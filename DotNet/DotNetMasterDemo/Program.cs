using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using DNPDotNet;
using DNP3.Interface;

namespace DotNetMasterDemo
{

    class PrintingDataObserver : IDataObserver
    {
        public void End()
        {
            Console.WriteLine("End");
        }

        public void Start()
        {
            Console.WriteLine("Start");
        }

        public void Update(Binary update, System.UInt32 index)
        {
            Console.WriteLine(update.value);
        }

        public void Update(Analog update, System.UInt32 index)
        {
            Console.WriteLine(update.value);
        }

        public void Update(Counter update, System.UInt32 index)
        {
            Console.WriteLine(update.value);
        }

        public void Update(ControlStatus update, System.UInt32 index)
        {
            Console.WriteLine(update.value);
        }

        public void Update(SetpointStatus update, System.UInt32 index)
        {
            Console.WriteLine(update.value);
        }
    }

    class Program
    {
        static void Main(string[] args)
        {
            var dnsm = new DotNetStackManager();
            dnsm.AddTCPClient("client", FilterLevel.LEV_INFO, 5000, "127.0.0.1", 20000);
            var commandAcceptor = dnsm.AddMaster("clint", "master", FilterLevel.LEV_INFO, new PrintingDataObserver());

            Console.WriteLine("Enter an index to send a command");

            while (true)
            {
                System.UInt32 index = System.UInt32.Parse(Console.ReadLine());
                var future = commandAcceptor.AcceptCommand(new BinaryOutput(ControlCode.CC_PULSE, 1, 100, 100), index);
                Console.WriteLine("Result: " + future.Await());
            }
        }
    }
}
