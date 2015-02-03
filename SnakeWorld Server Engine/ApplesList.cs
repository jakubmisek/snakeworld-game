using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Threading;


namespace SnakeWorld_Server
{
    /// <summary>
    /// List of the apples in the scene
    /// </summary>
    class ApplesList
    {
        /// <summary>
        /// Apple state
        /// </summary>
        public struct Apple
        {
            /// <summary>
            /// apple coordination
            /// </summary>
            public double x, y;

            /// <summary>
            /// inicialization
            /// </summary>
            /// <param name="x">x coordination</param>
            /// <param name="y">y coordination</param>
            public Apple(double x, double y)
            {
                this.x = x;
                this.y = y;
            }
        }

        /// <summary>
        /// List of the apples
        /// </summary>
        private readonly Apple[] Apples;

        /// <summary>
        /// scene bounding rectangle used for applpes position inicialization
        /// </summary>
        private readonly BoundRect sceneRect;

        /// <summary>
        /// random numbers generator used for new apples position
        /// </summary>
        private readonly Random rndNumbers = new Random();

        /// <summary>
        /// List inicialization
        /// </summary>
        /// <param name="ApplesCount">Apples count</param>
        /// <param name="sceneRect">scene bounding rect used for apples position inicialization</param>
        public ApplesList(UInt32 ApplesCount, BoundRect sceneRect)
        {
            this.sceneRect = sceneRect;

            Apples = new Apple[ApplesCount];

            for (UInt32 i = 0; i < ApplesCount; ++i)
            {
                ResetApple(i);
            }
        }

        /// <summary>
        /// Resets apple position
        /// </summary>
        /// <param name="iApple">apple to be reseted</param>
        /// <returns>new apple state</returns>
        public void ResetApple(UInt32 iApple)
        {
            Apple newApple = new Apple(
                            rndNumbers.NextDouble() * sceneRect.Width + sceneRect.Left,
                            rndNumbers.NextDouble() * sceneRect.Height + sceneRect.Top);

            lock (this)
            {
                Apples[iApple] = newApple;
            }
        }

        /// <summary>
        /// Send CMD_APPLE command of specified apple to the given snake.
        /// </summary>
        /// <param name="bw">target writer</param>
        /// <param name="iApple">apple id</param>
        /// <param name="aApple">apple state</param>
        public void SendAppleState(BinaryWriter bw, UInt32 iApple)
        {
            Apple aApple = Apples[iApple];

            lock (bw)
            {
                try
                {
                    ConnectionData.Send_CmdId(bw, ECmdIds.CMD_APPLE);
                    bw.Write((UInt32)iApple);

                    bw.Write((double)aApple.x);
                    bw.Write((double)aApple.y);

                    bw.Flush();
                }
                catch (Exception)
                {
                    // BinaryWriter should be closed
                    try { bw.Close(); }
                    catch (Exception) { }
                }
            }
        }

        #region Sending apple state on the own thread
        /// <summary>
        /// Send apple state parameters.
        /// </summary>
        private class _SendAppleStateParam
        {
            public readonly BinaryWriter bw;
            public readonly UInt32 iApple;

            public _SendAppleStateParam(BinaryWriter/*!*/bw, UInt32 iApple)
            {
                this.bw = bw;
                this.iApple = iApple;
            }
        }

        
        #endregion

        /// <summary>
        /// Send CMD_APPLE command of all apples to the given snake.
        /// </summary>
        /// <param name="bw">target writer</param>
        public void SendApplesState(BinaryWriter bw)
        {
            for (UInt32 i = 0; i < Apples.Length; ++i)
            {
                SendAppleState(bw, i);
            }
        }

        /// <summary>
        /// Apples count
        /// </summary>
        public int ApplesCount
        {
            get
            {
                return Apples.Length;
            }
        }
    }
}
