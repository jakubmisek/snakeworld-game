using System;
using System.Collections.Generic;
using System.Collections;
using System.Text;
using System.Threading;

namespace SnakeWorld_Server
{
    /// <summary>
    /// Quad tree item
    /// Base item that scene tree should contains
    /// </summary>
    public class SceneTreeItem<T> where T:SceneTreeItem<T>
    {
        /// <summary>
        /// object bounding box
        /// </summary>
        public virtual BoundRect Bound
        {
            get
            {
                throw new NotImplementedException();
            }
            set
            {
                throw new NotImplementedException();
            }
        }

        /// <summary>
        /// current tree node
        /// when changing, previous and new treeNode have to be locked for writing
        /// </summary>
        public SceneTree<T>.SceneTreeNodeBase treeNode = null;

        /// <summary>
        /// Remove the item from the tree.
        /// Thread unsafe.
        /// </summary>
        public void RemoveFromTree()
        {
            if (treeNode != null)
                treeNode.Remove((T)this);
        }

        /// <summary>
        /// moves the item into the best node.
        /// </summary>
        /// <param name="newBound">New item bound rect.</param>
        /// <param name="minNodeSize">Minimal node size when creating new nodes.</param>
        public void Move( BoundRect newBound, Position minNodeSize )
        {
            if (treeNode != null)
                treeNode.Move((T)this, newBound, minNodeSize);
        }
    }

    /// <summary>
    /// Quad tree
    /// </summary>
    /// <typeparam name="T">tree item type</typeparam>
    public class SceneTree<T> where T : SceneTreeItem<T>
    {

        /// <summary>
        /// the enumerator callback method called from EnumerateItems
        /// </summary>
        /// <param name="item">given object</param>
        /// <param name="param">parameter specified in EnumerateItems</param>
        public delegate void EnumeratorCallback(T item, Object param);

        /// <summary>
        /// Quad tree root node.
        /// </summary>
        private readonly SceneTreeNodeBase root;

        private readonly Position minNodeSize;

        /// <summary>
        /// Base quad tree node
        /// </summary>
        public class SceneTreeNodeBase
        {
            /// <summary>
            /// items count in whole subtree
            /// </summary>
            public int itemscount = 0;

            /// <summary>
            /// items in this node
            /// might be null
            /// </summary>
            protected List<T> items = null;    // items which do not fit to any child node

            /// <summary>
            /// reader/writer lock
            /// </summary>
            protected readonly ReaderWriterLock rwLock = new ReaderWriterLock();

            /// <summary>
            /// Node bound box.
            /// </summary>
            protected readonly BoundRect bound;

            /// <summary>
            /// Parent node.
            /// </summary>
            private readonly SceneTreeNodeBase parent;

            /// <summary>
            /// Protected constructor
            /// Initialize the bound box
            /// </summary>
            /// <param name="nodeBound"></param>
            protected SceneTreeNodeBase( SceneTreeNodeBase parent, BoundRect nodeBound )
            {
                this.parent = parent;
                this.bound = nodeBound;
            }

            /// <summary>
            /// Tree node bounding box.
            /// </summary>
            public BoundRect BoundBox
            {
                get
                {
                    return bound;
                }
            }

            /// <summary>
            /// Add an item to the tree node or node child recursively.
            /// If the list object is not created yet, it will be created here.
            /// </summary>
            /// <param name="item">An item to be inserted.</param>
            /// <param name="minNodeSize">Minimal size for divided child node.</param>
            public virtual void Add(T item, Position minNodeSize)
            {
                // add the item into the list
                if (item.treeNode == this)
                {
                    return; // already here
                }
                else
                {
                    // increase items count in this node and all parent nodes
                    for (SceneTreeNodeBase node = this; node != null; node = node.parent)
                        ++node.itemscount;

                    // add item into this node
                    rwLock.AcquireWriterLock(-1);   // writer lock

                        if (items == null)
                            items = new List<T>();  // create list object

                        items.Add(item);            // add the item into the list

                    rwLock.ReleaseWriterLock();     // writer unlock

                    // remove from the previous node
                    item.RemoveFromTree();
                }
                
                // finally
                item.treeNode = this;       // connect the item with this node
            }

            /// <summary>
            /// Removes an item from the tree node.
            /// </summary>
            /// <param name="item"></param>
            public void Remove(T item)
            {
                // decrease items count in this node and all parent nodes
                for (SceneTreeNodeBase node = item.treeNode; node != null; node = node.parent)
                    --node.itemscount; 
                
                rwLock.AcquireWriterLock(-1);   // write lock

                // remove from the tree node
                item.treeNode = null;           // to loose the connection with the tree node

                if (items != null)
                    items.Remove(item);         // remove the item from the tree node

                rwLock.ReleaseWriterLock();     // write unlock
            }

