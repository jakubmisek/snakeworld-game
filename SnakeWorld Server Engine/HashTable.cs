using System;
using System.Collections.Generic;
using System.Collections;
using System.Text;
using System.Threading;

namespace SnakeWorld_Server
{
    /// <summary>
    /// generic class to manage object by Id()
    /// hash table
    /// TODO locks (reader/writer)
    /// </summary>
    /// <typeparam name="T">hash table item type</typeparam>
    class HashTable<T> where T : Identificable
    {
        private ReaderWriterLock rwLock = new ReaderWriterLock();
        /// <summary>
        /// Hash table lines count
        /// Initialized in ctor
        /// </summary>
        private readonly UInt32 tablesize;

        /// <summary>
        /// The hash table
        /// </summary>
        private List<T>[] table;

        /// <summary>
        /// Default constructor
        /// </summary>
        public HashTable()
            :this(128)
        {
        }

        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="tablesize">Hash table lines count</param>
        public HashTable( UInt32 tablesize )
        {
            this.tablesize = tablesize;
            table = new List<T>[ tablesize ];
        }

        /// <summary>
        /// Add an element to the hash table by his ID
        /// </summary>
        /// <param name="item">Given item</param>
        public void Add(T item)
        {
            UInt32 h = HashValue(item.Id());

            rwLock.AcquireWriterLock(-1); // LOCK

            if (table[h] == null)
                table[h] = new List<T>();   // once the table item is created, it will never be null

            table[h].Add(item);

            rwLock.ReleaseWriterLock(); // UNLOCK
        }

        /// <summary>
        /// Removes the element with the given ID
        /// </summary>
        /// <param name="id">Given ID</param>
        /// <returns>Returns if the element with the given ID was found and removed</returns>
        public bool Remove(UInt32 id)
        {
            UInt32 h = HashValue(id);

            rwLock.AcquireReaderLock(-1); // LOCK

            if (table[h] != null)
                foreach (T item in table[h])
                    if (item.Id() == id)
                    {
                        rwLock.UpgradeToWriterLock(-1);   // AS WRITER

                        table[h].Remove(item);  // once the table item is created, it will never be null - see Add( T )

                        rwLock.ReleaseWriterLock();     // UNLOCK

                        return true;
                    }

            rwLock.ReleaseReaderLock(); // UNLOCK

            return false;
            
        }

        /// <summary>
        /// Removes the given item
        /// </summary>
        /// <param name="item">Given item</param>
        /// <returns>Returns if the given element was found and removed</returns>
        public void Remove(T item)
        {
            UInt32 h = HashValue(item.Id());

            rwLock.AcquireWriterLock(-1);

            if (table[h] != null)
                table[h].Remove(item);

            rwLock.ReleaseWriterLock();
        }

        /// <summary>
        /// Element with the specified ID
        /// </summary>
        /// <param name="id">Given ID</param>
        /// <returns>Element with the specified ID</returns>
        public T    Get( UInt32 id )
        {
            UInt32 h = HashValue(id);

            rwLock.AcquireReaderLock(-1);// LOCK

                if (table[h] != null)
                    foreach (T item in table[h])
                        if (item.Id() == id)
                        {
                            rwLock.ReleaseReaderLock(); // UNLOCK
                            return item;
                        }

            rwLock.ReleaseReaderLock(); // UNLOCK
            
            return default(T);
        }

        /// <summary>
        /// the hash table size
        /// (hash table lines count)
        /// </summary>
        public UInt32 HashTableSize
        {
            get
            {
                return tablesize;
            }
        }

        /// <summary>
        /// hash of the given key
        /// </summary>
        /// <param name="key">key to be hashed</param>
        /// <returns>hash of the given key</returns>
        private UInt32 HashValue( UInt32 key )
        {
            return key % tablesize;
        }


        public List<T> GetItems()
        {
            List<T> list = new List<T>();

            rwLock.AcquireReaderLock(-1);// LOCK

            for (UInt32 i = 0; i < tablesize; ++i)
            {
                if (table[i] != null)
                    foreach (T x in table[i])
                    {
                        list.Add(x);
                    }
            }

            rwLock.ReleaseReaderLock(); // UNLOCK

            // done
            return list;
        }
        
    }
}
