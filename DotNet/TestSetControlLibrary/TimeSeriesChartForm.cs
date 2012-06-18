using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

using System.Windows.Forms.DataVisualization.Charting;

namespace TestSetControlLibrary
{
    public partial class TimeSeriesChartForm : Form
    {
        private Timer t1 = new System.Windows.Forms.Timer();
        Series series;
        Random r = new Random();

        public TimeSeriesChartForm()
        {
            InitializeComponent();
            
            series = this.chart1.Series.Add("series 1");
            series.XValueType = ChartValueType.Double;
            series.YValueType = ChartValueType.Double;
            series.ChartType = SeriesChartType.Spline;
            t1.Tick += new EventHandler(t1_Tick);
            t1.Interval = 1000;
            t1.Start();
            
        }

        void t1_Tick(object sender, EventArgs e)
        {
            series.Points.AddXY(DateTime.Now, r.NextDouble());                       
        }

        private void TimeSeriesChartForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            this.t1.Stop();
        }
    }
}
