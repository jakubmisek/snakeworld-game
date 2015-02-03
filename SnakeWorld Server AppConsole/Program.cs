using System;
using System.Collections.Generic;
using System.Text;

using CommandLineParser;

namespace SnakeWorld_Server_ConsoleApp
{
    class Program
    {
        /// <summary>
        /// Command function delegate
        /// </summary>
        /// <param name="main">server object</param>
        /// <param name="prms">command parameters</param>
        private delegate void CommandDelegate( ConsoleMain main, string[] prms );

        /// <summary>
        /// Command description
        /// </summary>
        struct CommandDesc
        {
            /// <summary>
            /// Command
            /// </summary>
            public readonly string[] names;

            /// <summary>
            /// Description
            /// </summary>
            public readonly string desc;

            /// <summary>
            /// Callback method
            /// </summary>
            public CommandDelegate function;

            /// <summary>
            /// Command desc init
            /// </summary>
            /// <param name="name">command name</param>
            /// <param name="desc">description</param>
            /// <param name="function">command method</param>
            public CommandDesc( string name, string desc, CommandDelegate function )
            {
                this.names = new string[]{name};
                this.desc = desc;
                this.function = function;
            }

            /// <summary>
            /// Command desc init
            /// </summary>
            /// <param name="names">command names</param>
            /// <param name="desc">description</param>
            /// <param name="function">command method</param>
            public CommandDesc(string[] names, string desc, CommandDelegate function)
            {
                this.names = names;
                this.desc = desc;
                this.function = function;
            }

            /// <summary>
            /// check name with this command names
            /// </summary>
            /// <param name="name">name to check</param>
            /// <returns>true if given name matches</returns>
            public bool Match(string name)
            {
                foreach (string myname in names)
                {
                    if (myname.ToLower() == name.ToLower())
                        return true;
                }

                return false;
            }

            /// <summary>
            /// All command names separated by comma
            /// </summary>
            /// <returns>command names in one string separated by comma</returns>
            public string GetNames()
            {
                string str = "";
                
                foreach (string myname in names)
                {
                    if (str.Length > 0)
                        str += ",";

                    str += myname;
                }

                return str;
            }
        }

        #region commands

        /// <summary>
        /// Avaailable commands
        /// </summary>
        static CommandDesc[] commands = {
                                         new CommandDesc( new string[]{"help","?"}, "display available commands", cmdHelp ),
                                         new CommandDesc( new string[]{"stats","statistics","state"}, "display game info", cmdStats ),
                                         new CommandDesc( new string[]{"maxsnakes"}, "set maximum snakes count", cmdMaxSnakes ),
                                         new CommandDesc( new string[]{"closelistener", "stop"}, "closes the listener", cmdCloseListener ),
                                         };

        static bool CallCommand(ConsoleMain main, string[] prms)
        {
            if ( prms.Length > 0 )
            foreach (CommandDesc cmd in commands)
            {
                // find command
                if (cmd.Match( prms[0] ))
                {
                    try
                    {
                        // call command
                        cmd.function(main, prms);
                    }
                    catch (Exception ex)
                    {
                        Console.WriteLine("Exception: {0}", ex.ToString());
                    }

                    // command found
                    return true;
                }
            }

            // not found
            return false;
        }

        static void cmdHelp(ConsoleMain main, string[] prms)
        {
            foreach (CommandDesc cmd in commands)
            {
                Console.WriteLine("{0} ... {1}", cmd.GetNames(), cmd.desc);
            }
        }

        static void cmdStats(ConsoleMain main, string[] prms)
        {
            Console.WriteLine("Snakes count: {0}", main.SnakeWorld.SnakesCount.ToString());
            Console.WriteLine("Maximum snakes: {0}", main.SnakeWorld.MaxSnakesCount.ToString());
            Console.WriteLine("Maximum snakes at one time: {0}", main.MaxSnakesAtSameTime.ToString());

            if (main.SnakeWorld.BestScore.BestName != null)
                Console.WriteLine("Best player: {0}, {1} points", main.SnakeWorld.BestScore.BestName, main.SnakeWorld.BestScore.BestLength.ToString());
        }

