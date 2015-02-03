using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;

using System.Threading;

using SnakeWorld_Server;



namespace SnakeWorld_Server_App
{
    class SnakeServer: ListViewItem
    {
        /// <summary>
        /// Server data changed
        /// </summary>
        /// <param name="server">server invoked the event</param>
        public delegate void OnUpdate(SnakeServer server);

        /// <summary>
        /// on update event handler
        /// </summary>
        private OnUpdate OnUpdateEvent;

        /// <summary>
        /// server objects
        /// </summary>
        private Listener listener;
        private World world;
        
        /// <summary>
        /// World object
        /// </summary>
        public World WorldObject
        {
            get
            {
                return world;
            }
        }

        /// <summary>
        /// Max snakes at one time
        /// </summary>
        private uint _maxSnakesAtOneTime = 0;

        /// <summary>
        /// Max snakes at one time
        /// </summary>
        public uint MaxSnakesAtOneTime
        {
            get
            {
                return _maxSnakesAtOneTime;
            }
        }

        /// <summary>
        /// Current servers state
        /// </summary>
        private string _state = "";

        /// <summary>
        /// Current servers state
        /// </summary>
        public string State
        {
            get
            {
                return _state;
            }
            set
            {
                _state = value;

                if (OnUpdateEvent != null)
                {
                    OnUpdateEvent(this);
                }
            }
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
        /// Initialization
        /// </summary>
        /// <param name="onupdate">on update event handler</param>
        public SnakeServer(OnUpdate onupdate,LoaderParams parameters)
        {
            OnUpdateEvent = onupdate;
            this.parameters = parameters;

            ThreadPool.QueueUserWorkItem(new WaitCallback(Loader));
        }

        /// <summary>
        /// restart listener if not started yet
        /// </summary>
        public void RestartListener()
        {
            if (listener == null)
            {
                StartListening();
            }
        }

        /// <summary>
        /// parameters of this server
        /// </summary>
        private readonly LoaderParams parameters;

        /// <summary>
        /// Server loading, in separate thread.
        /// </summary>
        /// <param name="obj">null</param>
        private void Loader(object obj)
        {
            if (parameters == null)
            {
                State = "no parameters";
                return;
            }

            // create the world
            InitWorld();

            // start listening on specified port
            StartListening();

        }

        /// <summary>
        /// Init the world
        /// </summary>
        private void InitWorld()
        {
            //State = "Creating the world ...";
            //world = new World(OnNewSnake, OnSnakeDead, parameters.cx, parameters.cy, parameters.refreshdist, parameters.applesinsquare);
            //world.MaxSnakesCount = parameters.maxsnakes;
        }

        /// <summary>
        /// Start listener object
        /// </summary>
        private void StartListening()
        {
            State = "Starting listening on port " + parameters.port;
            
            try
            {
                //listener = new Listener(parameters.port, world);

                // done
                State = "Running on port " + parameters.port;
            }
            catch (Exception)
            {
                // listening error
                State = "Listening on port " + parameters.port + " failed!";
            }
        }

        /// <summary>
        /// on new snake
        /// </summary>
        /// <param name="newSnake">new snake</param>
        private void OnNewSnake(Snake newSnake)
        {
            uint n = world.SnakesCount;

            if (n > _maxSnakesAtOneTime)
                _maxSnakesAtOneTime = n;

            if (OnUpdateEvent != null)
            {
                OnUpdateEvent(this);
            }
        }

        /// <summary>
        /// on snake dead
        /// </summary>
        /// <param name="deadSnake">dead snake</param>
        private void OnSnakeDead(Snake deadSnake)
        {
            if (OnUpdateEvent != null)
            {
                OnUpdateEvent(this);
            }
        }

    }
}
