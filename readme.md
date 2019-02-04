## webshellkill-cli

webshellkill-cli 是一个通过DLL Injection + Hook实现的库，实现了通过命令行操作 [D盾_Web查杀](http://www.d99net.net/) 的功能，支持Web查杀版本 V2.0.9。

注意：本项目处于早期版本，不可于生产环境使用。

## 用法

将 Version.dll 放置于 WebShellKill.exe 同目录下即可使用。

```txt

D:\Projects\WebShellKillHook\Debug>WebShellKill.exe -h

WebShellKill CLI
Usage: WebShellKill.exe [OPTIONS] [FILE]

Options:
  -h,--help                        Print this help message and exit
  -i,--show-hidden-file TEXT       扫描隐藏文件（TBD）
  -z,--show-zend-file TEXT         显示经过Zend加密的文件（TBD）
  -a,--auto-check-new-version TEXT 自动更新行为库（TBD）
  -d,--hide-level1 TEXT            隐藏低级别威胁（TBD）
  -t,--type TEXT                   TBD
  -e,--exclude-dir TEXT            排除指定的文件夹（TBD）
  -f,--output TEXT                 输出到文件

```
### Wine使用

```bash
export LC_ALL=zh_CN.UTF-8
winetricks -q mdac28
```

接着，需要使用 winecfg，将``version.dll``配置为“原装先于内建”；或者直接导入``docker-items/override.reg``注册表也可。之后即可``wine WebShellKill.exe``。

### Docker

```bash
docker run --rm -v "/your-website:/data" -it zsxsoft:webshellkill-cli "Z:\\data\\"
```

## 示例输出

```json
[
  {
    "description": "Eval后门 {参数:$_POST[a]}",
    "file": "Z:\\data\\zbphp\\zb_users\\plugin\\test2\\a.php",
    "level": "4",
    "size": "40",
    "time": "2019-01-21 07:28:43"
  },
  {
    "description": "(内藏)preg_replace执行 参数:{\"#a#eis\",\"eval(base64_decode($_SESSION[#theCode#]))\",\"a\"}",
    "file": "Z:\\data\\zbphp\\zb_users\\plugin\\test2\\j.php",
    "level": "4",
    "size": "209",
    "time": "2019-01-21 07:29:20"
  },
  {
    "description": "fputs写后门",
    "file": "Z:\\data\\zbphp\\zb_users\\plugin\\test2\\n.php",
    "level": "5",
    "size": "135",
    "time": "2019-01-21 07:31:54"
  },
  {
    "description": "fputs 参数:{\"a.php\",\"<?@eval($_POST[a])?>\"}",
    "file": "Z:\\data\\zbphp\\zb_users\\plugin\\test2\\p.php",
    "level": "4",
    "size": "146",
    "time": "2019-01-21 07:33:01"
  }
]
```

## 注意事项

1. WebShellKill 本身不是绿色软件，其在32位系统下会读写``HKEY_LOCAL_MACHINE\Software\d99net\d_webshell_kill``，本项目仅对读下了钩子，未对写进行处理。
2. 因为 WebShellKill 是一个 GUI 程序，本项目只对窗口进行了隐藏。因此，如果需要在Wine下使用，仍然需要 X Window 或 Xvfb。
3. 本程序使用的API均为A系列，因此无论是输入输出均为ANSI编码，非中文环境下可能无法使用。在 Wine 下使用，必须配置环境变量``LC_ALL=zh_CN.UTF-8``，且输出需要进行编码转换：``iconv -f GBK -t UTF8``。如直接使用Docker镜像，其已内置转换，故不再需要。

## 协议
The MIT License