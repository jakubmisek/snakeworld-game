using System;
using System.Collections.Generic;
using System.Text;
using System.Diagnostics;

using System.IO;
using System.Net;
using System.Net.Sockets;

using System.Threading;

///
namespace SnakeWorld_Server
{
    /// <summary>
    /// The World class.
    /// Manages snakes and other objects shared by all clients.
    /// Manages communication with clients
    /// </summary>
    public class World
    {
        /// <summary>
        /// dead snake specification
        /// </summary>
        class DeadSnake
        {
            public readonly Snake  snake;
            public readonly UInt32 iKilledById;

            public DeadSnake( Snake snake, UInt32 iKilledById )
            {
                this.snake = snake;
                this.iKilledById = iKilledById;
            }
        }

        /// <summary>
        /// on new snake callback
        /// </summary>
        /// <param name="id">new snake id</param>
        /// <param name="strName">new snake name</param>
        public delegate void NewSnakeDelegate(Snake newSnake, World world);

        /// <summary>
        /// on snake dead callback
        /// </summary>
        /// <param name="id">dead snake id</param>
        public delegate void SnakeDeadDelegate(Snake deadSnake, World world);

        #region world objects
        /// <summary>
        /// list of the snakes in the world
        /// </summary>
        private readonly WorldScene scene;

        /// <summary>
        /// Best snake
        /// </summary>
        public readonly HighScore BestScore = new HighScore();

        /// <summary>
        /// Snakes count
        /// </summary>
        private UInt32 snakescount = 0;

        /// <summary>
        /// Maximum snakes count, other snakes connection will be refused
        /// </summary>
        public UInt32 MaxSnakesCount = 1000;

        /// <summary>
        /// Current snakes count
        /// </summary>
        public UInt32 SnakesCount
        {
            get
            {
                return snakescount;
            }
        }

        /// <summary>
        /// callback methods
        /// </summary>
        NewSnakeDelegate onNewSnake;
        SnakeDeadDelegate onSnakeDead;

        private string _worldName = null;
        public string WorldName
        {
            get
            {
                return _worldName;
            }
        }


        private DateTime _lastActivityTime = DateTime.Now;
        public DateTime LastActivityTime
        {
            get
            {
                return _lastActivityTime;
            }
        }

        #endregion

        /// <summary>
        /// World initialization
        /// </summary>
        /// <param name="onNewSnake">New snake event</param>
        /// <param name="onSnakeDead">Snake dead event</param>
        /// <param name="sceneCX">x-size of the scene</param>
        /// <param name="sceneCY">y-size of the scene</param>
        /// <param name="sceneRefreshDistance">scene draw distance (+ some tolerance)</param>
        /// <param name="applesCountInSqueareUnit">apples in one square unit</param>
        public World(NewSnakeDelegate onNewSnake, SnakeDeadDelegate onSnakeDead, double sceneCX, double sceneCY, double refreshDistace, double applesCountInSqueareUnit, string worldName)
        {
            this.onNewSnake = onNewSnake;
            this.onSnakeDead = onSnakeDead;

            scene = new WorldScene( sceneCX, sceneCY, refreshDistace, applesCountInSqueareUnit );  // world size-x, size-y, refresh-distance

            _worldName = WorldName;
            BestScore.FileNameId = worldName;
        }

        #region adding new snakes
        /// <summary>
        /// Try to add new snake into the world.
        /// </summary>
        /// <param name="tcpClient">New accepted connection. It's a request from new client.</param>
        public void AddSnake(TcpClient tcpClient)
        {
            Snake newSnake = new Snake(tcpClient, scene.GetFreePositionInScene(), GenerateSnakeId());

            ThreadPool.QueueUserWorkItem(new WaitCallback(StartSnake), newSnake);
        }

        #endregion

        #region snake initialization

        /// <summary>
        /// Next unique identifier for snakes.
        /// </summary>
        private UInt32 nextSnakeId = 0;

