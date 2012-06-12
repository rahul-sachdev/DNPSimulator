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
using TestSetControlLibrary;

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
                MessageBox.Show(ex.ToString());
            }            
        }

        private void stackBrowser1_OnRemovePort(string name)
        {
            try
            {
                sm.RemovePort(name);
                this.stackBrowser1.RemoveRecordPort(name);
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.ToString());
            }
        }

        private void stackBrowser1_OnAddMaster(string name, string port, FilterLevel level, MasterStackConfig config)
        {
            try
            {
                //create the new form
                var display = new MasterDataDisplay();
                var adapter = new DisplayAdapterDataObserver(display);
                display.Dock = DockStyle.Fill;
                TabPage page = new TabPage(name);                
                this.tabControlDisplay.TabPages.Add(page);                
                page.Controls.Add(display);
                sm.AddMaster(port, name, level, adapter, config);
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.ToString());
            }
        }
    }

    
    
}