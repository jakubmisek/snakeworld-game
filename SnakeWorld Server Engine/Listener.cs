using System;
using System.Collections.Generic;
using System.Text;

using System.IO;
using System.Net;
using System.Net.Sockets;

using System.Threading;


namespace SnakeWorld_Server
{
    /// <summary>
    /// The listener object is listening on specified port, accepts incoming connections and tries to add them into the world.
    /// </summary>
    public class Listener
    {
        /// <summary>
        /// the world object used by all connections accepted by this listener
        /// </summary>
        private readonly WorldParams _worldParameters;

        private readonly int _listenerPort = 0;

        /// <summary>
        /// current tcp listener started in constructor
        /// </summary>
        private TcpListener tcpListener;

        /// <summary>
        /// Loader parameters
        /// </summary>
        public class WorldParams
        {
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
            public WorldParams(double cx, double cy, double refreshdist, double applesinsquare, uint maxsnakes)
            {
                this.cx = cx;
                this.cy = cy;
                this.refreshdist = refreshdist;
                this.applesinsquare = applesinsquare;
                this.maxsnakes = maxsnakes;
            }
        }

        /// <summary>
        /// start the server
        /// </summary>
        /// <param name="port">listening port</param>
        /// <param name="world">the world associated with this listener</param>
        public Listener(int port, WorldParams worldParameters)
        {
            // the world associated with this listener
            _worldParameters = worldParameters;
            _listenerPort = port;
            
            // create the listener object
            // start listening on the specified port
            tcpListener = new TcpListener(IPAddress.Any, port);
            tcpListener.Start();

            // start accepting incoming connections on new thread
            ThreadPool.QueueUserWorkItem(new WaitCallback(ConnectionAccepter), tcpListener);
        }

        /// <summary>
        /// Stop listening
        /// </summary>
        public void CloseListener()
        {
            if (tcpListener != null)
            {
                tcpListener.Stop();
            }
        }

        /// <summary>
        /// accept and process all incoming connections
        /// </summary>
        /// <param name="obj">(TcpListener) waiting for incoming connections</param>
        private void ConnectionAccepter(object obj)
        {
            // used listener object
            TcpListener tcpListener = obj as TcpListener;

            if (obj == null)
                throw new ArgumentNullException("obj", "not a TcpListener object");

            // main connection accepting loop
            while (true)
            {
                try
                {
                    // accept new connection
                    TcpClient newClient = tcpListener.AcceptTcpClient();

                    // add accepted "snake" to the world
                    if (newClient.Connected)
                    {
                        ThreadPool.QueueUserWorkItem(new WaitCallback(OnNewConnection), newClient);
                    }
                }
                catch (Exception)
                {
                    break;
                }                
            }
        }

        /// <summary>
        /// Process new incoming connection.
        /// Runs on the own thread.
        /// </summary>
        /// <param name="obj">TcpClient</param>
        private void OnNewConnection( object obj )
        {
            // used listener object
            TcpClient newClient = obj as TcpClient;

            if (newClient == null)
                throw new ArgumentNullException("obj", "not a TcpClient object");

            
            try
            {
                NetworkStream networkStream = newClient.GetStream();

                BinaryReader br = new BinaryReader(networkStream);
                BinaryWriter bw = new BinaryWriter(networkStream);
                
                // read the first string (init command)
                string[] strCommand = ConnectionData.Receive_String(br).Split( new char[]{' '} );

                if (strCommand.Length > 0)
                switch (strCommand[0])
                {
                    case ConnectionData.CMD_NEWGAMECMD:
                        GetWorld(strCommand).AddSnake(newClient);
                        return; // do not close the connection !

                    case ConnectionData.CMD_GETTEXTURE:
                        WriteFile(bw, "public\\" + ConnectionData.Receive_String(br));
                        break;

                    default:
                        // unknown request
                        break;
                }

                // close the connection
                networkStream.Close();
            }
            catch(Exception)
            {
                // connection end or something
            }
            finally
            {
                
            }
        }


        /// <summary>
        /// Worlds by their names.
        /// </summary>
        private Dictionary<string, World> _worlds = new Dictionary<string, World>();


        /// <summary>
        /// Get the world by its name.
        /// The new world can be initialized.
        /// </summary>
        /// <param name="worldNames"></param>
        /// <returns></returns>
        private World GetWorld(string[] worldNames)
        {
            string worldName = _listenerPort.ToString();

            for (int i = 1; i < worldNames.Length; ++i )
                worldName += worldNames[i].ToLower();

            lock(this)
            {
                World world = null;

                if ( !_worlds.TryGetValue(worldName, out world))
                {
                    if (_worlds.Count > 50)
                        RemoveEmptyWorldsL();

                    world = new World(null, null, _worldParameters.cx, _worldParameters.cy, _worldParameters.refreshdist, _worldParameters.applesinsquare, worldName);
                    world.MaxSnakesCount = _worldParameters.maxsnakes;

                    _worlds[worldName] = world;                    
                }

                return world;
            }
        }


        /// <summary>
        /// removes all inactive worlds without snakes.
        /// </summary>
        private void RemoveEmptyWorldsL()
        {
            List<string> toremove = new List<string>();

            foreach (KeyValuePair<string,World> w in _worlds)
            {
                if (w.Value.SnakesCount == 0 &&
                    w.Value.LastActivityTime.AddMinutes(5.0) < DateTime.Now)
                {
                    toremove.Add(w.Key);
                }
            }

            foreach (string w in toremove)
            {
                _worlds.Remove(w);
            }
        }


        /// <summary>
        /// Sends the specified file into the binary writer
        /// </summary>
        /// <param name="bw">binary writer</param>
        /// <param name="strFileName">file to be sent</param>
        private void WriteFile(BinaryWriter bw, string strFileName)
        {
            FileInfo fi = new FileInfo( strFileName.ToLower().TrimStart(new char[]{'.'}) );

            if (fi.Exists)
            {
                FileStream fs = fi.OpenRead();
                
                byte[] buff = new byte[4096];
                int readlength;

                while ( (readlength = fs.Read( buff, 0, buff.Length )) > 0 )
                {
                    bw.Write(buff, 0, readlength);
                }

                fs.Close();
            }
        }
    }
    
}
