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
        private Listener.WorldParams world;

        /// <summary>
        /// App ctor
        /// </summary>
        public ConsoleMain(int port, double sceneCX, double sceneCY, double refreshDistace, double applesCountInSqueareUnit, uint maxSnakes)
        {
            // create the world
            InitWorld(sceneCX, sceneCY, refreshDistace, applesCountInSqueareUnit, maxSnakes);

            // start listening on specified port
            StartListening(port);
        }


        /// <summary>
        /// Init the world
        /// </summary>
        private void InitWorld(double sceneCX, double sceneCY, double refreshDistace, double applesCountInSqueareUnit, uint maxSnakes)
        {
            Console.Write("Creating world object ... ");

            world = new Listener.WorldParams(sceneCX, sceneCY, refreshDistace, applesCountInSqueareUnit, maxSnakes);

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

    }
}