        /// <summary>
        /// Generates new unique identifier for a snake.
        /// </summary>
        /// <returns>new unique identifier</returns>
        private UInt32 GenerateSnakeId()
        {
            lock(this)
            {
                if (nextSnakeId >= 255) nextSnakeId = 0;    // because of the bug in the first release of the Game
                while (scene.FindSnakeById(++nextSnakeId) != null);

                return nextSnakeId;
            }
        }

        /// <summary>
        /// Wait for data from the client and initialize his snake.
        /// </summary>
        /// <param name="snake">Snake waiting to be initialized.</param>
        private void StartSnake(Object/*!*/objSnake)
        {
            //
            Snake newSnake = objSnake as Snake;

            if (newSnake == null)
                throw new ArgumentNullException("snake", "must be valid Snake object");

            // check snakes count
            lock (this)
            {
                if (snakescount >= MaxSnakesCount)
                {
                    // send error message
                    SendError(newSnake, "Too many connections, server is full. Try again later.");
                    // close the connection
                    newSnake.networkStream.Close();
                    return;
                }

                // ok
                ++snakescount;  // increase snakes count
                _lastActivityTime = DateTime.Now;
            }

            // this snake is killed by:
            UInt32 newSnakeIsKilledBy = newSnake.Id();

            // snake live
            try
            {
                // initialize, the first commands block, should throw InvalidDataException
                ReceiveCommandsBlock(newSnake);

                // send initialization done and successful
                SendInitDone(newSnake);

                // add the snake to the list of snakes
                if (onNewSnake != null) onNewSnake(newSnake, this);
                scene.AddSnake(newSnake);

                // send apples list
                scene.SendApplesState(newSnake);
                
                // start main data receive loop for this snake
                StartReceive(newSnake);
            }
            catch (InvalidDataException ex)
            {
                SendError(newSnake, ex.Message);
            }
            catch (SnakeDeadException ex)
            {
                newSnakeIsKilledBy = ex.KilledBySnakeId;

                Snake killedBy = scene.FindSnakeById(ex.KilledBySnakeId);
                if (killedBy != null)
                    killedBy.SnakeKilled(newSnake.Id());
            }
            catch(IOException)
            {
                // connection lost
                // client application was probably terminated
            }
            catch (Exception)
            {
                // unknown error
                // this should happen when client send wrong data and ParseCommand did not catch the error
                // or something is wrong -> disconnect+kill the client
            }
            
            // remove the snake from the world
            scene.RemoveSnake(newSnake);

            lock (this)
            {
                --snakescount;  // decrease snakes count
            }

            _lastActivityTime = DateTime.Now;
            
            if (onSnakeDead != null)
                onSnakeDead(newSnake, this);

            
            // inform the clients, the "newSnake" is dead
            scene.EnumerateSnakes(
                new BoundRect(newSnake.HeadPosition, scene.sceneRefreshDistance),  // everything near the newSnake
                SendHeIsDead, new DeadSnake(newSnake, newSnakeIsKilledBy));     // call this method
            
            // close the connection
            newSnake.bw.StopSending();
            newSnake.networkStream.Close();

            // finish logged snake session
            // update statistics
            newSnake.Logout();
        }

        #endregion

        #region sending commands

        /// <summary>
        /// Send successful initialization packet to the client.
        /// </summary>
        /// <param name="snake">Specifies the client.</param>
        private void SendInitDone(Snake snake)
        {
            lock (snake.bw)
            {
                // SERVER NAME
                ConnectionData.Send_CmdId(snake.bw,ECmdIds.CMD_SERVER);
                ConnectionData.Send_String(snake.bw,System.Security.Principal.WindowsIdentity.GetCurrent().Name);

                // SCENE SIZE
                ConnectionData.Send_CmdId(snake.bw, ECmdIds.CMD_SCENE);
                snake.bw.Write(scene.sceneCX);
                snake.bw.Write(scene.sceneCY);
                snake.bw.Write(scene.sceneRefreshDistance);

                // BEST SCORE
                BestScore.SendBestScore(snake);
                
                // YOUR INIT INFO SNAKE
                snake.SendAsYourSnake();
            }
        }

