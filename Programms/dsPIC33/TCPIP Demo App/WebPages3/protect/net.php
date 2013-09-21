<html>
<head>
<title>Сеть</title>
</head>
<body>
<form action="netDHCP.html" method="get">
    <table>
        <tr><td>Имя</td><td><input type="text" name=host value="AC-CONTROL" size="12"></td></tr>
	<tr><td>Тип</td><td>
        <select name=dhcp>
            <option selected value=0>DHCP</option>
            <option value=1>Static</option>
        </select>
	</td></tr>
        <tr><td>IP</td><td><input disabled type="text" name=ip value="192.168.1.10" size="12"></td></tr>
        <tr><td>Маска</td><td><input disabled type="text" name=mask value="255.255.255.0" size="12"></td></tr>
        <tr><td>Шлюз</td><td><input disabled type="text" name=gate value="192.168.1.1" size="12"></td></tr>
        <tr><td>DNS1</td><td><input disabled type="text" name=dns1 value="192.168.1.1" size="12"></td></tr>
        <tr><td>DNS2</td><td><input disabled type="text" name=dns2 value="192.168.1.1" size="12"></td></tr>
        <tr><td>NTP</td><td><input disabled type="text" name=ntp1 value="192.168.1.1" size="12"></td></tr>
        <tr><td>Сервер</td><td><input disabled type="text" name=DestIp value="192.168.1.1" size="12"></td></tr>
        <tr><td colspan="2"><input type="submit" value="Сохранить"></td></tr>
    </table>
</form>    
</body>
</html>
