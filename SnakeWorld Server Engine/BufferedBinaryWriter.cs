using System;
using System.Collections.Generic;
using System.Text;
using System.Diagnostics;
using System.Threading;

using System.IO;
using System.Net;
using System.Net.Sockets;

namespace SnakeWorld_Server
{
    public class BufferedBinaryWriter:BinaryWriter
    {
        /// <summary>
        /// Init.
        /// </summary>
        /// <param name="ostream">The buffered output stream.</param>
        /// <param name="memorystream">The buffer.</param>
        public BufferedBinaryWriter( Stream ostream, MemoryStream memorystream )
            : base(memorystream)
        {
            ms = memorystream;
            ms.SetLength(0);
            ms.Position = 0;

            OutputStream = new BinaryWriter(ostream);

            ThreadPool.QueueUserWorkItem(DelayFlush, null);
        }

        /// <summary>
        /// the output stream.
        /// </summary>
        protected readonly BinaryWriter OutputStream;

        /// <summary>
        /// the buffer of bytes to be sent.
        /// </summary>
        protected MemoryStream ms;

        /// <summary>
        /// The lock object, locks the operations on the ms.
        /// </summary>
        private object mslock = new object();

        /// <summary>
        /// Is the stream explicitly closed?
        /// </summary>
        private bool isClosed = false;

        /// <summary>
        /// Delayed flush.
        /// </summary>
        public override void Flush()
        {
            // do nothing, data will be sent on the separate "send thread" (DelayFlush)
        }

        public void StopSending()
        {
            lock(mslock)
            {
                isClosed = true;
            }
        }

        /// <summary>
        /// Wait some time and then call FLUSH.
        /// </summary>
        /// <param name="obj"></param>
        private void DelayFlush(object obj)
        {
            // the buffer of the buffer
            MemoryStream toSend = new MemoryStream();

            while (true)
            {
                // wait some time
                Thread.Sleep(250);

                // write ms to the second buffer, so other threads waiting for the ms should continue
                lock(mslock)
                {
                    if (isClosed)
                        return;

                    toSend.SetLength(0);
                    toSend.Position = 0;

                    ms.WriteTo(toSend);
                    ms.SetLength(0);
                    ms.Position = 0;
                }

                // sends the second buffer to the output stream (should take some time)
                if (toSend.Length > 0)
                {
                    try
                    {
                        toSend.WriteTo(OutputStream.BaseStream);
                        OutputStream.Flush();
                    }
                    catch (Exception)
                    {
                        return; // stop sending the data
                    }
                }
            }
        }

        #region Overrided writes with locks
        public override void Write(bool value)
        {
            lock(mslock){base.Write(value);}
        }
        public override void Write(byte value)
        {
            lock(mslock){base.Write(value);}
        }
        public override void Write(byte[] buffer)
        {
            lock(mslock){base.Write(buffer);}
        }
        public override void Write(byte[] buffer, int index, int count)
        {
            lock(mslock){base.Write(buffer, index, count);}
        }
        public override void Write(char ch)
        {
            lock(mslock){base.Write(ch);}
        }
        public override void Write(char[] chars)
        {
            lock(mslock){base.Write(chars);}
        }
        public override void Write(char[] chars, int index, int count)
        {
            lock(mslock){base.Write(chars, index, count);}
        }
        public override void Write(decimal value)
        {
            lock(mslock){base.Write(value);}
        }
        public override void Write(double value)
        {
            lock(mslock){base.Write(value);}
        }
        public override void Write(float value)
        {
            lock(mslock){base.Write(value);}
        }
        public override void Write(int value)
        {
            lock(mslock){base.Write(value);}
        }
        public override void Write(long value)
        {
            lock(mslock){base.Write(value);}
        }
        public override void Write(sbyte value)
        {
            lock(mslock){base.Write(value);}
        }
        public override void Write(short value)
        {
            lock(mslock){base.Write(value);}
        }
        public override void Write(string value)
        {
            lock(mslock){base.Write(value);}
        }
        public override void Write(uint value)
        {
            lock(mslock){base.Write(value);}
        }
        public override void Write(ulong value)
        {
            lock(mslock){base.Write(value);}
        }
        public override void Write(ushort value)
        {
            lock(mslock){base.Write(value);}
        }
        #endregion Overrided writes with locks

    }
}
