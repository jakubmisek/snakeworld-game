using System;
using System.Collections.Generic;
using System.Text;

namespace CommandLineParser
{
    /// <summary>
    /// Represents pair short option and Option object
    /// </summary>
    public class OptionPair
    {
        private char _shortOption;
        private Option _option;

        /// <summary>
        /// Initialize new instance of ShortOption
        /// </summary>
        internal OptionPair(char ShortOption,Option Option)
        {
            _shortOption = ShortOption;
            _option = Option;
        }

        /// <summary>
        /// Gets short option
        /// </summary>
        public char ShortOption
        {
            get { return _shortOption; }
        }

        /// <summary>
        /// Gets Option object for short option represented by this instance
        /// </summary>
        public Option Option
        {
            get { return _option; }
        }

    }
}
