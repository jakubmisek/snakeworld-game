namespace SnakeWorld_Server_App
{
    partial class FormMain
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

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(FormMain));
            this.listServers = new System.Windows.Forms.ListView();
            this.groupServer = new System.Windows.Forms.GroupBox();
            this.numericMaxSnakes = new System.Windows.Forms.NumericUpDown();
            this.lblSnakesMax = new System.Windows.Forms.Label();
            this.lblSnakesAtOneTime = new System.Windows.Forms.Label();
            this.lblSnakesCount = new System.Windows.Forms.Label();
            this.lblRecord = new System.Windows.Forms.Label();
            this.notifyIcon1 = new System.Windows.Forms.NotifyIcon(this.components);
            this.groupServer.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numericMaxSnakes)).BeginInit();
            this.SuspendLayout();
            // 
            // listServers
            // 
            this.listServers.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.listServers.Location = new System.Drawing.Point(12, 83);
            this.listServers.Name = "listServers";
            this.listServers.Size = new System.Drawing.Size(237, 67);
            this.listServers.TabIndex = 0;
            this.listServers.UseCompatibleStateImageBehavior = false;
            this.listServers.View = System.Windows.Forms.View.List;
            this.listServers.SelectedIndexChanged += new System.EventHandler(this.listServers_SelectedIndexChanged);
            this.listServers.DoubleClick += new System.EventHandler(this.listServers_DoubleClick);
            // 
            // groupServer
            // 
            this.groupServer.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.groupServer.Controls.Add(this.numericMaxSnakes);
            this.groupServer.Controls.Add(this.lblSnakesMax);
            this.groupServer.Controls.Add(this.lblSnakesAtOneTime);
            this.groupServer.Controls.Add(this.lblSnakesCount);
            this.groupServer.Controls.Add(this.lblRecord);
            this.groupServer.Location = new System.Drawing.Point(12, 12);
            this.groupServer.Name = "groupServer";
            this.groupServer.Size = new System.Drawing.Size(236, 65);
            this.groupServer.TabIndex = 1;
            this.groupServer.TabStop = false;
            this.groupServer.Text = "Server Info";
            // 
            // numericMaxSnakes
            // 
            this.numericMaxSnakes.Location = new System.Drawing.Point(178, 27);
            this.numericMaxSnakes.Maximum = new decimal(new int[] {
            1000,
            0,
            0,
            0});
            this.numericMaxSnakes.Name = "numericMaxSnakes";
            this.numericMaxSnakes.Size = new System.Drawing.Size(47, 20);
            this.numericMaxSnakes.TabIndex = 1;
            this.numericMaxSnakes.Value = new decimal(new int[] {
            100,
            0,
            0,
            0});
            this.numericMaxSnakes.ValueChanged += new System.EventHandler(this.numMaxSnakesCount_ValueChanged);
            // 
            // lblSnakesMax
            // 
            this.lblSnakesMax.AutoSize = true;
            this.lblSnakesMax.Location = new System.Drawing.Point(142, 29);
            this.lblSnakesMax.Name = "lblSnakesMax";
            this.lblSnakesMax.Size = new System.Drawing.Size(30, 13);
            this.lblSnakesMax.TabIndex = 0;
            this.lblSnakesMax.Text = "Max:";
            // 
            // lblSnakesAtOneTime
            // 
            this.lblSnakesAtOneTime.AutoSize = true;
            this.lblSnakesAtOneTime.Location = new System.Drawing.Point(6, 42);
            this.lblSnakesAtOneTime.Name = "lblSnakesAtOneTime";
            this.lblSnakesAtOneTime.Size = new System.Drawing.Size(16, 13);
            this.lblSnakesAtOneTime.TabIndex = 0;
            this.lblSnakesAtOneTime.Text = "...";
            // 
            // lblSnakesCount
            // 
            this.lblSnakesCount.AutoSize = true;
            this.lblSnakesCount.Location = new System.Drawing.Point(6, 29);
            this.lblSnakesCount.Name = "lblSnakesCount";
            this.lblSnakesCount.Size = new System.Drawing.Size(16, 13);
            this.lblSnakesCount.TabIndex = 0;
            this.lblSnakesCount.Text = "...";
            // 
            // lblRecord
            // 
            this.lblRecord.AutoSize = true;
            this.lblRecord.Location = new System.Drawing.Point(6, 16);
            this.lblRecord.Name = "lblRecord";
            this.lblRecord.Size = new System.Drawing.Size(16, 13);
            this.lblRecord.TabIndex = 0;
            this.lblRecord.Text = "...";
            // 
            // notifyIcon1
            // 
            this.notifyIcon1.Text = "SnakeWorld server";
            this.notifyIcon1.Visible = true;
            this.notifyIcon1.MouseDoubleClick += new System.Windows.Forms.MouseEventHandler(this.notifyIcon1_MouseDoubleClick);
            // 
            // FormMain
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(261, 162);
            this.Controls.Add(this.groupServer);
            this.Controls.Add(this.listServers);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MaximizeBox = false;
            this.Name = "FormMain";
            this.Text = "Snake World Server";
            this.Load += new System.EventHandler(this.FormMain_Load);
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.FormMain_FormClosing);
            this.Resize += new System.EventHandler(this.FormMain_Resize);
            this.groupServer.ResumeLayout(false);
            this.groupServer.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numericMaxSnakes)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.ListView listServers;
        private System.Windows.Forms.GroupBox groupServer;
        private System.Windows.Forms.NumericUpDown numericMaxSnakes;
        private System.Windows.Forms.Label lblSnakesMax;
        private System.Windows.Forms.Label lblSnakesAtOneTime;
        private System.Windows.Forms.Label lblSnakesCount;
        private System.Windows.Forms.Label lblRecord;
        private System.Windows.Forms.NotifyIcon notifyIcon1;


    }
}

