using System;
using System.Collections.Generic;
using System.Text;
using System.IO;

namespace SnakeWorld_Server
{
    public class HighScore
    {
        /// <summary>
        /// Score associated with the port number.
        /// </summary>
        private int _PortNumber = 0;

        public int PortNumber
        {
            get
            {
                return _PortNumber;
            }
            set
            {
                _PortNumber = value;

                // init high score
                ReadScore();
            }
        }

        /// <summary>
        /// Current best snake name
        /// </summary>
        private string BestSnakeLengthName = null;

        /// <summary>
        /// Current best snake length
        /// </summary>
        private double BestSnakeLength = 0.0;

        /// <summary>
        /// Compare the current best score with the given snake
        /// </summary>
        /// <param name="snake">snake to compare with</param>
        public void CheckBest(Snake snake)
        {
            lock (this)
            {
                if (snake.Length > BestLength)
                {
                    BestSnakeLength = snake.Length;
                    BestSnakeLengthName = snake.Name;

                    // save
                    WriteScore();
                }
            }
        }

        private void ReadScore()
        {
            if (PortNumber != 0)
            {
                try
                {
                    FileStream fs = new FileStream("record." + PortNumber + ".txt", FileMode.Open);
                    TextReader tr = new StreamReader(fs);

                    BestSnakeLengthName = tr.ReadLine();
                    BestSnakeLength = int.Parse(tr.ReadLine());

                    tr.Close();
                    fs.Close();
                }
                catch (Exception)
                {
                }
            }
        }

        private void WriteScore()
        {
            if (PortNumber != 0)
            {
                try
                {
                    FileStream fs = new FileStream("record." + PortNumber + ".txt", FileMode.Create);
                    TextWriter tw = new StreamWriter(fs);

                    tw.WriteLine(BestName);
                    tw.WriteLine(BestLength);
                    tw.Flush();

                    tw.Close();
                    fs.Close();
                }
                catch (Exception)
                {
                    
                }
            }
        }

        /// <summary>
        /// Send the current best snake name and length to the specified snake
        /// </summary>
        /// <param name="snakeTo">Target snake</param>
        public void SendBestScore(Snake snakeTo)
        {
            if (BestName != null && BestLength > 0.0)
            {
                lock (snakeTo.bw)
                {
                    ConnectionData.Send_CmdId(snakeTo.bw, ECmdIds.CMD_BESTSNAKE);
                    ConnectionData.Send_String(snakeTo.bw, BestName);
                    snakeTo.bw.Write(BestLength);
                }
            }
        }

        /// <summary>
        /// Current best snakes name
        /// </summary>
        public string BestName
        {
            get
            {
                return BestSnakeLengthName;
            }
        }

        /// <summary>
        /// Current best snake length
        /// </summary>
        public double BestLength
        {
            get
            {
                return BestSnakeLength;
            }
        }
    }
}
