using System;
using System.Collections.Generic;
using System.Collections;
using System.Text;

namespace CommandLineParser
{
    /// <summary>
    /// Option class.
    /// </summary>
    public class Option
    {
        private static readonly string[] TrueValues = { "true", "1", "y", "yes", "a" };
        private static readonly string[] FalseValues = { "false", "0", "n", "no", "ne" };

        /// <summary>
        /// Specifies if this option is obligatory and must be present in th arguments list.
        /// </summary>
        public readonly bool IsObligatory;

        /// <summary>
        /// Option description
        /// </summary>
        public readonly string Description = "";

        /// <summary>
        /// Parameter type enum.
        /// </summary>
        public enum ParameterType
        {
            NoArgument = 0,
            Integer,
            String,
            UnsignedInteger,
            Boolean,
            HexadecimalNumber,
            Character,
        }

        /// <summary>
        /// Parameter type.
        /// </summary>
        public readonly ParameterType ParameterTypeValue;

        /// <summary>
        /// Current argument of this option given from the command line after parsing.
        /// </summary>
        private string CurrentParameter = null;


        /// <summary>
        /// Option exists in the parsed command line.
        /// </summary>
        private bool ExistsValue = false;

        /// <summary>
        /// Option exists in the parsed command line.
        /// </summary>
        public bool Exists
        {
            get
            {
                return ExistsValue;
            }
        }

        /// <summary>
        /// Option constructor.
        /// </summary>
        /// <param name="IsObligatory">Specifies if this option must be present in the arguments list.</param>
        /// <param name="ParameterTypeValue">Argument type.</param>
        /// <param name="Description">Option decription</param>
        public Option(bool IsObligatory, ParameterType ParameterTypeValue, string Description)
        {
            this.IsObligatory = IsObligatory;
            this.ParameterTypeValue = ParameterTypeValue;
            this.Description = Description;
        }


        /// <summary>
        /// Get/Set integer parameter.
        /// <throws>OptionException if CurrentParameter is null.</throws>
        /// </summary>
        public Int32 IntParameter
        {
            get
            {
                if (CurrentParameter == null)
                    throw new OptionException(this, null, "Parameter not set");

                return Int32.Parse(CurrentParameter);
            }
            set
            {
                CurrentParameter = value.ToString();
            }
        }

        /// <summary>
        /// Get/Set unsigned integer parameter.
        /// <throws>OptionException if CurrentParameter is null.</throws>
        /// </summary>
        public UInt32 UIntParameter
        {
            get
            {
                if (CurrentParameter == null)
                    throw new OptionException(this, null, "Parameter not set");

                return UInt32.Parse(CurrentParameter);

            }
            set
            {
                CurrentParameter = value.ToString();
            }
        }

        /// <summary>
        /// Get/Set hexadecimal number parameter.
        /// <throws>OptionException if CurrentParameter is null.</throws>
        /// </summary>
        public UInt32 HexNumberParameter
        {
            get
            {
                if (CurrentParameter == null)
                    throw new OptionException(this, null, "Parameter not set");

                return UInt32.Parse(CurrentParameter, System.Globalization.NumberStyles.HexNumber);

            }
            set
            {
                CurrentParameter = value.ToString("X");
            }
        }

        /// <summary>
        /// Get/Set string parameter.
        /// <throws>OptionException if CurrentParameter is null.</throws>
        /// </summary>
        public string StringParameter
        {
            get
            {
                if (CurrentParameter == null)
                    throw new OptionException(this, null, "Parameter not set");

                return CurrentParameter;
            }
            set
            {
                CurrentParameter = value;
            }
        }

        /// <summary>
        /// Get/Set boolean parameter.
        /// <throws>OptionException if CurrentParameter is null.</throws>
        /// </summary>
        public bool BooleanParameter
        {
            get
            {
                if (CurrentParameter == null)
                    throw new OptionException(this, null, "Parameter not set");

                // try to find in TrueValues
                foreach (string sv in TrueValues)
                    if (sv.ToLower() == CurrentParameter.ToLower())
                        return true;

                // try to find in False values
                foreach (string sv in FalseValues)
                    if (sv.ToLower() == CurrentParameter.ToLower())
                        return false;

                // error, value no True and not False
                throw new OptionException(this, null, "'" + CurrentParameter + "' is not valid boolean value.");
            }
            set
            {
                CurrentParameter = value ? TrueValues[0] : FalseValues[0];
            }
        }

        /// <summary>
        /// Get/Set character parameter.
        /// <throws>OptionException if CurrentParameter is null.</throws>
        /// </summary>
        public char CharacterParameter
        {
            get
            {
                if (CurrentParameter == null)
                    throw new OptionException(this, null, "Parameter not set");

                if (CurrentParameter.Length != 1)
                    throw new OptionException(this, null, "Parameter is not a single character");

                return CurrentParameter[0];
            }
            set
            {
                CurrentParameter = value.ToString();
            }
        }


        /// <summary>
        /// Parse this option.
        /// </summary>
        /// <param name="args">Arguments list.</param>
        /// <param name="index">Index of this option in the arguments list. Will be moved on the end of this option.</param>
        public void ParseOption(string[] args, ref int index)
        {
            // option is specified in the command line
            ExistsValue = true;

            // check parameter type
            if (ParameterTypeValue == ParameterType.NoArgument)
                return; // no parameter required
            else
            {
                // option parameter is required

                ++index;

                if (index >= args.Length)
                    throw new OptionException(this, null, "Parameter not specified.");

                string strParameter = args[index];

                // store the parameter
                CurrentParameter = strParameter;

                // check parameter type
                try
                {
                    switch (ParameterTypeValue)
                    {
                        case ParameterType.Integer:
                            {
                                Int32 val = IntParameter;
                            }
                            break;
                        case ParameterType.UnsignedInteger:
                            {
                                UInt32 val = UIntParameter;
                            }
                            break;
                        case ParameterType.HexadecimalNumber:
                            {
                                UInt32 val = HexNumberParameter;
                            }
                            break;
                        case ParameterType.Boolean:
                            {
                                bool val = BooleanParameter;
                            }
                            break;
                        case ParameterType.String:
                            {
                                string val = StringParameter;
                            }
                            break;
                        case ParameterType.Character:
                            {
                                char val = CharacterParameter;
                            }
                            break;
                        default:
                            throw new OptionException(this, null, "Type '" + ParameterTypeValue.ToString() + "' not checked.");
                    }
                }
                catch (Exception ex)
                {
                    // error occurred while parsing parameter
                    throw new OptionException(this, null, ex.Message);
                }


            }

        }


        /// <summary>
        /// Reset the option to the initial state.
        /// </summary>
        public void Reset()
        {
            CurrentParameter = null;
            ExistsValue = false;
        }
    }

}