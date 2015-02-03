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
        private TimeSpan _playtime;

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


        public ResultInfo(UserInfo user, SnakeInfo snake)
        {
            _name = user.name;
            _length = (int)snake.maxLength;
            _kills = snake.kills;
            _suicides = snake.suicides;
            _plays = snake.plays;
            _playtime = TimeSpan.FromSeconds((double)snake.timeSecondsPlayed);
        }
    }

    protected enum StatsPeriod
    {
        Total = 0,
        Today = 1,
        Week = 2,
        Month = 3,
    }

    protected StatsPeriod statsPeriod = StatsPeriod.Total;
    protected LanguageInfo CurrentLanguage = null;
    protected string sortColumn = "length";
    
    protected Texts TextItems
    {
        get
        {
            return CurrentLanguage.TextItems;
        }
    }

    protected string MakeLink( StatsPeriod p, string sort )
    {
        return string.Format("Default.aspx?l={0}&p={1}&s={2}", CurrentLanguage.CultureName, (int)p, sort);
    }

    protected void Page_Load(object sender, EventArgs e)
    {
        // current language
        CurrentLanguage = Languages.LanguageByCulture(Request["l"]);

        if (Request["p"] != null)
        {
            statsPeriod = (StatsPeriod)int.Parse(Request["p"]);
        }
        if (Request["s"] != null)
        {
            sortColumn = Request["s"];
        }

        if (IsPostBack)
            return;

        // texts
        results.Columns[1].HeaderText = TextItems.username;
        results.Columns[2].HeaderText = TextItems.bestscore;
        results.Columns[3].HeaderText = TextItems.playscount;
        results.Columns[4].HeaderText = TextItems.kills;
        results.Columns[5].HeaderText = TextItems.suicides;
        results.Columns[6].HeaderText = TextItems.playtime;

        statsTotal.Text = statsTotal.ToolTip = TextItems.stats_total;
        statsMonth.Text = statsMonth.ToolTip = TextItems.stats_month;
        statsWeek.Text = statsWeek.ToolTip = TextItems.stats_week;
        statsDay.Text = statsDay.ToolTip = TextItems.stats_day;
        
        // results
        RebindResults();
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

    protected void ResetPeriodUrls()
    {
        statsTotal.NavigateUrl = MakeLink(StatsPeriod.Total, sortColumn);
        statsMonth.NavigateUrl = MakeLink(StatsPeriod.Month, sortColumn);
        statsWeek.NavigateUrl = MakeLink(StatsPeriod.Week, sortColumn);
        statsDay.NavigateUrl = MakeLink(StatsPeriod.Today, sortColumn);
    }

    protected void RebindResults()
    {
        SetActiveHeader(sortColumn);
        ResetPeriodUrls();

        var snakeworldDb = new snakeworldDataContext();
        var webDb = new webDataContext();

        IEnumerable<SnakeInfo> requestedInfos = null;

        switch (statsPeriod)
        {
            case StatsPeriod.Total:
                requestedInfos = (from a in snakeworldDb.SnakeInfos
                                  select a);
                break;
            case StatsPeriod.Today:
                {
                    DateTime todayDate = DateTime.Now.Date;

                    requestedInfos = (from a in snakeworldDb.SnakeInfos
                                      where a.playDate == todayDate
                                      select a);
                }
                break;
            case StatsPeriod.Week:
                {
                    DateTime weekStart = DateTime.Now.AddDays(-(int)DateTime.Now.DayOfWeek);
                    DateTime week2Start = weekStart.AddDays(7.0);
                    requestedInfos = (from a in snakeworldDb.SnakeInfos
                                      where a.playDate >= weekStart && a.playDate < week2Start
                                      select a);
                }
                break;
            case StatsPeriod.Month:
                {
                    DateTime monthStart = new DateTime(DateTime.Now.Year, DateTime.Now.Month, 1);
                    DateTime month2Start = monthStart.AddMonths(1);
                    
                    requestedInfos = (from a in snakeworldDb.SnakeInfos
                                      where a.playDate >= monthStart && a.playDate < month2Start
                                      select a);
                }
                break;
        }

        var users = (from a in requestedInfos
                     group a by a.userId into g
                     select new SnakeInfo()
                     {
                         userId = g.Key,
                         kills = g.Sum(s => s.kills),
                         maxLength = g.Max(s => s.maxLength),
                         playDate = g.Max(s => s.playDate),
                         plays = g.Sum(s => s.plays),
                         suicides = g.Sum(s => s.suicides),
                         timeSecondsPlayed = g.Sum(s => s.timeSecondsPlayed)
                     });

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
        sortColumn = e.SortExpression;
        RebindResults();
    }
}
