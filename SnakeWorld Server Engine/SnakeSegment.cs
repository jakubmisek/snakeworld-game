using System;
using System.Collections.Generic;
using System.Text;

namespace SnakeWorld_Server
{
    class SnakeSegment
    {
        public readonly UInt32 id;
        public readonly Position position;

        /// <summary>
        /// Time the segment was created.
        /// </summary>
        public readonly DateTime timeCreated = DateTime.Now;

        /// <summary>
        /// The next segment of the snake body.
        /// Next segment is more closely to the head.
        /// </summary>
        public SnakeSegment nextSegment = null;

        /// <summary>
        /// Constructor.
        /// </summary>
        /// <param name="position">Position of this segment. Not null.</param>
        /// <param name="id">Id of this segment.</param>
        public SnakeSegment(Position position, UInt32 id)
        {
            // position
            this.position = position;
            this.id = id;            
        }
    }
}
