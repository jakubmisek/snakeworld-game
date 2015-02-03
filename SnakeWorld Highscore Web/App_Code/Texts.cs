using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Xml;

/// <summary>
/// Summary description for Texts
/// </summary>
public partial class Texts
{
    /// <summary>
    /// string xml nodes
    /// </summary>
    private XmlNodeList _stringNodes = null;

    protected XmlNodeList StringNodes
    {
        get
        {
            if (_stringNodes == null)
            {
                XmlDocument doc = new XmlDocument();

                try
                {
                    doc.Load(HttpContext.Current.Server.MapPath("~/App_Data/languages/" + language.CultureName + ".xml"));
                    _stringNodes = doc.DocumentElement.SelectNodes("//language/string");
                }
                catch (Exception)
                {
                }
            }

            return _stringNodes;
        }
    }

    /// <summary>
    /// Reload XML with texts and reinitialize all text items.
    /// </summary>
    public void ReloadTexts()
    {
        lock(this)
        {
            _stringNodes = null;
            InitTextItems();
        }
    }

    private LanguageInfo language;

    #region Text Field
    /// <summary>
    /// One text item field.
    /// </summary>
    public class TextField
    {
        /// <summary>
        /// the key in the xml file.
        /// </summary>
        private readonly string key;

        /// <summary>
        /// the language xml file.
        /// </summary>
        private Texts texts;

        /// <summary>
        /// Get xml node with the current key.
        /// </summary>
        /// <returns></returns>
        private XmlNode GetNode()
        {
            if (texts.StringNodes != null)
            foreach (XmlNode n in texts.StringNodes)
                if (n.Attributes["key"].Value.ToLower() == key)
                    return n;

            return null;
        }

        /// <summary>
        /// Tool tip text.
        /// </summary>
        private string _ToolTipText = null;

        /// <summary>
        /// Text.
        /// </summary>
        private string _Text = null;

        /// <summary>
        /// the tool tip
        /// </summary>
        public string ToolTipText
        {
            get
            {
                if (_ToolTipText == null)
                {
                    XmlNode node = GetNode();

                    _ToolTipText =
                        (node != null) ?
                        ((node.Attributes["title"] != null) ? node.Attributes["title"].Value : Text) :
                        ("##" + key + "##");
                }

                return _ToolTipText;
            }
        }

        /// <summary>
        /// the text
        /// </summary>
        public string Text
        {
            get
            {
                if (_Text == null)
                {
                    XmlNode node = GetNode();
                    _Text = (node != null) ? (node.InnerXml) : ("##" + key + "##");
                }

                return _Text;
            }
        }

        /// <summary>
        /// Text.
        /// </summary>
        /// <param name="field"></param>
        /// <returns></returns>
        public static implicit operator string(TextField field)
        {
            return field.Text;
        }

        /// <summary>
        /// Init.
        /// </summary>
        /// <param name="tooltip"></param>
        /// <param name="text"></param>
        public TextField(string key, Texts texts)
        {
            this.key = key;
            this.texts = texts;
        }
    }
    #endregion

    /// <summary>
    /// Init.
    /// Load XML.
    /// </summary>
    /// <param name="xmlFileName"></param>
    public Texts(LanguageInfo language)
	{
        this.language = language;

        InitTextItems();
    }
}
