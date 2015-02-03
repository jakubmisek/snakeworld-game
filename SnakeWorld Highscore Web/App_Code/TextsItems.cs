/// <summary>
/// Language texts.
/// Text items.
/// </summary>
public partial class Texts
{

	/// <summary>User name</summary>
	public TextField username;

	/// <summary>Best score</summary>
	public TextField bestscore;

	/// <summary>Plays count</summary>
	public TextField playscount;

	/// <summary>Kills</summary>
	public TextField kills;

	/// <summary>Suicides</summary>
	public TextField suicides;

	/// <summary>Total time</summary>
	public TextField playtime;

	/// <summary>Last play</summary>
	public TextField lastplaybefore;

	/// <summary>Playing now</summary>
	public TextField playingnow;

	/// <summary>Total</summary>
	public TextField stats_total;

	/// <summary>Today</summary>
	public TextField stats_day;

	/// <summary>This week</summary>
	public TextField stats_week;

	/// <summary>This month</summary>
	public TextField stats_month;

	/// <summary>(Not registered)</summary>
	public TextField unregistered;

	// Text items initialization
	private void InitTextItems()
	{
username=new TextField("username", this);bestscore=new TextField("bestscore", this);playscount=new TextField("playscount", this);kills=new TextField("kills", this);suicides=new TextField("suicides", this);playtime=new TextField("playtime", this);lastplaybefore=new TextField("lastplaybefore", this);playingnow=new TextField("playingnow", this);stats_total=new TextField("stats_total", this);stats_day=new TextField("stats_day", this);stats_week=new TextField("stats_week", this);stats_month=new TextField("stats_month", this);unregistered=new TextField("unregistered", this);
	}
}