            /// <summary>
            /// Selects all items in the given box
            /// Calls the callback function with all selected items
            /// </summary>
            /// <param name="rectIn">Given box</param>
            /// <param name="callback">The callback function</param>
            /// <param name="param">Parameter for the callback function</param>
            public virtual void EnumerateItems(BoundRect rectIn, EnumeratorCallback callback, Object param)
            {
                throw new NotImplementedException();
            }

            /// <summary>
            /// Changes the bounding box of the given item.
            /// Reorganize the tree.
            /// Only one thread should move with item at same time. (More items should be moved at same time.)
            /// </summary>
            /// <param name="item">Given item</param>
            /// <param name="newBound">New item bound box</param>
            public void Move( T item, BoundRect newBound, Position minNodeSize )
            {
                // move up while this node doesn't fit (no locks needed)
                SceneTreeNodeBase node = this;
                while (node.parent != null && !newBound.IsIn(node.BoundBox))
                    node = node.parent; // upper node

                // update the item bounding box
                item.Bound = newBound;

                // and move down (just Add)
                node.Add(item, minNodeSize);
            }


            /// <summary>
            /// Find the emptiest node in this subtree.
            /// </summary>
            /// <returns>Tree node with less snakes count.</returns>
            public virtual SceneTreeNodeBase FindEmptiestNode()
            {
                return this;
            }
        }

        /// <summary>
        /// Quad tree node with childs
        /// </summary>
        public class SceneTreeNode : SceneTreeNodeBase
        {
            /// <summary>
            /// Tree inner node constructor.
            /// </summary>
            /// <param name="nodeBound"></param>
            public SceneTreeNode(SceneTreeNodeBase parent, BoundRect nodeBound)
                : base( parent, nodeBound )
            {
                childs[0] =
                childs[1] =
                childs[2] =
                childs[3] = null;
            }
            
            /// <summary>
            /// child nodes
            /// 0  1
            /// 2  3
            /// </summary>
            public readonly SceneTreeNodeBase[] childs = new SceneTreeNodeBase[4];

            /// <summary>
            /// Child node bounding box.
            /// </summary>
            /// <param name="child">Child node index.</param>
            /// <returns>Child node with given index bounding box.</returns>
            private BoundRect GetChildBoundBox( int child )
            {
                switch (child)
                {
                    case 0:
                        return new BoundRect(bound.topleft, bound.Center);  // top left
                    case 1:
                        return new BoundRect(bound.CenterX, bound.Top, bound.Right, bound.CenterY); // top right
                    case 2:
                        return new BoundRect(bound.Left, bound.CenterY, bound.CenterX, bound.Bottom);   // bottom left
                    case 3:
                        return new BoundRect(bound.Center, bound.bottomright);  // bottom right
                    default:
                        throw new ArgumentOutOfRangeException("child", "Valid values are 0,1,2,3.");
                }
            }

            /// <summary>
            /// Add an item to the tree node or node child recursively.
            /// The given item bound box must be in this nodes bound box.
            /// </summary>
            /// <param name="item">An item to be inserted.</param>
            /// <param name="minNodeSize">Minimal size for divided child node.</param>
            public override void Add(T item, Position minNodeSize)
            {
                // try to add the item into a child node.
                for (int i = 0; i < 4; ++i)
                {
                    // create the child node if not yet
                    if ( childs[i] == null )
                    {
                        // writing lock
                        rwLock.AcquireWriterLock(-1);

                        if ( childs[i] == null )
                        {
                            // check for new child node size
                            BoundRect childRect = GetChildBoundBox(i);

                            // create child node
                            if (childRect.Width > minNodeSize.x && childRect.Height > minNodeSize.y)
                                childs[i] = new SceneTreeNode(this, childRect);   // create inner node
                            else
                                childs[i] = new SceneTreeLeaf(this, childRect);   // create leaf node

                        }
                        
                        // writing unlock
                        rwLock.ReleaseWriterLock();
                    }

                    // try to add the item into the child node
                    if ( item.Bound.IsIn( childs[i].BoundBox ) )
                    {
                        // add this item to the child node

                        childs[i].Add(item, minNodeSize);   // add recursively into the child node

                        return;
                    }
                }

                // add the item into this node
                base.Add(item, minNodeSize);
            }

