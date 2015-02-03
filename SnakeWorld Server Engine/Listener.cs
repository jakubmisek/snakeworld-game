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
        private World world;

        /// <summary>
        /// current tcp listener started in constructor
        /// </summary>
        private TcpListener tcpListener;

        /// <summary>
        /// start the server
        /// </summary>
        /// <param name="port">listening port</param>
        /// <param name="world">the world associated with this listener</param>
        public Listener(int port, World world)
        {
            // the world associated with this listener
            this.world = world;
            world.BestScore.PortNumber = port;

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

            if (obj == null)
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
                        world.AddSnake(newClient);
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
