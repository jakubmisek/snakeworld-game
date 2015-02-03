using System;
using System.Collections.Generic;
using System.Collections;
using System.Text;

namespace CommandLineParser
{
    /// <summary>
    /// Option exception class.
    /// </summary>
    public class OptionException : Exception
    {
        public readonly Option WrongOption;
        public readonly string OptName;

        public OptionException(Option WrongOption, string OptName, string ExceptionMessage)
            : base(ExceptionMessage)
        {
            this.WrongOption = WrongOption;
            this.OptName = OptName;
        }

    }
}