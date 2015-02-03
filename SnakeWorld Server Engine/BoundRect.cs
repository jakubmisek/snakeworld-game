using System;
using System.Collections.Generic;
using System.Text;

namespace SnakeWorld_Server
{
    public class BoundRect
    {
        /// <summary>
        /// topleft and bottomright corners coordinates
        /// </summary>
        public Position topleft, bottomright;

        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="topleft">new top-left corner coordinates</param>
        /// <param name="bottomright">new bottom-right corner ccoordinates</param>
        public BoundRect(Position topleft, Position bottomright)
        {
            this.topleft = new Position(topleft);
            this.bottomright = new Position(bottomright);
        }

        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="x1">left side</param>
        /// <param name="y1">top side</param>
        /// <param name="x2">right side</param>
        /// <param name="y2">bottom side</param>
        public BoundRect(double x1, double y1, double x2, double y2)
        {
            topleft = new Position(x1, y1);
            bottomright = new Position(x2, y2);
        }

        public BoundRect( BoundRect rect )
        {
            this.topleft = new Position(rect.topleft);
            this.bottomright = new Position(rect.bottomright);
        }

        /// <summary>
        /// Constructor
        /// New bounding box will include given rectangle and given point
        /// </summary>
        /// <param name="rect">rectangle</param>
        /// <param name="pos">point</param>
        public BoundRect( BoundRect rect, Position pos )
            :this(rect)
        {
            Add(pos);
        }

        /// <summary>
        /// Constructor
        /// Create new rectangle from the center vector and half of side length.
        /// </summary>
        /// <param name="center">Rectangle center</param>
        /// <param name="halfWidth">Side length / 2, have to be greater or equal to zero</param>
        public BoundRect( Position center, double halfSide )
        {
            topleft = new Position(center.x - halfSide, center.y - halfSide);
            bottomright = new Position(center.x + halfSide, center.y + halfSide);
        }

        /// <summary>
        /// Expand the rectangle to cover given point
        /// </summary>
        /// <param name="pos">Point to be included in the bounding box.</param>
        public void Add( Position pos )
        {
            if (pos.x < topleft.x) topleft.x = pos.x;
            else if (pos.x > bottomright.x) bottomright.x = pos.x;

            if (pos.y < topleft.y) topleft.y = pos.y;
            else if (pos.y > bottomright.y) bottomright.y = pos.y;
        }

        /// <summary>
        /// Test for point intersection.
        /// </summary>
        /// <param name="pt">Testes point</param>
        /// <returns>Resturns true if the point is in the bounds, otherwise false.</returns>
        public bool Intersects(Position pt)
        {
            return
                pt.x >= topleft.x &&
                pt.x <= bottomright.x &&
                pt.y >= topleft.y &&
                pt.y <= bottomright.y;
        }

        /// <summary>
        /// Test for intersection of two rectangles.
        /// </summary>
        /// <param name="rect">Second tested rectangle.</param>
        /// <returns>Returns true if this and the second rectangles are in intersection, otherwise false.</returns>
        public bool Intersects(BoundRect rect)
        {
            return !(
                rect.topleft.x > bottomright.x ||
                rect.topleft.y > bottomright.y ||
                rect.bottomright.x < topleft.x ||
                rect.bottomright.y < topleft.y
                );
        }

        /// <summary>
        /// Determine if this rectangle is in the given rectangle
        /// </summary>
        /// <param name="rect">given rectangle</param>
        /// <returns>true if this rectangle is in the given rectangle.</returns>
        public bool IsIn( BoundRect rect )
        {
            return
                topleft.x >= rect.topleft.x && topleft.y >= rect.topleft.y &&
                bottomright.x <= rect.bottomright.x && bottomright.y <= rect.bottomright.y;
        }

        /// <summary>
        /// Width of the rectangle
        /// </summary>
        public double Width
        {
            get
            {
                return bottomright.x - topleft.x;
            }
        }

        /// <summary>
        /// Height of the rectangle
        /// </summary>
        public double Height
        {
            get
            {
                return bottomright.y - topleft.y;
            }
        }

        /// <summary>
        /// Rectangle center.
        /// </summary>
        public Position Center
        {
            get
            {
                return new Position(CenterX, CenterY);
            }
        }

        /// <summary>
        /// x-center
        /// </summary>
        public double CenterX
        {
            get
            {
                return (topleft.x + bottomright.x) * 0.5;
            }
        }

        /// <summary>
        /// y-center
        /// </summary>
        public double CenterY
        {
            get
            {
                return (topleft.y + bottomright.y) * 0.5;
            }
        }

        /// <summary>
        /// top
        /// </summary>
        public double Top
        {
            get
            {
                return topleft.y;
            }
        }

        /// <summary>
        /// left
        /// </summary>
        public double Left
        {
            get
            {
                return topleft.x;
            }
        }

        /// <summary>
        /// bottom
        /// </summary>
        public double Bottom
        {
            get
            {
                return bottomright.y;
            }
        }

        /// <summary>
        /// right
        /// </summary>
        public double Right
        {
            get
            {
                return bottomright.x;
            }
        }
        
    }
}
