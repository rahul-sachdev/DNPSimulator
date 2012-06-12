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
    public delegate void AddTcpClient(String name, String address, UInt16 port, FilterLevel level, UInt64 timeoutMs);
    public delegate void RemovePort(String name);

    public partial class StackBrowser : UserControl
    {
        public event AddTcpClient OnTcpClientAdded;
        public event RemovePort OnRemovePort;
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

        private void removeToolStripMenuItem_Click(object sender, EventArgs e)
        {
            TreeNode selected = this.treeView.SelectedNode;
            this.OnRemovePort(selected.Text);
        }

        public void AddRecordTcpClient(string name)
        {
            TreeNode comms = this.root.Nodes.Add(name, name);
            comms.ContextMenuStrip = this.contextMenuStripAddStack;            
        }

        public void RemoveRecordPort(string name)
        {
            this.root.Nodes.RemoveByKey(name);
        }

      
        
    }
}
