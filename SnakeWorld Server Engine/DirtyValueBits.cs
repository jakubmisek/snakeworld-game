using System;
using System.Collections.Generic;
using System.Text;

namespace SnakeWorld_Server
{
    class DirtyValueBits
    {
        /// <summary>
        /// Bits of dirty values bitmap.
        /// </summary>
        public const UInt32 Nothing = 0;

        public const UInt32 Head = 1;
        public const UInt32 Length = 2;
        public const UInt32 Speed = 4;
        public const UInt32 Bounds = 8;
        public const UInt32 Segment = 16;
        public const UInt32 HeadDirection = 32;

        public const UInt32 Everything = 0xffffffff;
    }
}
