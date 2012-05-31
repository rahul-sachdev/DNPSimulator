using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using DNPDotNet;

namespace DotNetMasterDemo
{
    class Program
    {
        static void Main(string[] args)
        {
            var dnsm = new DotNetStackManager();
            dnsm.AddTCPClient("client", FilterLevelDN.LEV_INFO, 5000, "127.0.0.1", 20000);
            dnsm.AddMaster("client", "master", FilterLevelDN.LEV_INFO);

            Console.WriteLine("Press <Enter> to quit");
            Console.ReadLine();

        }
    }
}
