using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;

/// <summary>
/// HTML box code
/// </summary>
public class RoundBox
{
    public const string LightStyle = "boxlight";
    public const string BlueStyle = "boxblue";

    public static string HtmlBegin
    {
        get
        {
            return string.Format(_HtmlBeginFormat, "rndboxtop");
        }
    }

    public static string HtmlBeginEx( bool joinTop, bool joinBottom, string specialStyle )
    {
        string classes = null;

        if (joinTop) classes += " topjoin";
        if (joinBottom) classes += " bottomjoin";
        if (specialStyle != null) classes += " " + specialStyle;

        return string.Format(_HtmlBeginFormat, "rndboxtop" + classes);
    }

    public static string HtmlEnd
    {
        get
        {
            return _HtmlEndFormat;
        }
    }

    /// <summary>
    /// Html code with the beginning of the round box.
    /// </summary>
    private const string _HtmlBeginFormat = "<div class='{0}'><div class='b'><div class='l'><div class='r'><div class='bl'><div class='br'><div class='tl'><div class='tr'>";

    /// <summary>
    /// Html code with the ending of the rounded box.
    /// </summary>
    private const string _HtmlEndFormat = "<div class='rndboxend'></div></div></div></div></div></div></div></div></div><div></div>";
}