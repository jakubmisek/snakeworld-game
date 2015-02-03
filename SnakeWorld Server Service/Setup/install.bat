md "C:\Program Files\SnakeWorldServer\"
xcopy SnakeWorldServer "C:\Program Files\SnakeWorldServer\"  /E
InstallUtil.exe "..\Release\SnakeWorld Server Service.exe"

@echo Service installation finished.
@echo Be sure to allow the program in Windows firewall.