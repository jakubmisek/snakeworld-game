using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Linq;
using System.ServiceProcess;
using System.Text;
using System.Threading;

using SnakeWorld_Server;

namespace SnakeWorld_Server_Service
{
    public partial class SnakeWorldService : ServiceBase
    {
        public SnakeWorldService()
        {
            InitializeComponent();
        }

        private Listener _listener = null;


        protected override void OnStart(string[] args)
        {
            System.IO.Directory.SetCurrentDirectory("C:\\Program Files\\SnakeWorldServer\\");

            _listener = new Listener(1234, new Listener.WorldParams(400.0, 400.0, 300.0, 0.00006, 50));
        }

        protected override void OnStop()
        {

        }
        
    }
}
