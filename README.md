## Spdlog Wrapper

封装 [gabime/spdlog](https://github.com/gabime/spdlog)

版本`v1.10.0`

## 1. 使用

```cpp
#include "log/simple_console_logger.h"
#include "log/logger_config.h"

#define LOG_DEFAULT
#include "log/logger.h"

bool InitLogger() {
    ic::log::LoggerConfig config;
    if (!config.ReadFromFile("config/log.ini")) {
        ic::log::Log("Read file config/log.ini failed.");
        return false;
    }
    ic::log::Logger::SetConfig(config);
    return true;
}

int main() {
    /* 必须初始化后，才能输出日志 */
    if (!InitLogger()) {
        return 1;
    }

    LTrace("Application start!");
    LDebug("Application start!");
    LInfo("Application start!");
    LWarn("Application start!");
    LError("Application start!");
    LCritical("Application start!");

    int a = 7;
    int b = 8;
    LInfo("ADD: {} + {} = {}", a, b, a + b);

    std::string err_msg = "open file failed";
    LError("{}", err_msg);

    err_msg = "file opened";
    LInfo("{}", err_msg);

    return 0;
}
```

## 2. 日志宏，控制详细程度

 + `LOG_SIMPLE`:  简单，不输出当前函数名和源文件
 + `LOG_DETAIL`:  详细，输出当前函数名和源文件
 + `LOG_DEFAULT`: 默认，级别低于error的简单输出，否则详细输出  (默认！)

在包含log/logger.h前定义，或者在编译的时候指定。

示例如下：

`LOG_SIMPLE`

```txt
[02:16:59.561] [debug] Application start!
[02:16:59.561] [info] Application start!
[02:16:59.561] [warning] Application start!
[02:16:59.561] [error] Application start!
[02:16:59.561] [critical] Application start!
```

`LOG_DETAIL`

```txt
[02:16:59.561] [debug] Application start! <main.cpp> <main> <28>
[02:16:59.561] [info] Application start! <main.cpp> <main> <28>
[02:16:59.561] [warning] Application start! <main.cpp> <main> <28>
[02:16:59.561] [error] Application start! <main.cpp> <main> <28>
[02:16:59.561] [critical] Application start! <main.cpp> <main> <29>
```

`LOG_DEFAULT`

```txt
[02:16:59.561] [debug] Application start!
[02:16:59.561] [info] Application start!
[02:16:59.561] [warning] Application start!
[02:16:59.561] [error] Application start! <main.cpp> <main> <28>
[02:16:59.561] [critical] Application start! <main.cpp> <main> <29>
```

## 3. 日志配置文件格式

见 `spdlog-wrapper/config/log.ini`

```ini
# 基础配置【本节必选】
[basic]
name = log
flush_every = 1  # 单位：秒
flush_on = warning  # warning=warn, error=err

# 1. 打印到控制台 【本节可选】
# 以 console 开头即可，如果有多个，可以后缀任意内容区分（如-1, _1）
[console]
level = info
pattern = [%H:%M:%S.%e] %^[%l]%$ %v   # %^ 和 %$ 之间的内容会用彩色显示（根据级别选择相应的颜色，如error用红色显示）

# 2.1 每日日志，输出到文件 【本节可选】
# 以 daily 开头即可，如果有多个，可以后缀任意内容区分（如-1, _1）
# ${bin}宏表示可执行文件所在的目录
[daily-1]
name = daily
ext = .txt
directory = ${bin}/../logs
level = debug
pattern = [%H:%M:%S.%e] [%l] %v

[daily-2]
name = daily_error
ext = .txt
directory = ${bin}/../logs
level = warning
pattern = [%H:%M:%S.%e] [%l] %v

# 2.2 滚动日志，输出到文件 【本节可选】
# 以 rotating 开头即可，如果有多个，可以后缀任意内容区分（如-1, _1）
[rotating]
name = log
ext = .txt
directory = ${bin}/../logs
level = trace
pattern = [%Y-%m-%d %H:%M:%S.%e] [%l] %v
max_files_count = 5
max_file_size = 5M
```

## 4. 编译

### 4.1 Linux平台

(a) 编译`spdlog`

```shell
cd spdlog
mkdir build && cd build
cmake ..
make
```

将编译后的`libspdlog.a`复制到`spdlog-wrapper/lib/linux/`下

```shell
cp libspdlog.a ../../spdlog-wrapper/lib/linux
```

(b) 编译`spdlog-wrapper`

```shell
cd spdlog-wrapper
make
```

得到静态库:

+ lib/linux/libspdlog_wrapper.a

以及测试程序:

+ bin/example.out

### 4.2 Windows平台

(a) 编译 `spdlog`

需要安装`cmake`。安装后，进入`spdlog`，修改`configure.bat`，选择需要使用的`Visual Studio`版本。（默认是VS2019）

`REM`表示注释，选择电脑上安装的VS版本，注释掉其他版本的。

```bat
set HOME=%cd%
set VS=%HOME%\build\VisualStudio

md %VS%\x86 %VS%\x64

cd %VS%\x86
REM cmake -G "Visual Studio 15 2017" -A Win32 ../../..
cmake -G "Visual Studio 16 2019" -A Win32 ../../..
REM cmake -G "Visual Studio 17 2022" -A Win32 ../../..

cd %VS%\x64
REM cmake -G "Visual Studio 15 2017" -A x64 ../../..
cmake -G "Visual Studio 16 2019" -A x64 ../../..
REM cmake -G "Visual Studio 17 2022" -A x64 ../../..
```

附上VS的几个版本：

```txt
Visual Studio 11 2012
Visual Studio 12 2013
Visual Studio 14 2015
Visual Studio 15 2017
Visual Studio 16 2019
Visual Studio 17 2022
```

运行该脚本，如果没有报错，将得到`Win32`和`x64`类型的两个项目：

+ spdlog/build/VisualStudio/x86/spdlog.sln
+ spdlog/build/VisualStudio/x64/spdlog.sln

各编译`Debug`和`Release`版本，得到4个静态库，复制到如下位置：

+ spdlog-wrapper/lib/windows/Win32/Debug/spdlog.lib
+ spdlog-wrapper/lib/windows/Win32/Release/spdlog.lib
+ spdlog-wrapper/lib/windows/x64/Debug/spdlog.lib
+ spdlog-wrapper/lib/windows/x64/Release/spdlog.lib

(b) 编译 `spdlog-wrapper`

进入 `spdlog-wrapper/win-vsproj`，编译即可。

得到静态库：

+ spdlog-wrapper/lib/windows/Win32/Debug/spdlog_wrapper.lib
+ spdlog-wrapper/lib/windows/Win32/Release/spdlog_wrapper.lib
+ spdlog-wrapper/lib/windows/x64/Debug/spdlog_wrapper.lib
+ spdlog-wrapper/lib/windows/x64/Release/spdlog_wrapper.lib

以及测试程序

+ spdlog-wrapper/bin/example.exe

## END
