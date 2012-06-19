using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using NUnit.Framework;
using DNP3.Interface;

namespace DotNetInterface.Tests
{
    [TestFixture]
    public class ConfigSerializationTestSuite
    {
        [Test]
        public void TestXmlSerializationOfMasterConfig()
        {
            var mc = new SlaveStackConfig();
            var ser = new System.Xml.Serialization.XmlSerializer(mc.GetType());
            ser.Serialize(Console.Out, mc);

        }
    }
}
