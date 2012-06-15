using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

using DNP3.Interface;

namespace TestSetControlLibrary
{    
    public partial class TcpClientSettingsForm : Form
    {       
        public delegate void AddTcpClient(TcpClientSettings settings);
        public event AddTcpClient OnTcpClientAdded;               

        public TcpClientSettingsForm()
        {
            InitializeComponent();            
            this.FormClosed += new FormClosedEventHandler(TcpClientSettingsForm_FormClosed);
        }

        void TcpClientSettingsForm_FormClosed(object sender, FormClosedEventArgs e)
        {
            if(DialogResult == DialogResult.OK && OnTcpClientAdded != null) 
            {
                String name = this.textBoxName.Text;
                String address = this.textBoxAddress.Text;
                UInt16 port = Decimal.ToUInt16(this.numericUpDownPort.Value);
                FilterLevel level = this.filterLevelComboBox1.SelectedLevel;
                UInt64 timeout = Decimal.ToUInt64(this.numericUpDownTimeout.Value);
                OnTcpClientAdded(new TcpClientSettings(name, address, port, level, timeout));            
            }
        }

        private void buttonAdd_Click(object sender, EventArgs e)
        {
            this.DialogResult = DialogResult.OK;
            this.Close();
        }

        
    }
}
