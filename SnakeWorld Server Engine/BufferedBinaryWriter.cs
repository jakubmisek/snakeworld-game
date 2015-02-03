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
        /// Delayed flush.
        /// </summary>
        public override void Flush()
        {
            // do nothing, data will be sent on the separate "send thread" (DelayFlush)
        }

        /// <summary>
        /// Wait some time and then call FLUSH.
        /// </summary>
        /// <param name="obj"></param>
        private void DelayFlush(object obj)
        {
            while (true)
            {
                // wait some time
                Thread.Sleep(330);

                lock(ms)
                {
                    try
                    {
                        ms.WriteTo(OutputStream.BaseStream);
                        OutputStream.Flush();

                        ms.SetLength(0);
                        ms.Position = 0;
                    }
                    catch (Exception)
                    {
                        return; // stop sending the data
                    }
                }
            }
        }

        public override void Write(bool value)
        {
            lock(ms){base.Write(value);}
        }
        public override void Write(byte value)
        {
            lock(ms){base.Write(value);}
        }
        public override void Write(byte[] buffer)
        {
            lock(ms){base.Write(buffer);}
        }
        public override void Write(byte[] buffer, int index, int count)
        {
            lock(ms){base.Write(buffer, index, count);}
        }
        public override void Write(char ch)
        {
            lock(ms){base.Write(ch);}
        }
        public override void Write(char[] chars)
        {
            lock(ms){base.Write(chars);}
        }
        public override void Write(char[] chars, int index, int count)
        {
            lock(ms){base.Write(chars, index, count);}
        }
        public override void Write(decimal value)
        {
            lock(ms){base.Write(value);}
        }
        public override void Write(double value)
        {
            lock(ms){base.Write(value);}
        }
        public override void Write(float value)
        {
            lock(ms){base.Write(value);}
        }
        public override void Write(int value)
        {
            lock(ms){base.Write(value);}
        }
        public override void Write(long value)
        {
            lock(ms){base.Write(value);}
        }
        public override void Write(sbyte value)
        {
            lock(ms){base.Write(value);}
        }
        public override void Write(short value)
        {
            lock(ms){base.Write(value);}
        }
        public override void Write(string value)
        {
            lock(ms){base.Write(value);}
        }
        public override void Write(uint value)
        {
            lock(ms){base.Write(value);}
        }
        public override void Write(ulong value)
        {
            lock(ms){base.Write(value);}
        }
        public override void Write(ushort value)
        {
            lock(ms){base.Write(value);}
        }
        

    }
}