        /// <summary>
        /// Send initialization failed packet.
        /// </summary>
        /// <param name="snake">Specifies the client.</param>
        /// <param name="message">Error message</param>
        private void SendError(Snake snake, string message)
        {
            lock (snake.bw)
            {
                ConnectionData.Send_CmdId(snake.bw, ECmdIds.CMD_ERROR);
                ConnectionData.Send_String(snake.bw, message);

                ConnectionData.Send_CmdId(snake.bw, ECmdIds.CMD_BLOCKEND);

                snake.bw.Flush();
            }
        }

        /// <summary>
        /// Send all known info about specified snake.
        /// Specification and all segments will be sent.
        /// </summary>
        /// <param name="snakeTo">Client requesting this data.</param>
        /// <param name="snakeToBeSentId">Snake ID which will be sent.</param>
        private void SendWholeSnake(Snake snakeTo, UInt32 snakeToBeSentId)
        {
            if (snakeTo.Id() == snakeToBeSentId)
                return;

            Snake snakeToBeSent = scene.FindSnakeById(snakeToBeSentId);

            if (snakeToBeSent != null)
            {
                snakeToBeSent.SendEverything(snakeTo.bw);
            }
            else
            {
                // unknown snake ID, ignore
            }
        }

        /// <summary>
        /// Sends new information (dirty values) to the other snakes.
        /// </summary>
        /// <param name="snake">Snake which will be sent.</param>
        private void SendModifications(Snake snake)
        {
            if (snake.HasDirtyValues())
            {
                /// find the snakes near this snake
                scene.EnumerateSnakes(
                    new BoundRect(snake.HeadPosition, scene.sceneRefreshDistance), // everything near the snake
                    SendDirtyValues, snake);    // call this method

                // reset changed values flags
                snake.ClearDirtyValues();
            }
        }

        /// <summary>
        /// send dead message
        /// </summary>
        /// <param name="snakeTo">send message to this snake</param>
        /// <param name="objSnakeFrom">send message from the snake specified by this struct</param>
        private void SendHeIsDead( Snake snakeTo, Object objSnakeFrom )
        {
            DeadSnake ds = (objSnakeFrom as DeadSnake);
            if (ds.snake != snakeTo)
                ds.snake.SendImDead(snakeTo.bw, ds.iKilledById);
        }

        /// <summary>
        /// send dirty values
        /// </summary>
        /// <param name="snakeTo">target client</param>
        /// <param name="snakeFrom">the dirty snake</param>
        private void SendDirtyValues( Snake snakeTo, Object snakeFrom )
        {
            if (snakeFrom != snakeTo)
                (snakeFrom as Snake).SendDirtyValues(snakeTo);
        }

        #endregion

        #region receiving commands

        private readonly char[] parameters_delimiter = new char[] { ',' };

        /// <summary>
        /// Receives commands block terminated by null-length line
        /// </summary>
        /// <param name="snake">The snake object describing the client.</param>
        private void ReceiveCommandsBlock(Snake snake)
        {
            // data receive loop
            while (true)
            {
                // read command ID from the stream
                ECmdIds uCmdId = (ECmdIds)snake.br.ReadByte();

                if (uCmdId == ECmdIds.CMD_BLOCKEND)
                    break;

                // read the rest of the command, process the command
                ParseCommand(snake, uCmdId);
            }
        }

        /// <summary>
        /// Start receiving of data packets from the new snake.
        /// </summary>
        /// <param name="snake">Snake object which will send data</param>
        private void StartReceive(Snake snake)
        {
            // data receive loop
            while (true)
            {
                // receive&parse next commands block
                ReceiveCommandsBlock(snake);
            
                // send new info to the other snakes
                SendModifications(snake);
            }
        }

