using System;
using System.Collections.Generic;
using System.Collections;
using System.Text;

namespace CommandLineParser
{
    
    public class Parser
    {
        /// <summary>
        /// Arguments without options (no-named arguments).
        /// </summary>
        public readonly List<string> Arguments = new List<string>();

        
        /// <summary>
        /// Option arguments.
        /// Get/Set the specified option by the options short name.
        /// </summary>
        public readonly OptionCollection Options = new OptionCollection();
        
        
        /// <summary>
        /// Default constructor with no options defined.
        /// Options should be defined later.
        /// </summary>
        public Parser()
        {

        }

        /// <summary>
        /// Constructor with given predefined short options list.
        /// Other options should be defined later.
        /// </summary>
        /// <param name="ShortNames">Predefined options list.</param>
        public Parser(char[] ShortNames)
        {
            // copy options from PredefinedOptions to Options
            if (ShortNames != null)
                foreach (char opt in ShortNames)
                {
                    Options.AddOption(opt, new Option(false, Option.ParameterType.String,null) );
                }
        }

        /// <summary>
        /// Constructor with given predefined short options list.
        /// Other options should be defined later.
        /// Command line arguments are parsed immediately.
        /// </summary>
        /// <param name="ShortNames">Predefined options list.</param>
        /// <param name="args">Array of command line arguments. Should be null.</param>
        public Parser(char[] ShortNames, string[] args)
            :this(ShortNames)
        {
            Parse(args);
        }


        /// <summary>
        /// Constructor with given predefined options list.
        /// Other options should be defined later.
        /// </summary>
        /// <param name="PredefinedOptions">Predefined options list.</param>
        public Parser(IEnumerable<OptionPair> PredefinedOptions)
        {
            // copy options from PredefinedOptions to Options
            if ( PredefinedOptions != null )
            foreach ( OptionPair opt in PredefinedOptions )
            {
                Options.AddOption(opt.ShortOption,opt.Option);
            }
        }


        /// <summary>
        /// Constructor with given predefined options list,
        /// command line arguments are parsed immediately.
        /// </summary>
        /// <param name="PredefinedOptions">Predefined options list.</param>
        /// <param name="args">Array of command line arguments. Should be null.</param>
        public Parser(IEnumerable<OptionPair> PredefinedOptions, string[] args)
            :this(PredefinedOptions)
        {
            Parse(args);
        }


        /// <summary>
        /// Načte vstupní argumenty programu.
        /// Zpracuje argumenty, ověří korektnost vůči nastaveným volbám a uloží které volby s jakými parametry jsou zapnuté.
        /// </summary>
        /// <param name="args">Pole vstupních argumentů. Should be null.</param>
        public void Parse( string[] args )
        {
            // reset existing options state and arguments.
            Reset();

            // parse the command line
            if ( args != null )
            for (int i = 0; i < args.Length; ++i)
            {
                string str = args[i];
                
                // parse argument "str"
                if ( str.StartsWith("--") )
                {
                    // find the option by long name
                    string strLongName = str.Substring(2);
                    Option curOption;

                    try
                    {
                        curOption = Options[strLongName];
                    }
                    catch (Exception)
                    {
                        throw new OptionException(null, strLongName, "Option with long name '" + strLongName + "' is not defined.");
                    }
                    

                    // parse the option parameters
                    ParseOption(curOption, strLongName, args, ref i);
                    
                }
                else if ( str.StartsWith("-") )
                {
                    if (str.Length == 2)
                    {
                        // find the option by short name
                        char cShortName = str.ToLower()[1];
                        Option curOption;

                        try
                        {
                            curOption = Options[cShortName];
                        }
                        catch (Exception)
                        {
                            throw new OptionException(null, cShortName.ToString(), "Option with short name '" + cShortName + "' is not defined.");
                        }
                         
                        // parse the option parameters
                        ParseOption(curOption, cShortName.ToString(), args, ref i);
                        
                    }
                    else
                    {
                        // invalid short-name option specified
                        if (str.Length == 1)
                            throw new OptionException(null,str,"Character '-' not followed by the options short name.");
                        else
                            throw new OptionException(null,str,"Character '-' followed by more than one character.");
                    }
                        
                }
                else
                {
                    // no named argument
                    Arguments.Add(str);
                }
                
            }

            // check obligatory options
            foreach (OptionPair opt in Options)
            {
                if (opt.Option.IsObligatory && !opt.Option.Exists)
                    throw new OptionException(opt.Option, opt.ShortOption.ToString(),"Obligatory option '" + opt.ShortOption + "' was not specified");
            }
        }


        /// <summary>
        /// Parse the option.
        /// </summary>
        /// <param name="option">Option to be parsed</param>
        /// <param name="args">Command line arguments.</param>
        /// <param name="i">Position in the args.</param>
        private void ParseOption( Option option, string OptName, string[] args, ref int i )
        {
            // parse the option parameters
            try
            {
                option.ParseOption(args, ref i);
            }
            catch (OptionException ex)
            {
                throw new OptionException(ex.WrongOption, ex.OptName, "'" + OptName + "': " + ex.Message);
            }
        }


        /// <summary>
        /// Reset existing options and arguments.
        /// </summary>
        private void Reset()
        {
            // clear no named arguments
            Arguments.Clear();

            // reset options to initial state
            foreach (OptionPair opt in Options)
            {
                opt.Option.Reset();
            }
        }


    }
}
