using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Xml;
using System.Globalization;

/// <summary>
/// Language information.
/// </summary>
public class LanguageInfo
{
    private readonly string _title, _culture;
    private readonly int _id;

    private Texts _texts;

    private CultureInfo _cultureInfo;

    /// <summary>
    /// Language culture info.
    /// </summary>
    public CultureInfo CultureInfo
    {
        get
        {
            return _cultureInfo;
        }
    }

    /// <summary>
    /// Language texts.
    /// </summary>
    public Texts TextItems
    {
        get
        {
            return _texts;
        }
    }

    /// <summary>
    /// Language long title.
    /// </summary>
    public string Title
    {
        get
        {
            return _title;
        }
    }

    /// <summary>
    /// Language culture name.
    /// </summary>
    public string CultureName
    {
        get
        {
            return _culture;
        }
    }

    /// <summary>
    /// Language internal ID.
    /// </summary>
    public int Id
    {
        get
        {
            return _id;
        }
    }

    /// <summary>
    /// this
    /// </summary>
    public LanguageInfo This
    {
        get
        {
            return this;
        }
    }

    /// <summary>
    /// Init language info from the XMl node.
    /// XML node must have attributes "title", "culture" and "id".
    /// </summary>
    /// <param name="node"></param>
    public LanguageInfo( XmlNode node )
    {
        _title = node.Attributes["title"].InnerText;
        _culture = node.Attributes["culture"].InnerText;
        _id = int.Parse(node.Attributes["id"].InnerText);

        _texts = new Texts(this);

        _cultureInfo = new CultureInfo(_culture);
    }
}

/// <summary>
/// Available languages and their info.
/// </summary>
public class Languages
{
    private static List<LanguageInfo> _languages = null;

    private static LanguageInfo _defaultLanguage = null;

    private static object locker = new object();

    /// <summary>
    /// Load languages list from given XMl file.
    /// </summary>
    /// <param name="xmlFile"></param>
    /// <returns></returns>
    private static List<LanguageInfo> LoadLanguages( string xmlFile )
    {
        List<LanguageInfo> languages = new List<LanguageInfo>();

        XmlDocument doc = new XmlDocument();
        doc.Load(xmlFile);

        XmlNodeList nodes = doc.DocumentElement.SelectNodes("//languages/language");

        foreach (XmlNode n in nodes)
        {
            LanguageInfo l = new LanguageInfo(n);
            languages.Add(l);

            if (n.Attributes["default"] != null)
                _defaultLanguage = l;
        }

        if (_defaultLanguage == null && languages.Count > 0)
            _defaultLanguage = languages[0];

        return languages;
    }

    /// <summary>
    /// Ensure languages list is loaded.
    /// </summary>
    private static void CheckLanguages()
    {
        if (_languages == null)
        {
            lock(locker)
            {
                if (_languages == null)
                    _languages = LoadLanguages(HttpContext.Current.Server.MapPath("~/App_Data/languages/languages.xml"));
            }
        }
    }

    /// <summary>
    /// List of available languages.
    /// </summary>
    public static List<LanguageInfo> LanguagesList
    {
        get
        {
            CheckLanguages();

            return _languages;
        }
    }

    public static LanguageInfo DefaultLanguage
    {
        get
        {
            CheckLanguages();

            return _defaultLanguage;
        }
    }

    /// <summary>
    /// Get the language by given culture name.
    /// </summary>
    /// <param name="culture"></param>
    /// <returns></returns>
    public static LanguageInfo LanguageByCulture( string culture )
    {
        foreach (LanguageInfo l in LanguagesList)
        {
            if (l.CultureName == culture)
                return l;
        }

        return DefaultLanguage;
    }
}
