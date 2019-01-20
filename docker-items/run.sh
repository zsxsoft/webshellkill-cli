#!/bin/bash
export LC_CTYPE=zh_CN.UTF-8
export WINEDEBUG=-all
export DISPLAY=:99
nohup /usr/bin/Xvfb :99 -screen 0 1280x720x24 -ac +extension GLX +render -noreset > /dev/null 2>&1 &
wine /root/WebShellKill.exe $@ | iconv -f GBK -t UTF8