        /// <summary>
        /// Parse the received command line.
        /// </summary>
        /// <param name="snake">Snake which sent the command.</param>
        /// <param name="strLine">The command line.</param>
        private void ParseCommand(Snake snake, ECmdIds cmd)
        {
            BinaryReader br = snake.br;
            
            //Debug.WriteLine("from "+snake.Id + ":" + strLine);
            
            // process the command
            switch (cmd)
            {
                // Initialization commands
                case ECmdIds.CMD_NAME:
                    snake.Name = ConnectionData.Receive_String(br);
                    break;
                case ECmdIds.CMD_LOGIN:
                    {
                        string userEmail = ConnectionData.Receive_String(br);
                        string userPassword = ConnectionData.Receive_String(br);

                        snake.Login(userEmail, userPassword);
                    }
                    break;
                case ECmdIds.CMD_DESCRIPTION:
                    snake.Description = ConnectionData.Receive_String(br);
                    break;
                case ECmdIds.CMD_LANGUAGE:
                    snake.LanguageCode = br.ReadUInt32();
                    break;
                case ECmdIds.CMD_TEXTURE:
                    snake.TextureFile = ConnectionData.Receive_String(br);
                    break;
                case ECmdIds.CMD_TYPE:
                    snake.BodyType = ConnectionData.Receive_String(br);
                    break;
                // In-game commands
                case ECmdIds.CMD_SNAKE_HEAD:
                    {
                        double x = br.ReadDouble();
                        double y = br.ReadDouble();
                        snake.HeadPosition = new Position(x,y);
                    }
                    break;
                case ECmdIds.CMD_SNAKE_DIR:
                    snake.HeadDirection = br.ReadDouble();
                    break;
                case ECmdIds.CMD_SNAKE_SPEED:
                    snake.Speed = br.ReadDouble();
                    break;
                case ECmdIds.CMD_SNAKE_LENGTH:
                    snake.Length = br.ReadDouble();

                    // check best scores
                    BestScore.CheckBest(snake);

                    // re-send best score to this snake
                    BestScore.SendBestScore(snake);

                    break;
                case ECmdIds.CMD_SNAKE_SEG:
                    {
                        UInt32 segId = br.ReadUInt32();

                        double x = br.ReadDouble();
                        double y = br.ReadDouble();
                        Position segPos = new Position( x,y );

                        // update the snake body
                        snake.OnNewSegmentReceived(segId, segPos);

                        // update the position in the world scene tree
                        scene.MoveSnakeBounds(snake, new BoundRect(snake.Bound, segPos));
                    }
                    break;
                case ECmdIds.CMD_SNAKE_DEAD:
                    throw new SnakeDeadException(br.ReadUInt32());
                case ECmdIds.CMD_SNAKE_BOUND:
                    {
                        double x1, y1, x2, y2;
                        x1 = br.ReadDouble();
                        y1 = br.ReadDouble();
                        x2 = br.ReadDouble();
                        y2 = br.ReadDouble();

                        // update the position in the world scene tree
                        scene.MoveSnakeBounds(snake, new BoundRect(x1, y1, x2, y2));
                    }
                    break;
                case ECmdIds.CMD_IDONTKNOW:
                    {
                        UInt32 unknownSnakeId = br.ReadUInt32();
                        SendWholeSnake(snake, unknownSnakeId);
                    }
                    break;
                case ECmdIds.CMD_APPLEEATEN:
                    {
                        UInt32 iEatenApple = br.ReadUInt32();
                        scene.OnAppleEaten(iEatenApple);
                    }
                    break;
                case ECmdIds.CMD_CHATMESSAGE:
                    {
                        string strToUsers = ConnectionData.Receive_String(br);
                        string strMessage = ConnectionData.Receive_String(br);

                        scene.ResendMessage(snake, strToUsers, strMessage);
                    }
                    break;
                // Unknown commands
                default:
                    throw new InvalidDataException("Unknown command '" + cmd.ToString() + "' received");
            }
        }
        #endregion

    }
}
