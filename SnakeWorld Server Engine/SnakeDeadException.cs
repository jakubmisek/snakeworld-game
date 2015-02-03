using System;
using System.Collections.Generic;
using System.Text;

namespace SnakeWorld_Server
{
    class SnakeDeadException: Exception
    {
        /// <summary>
        /// id of the snake that killed me
        /// </summary>
        public readonly UInt32 KilledBySnakeId;

        /// <summary>
        /// Constructor.
        /// </summary>
        /// <param name="KilledBySnakeId">Id of the snake that killed me.</param>
        public SnakeDeadException(UInt32 KilledBySnakeId)
        {
            this.KilledBySnakeId = KilledBySnakeId;
        }
    }
}
