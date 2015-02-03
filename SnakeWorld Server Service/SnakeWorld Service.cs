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

        protected List<Listener> listeners = new List<Listener>();


        protected override void OnStart(string[] args)
        {
            System.IO.Directory.SetCurrentDirectory("C:\\Program Files\\SnakeWorldServer\\");

            ThreadPool.QueueUserWorkItem(new WaitCallback(Loader), new LoaderParams(1234, 400.0, 400.0, 300.0, 0.00006, 50));
            ThreadPool.QueueUserWorkItem(new WaitCallback(Loader), new LoaderParams(1235, 200.0, 200.0, 300.0, 0.00004, 10));
        }

        protected override void OnStop()
        {

        }

        /// <summary>
        /// Loader parameters
        /// </summary>
        public class LoaderParams
        {
            public readonly int port;
            public readonly double cx, cy, refreshdist, applesinsquare;
            public readonly uint maxsnakes;

            /// <summary>
            /// Init
            /// </summary>
            /// <param name="port">listening port number</param>
            /// <param name="cx">scene cx</param>
            /// <param name="cy">scene cy</param>
            /// <param name="refreshdist">scene refresh distance</param>
            /// <param name="applesinsquare">apples count in square</param>
            /// <param name="maxsnakes">maximum snakes count</param>
            public LoaderParams(int port, double cx, double cy, double refreshdist, double applesinsquare,uint maxsnakes)
            {
                this.port = port;
                this.cx = cx;
                this.cy = cy;
                this.refreshdist = refreshdist;
                this.applesinsquare = applesinsquare;
                this.maxsnakes = maxsnakes;
            }
        }

        /// <summary>
        /// Server loading, in separate thread.
        /// </summary>
        /// <param name="obj">null</param>
        private void Loader(object obj)
        {
            try
            {
                LoaderParams parameters = obj as LoaderParams;

                if (parameters == null)
                    return;

                // create the world
                World world = InitWorld(parameters);

                // start listening on specified port
                Listener listener = StartListening(parameters, world);

                // add into the list
                if (listener != null)
                    lock (this)
                        listeners.Add(listener);

            }
            catch(Exception)
            {

            }

        }

        /// <summary>
        /// Init the world
        /// </summary>
        private World InitWorld(LoaderParams parameters)
        {
            World world = new World(null, null, parameters.cx, parameters.cy, parameters.refreshdist, parameters.applesinsquare);
            world.MaxSnakesCount = parameters.maxsnakes;

            return world;
        }

        /// <summary>
        /// Start listener object
        /// </summary>
        private Listener StartListening(LoaderParams parameters, World world)
        {
            try
            {
                return new Listener(parameters.port, world);
            }
            catch (Exception)
            {
                // listening error
                return null;
            }
        }
    }
}
