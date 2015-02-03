using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;

public partial class _Default : System.Web.UI.Page 
{
    public class ResultInfo
    {
        private string _name;
        private int _length, _kills, _suicides, _plays;
        private TimeSpan _playtime, _lastplayago;

        public string name
        {
            get
            {
                return _name;
            }
        }
        public int length
        {
            get
            {
                return _length;
            }
        }
        public int kills
        {
            get
            {
                return _kills;
            }
        }
        public int suicides
        {
            get
            {
                return _suicides;
            }
        }
        public int plays
        {
            get
            {
                return _plays;
            }
        }
        public TimeSpan playtime
        {
            get
            {
                return _playtime;
            }
        }
        public TimeSpan lastplayago
        {
            get
            {
                return _lastplayago;
            }
        }


        public ResultInfo(UserInfo user, SnakeInfo snake)
        {
            _name = user.name;
            _length = (int)snake.maxLength;
            _kills = snake.kills;
            _suicides = snake.suicides;
            _plays = snake.plays;
            _playtime = TimeSpan.FromSeconds((double)snake.timeSecondsPlayed);

            if (snake.lastPlayTime != null)
                _lastplayago = DateTime.Now - (DateTime)snake.lastPlayTime;
            else
                _lastplayago = TimeSpan.MaxValue;
        }
    }

    protected LanguageInfo CurrentLanguage = null;
    protected Texts TextItems
    {
        get
        {
            return CurrentLanguage.TextItems;
        }
    }

    protected void Page_Load(object sender, EventArgs e)
    {
        // current language
        CurrentLanguage = Languages.LanguageByCulture(Request["l"]);

        if (IsPostBack)
            return;

        // texts
        results.Columns[1].HeaderText = TextItems.username;
        results.Columns[2].HeaderText = TextItems.bestscore;
        results.Columns[3].HeaderText = TextItems.playscount;
        results.Columns[4].HeaderText = TextItems.kills;
        results.Columns[5].HeaderText = TextItems.suicides;
        results.Columns[6].HeaderText = TextItems.playtime;

        // results
        RebindResults("length");
    }

    protected void SetActiveHeader(string sortColumn)
    {
        for (int i = 0; i < 7; ++i)
            results.Columns[i].HeaderStyle.CssClass = string.Empty;

        switch (sortColumn)
        {
            case "length":
                results.Columns[2].HeaderStyle.CssClass = "sort";
                break;
            case "plays":
                results.Columns[3].HeaderStyle.CssClass = "sort";
                break;
            case "kills":
                results.Columns[4].HeaderStyle.CssClass = "sort";
                break;
            case "suicides":
                results.Columns[5].HeaderStyle.CssClass = "sort";
                break;
            case "playtime":
                results.Columns[6].HeaderStyle.CssClass = "sort";
                break;

        }
    }

    protected void RebindResults(string sortColumn)
    {
        SetActiveHeader(sortColumn);

        var snakeworldDb = new snakeworldDataContext();
        var webDb = new webDataContext();

        var users = (from a in snakeworldDb.SnakeInfos
                     //orderby a.maxLength descending
                     select a);

        List<ResultInfo> resultInfo = new List<ResultInfo>();
        
        foreach (var v in users)
        {
            var user = webDb.UserInfos.SingleOrDefault(u => u.userId == v.userId);
            if (user != null)
                resultInfo.Add(new ResultInfo(user, v));
        }

        IEnumerable<ResultInfo> shortedResults = null;

        switch (sortColumn)
        {
            case "length":
                shortedResults = resultInfo.OrderByDescending(a => a.length);
                break;
            case "plays":
                shortedResults = resultInfo.OrderByDescending(a => a.plays);
                break;
            case "kills":
                shortedResults = resultInfo.OrderByDescending(a => a.kills);
                break;
            case "suicides":
                shortedResults = resultInfo.OrderByDescending(a => a.suicides);
                break;
            case "playtime":
                shortedResults = resultInfo.OrderByDescending(a => a.playtime);
                break;
            
        }

        results.DataSource = shortedResults;
        results.DataBind();
    }

    protected void results_SortCommand(object source, DataGridSortCommandEventArgs e)
    {
        RebindResults(e.SortExpression);
    }
}
