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
        private ICommandAcceptor cmdAcceptor = null;

        public MasterDataDisplay()
        {
            InitializeComponent();            
        }

        public ICommandAcceptor CommandAcceptor
        {
            set
            {
                this.cmdAcceptor = value;
            }

        }

        private ListViewItem RenderBinary(Binary meas, UInt32 index)
        {
            return new ListViewItem(new String[] { index.ToString(), meas.value.ToString() });
        }

        private ListViewItem RenderAnalog(Analog meas, UInt32 index)
        {
            return new ListViewItem(new String[] { index.ToString(), meas.value.ToString() });
        }

        private ListViewItem RenderCounter(Counter meas, UInt32 index)
        {
            return new ListViewItem(new String[] { index.ToString(), meas.value.ToString() });
        }

        private ListViewItem RenderBOStatus(ControlStatus meas, UInt32 index)
        {
            return new ListViewItem(new String[] { index.ToString(), meas.value.ToString() });
        }

        private ListViewItem RenderAOStatus(SetpointStatus meas, UInt32 index)
        {
            return new ListViewItem(new String[] { index.ToString(), meas.value.ToString() });
        }        

        public void UpdateBinary(Binary meas, UInt32 value)
        {
            Update(meas, value, flickerFreeListViewBinary, RenderBinary);
        }

        public void UpdateAnalog(Analog meas, UInt32 value)
        { 
            Update(meas, value, flickerFreeListViewAnalog, RenderAnalog);
        }

        public void UpdateCounter(Counter meas, UInt32 value)
        {
            Update(meas, value, flickerFreeListViewCounter, RenderCounter);
        }

        public void UpdateBOStatus(ControlStatus meas, UInt32 value)
        {
            Update(meas, value, flickerFreeListViewBOStatus, RenderBOStatus);
        }

        public void UpdateAOStatus(SetpointStatus meas, UInt32 value)
        {
            Update(meas, value, flickerFreeListViewAOStatus, RenderAOStatus);
        }
        
        private static void Update<T>(T meas, UInt32 index, FlickerFreeListView view, Func<T, UInt32, ListViewItem> render)
        {
            int idx = (int) index;

            if (idx >= view.Items.Count) //add items to the view
            {
                for (int i = view.Items.Count; i <= index; ++i)
                {
                    view.Items.Add(i.ToString());
                }
            }
            
            view.Items[idx] = render(meas, index);  
        }

        private void analogOutputToolStripMenuItem_Click(object sender, EventArgs e)
        {
            var form = new SetpointForm(this.cmdAcceptor);
            form.Show();
        }
    }
}