        static void cmdMaxSnakes(ConsoleMain main, string[] prms)
        {
            UInt32 n = UInt32.Parse(prms[1]);

            main.SnakeWorld.MaxSnakesCount = n;

            Console.WriteLine("Maximum snakes count set to {0}", n.ToString());
        }

        static void cmdCloseListener(ConsoleMain main, string[] prms)
        {
            try
            {
                main.SnakeListener.CloseListener();
            }
            catch (Exception)
            {
                
            }

            Console.WriteLine("Listener closed, new connection won't be accepted.");
        }

        #endregion

        /// <summary>
        /// Create ConsoleMain object
        /// parse parametrs from the given command line
        /// </summary>
        /// <param name="args">command line arguments</param>
        /// <returns>new ConsoleMain object</returns>
        static ConsoleMain CreateConsoleMain(string[] args)
        {
            int port = 1234;
            double sceneCX = 200.0, sceneCY = 200.0, refreshDistance = 300.0, applesCountInSqueareUnit = 0.00004;

            // parse command line
            Parser parser = new Parser();

            parser.Options["help"] = parser.Options['?'] = new Option(false, Option.ParameterType.NoArgument, "Program info and arguments");
            parser.Options["port"] = parser.Options['p'] = new Option(false, Option.ParameterType.UnsignedInteger, "Listening port number");
            parser.Options["scenecx"] = parser.Options['x'] = new Option(false, Option.ParameterType.UnsignedInteger, "Scene x-size");
            parser.Options["scenecy"] = parser.Options['y'] = new Option(false, Option.ParameterType.UnsignedInteger, "Scene y-size");
            parser.Options["refresh"] = parser.Options['r'] = new Option(false, Option.ParameterType.UnsignedInteger, "Scene refresh distance");
            parser.Options["apples"] = parser.Options['a'] = new Option(false, Option.ParameterType.UnsignedInteger, "Apples count");
            parser.Options["maxsnakes"] = parser.Options['m'] = new Option(false, Option.ParameterType.UnsignedInteger, "Max snakes count");

            parser.Parse(args);

            if (parser.Options['?'].Exists)
            {
                Console.WriteLine("SnakeWorld command line server");
                Console.WriteLine();

                foreach (OptionPair o in parser.Options)
                {
                    Console.WriteLine("{0} - {1}", o.ShortOption, o.Option.Description);
                }

                return null;
            }

            if (parser.Options['p'].Exists) port = (int)parser.Options['p'].UIntParameter;
            if (parser.Options['x'].Exists) sceneCX = (double)parser.Options['x'].UIntParameter;
            if (parser.Options['y'].Exists) sceneCY = (double)parser.Options['y'].UIntParameter;
            if (parser.Options['r'].Exists) refreshDistance = (double)parser.Options['r'].UIntParameter;

            if (parser.Options['a'].Exists) applesCountInSqueareUnit = ((double)parser.Options['a'].UIntParameter) / (sceneCX * sceneCY);

            // create server object
            ConsoleMain newMain = new ConsoleMain(port, sceneCX, sceneCY, refreshDistance, applesCountInSqueareUnit);

            // check some params
            if (parser.Options['m'].Exists) newMain.SnakeWorld.MaxSnakesCount = parser.Options['m'].UIntParameter;

            // done
            return newMain;
        }

        /// <summary>
        /// Program main
        /// </summary>
        /// <param name="args">command line arguments</param>
        static void Main(string[] args)
        {
            // create server
            ConsoleMain main = CreateConsoleMain(args);

            if (main == null)
                return;

            Console.WriteLine("\nServer started!\n");

            // read commands
            bool bDone = false;

            while (!bDone)
            {
                Console.Write(" > ");
                // read command
                string command = Console.ReadLine();

                // parse command
                string[] prms = command.Split(new char[] { ' ' }, 10, StringSplitOptions.RemoveEmptyEntries);

                // execute command
                bool bExecuted = CallCommand(main, prms);

                if (!bExecuted && prms.Length > 0)
                {
                    // try special commands
                    switch (prms[0])
                    {
                        case "quit":
                        case "exit":
                            // exit
                            bDone = true;
                            break;
                        default:
                            // unknown command
                            Console.WriteLine("unknow command '{0}'", prms[0]);
                            break;
                    }                   
                }
            }

            Console.WriteLine("\nBye!");
        }
    }
}
