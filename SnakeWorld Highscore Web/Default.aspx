﻿<%@ Page Language="C#" AutoEventWireup="true"  CodeFile="Default.aspx.cs" Inherits="_Default" %>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<html xmlns="http://www.w3.org/1999/xhtml">
<head runat="server">
    <title>Sacredware Snakeworld Highscore table</title>
    <link rel="stylesheet" href="style/box.css" type="text/css" />
    <link rel="stylesheet" href="style/default.css" type="text/css" />
</head>
<body>
    <form id="form1" runat="server">
    
    <table cellpadding="0" cellspacing="0" class="period">
    <tr>
        <td>
            <%= (statsPeriod == StatsPeriod.Today) ? RoundBox.HtmlBeginEx(false,true,"c") : null %>
            <asp:HyperLink runat="server" ID="statsDay"></asp:HyperLink>
            <%= (statsPeriod == StatsPeriod.Today) ? RoundBox.HtmlEnd : null%>
        </td>
        <td>
            <%= (statsPeriod == StatsPeriod.Week) ? RoundBox.HtmlBeginEx(false, true, "c") : null%>
            <asp:HyperLink runat="server" ID="statsWeek"></asp:HyperLink>
            <%= (statsPeriod == StatsPeriod.Week) ? RoundBox.HtmlEnd : null%>
        </td>
        <td>
            <%= (statsPeriod == StatsPeriod.Month) ? RoundBox.HtmlBeginEx(false, true, "c") : null%>
            <asp:HyperLink runat="server" ID="statsMonth"></asp:HyperLink>
            <%= (statsPeriod == StatsPeriod.Month) ? RoundBox.HtmlEnd : null%>
        </td>
        <td>
            <%= (statsPeriod == StatsPeriod.Total) ? RoundBox.HtmlBeginEx(false, true, "c") : null%>
            <asp:HyperLink runat="server" ID="statsTotal"></asp:HyperLink>
            <%= (statsPeriod == StatsPeriod.Total) ? RoundBox.HtmlEnd : null%>
        </td>
    </tr>
    </table>
    
    <%= RoundBox.HtmlBegin %>
    
    <asp:DataGrid
     runat="server" ID="results"
      AllowSorting="true"
       GridLines="Vertical" CellSpacing="4" ShowHeader="true" Width="100%" BorderStyle="None"
        AutoGenerateColumns="false" onsortcommand="results_SortCommand">
    <Columns>
        <asp:TemplateColumn><ItemTemplate><%# (Container.ItemIndex+1) + "." %></ItemTemplate></asp:TemplateColumn>
        <asp:BoundColumn DataField="name" ></asp:BoundColumn>
        <asp:BoundColumn DataField="length" SortExpression="length"  />
        <asp:BoundColumn DataField="plays" SortExpression="plays"/>
        <asp:BoundColumn DataField="kills" SortExpression="kills" />
        <asp:BoundColumn DataField="suicides" SortExpression="suicides"  />
        <asp:BoundColumn DataField="playtime" SortExpression="playtime" />
    </Columns>
    <HeaderStyle />
    <ItemStyle BackColor="#ffffff" />
    <AlternatingItemStyle BackColor="#e5effc" />
    </asp:DataGrid>
    
    <%= RoundBox.HtmlEnd %>
    </form>
</body>
</html>
