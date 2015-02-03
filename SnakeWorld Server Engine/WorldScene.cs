using System;
using System.Collections.Generic;
using System.Text;
using System.Threading;

namespace SnakeWorld_Server
{
    class WorldScene
    {
        /// <summary>
        /// Random number generator.
        /// </summary>
        private Random rnd = new Random();

        /// <summary>
        /// scene size
        /// </summary>
        public readonly double sceneCX, sceneCY;

        /// <summary>
        /// refresh distance
        /// </summary>
        public readonly double sceneRefreshDistance;

        /// <summary>
        /// snakes saved by id
        /// </summary>
        private readonly HashTable<Snake> snakesHashTable;

        /// <summary>
        /// snakes saved in the tree by position
        /// </summary>
        private readonly SceneTree<Snake> snakesTree;

        /// <summary>
        /// List of the apples in the scene
        /// </summary>
        private readonly ApplesList applesList;

        /// <summary>
        /// Scene constructor.
        /// </summary>
        /// <param name="sceneCX">x-size of the scene</param>
        /// <param name="sceneCY">y-size of the scene</param>
        /// <param name="sceneRefreshDistance">scene draw distance (+ some tolerance)</param>
        /// <param name="applesCountInSqueareUnit">apples in one square unit</param>
        public WorldScene(double sceneCX, double sceneCY, double sceneRefreshDistance, double applesCountInSqueareUnit)
        {
            this.sceneCX = sceneCX;
            this.sceneCY = sceneCY;
            this.sceneRefreshDistance = sceneRefreshDistance;

            BoundRect sceneRect = new BoundRect(
                        -sceneCX / 2.0, -sceneCY / 2.0,
                        sceneCX / 2.0, sceneCY / 2.0);

            snakesHashTable = new HashTable<Snake>(128);
            snakesTree = new SceneTree<Snake>(
                sceneRect,
                new Position( 50.0, 50.0 ) );

            // apples
            UInt32 applesCount = (UInt32)(sceneCX * sceneCY * applesCountInSqueareUnit);

            applesList = new ApplesList(applesCount, sceneRect);
        }

        /// <summary>
        /// Add snake into the scene
        /// </summary>
        /// <param name="snake">Snake to be added</param>
        public void AddSnake( Snake snake )
        {
            snakesHashTable.Add(snake);
            snakesTree.Add(snake);
        }

        /// <summary>
        /// Remove snake from the scene
        /// </summary>
        /// <param name="snake">Snake to be removed</param>
        public void RemoveSnake( Snake snake )
        {
            snakesHashTable.Remove( snake );
            snakesTree.Remove(snake);
        }

        /// <summary>
        /// Selects all snakes in the given box
        /// Calls the callback function with all selected items
        /// </summary>
        /// <param name="rectIn">Given box</param>
        /// <param name="callback">The callback function</param>
        /// <param name="param">Parameter for the callback function</param>
        public void EnumerateSnakes(BoundRect rectIn, SceneTree<Snake>.EnumeratorCallback callback, Object param)
        {
            snakesTree.EnumerateItems(rectIn, callback, param);
        }

        /// <summary>
        /// Find snake by ID
        /// </summary>
        /// <param name="id">Requested Snake ID</param>
        /// <returns>Snake with the given ID or null if that snake does not exist</returns>
        public Snake FindSnakeById( UInt32 id )
        {
            return snakesHashTable.Get(id);
        }

        /// <summary>
        /// Move snake in the scene tree
        /// </summary>
        /// <param name="snake"></param>
        /// <param name="newBound"></param>
        public void MoveSnakeBounds( Snake snake, BoundRect newBound )
        {
            snakesTree.Move(snake, newBound);
        }

        /// <summary>
        /// Find the best position for new snake.
        /// </summary>
        /// <returns>Free position in the scene.</returns>
        public Position GetFreePositionInScene()
        {
            SceneTree<Snake>.SceneTreeNodeBase emptiestNode = snakesTree.FindEmptiestNode();
            Position freePos = emptiestNode.BoundBox.Center;

            freePos.x += (emptiestNode.BoundBox.Width * 0.3) * (rnd.NextDouble() * 2.0 - 1.0);
            freePos.y += (emptiestNode.BoundBox.Height * 0.3) * (rnd.NextDouble() * 2.0 - 1.0);

            return freePos;
        }

        /// <summary>
        /// apple eaten event
        /// </summary>
        /// <param name="iApple">eaten apple id</param>
        public void OnAppleEaten(UInt32 iApple)
        {
            if (iApple >= applesList.ApplesCount)
                return;

            // recycle apple
            applesList.ResetApple(iApple);

            // send new apple state to the clients
            List<Snake> snakes = snakesHashTable.GetItems();
            foreach (Snake snake in snakes)
            {
                applesList.SendAppleState(snake.bw, iApple);
            }
        }

        /// <summary>
        /// send all apples state to the specified snake
        /// </summary>
        /// <param name="snake">target snake client</param>
        public void SendApplesState(Snake snake)
        {
            applesList.SendApplesState(snake.bw);
        }

        /// <summary>
        /// sending message
        /// container for three parameters
        /// </summary>
        private class SendMessageData
        {
            public Snake snakeFrom;
            public string strUserTo;
            public string strMessage;
        }

        /// <summary>
        /// Send message
        /// </summary>
        /// <param name="snakeFrom">snake which sent the message</param>
        /// <param name="strUserTo">target snakes</param>
        /// <param name="strMessage">message to be sent</param>
        public void ResendMessage(Snake snakeFrom, string strUserTo, string strMessage)
        {
            SendMessageData d = new SendMessageData();
            d.snakeFrom = snakeFrom;
            d.strMessage = strMessage;
            d.strUserTo = strUserTo;

            // resend message to the clients (or only clients specified in d.strUserTo)
            List<Snake> snakes = snakesHashTable.GetItems();
            foreach (Snake snake in snakes)
            {
                if (d.strUserTo.Length == 0 || d.strUserTo == snake.Name)
                {
                    // do the sending on the separate thread
                    ThreadPool.QueueUserWorkItem(ResendMessage, new ResendMessageParams(d.snakeFrom, snake, d.strMessage));
                }
            }
        }

        /// <summary>
        /// Send a message to the client parameters.
        /// </summary>
        private class ResendMessageParams
        {
            public readonly Snake snakeFrom;
            public readonly Snake snakeTo;
            public readonly string strMessage;

            public ResendMessageParams(Snake snakeFrom, Snake snakeTo, string strMessage)
            {
                this.snakeFrom = snakeFrom;
                this.snakeTo = snakeTo;
                this.strMessage = strMessage;
            }
        }

        /// <summary>
        /// Send a message to the client
        /// </summary>
        /// <param name="snakeFrom">message from</param>
        /// <param name="snakeTo">message to</param>
        /// <param name="strMessage">message text</param>
        private void ResendMessage(object obj)
        {
            var p = (ResendMessageParams)obj;

            lock (p.snakeTo.bw)
            {
                try
                {
                    ConnectionData.Send_CmdId(p.snakeTo.bw, ECmdIds.CMD_CHATMESSAGE);
                    ConnectionData.Send_String(p.snakeTo.bw, p.snakeFrom.Name);
                    ConnectionData.Send_String(p.snakeTo.bw, p.strMessage);
                }
                catch (Exception)
                {
                    // BinaryWriter should be closed
                    try { p.snakeTo.bw.Close(); }
                    catch (Exception) { }
                }
            }
        }
    }
}
