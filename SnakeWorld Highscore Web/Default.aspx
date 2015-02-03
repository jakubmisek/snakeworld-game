<%@ Page Language="C#" AutoEventWireup="true"  CodeFile="Default.aspx.cs" Inherits="_Default" %>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<html xmlns="http://www.w3.org/1999/xhtml">
<head runat="server">
    <title>Sacredware Snakeworld Highscore table</title>
</head>
<body>
    <form id="form1" runat="server">
    <div>
    
    <asp:DataGrid
     runat="server" ID="results"
      AllowSorting="true"
       CellSpacing="8" GridLines="Vertical" ShowHeader="true" Width="100%" BorderStyle="None"
        AutoGenerateColumns="false">
    <Columns>
        <asp:BoundColumn DataField="name" HeaderText="User name" />
        <asp:BoundColumn DataField="length" SortExpression="length" HeaderText="Best score" />
        <asp:BoundColumn DataField="plays" SortExpression="plays" HeaderText="Plays count" />
        <asp:BoundColumn DataField="kills" SortExpression="kills" HeaderText="Kills" />
        <asp:BoundColumn DataField="suicides" SortExpression="suicides" HeaderText="Suicides" />
        <asp:BoundColumn DataField="playtime" SortExpression="playtime" HeaderText="Total play time" />
    </Columns>
    <HeaderStyle BackColor="#c0c0c0"  />
    <ItemStyle BackColor="#ffffff" />
    <AlternatingItemStyle BackColor="#e5effc" />
    </asp:DataGrid>
    
    </div>
    </form>
</body>
</html>
