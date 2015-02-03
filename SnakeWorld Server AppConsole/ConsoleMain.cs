using System;
using System.Collections.Generic;
using System.Text;

using SnakeWorld_Server;

namespace SnakeWorld_Server_ConsoleApp
{
    class ConsoleMain
    {
        /// <summary>
        /// server objects
        /// </summary>
        private Listener listener;
        private World world;

        /// <summary>
        /// App ctor
        /// </summary>
        public ConsoleMain(int port, double sceneCX, double sceneCY, double refreshDistace, double applesCountInSqueareUnit)
        {
            // create the world
            InitWorld(sceneCX,sceneCY,refreshDistace,applesCountInSqueareUnit);

            // start listening on specified port
            StartListening(port);
        }


        /// <summary>
        /// Init the world
        /// </summary>
        private void InitWorld(double sceneCX, double sceneCY, double refreshDistace, double applesCountInSqueareUnit)
        {
            Console.Write("Creating world object ... ");

            world = new World(OnNewSnake, null, sceneCX, sceneCY, refreshDistace, applesCountInSqueareUnit);

            Console.WriteLine("ok");
        }

        /// <summary>
        /// Start listener object
        /// </summary>
        /// <param name="port">port used for listening</param>
        private void StartListening( int port )
        {
            Console.Write("Starting listening on port {0} ... ", port);

            try
            {
                listener = new Listener(port, world);

                Console.WriteLine("ok");
            }
            catch (Exception)
            {
                // listening error
                Console.WriteLine("failed");
            }
        }

        /// <summary>
        /// maximum snakes count at same time
        /// </summary>
        private UInt32 maxSnakesAtTime = 0;

        /// <summary>
        /// maximum snakes count at same time
        /// </summary>
        /// <returns>maximum snakes count at same time</returns>
        public UInt32 MaxSnakesAtSameTime
        {
            get
            {
                return maxSnakesAtTime;
            }
        }

        /// <summary>
        /// callback from the world when new snake is created
        /// </summary>
        /// <param name="id">id of the new snake</param>
        /// <param name="strName">name of the new snake</param>
        private void OnNewSnake(Snake newSnake)
        {
            UInt32 cnt = world.SnakesCount;

            // check max snakes count
            if (maxSnakesAtTime < cnt)
            {
                maxSnakesAtTime = cnt;
            }
        }

        /// <summary>
        /// Snake world object
        /// </summary>
        public World SnakeWorld
        {
            get
            {
                return world;
            }
        }

        /// <summary>
        /// Snake world TCP listener
        /// </summary>
        public Listener SnakeListener
        {
            get
            {
                return listener;
            }
        }
    }
}
