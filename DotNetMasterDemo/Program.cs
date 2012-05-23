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
            BinaryOutputDN bo = new BinaryOutputDN(ControlCodeDN.CC_LATCH_ON, 1, 100, 100);

        }
    }
}
