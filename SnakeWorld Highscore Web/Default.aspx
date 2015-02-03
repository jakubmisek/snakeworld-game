<%@ Page Language="C#" AutoEventWireup="true"  CodeFile="Default.aspx.cs" Inherits="_Default" %>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<html xmlns="http://www.w3.org/1999/xhtml">
<head runat="server">
    <title>Sacredware Snake World - Highscore table</title>
    <link rel="stylesheet" href="style/box.css" type="text/css" />
    <link rel="stylesheet" href="style/default.css" type="text/css" />
</head>
<body>
    <form id="form1" runat="server">
    
    <center>
    <table cellpadding="0" cellspacing="0" class="period">
    <tr>
        <td><%= (statsPeriod == StatsPeriod.Today) ? RoundBox.HtmlBeginEx(false,true,"c") : null %><asp:LinkButton runat="server" ID="statsDay" onclick="statsDay_Click" /><%= (statsPeriod == StatsPeriod.Today) ? RoundBox.HtmlEnd : null%></td>
        <td><%= (statsPeriod == StatsPeriod.Week) ? RoundBox.HtmlBeginEx(false, true, "c") : null%><asp:LinkButton runat="server" ID="statsWeek" onclick="statsWeek_Click" /><%= (statsPeriod == StatsPeriod.Week) ? RoundBox.HtmlEnd : null%></td>
        <td><%= (statsPeriod == StatsPeriod.Month) ? RoundBox.HtmlBeginEx(false, true, "c") : null%><asp:LinkButton runat="server" ID="statsMonth" onclick="statsMonth_Click" /><%= (statsPeriod == StatsPeriod.Month) ? RoundBox.HtmlEnd : null%></td>
        <td><%= (statsPeriod == StatsPeriod.Total) ? RoundBox.HtmlBeginEx(false, true, "c") : null%><asp:LinkButton runat="server" ID="statsTotal" onclick="statsTotal_Click" /><%= (statsPeriod == StatsPeriod.Total) ? RoundBox.HtmlEnd : null%></td>
    </tr>
    </table>
    </center>
    
    <%= RoundBox.HtmlBegin %>
    
    <asp:DataGrid
     runat="server" ID="results"
      AllowSorting="true"
      onpageindexchanged="results_Change"
       GridLines="Vertical" CellSpacing="3" ShowHeader="true" Width="100%" BorderStyle="None"
        AutoGenerateColumns="false" onsortcommand="results_SortCommand">
    <Columns>
        <asp:TemplateColumn><ItemTemplate><%# ((int)Eval("num") >= 0)?(Eval("num")):(null) %></ItemTemplate></asp:TemplateColumn>
        <asp:BoundColumn DataField="name" ></asp:BoundColumn>
        <asp:BoundColumn DataField="length" SortExpression="length"  />
        <asp:BoundColumn DataField="apples" SortExpression="apples"/>
        <asp:BoundColumn DataField="kills" SortExpression="kills" />
        <asp:BoundColumn DataField="suicides" SortExpression="suicides"  />
        <asp:BoundColumn DataField="playtime" SortExpression="playtime" />
    </Columns>
    <PagerStyle Mode="NumericPages" CssClass="pager" HorizontalAlign="Center" PageButtonCount="15" Position="Bottom" />
    <ItemStyle BackColor="#ffffff" />
    <AlternatingItemStyle BackColor="#e5effc" />
    <SelectedItemStyle ForeColor="#606060" />
    </asp:DataGrid>
    
    <%= RoundBox.HtmlEnd %>
    
    </form>
    <div class="bottomLink">[<a href="http://www.sacredware.com/" target="_top" title="Snake World Game">Snake World</a>]</div>
</body>
</html>
