using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Windows.Forms;

using DNP3.Interface;

namespace DotNetTestSet
{
    public partial class LogControl : UserControl
    {
        public LogControl()
        {
            InitializeComponent();
        }

        public void AddLogEntry(LogEntry le)
        {
            this.listViewMessages.Items.Add(new ListViewItem(new String[] { le.filterLevel.ToString(), le.time.ToShortTimeString(), le.message }));            
        }
    }
}
