using System;
using System.Collections.Generic;
using System.Text;

namespace SnakeWorld_Server
{
    /// <summary>
    /// Generic 2 dimensional position with double precision.
    /// </summary>
    public class Position
    {
        /// <summary>
        /// [x,y] coordinates
        /// </summary>
        public double x, y;

        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="x">new x value</param>
        /// <param name="y">new y value</param>
        public Position(double x, double y)
        {
            this.x = x;
            this.y = y;
        }

        /// <summary>
        /// Copy constructor.
        /// </summary>
        /// <param name="position">Copy coordinates from this initialized object.</param>
        public Position(Position position)
        {
            x = position.x;
            y = position.y;
        }

        /// <summary>
        /// Constructor, define the values from the strings.
        /// </summary>
        /// <param name="strX">X-value</param>
        /// <param name="strY">Y-value</param>
        public Position(string strX, string strY)
        {
            x = double.Parse(strX);
            y = double.Parse(strY);
        }

        /// <summary>
        /// Compute the distance between two points.
        /// </summary>
        /// <param name="pos">The second point.</param>
        /// <returns>Returns the distance between this and the second point.</returns>
        public double Distance(Position pos)
        {
            return Math.Sqrt((pos.x - x) * (pos.x - x) + (pos.y - y) * (pos.y - y));
        }
        
    }
}
