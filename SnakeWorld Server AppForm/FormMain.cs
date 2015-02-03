using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

using System.Threading;

using SnakeWorld_Server;

namespace SnakeWorld_Server_App
{
    public partial class FormMain : Form
    {
        /// <summary>
        /// form constructor
        /// </summary>
        public FormMain()
        {
            InitializeComponent();
        }

        /// <summary>
        /// Init servers
        /// </summary>
        /// <param name="sender">sender</param>
        /// <param name="e">form load parameter</param>
        private void FormMain_Load(object sender, EventArgs e)
        {
            notifyIcon1.Icon = this.Icon;
            notifyIcon1.Visible = true;
            notifyIcon1.Text = this.Text;

            listServers.Items.Add(new SnakeServer(OnServerUpdate, new SnakeServer.LoaderParams(1234, 400.0, 400.0, 300.0, 0.00004, 50)));
            listServers.Items.Add(new SnakeServer(OnServerUpdate, new SnakeServer.LoaderParams(1235, 200.0, 200.0, 300.0, 0.00004, 10)));
        }

        /// <summary>
        /// Server state updated
        /// </summary>
        /// <param name="server">server</param>
        private void OnServerUpdate(SnakeServer server)
        {
            if (server != SelectedServer)
                return;
            
            if (server == null)
            {
                InvokeSetLabelText(lblRecord, "...");
                InvokeSetLabelText(lblSnakesAtOneTime, "...");
                InvokeSetLabelText(lblSnakesCount, "...");
                InvokeSetNumericUpDownValue(numericMaxSnakes, 0);
            }
            else
            {
                // record
                if (server.WorldObject != null && server.WorldObject.BestScore.BestName != null)
                {
                    InvokeSetLabelText(lblRecord, "Record: " + server.WorldObject.BestScore.BestName + " ... " + server.WorldObject.BestScore.BestLength.ToString());
                }
                else
                {
                    InvokeSetLabelText(lblRecord, "Record: [not yet]");
                }

                // snakes count
                if (server.WorldObject != null)
                {
                    InvokeSetLabelText(lblSnakesCount, "Snakes count: " + server.WorldObject.SnakesCount);
                    InvokeSetNumericUpDownValue(numericMaxSnakes, server.WorldObject.MaxSnakesCount);
                }
                else
                {
                    InvokeSetLabelText(lblSnakesCount, "Snakes count: [unavailable]");
                }

                // Max snakes at one time
                InvokeSetLabelText(lblSnakesAtOneTime, "Max snakes at one time: " + server.MaxSnakesAtOneTime);

                // server label
                this.Invoke(new SetServerTextDelegate(SetServerText), new object[] { server, server.State });
            }
        }

        #region set info text
        private delegate void SetLabelTextDelegate(Control lbl, String str);
        private delegate void SetServerTextDelegate(SnakeServer lbl, String str);
        private delegate void SetNumericUpDownValueDelegate(NumericUpDown lbl, UInt32 value);

        private void SetLabelText(Control lbl, String str)
        {
            lbl.Text = str;
        }

        void SetServerText(SnakeServer lbl, String str)
        {
            lbl.Text = str;
        }

        void SetNumericUpDownValue(NumericUpDown lbl, UInt32 value)
        {
            lbl.Value = (decimal)value;
        }

        private void InvokeSetLabelText(Control lbl, String str)
        {
            if (lbl.InvokeRequired)
            {
                lbl.Invoke(new SetLabelTextDelegate(SetLabelText), new object[] { lbl, str });
            }
            else
            {
                SetLabelText(lbl, str);
            }
        }

        private void InvokeSetNumericUpDownValue(NumericUpDown lbl, UInt32 value)
        {
            if (lbl.InvokeRequired)
            {
                lbl.Invoke(new SetNumericUpDownValueDelegate(SetNumericUpDownValue), new object[] { lbl, value });
            }
            else
            {
                SetNumericUpDownValue(lbl, value);
            }
        }
        #endregion

        /// <summary>
        /// Change maximum count od snakes in the world
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void numMaxSnakesCount_ValueChanged(object sender, EventArgs e)
        {
            SnakeServer s = SelectedServer;

            if (s != null)
            {
                s.WorldObject.MaxSnakesCount = (uint)numericMaxSnakes.Value;
            }
        }

        private SnakeServer SelectedServer
        {
            get
            {
                try
                {
                    if (listServers.SelectedItems.Count > 0)
                    {
                        SnakeServer s = listServers.SelectedItems[0] as SnakeServer;

                        return s;
                    }
                }
                catch (Exception)
                { }

                return null;
            }
        }

        /// <summary>
        /// current displayed server select changed
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void listServers_SelectedIndexChanged(object sender, EventArgs e)
        {
            OnServerUpdate(SelectedServer);
        }

        private void listServers_DoubleClick(object sender, EventArgs e)
        {
            SnakeServer s = SelectedServer;

            if (s != null)
            {
                s.RestartListener();
            }
        }

        private void FormMain_Resize(object sender, EventArgs e)
        {
            if (WindowState == FormWindowState.Minimized)
                Hide();
        }

        private void notifyIcon1_MouseDoubleClick(object sender, MouseEventArgs e)
        {
            Show();
            WindowState = FormWindowState.Normal;
        }

        private void FormMain_FormClosing(object sender, FormClosingEventArgs e)
        {
            if ( MessageBox.Show(this, "Are you sure?", "Snake World Server", MessageBoxButtons.YesNo, MessageBoxIcon.Question ) == DialogResult.No )
            {
                e.Cancel = true;
            }
        }
        
    }
}
