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
        /// <param name="ns"></param>
        public BufferedBinaryWriter( Stream ostream, MemoryStream memorystream )
            : base(memorystream)
        {
            ms = memorystream;

            ms.SetLength(0);
            ms.Position = 0;

            OutputStream = new BinaryWriter(ostream);
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
        /// amount of bytes to be waiting in the buffer.
        /// </summary>
        protected const int maxbuffersize = 1024;

        /// <summary>
        /// The waiting thread is running. (flush will be called)
        /// </summary>
        protected bool IsDelayingFlush = false;

        /// <summary>
        /// Delayed flush.
        /// </summary>
        public override void Flush()
        {
            lock(this)
            {
                if (ms.Length >= maxbuffersize)
                {
                    DoFlushL();
                }
                else
                {
                    // flush after some time
                    DelayFlush();
                }
            }
            
        }

        /// <summary>
        /// Flush the data to the output stream.
        /// </summary>
        private void DoFlushL()
        {
            if (IsDelayingFlush)
                return;

            try
            {
                ms.WriteTo(OutputStream.BaseStream);
                OutputStream.Flush();

                ms.SetLength(0);
                ms.Position = 0;
            }
            catch(Exception)
            {
            }            
        }


        /// <summary>
        /// Wait some time on another thread and then call flush.
        /// </summary>
        private void DelayFlush()
        {
            lock(this)
            {
                if (IsDelayingFlush)
                    return;

                IsDelayingFlush = true;

                ThreadPool.QueueUserWorkItem(DelayFlush, null);
            }
        }


        /// <summary>
        /// Wait some time and then call FLUSH.
        /// </summary>
        /// <param name="obj"></param>
        private void DelayFlush(object obj)
        {
            Thread.Sleep(250);

            lock(this)
            {
                IsDelayingFlush = false;

                DoFlushL();
            }
        }

        public override void Write(bool value)
        {
            lock(this){base.Write(value);}
        }
        public override void Write(byte value)
        {
            lock(this){base.Write(value);}
        }
        public override void Write(byte[] buffer)
        {
            lock(this){base.Write(buffer);}
        }
        public override void Write(byte[] buffer, int index, int count)
        {
            lock(this){base.Write(buffer, index, count);}
        }
        public override void Write(char ch)
        {
            lock(this){base.Write(ch);}
        }
        public override void Write(char[] chars)
        {
            lock(this){base.Write(chars);}
        }
        public override void Write(char[] chars, int index, int count)
        {
            lock(this){base.Write(chars, index, count);}
        }
        public override void Write(decimal value)
        {
            lock(this){base.Write(value);}
        }
        public override void Write(double value)
        {
            lock(this){base.Write(value);}
        }
        public override void Write(float value)
        {
            lock(this){base.Write(value);}
        }
        public override void Write(int value)
        {
            lock(this){base.Write(value);}
        }
        public override void Write(long value)
        {
            lock(this){base.Write(value);}
        }
        public override void Write(sbyte value)
        {
            lock(this){base.Write(value);}
        }
        public override void Write(short value)
        {
            lock(this){base.Write(value);}
        }
        public override void Write(string value)
        {
            lock(this){base.Write(value);}
        }
        public override void Write(uint value)
        {
            lock(this){base.Write(value);}
        }
        public override void Write(ulong value)
        {
            lock(this){base.Write(value);}
        }
        public override void Write(ushort value)
        {
            lock(this){base.Write(value);}
        }
        

    }
}
