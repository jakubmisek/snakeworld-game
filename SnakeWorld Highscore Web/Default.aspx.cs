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

    protected void Page_Load(object sender, EventArgs e)
    {
        if (IsPostBack)
            return;

        var snakeworldDb = new snakeworldDataContext();
        var webDb = new webDataContext();

        var users = (from a in snakeworldDb.SnakeInfos
                     orderby a.maxLength descending
                     select a);

        List<ResultInfo> resultInfo = new List<ResultInfo>();

        foreach (var v in users)
        {
            var user = webDb.UserInfos.SingleOrDefault(u => u.userId == v.userId);
            if (user != null)
                resultInfo.Add(new ResultInfo(user, v));
        }

        results.DataSource = resultInfo;
        results.DataBind();
    }

}
