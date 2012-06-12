using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Windows.Forms;

using DNP3.Interface;

namespace TestSetControlLibrary
{
    public partial class StackBrowser : UserControl
    {
        public event AddTcpClient OnTcpClientAdded;
        private TreeNode root = new TreeNode("root");

        public StackBrowser()
        {
            InitializeComponent();
            root.ContextMenuStrip = this.contextMenuStripAddComms;
            this.treeView.Nodes.Add(root);
        }

        private void clientToolStripMenuItem1_Click(object sender, EventArgs e)
        {
            TcpClientSettingsForm dialog = new TcpClientSettingsForm();
            dialog.OnTcpClientAdded += new AddTcpClient(dialog_OnTcpClientAdded);
            dialog.Show();
        }

        private void dialog_OnTcpClientAdded(string name, string address, ushort port, FilterLevel level, UInt64 timeout)
        {
            this.OnTcpClientAdded(name, address, port, level, timeout);                   
        }

        public void AddRecordTcpClient(string name)
        {
            TreeNode comms = new TreeNode(name);
            comms.ContextMenuStrip = this.contextMenuStripAddStack;
            this.root.Nodes.Add(comms);
        }

      
        
    }
}
