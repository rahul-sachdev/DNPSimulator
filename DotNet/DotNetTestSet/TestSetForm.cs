using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

using DNPDotNet;
using DNP3.Interface;

namespace DotNetTestSet
{
    

    public partial class TestSetForm : Form
    {
        private StackManager sm;
        private LogControlAdapter lca;

        public TestSetForm()
        {
            

            InitializeComponent();

            this.sm = new StackManager();
            this.lca = new LogControlAdapter(this.logControl);
            sm.AddLogHandler(lca);            
        }

        private void button1_Click(object sender, EventArgs e)
        {            
            sm.AddTCPClient("client", FilterLevel.LEV_INFO, 5000, "127.0.0.1", 20000);
            ICommandAcceptor cmd = sm.AddMaster("client", "master", FilterLevel.LEV_INTERPRET, new NullDataObserver(), new MasterStackConfig());
        }
    }

    class LogControlAdapter : ILogHandler
    {


        public LogControlAdapter(LogControl ctrl)
        {
            this.logControl = ctrl;
        }

        private LogControl logControl;

        #region ILogHandler Members

        public void Log(LogEntry entry)
        {
            this.logControl.Invoke(new Action(() => logControl.AddLogEntry(entry)));
        }

        public void SetVar(string source, string varName, int value)
        {

        }

        #endregion
    }

    class NullDataObserver : IDataObserver
    {

        #region IDataObserver Members

        public void Start()
        {

        }

        public void Update(Binary update, uint index)
        {

        }

        public void Update(Analog update, uint index)
        {

        }

        public void Update(Counter update, uint index)
        {

        }

        public void Update(ControlStatus update, uint index)
        {

        }

        public void Update(SetpointStatus update, uint index)
        {

        }

        public void End()
        {

        }

        #endregion
    }
}