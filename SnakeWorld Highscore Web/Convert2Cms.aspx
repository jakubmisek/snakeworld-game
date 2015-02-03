<%@ Page Language="C#" AutoEventWireup="true" CodeFile="Convert2Cms.aspx.cs" Inherits="Convert2Cms" %>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<html xmlns="http://www.w3.org/1999/xhtml">
<head runat="server">
    <title></title>
</head>
<body>
    <form id="form1" runat="server">
    <div><center>
    <asp:Button runat="server" ID="doconvert2cms" 
            Text="Convert database to the new CMS (users, highscore)" 
            onclick="doconvert2cms_Click" />
            </center>
    </div>
    </form>
</body>
</html>
