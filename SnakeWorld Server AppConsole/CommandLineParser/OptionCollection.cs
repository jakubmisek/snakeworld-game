using System;
using System.Collections.Generic;
using System.Text;

namespace CommandLineParser
{
    /// <summary>
    /// Collection for Option objects
    /// </summary>
    public class OptionCollection : IEnumerable<OptionPair>
    {
        private Dictionary<char, Option> _data_short;
        private Dictionary<string, Option> _data_long;


        /// <summary>
        /// Initialize new instance of class OptionCollection
        /// </summary>
        public OptionCollection()
        {
            _data_short = new Dictionary<char, Option>();
            _data_long = new Dictionary<string, Option>();
        }

        /// <summary>
        /// Count of Option object in collection
        /// </summary>
        public int Count
        {
            get
            {
                return _data_short.Count + _data_long.Count;
            }
        }

        /// <summary>
        /// Gets or sets options by short name
        /// </summary>
        /// <param name="ShortOption"></param>
        /// <returns></returns>
        public Option this[char ShortOption]
        {
            get
            {
                return _data_short[ShortOption];
            }
            set
            {
                _data_short[ShortOption] = value;
            }
        }

        /// <summary>
        /// Gets or sets options by long name
        /// </summary>
        /// <param name="ShortOption"></param>
        /// <returns></returns>
        public Option this[string LongOption]
        {
            get
            {
                return _data_long[LongOption];
            }
            set
            {
                _data_long[LongOption] = value;
            }
        } 


        /// <summary>
        /// Erase all the elements of the collection
        /// </summary>
        public void Clear()
        {
            _data_short.Clear();
            _data_long.Clear();
        }


        /// <summary>
        /// Adds object Option with ShortOption indetifier to the collection
        /// </summary>
        public void AddOption(char ShortOption, Option Option)
        {
            _data_short.Add(ShortOption, Option);
        }

        /// <summary>
        /// Adds object Option with LongOption indetifier to the collection
        /// </summary>
        public void AddOption(string LongOption, Option Option)
        {
            _data_long.Add(LongOption, Option);
        }

        /// <summary>
        /// Remove object Option with ShortOption identifier
        /// </summary>
        /// <returns>Returns true if this metod succeeded</returns>
        public bool Remove(char ShortOption)
        {
            return _data_short.Remove(ShortOption);
        }

        /// <summary>
        /// Remove object Option with LongOption identifier
        /// </summary>
        /// <returns>Returns true if this metod succeeded</returns>
        public bool Remove(string LongOption)
        {
            return _data_long.Remove(LongOption);
        }

        #region IEnumerable<OptionPair> Members

        public IEnumerator<OptionPair> GetEnumerator()
        {
            foreach (KeyValuePair<char, Option> optionPair in _data_short)
            {
                yield return new OptionPair(optionPair.Key, optionPair.Value);
            }
        }

        #endregion

        #region IEnumerable Members

        System.Collections.IEnumerator System.Collections.IEnumerable.GetEnumerator()
        {
            return GetEnumerator();
        }

        #endregion
    }
}
