using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Globalization;
using System.Threading;
using System.Diagnostics;

namespace SnakeWorld_highscore
{

    /// <summary>
    /// Summary description for LocalizedText
    /// </summary>
    public class LocalizedText
    {
        private readonly string _defaultText;
        private readonly Dictionary<int, string> _localizedTexts = null;

        private string[] _preparedTexts = null;

        public LocalizedText(string defaultText)
        {
            _defaultText = defaultText;
        }

        public LocalizedText(string defaultText, Dictionary<int, string> localizedTexts)
        {
            _defaultText = defaultText;
            _localizedTexts = localizedTexts;
        }

        public LocalizedText(string defaultText, ICollection<KeyValuePair<int, string>> items)
        {
            _defaultText = defaultText;
            _localizedTexts = new Dictionary<int, string>();

            foreach (var i in items)
                _localizedTexts.Add(i.Key, i.Value);
        }

        /// <summary>
        /// Initialize the localized texts.
        /// </summary>
        /// <param name="defaultText">Default text.</param>
        /// <param name="lngText">Culture name followed by localized text (separated by \0). (e.g. "cs-CZ\0Český text.")</param>
        public LocalizedText(string defaultText, params string[] lngText)
        {
            _defaultText = defaultText;
            _localizedTexts = new Dictionary<int, string>();

            // save localized texts to be processed when it's needed
            _preparedTexts = lngText;
        }

        public string Text(CultureInfo ci)
        {
            // initialize localized texts if not yet
            if (_preparedTexts != null)
            {
                foreach (string s in _preparedTexts)
                {
                    string[] prms = s.Split(new char[] { '\0' });

                    CultureInfo culture = new CultureInfo(prms[0]);
                    _localizedTexts.Add(culture.LCID, prms[1]);
                }

                _preparedTexts = null;
            }

            // find localized text, corresponding to the given culture
            string text;

            if (_localizedTexts == null || ci == null || !_localizedTexts.TryGetValue(ci.LCID, out text))
                return _defaultText;
            else
                return text;
        }

        public static implicit operator string(LocalizedText text)
        {
            return text.Text(Thread.CurrentThread.CurrentCulture);
        }

        public override string ToString()
        {
            return Text(Thread.CurrentThread.CurrentCulture);
        }

    }

}