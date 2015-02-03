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
        AutoGenerateColumns="false" onsortcommand="results_SortCommand">
    <Columns>
        <asp:BoundColumn DataField="name" ></asp:BoundColumn>
        <asp:BoundColumn DataField="length" SortExpression="length"  />
        <asp:BoundColumn DataField="plays" SortExpression="plays"/>
        <asp:BoundColumn DataField="kills" SortExpression="kills" />
        <asp:BoundColumn DataField="suicides" SortExpression="suicides"  />
        <asp:BoundColumn DataField="playtime" SortExpression="playtime" />
    </Columns>
    <HeaderStyle BackColor="#c0c0c0"  />
    <ItemStyle BackColor="#ffffff" />
    <AlternatingItemStyle BackColor="#e5effc" />
    </asp:DataGrid>
    
    </div>
    </form>
</body>
</html>
