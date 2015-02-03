using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;

using SnakeWorld_highscore;

public partial class Convert2Cms : System.Web.UI.Page
{
    protected void Page_Load(object sender, EventArgs e)
    {

    }



    protected void doconvert2cms_Click(object sender, EventArgs e)
    {
        doconvert2cms.Enabled = false;

        const int newhighscoresbaseUserId = 1000000;

        var web = new oldweb.webDataContext();
        var sw = new snakeworldDataContext();
        var cms = new cmsDataContext();

        var usersToConvert = web.UserInfos.Where(x => x.isActive == true && x.isAdmin == false);

        foreach (var user in usersToConvert)
        {
            UserInfo newuser = new UserInfo();

            newuser.DateRegistered = user.dateRegistered;
            newuser.Email = user.email;
            newuser.FullName = user.name;
            newuser.IsActive = user.isActive;
            newuser.PasswordHash = user.passwordHash;
            newuser.SaltHash = user.saltHash;

            cms.UserInfos.InsertOnSubmit(newuser);
            cms.SubmitChanges();

            // convert highscore table
            var items = sw.SnakeInfos.Where(x => x.userId == user.userId);
            foreach (var i in items)
            {
                i.userId = newuser.UserId + newhighscoresbaseUserId;
            }
            sw.SubmitChanges();
        }

        // clear inconsistent highscore items
        var inconsistent = sw.SnakeInfos.Where(x => x.userId < newhighscoresbaseUserId);
        sw.SnakeInfos.DeleteAllOnSubmit(inconsistent);
        sw.SubmitChanges();

        // fix userId ( -= newhighscoresbaseUserId )
        var items2 = sw.SnakeInfos;
        foreach (var i in items2)
        {
            i.userId -= newhighscoresbaseUserId;
        }
        sw.SubmitChanges();
    }
}
