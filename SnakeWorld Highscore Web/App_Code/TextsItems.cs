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

	// Text items initialization
	private void InitTextItems()
	{
username=new TextField("username", this);bestscore=new TextField("bestscore", this);playscount=new TextField("playscount", this);kills=new TextField("kills", this);suicides=new TextField("suicides", this);playtime=new TextField("playtime", this);lastplaybefore=new TextField("lastplaybefore", this);playingnow=new TextField("playingnow", this);
	}
}
