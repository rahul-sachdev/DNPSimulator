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
    public partial class MasterDataDisplay : UserControl
    {
        public MasterDataDisplay()
        {
            InitializeComponent();
            this.DoubleBuffered = true;
        }

        public void UpdateBinary(Binary meas, UInt32 value)
        {             
            int index = (int) value;
            
            if(value >= this.flickerFreeListViewBinary.Items.Count) //add items to the view
            {
                for (int i = this.flickerFreeListViewBinary.Items.Count; i <= index; ++i)
                {
                    this.flickerFreeListViewBinary.Items.Add(i.ToString());
                }
            }            
                                   
            String[] items = { index.ToString(), meas.value.ToString() };
            this.flickerFreeListViewBinary.Items[index] = new ListViewItem(items);
        }
    }
}
