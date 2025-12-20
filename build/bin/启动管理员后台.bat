@echo off
chcp 65001 >nul
cd /d "%~dp0"
echo 正在启动管理员后台...
start "" "RainHub_Admin.exe"
pause