            /// <summary>
            /// Selects all items in the given box.
            /// Given box have to fits in this nodes bound box.
            /// Calls the callback function with all selected items
            /// </summary>
            /// <param name="rectIn">Given box</param>
            /// <param name="callback">The callback function</param>
            /// <param name="param">Parameter for the callback function</param>
            public override void EnumerateItems(BoundRect rectIn, EnumeratorCallback callback, Object param)
            {
                rwLock.AcquireReaderLock(-1);   // reader lock

                // enumerate items in this node
                if (items != null)
                    foreach (T item in items)
                    {
                        if (rectIn == null || item.Bound.Intersects(rectIn))  // if an item intersects given box
                            callback(item, param);          // call the callback function
                    }

                // enumerate items in the child nodes recursively
                for (int i = 0; i < 4; ++i)
                {
                    if (childs[i] != null && (rectIn == null || childs[i].BoundBox.Intersects(rectIn)))
                        childs[i].EnumerateItems( rectIn, callback, param);
                }

                rwLock.ReleaseReaderLock();     // reader unlock
            }

            /// <summary>
            /// Find the emptiest node in this subtree.
            /// </summary>
            /// <returns>Tree node with less snakes count.</returns>
            public override SceneTreeNodeBase FindEmptiestNode()
            {
                rwLock.AcquireReaderLock(-1);   // reader lock

                // find the child node with the less count of items
                int bestchild = -1, bestchilditems = 0x7fffffff;
                for (int i = 0; i < 4; ++i)
                {
                    if (childs[i] != null)
                    {
                        if (childs[i].itemscount < bestchilditems)
                        {
                            bestchild = i;
                            bestchilditems = childs[i].itemscount;
                        }
                    }
                }

                rwLock.ReleaseReaderLock();     // reader unlock

                // return the emptiest node
                if (bestchild >= 0)
                {   // best child node found
                    if (childs[bestchild].itemscount == 0)
                        return childs[bestchild];   // this child is empty
                    else
                        return childs[bestchild].FindEmptiestNode();    // this child is not empty, find his best child
                }
                else
                {   // no child node found (all null)
                    return this;
                }
            }
        }

        /// <summary>
        /// Quad tree leaf node
        /// </summary>
        public class SceneTreeLeaf : SceneTreeNodeBase
        {
            /// <summary>
            /// Leaf constructor.
            /// </summary>
            /// <param name="nodeBound"></param>
            public SceneTreeLeaf(SceneTreeNodeBase parent, BoundRect nodeBound)
                : base( parent, nodeBound )
            {

            }

            /// <summary>
            /// Selects all items in the given box
            /// Calls the callback function with all selected items
            /// </summary>
            /// <param name="rectIn">Given box</param>
            /// <param name="callback">The callback function</param>
            /// <param name="param">Parameter for the callback function</param>
            public override void EnumerateItems(BoundRect rectIn, EnumeratorCallback callback, Object param)
            {
                rwLock.AcquireReaderLock(-1);   // reader lock

                if (items != null)
                    foreach (T item in items)
                    {
                        if (rectIn == null || item.Bound.Intersects(rectIn))  // if an item intersects given box
                            callback(item, param);          // call the callback function
                    }

                rwLock.ReleaseReaderLock();     // reader unlock
            }
        }

        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="SceneSize">Initial scene size</param>
        /// <param name="minNodeSize">Minimal node size, that specifies tree leaves size</param>
        public SceneTree(BoundRect SceneSize, Position minNodeSize)
        {
            // leaf nodes size
            this.minNodeSize = minNodeSize;

            // create the tree root node
            this.root = new SceneTreeLeaf/*SceneTreeNode*/(null, SceneSize);
        }

        /// <summary>
        /// Whole scene size.
        /// </summary>
        BoundRect   SceneSize
        {
            get
            {
                return root.BoundBox;
            }
        }

        /// <summary>
        /// Selects all items in the given box
        /// Calls the callback function with all selected items
        /// </summary>
        /// <param name="rectIn">Given box</param>
        /// <param name="callback">The callback function</param>
        /// <param name="param">Parameter for the callback function</param>
        public void EnumerateItems(BoundRect rectIn, EnumeratorCallback callback, Object param)
        {
            root.EnumerateItems(rectIn, callback, param);
        }

        /// <summary>
        /// Removes the given item from the tree.
        /// </summary>
        /// <param name="item">An item to be removed.</param>
        public void Remove(T item)
        {
            item.RemoveFromTree();
        }

        /// <summary>
        /// Inserts new item into the tree
        /// </summary>
        /// <param name="item">given item</param>
        public void Add( T item )
        {
            root.Add(item, minNodeSize);
        }

        /// <summary>
        /// Changes the bounding box of the given item
        /// Reorganize the tree
        /// </summary>
        /// <param name="item">Given item</param>
        /// <param name="newBound">New item bound box</param>
        public void Move( T item, BoundRect newBound )
        {
            item.Move(newBound,minNodeSize);
        }

        /// <summary>
        /// Find the emptiest node in the tree.
        /// </summary>
        /// <returns>Tree node with less snakes count.</returns>
        public SceneTreeNodeBase FindEmptiestNode()
        {
            return root.FindEmptiestNode();
        }
    }
}
