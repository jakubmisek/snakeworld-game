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
        private int _length, _kills, _suicides, _plays, _apples;
        private TimeSpan _playtime;
        private int _num = -1;

        public readonly bool IsRegisteredPlayer;

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
        public int num
        {
            get
            {
                return _num;
            }
            set
            {
                _num = value;
            }
        }
        public int apples
        {
            get
            {
                return _apples;
            }
        }

        public ResultInfo(UserInfo user, SnakeInfo snake)
        {
            IsRegisteredPlayer = (user.userId != (-1));

            _name = user.name;
            _length = (int)snake.maxLength;
            _kills = snake.kills;
            _suicides = snake.suicides;
            _plays = snake.plays;
            _playtime = TimeSpan.FromSeconds((double)snake.timeSecondsPlayed);
            _apples = snake.applesEaten;

            if (_name.Length > 16)
            {
                _name = _name.Remove(14) + "..";
            }
        }
    }

    protected enum StatsPeriod
    {
        Total = 0,
        Today = 1,
        Week = 2,
        Month = 3,
    }

    protected LanguageInfo CurrentLanguage = null;

    protected string sortColumn
    {
        get
        {
            object o = Session["s"];

            if (o == null)
            {
                Session["s"] = o = "length";
            }

            return (string)o;
        }
        set
        {
            Session["s"] = value;
        }
    }

    protected StatsPeriod statsPeriod
    {
        get
        {
            object o = Session["p"];

            if (o == null)
            {
                Session["p"] = o = StatsPeriod.Today;
            }

            return (StatsPeriod)o;
        }
        set
        {
            Session["p"] = value;
        }
    }

    protected const int itemsOnPage = 15;

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
        results.Columns[3].HeaderText = TextItems.apples;
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

    protected void SetActiveHeader()
    {
        for (int i = 0; i < 7; ++i)
            results.Columns[i].HeaderStyle.CssClass = string.Empty;

        switch (sortColumn)
        {
            case "length":
                results.Columns[2].HeaderStyle.CssClass = "sort";
                //results.Columns[2].ItemStyle.BackColor = System.Drawing.Color.FromArgb(0xe5, 0xef, 0xfc);
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

    protected void RebindResults()
    {
        SetActiveHeader();
        
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
                    DateTime weekStart = DateTime.Now.AddDays(-(int)DateTime.Now.DayOfWeek).Date;
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
                         applesEaten = g.Sum(s => s.applesEaten),
                         timeSecondsPlayed = g.Sum(s => s.timeSecondsPlayed)
                     });

        List<ResultInfo> resultInfo = new List<ResultInfo>();

        foreach (var v in users)
        {
            UserInfo user = (v.userId != (-1)) ? webDb.UserInfos.SingleOrDefault(u => u.userId == v.userId) : (new UserInfo() { name = TextItems.unregistered, userId = (-1) });

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
            case "apples":
                shortedResults = resultInfo.OrderByDescending(a => a.apples);
                break;

        }

        // paging
        results.AllowPaging = true;
        results.AllowCustomPaging = true;
        results.PageSize = itemsOnPage;
        results.VirtualItemCount = shortedResults.Count();

        results.SelectedIndex = -1;

        var resultItems = shortedResults.Skip(results.CurrentPageIndex * itemsOnPage).Take(itemsOnPage);

        // rows num.
        int i = results.CurrentPageIndex * itemsOnPage + 1;
        int rowIndex = 0;
        foreach (var r in resultItems)
        {
            if (r.IsRegisteredPlayer)
            {
                r.num = i++;
            }
            else
            {
                results.SelectedIndex = rowIndex;
            }

            ++rowIndex;
        }

        // data bind
        results.DataSource = resultItems;
        results.DataBind();

    }

    protected void results_Change(Object sender, DataGridPageChangedEventArgs e)
    {

        // Set CurrentPageIndex to the page the user clicked.
        results.CurrentPageIndex = e.NewPageIndex;
        
        RebindResults();
    }


    protected void results_SortCommand(object source, DataGridSortCommandEventArgs e)
    {
        sortColumn = e.SortExpression;
        results.CurrentPageIndex = 0;

        RebindResults();
    }

    protected void statsDay_Click(object sender, EventArgs e)
    {
        results.CurrentPageIndex = 0;
        statsPeriod = StatsPeriod.Today;

        RebindResults();
    }
    protected void statsWeek_Click(object sender, EventArgs e)
    {
        results.CurrentPageIndex = 0;
        statsPeriod = StatsPeriod.Week;

        RebindResults();
    }
    protected void statsMonth_Click(object sender, EventArgs e)
    {
        results.CurrentPageIndex = 0;
        statsPeriod = StatsPeriod.Month;

        RebindResults();
    }
    protected void statsTotal_Click(object sender, EventArgs e)
    {
        results.CurrentPageIndex = 0;
        statsPeriod = StatsPeriod.Total;

        RebindResults();
    }
}
