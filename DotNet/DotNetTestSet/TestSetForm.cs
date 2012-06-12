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

        private void stackBrowser1_OnTcpClientAdded(string name, string address, ushort port, FilterLevel level, UInt64 timeout)
        {
            try
            {
                sm.AddTCPClient(name, level, timeout, address, port);
                this.stackBrowser1.AddRecordTcpClient(name);
            }
            catch(Exception ex)
            {
                this.logControl.AddLogEntry(new LogEntry(FilterLevel.LEV_ERROR, "testset", "", "A port already exists named: " + name, DateTime.Now, -1));
            }            
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