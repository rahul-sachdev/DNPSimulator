using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using DNPDotNet;
using DNP3.Interface;

namespace DotNetMasterDemo
{

    class PrintingDataObserver : IDataObserverDN
    {
        public void End()
        {
            Console.WriteLine("End");
        }

        public void Start()
        {
            Console.WriteLine("Start");
        }

        public void Update(Binary update, int index)
        {
            Console.WriteLine(update.value);
        }

        public void Update(Analog update, int index)
        {
            Console.WriteLine(update.value);
        }

        public void Update(Counter update, int index)
        {
            Console.WriteLine(update.value);
        }

        public void Update(ControlStatus update, int index)
        {
            Console.WriteLine(update.value);
        }

        public void Update(SetpointStatus update, int index)
        {
            Console.WriteLine(update.value);
        }
    }

    class Program
    {
        static void Main(string[] args)
        {
            var dnsm = new DotNetStackManager();
            dnsm.AddTCPClient("client", FilterLevelDN.LEV_INFO, 5000, "127.0.0.1", 20000);
            dnsm.AddMaster("client", "master", FilterLevelDN.LEV_INFO, new PrintingDataObserver());

            Console.WriteLine("Press <Enter> to quit");
            Console.ReadLine();

        }
    }
}
