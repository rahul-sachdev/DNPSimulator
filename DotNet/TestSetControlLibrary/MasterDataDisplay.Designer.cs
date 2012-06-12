namespace TestSetControlLibrary
{
    partial class MasterDataDisplay
    {
        /// <summary> 
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary> 
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Component Designer generated code

        /// <summary> 
        /// Required method for Designer support - do not modify 
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.tabControl1 = new System.Windows.Forms.TabControl();
            this.tabPageBinary = new System.Windows.Forms.TabPage();
            this.flickerFreeListViewBinary = new TestSetControlLibrary.FlickerFreeListView();
            this.columnHeaderIndex = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.columnHeaderValue = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.tabPageAnalog = new System.Windows.Forms.TabPage();
            this.tabControl1.SuspendLayout();
            this.tabPageBinary.SuspendLayout();
            this.SuspendLayout();
            // 
            // tabControl1
            // 
            this.tabControl1.Controls.Add(this.tabPageBinary);
            this.tabControl1.Controls.Add(this.tabPageAnalog);
            this.tabControl1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tabControl1.Location = new System.Drawing.Point(0, 0);
            this.tabControl1.Name = "tabControl1";
            this.tabControl1.SelectedIndex = 0;
            this.tabControl1.Size = new System.Drawing.Size(838, 525);
            this.tabControl1.TabIndex = 0;
            // 
            // tabPageBinary
            // 
            this.tabPageBinary.Controls.Add(this.flickerFreeListViewBinary);
            this.tabPageBinary.Location = new System.Drawing.Point(4, 22);
            this.tabPageBinary.Name = "tabPageBinary";
            this.tabPageBinary.Padding = new System.Windows.Forms.Padding(3);
            this.tabPageBinary.Size = new System.Drawing.Size(830, 499);
            this.tabPageBinary.TabIndex = 0;
            this.tabPageBinary.Text = "Binary Input";
            this.tabPageBinary.UseVisualStyleBackColor = true;
            // 
            // flickerFreeListViewBinary
            // 
            this.flickerFreeListViewBinary.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.flickerFreeListViewBinary.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnHeaderIndex,
            this.columnHeaderValue});
            this.flickerFreeListViewBinary.Dock = System.Windows.Forms.DockStyle.Fill;
            this.flickerFreeListViewBinary.HeaderStyle = System.Windows.Forms.ColumnHeaderStyle.Nonclickable;
            this.flickerFreeListViewBinary.Location = new System.Drawing.Point(3, 3);
            this.flickerFreeListViewBinary.MultiSelect = false;
            this.flickerFreeListViewBinary.Name = "flickerFreeListViewBinary";
            this.flickerFreeListViewBinary.Size = new System.Drawing.Size(824, 493);
            this.flickerFreeListViewBinary.TabIndex = 0;
            this.flickerFreeListViewBinary.UseCompatibleStateImageBehavior = false;
            this.flickerFreeListViewBinary.View = System.Windows.Forms.View.Details;
            // 
            // columnHeaderIndex
            // 
            this.columnHeaderIndex.Text = "Index";
            // 
            // columnHeaderValue
            // 
            this.columnHeaderValue.Text = "Value";
            // 
            // tabPageAnalog
            // 
            this.tabPageAnalog.Location = new System.Drawing.Point(4, 22);
            this.tabPageAnalog.Name = "tabPageAnalog";
            this.tabPageAnalog.Padding = new System.Windows.Forms.Padding(3);
            this.tabPageAnalog.Size = new System.Drawing.Size(830, 499);
            this.tabPageAnalog.TabIndex = 1;
            this.tabPageAnalog.Text = "Analog Input";
            this.tabPageAnalog.UseVisualStyleBackColor = true;
            // 
            // MasterDataDisplay
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.tabControl1);
            this.Name = "MasterDataDisplay";
            this.Size = new System.Drawing.Size(838, 525);
            this.tabControl1.ResumeLayout(false);
            this.tabPageBinary.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TabControl tabControl1;
        private System.Windows.Forms.TabPage tabPageBinary;
        private System.Windows.Forms.TabPage tabPageAnalog;
        private FlickerFreeListView flickerFreeListViewBinary;
        private System.Windows.Forms.ColumnHeader columnHeaderIndex;
        private System.Windows.Forms.ColumnHeader columnHeaderValue;
    }
}
