## Spdlog Wrapper

封装 [gabime/spdlog](https://github.com/gabime/spdlog)

版本`v1.10.0`

## 1. 使用

```cpp
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

    std::string info_msg = "file opened";
    LInfo(info_msg);

    return 0;
}
```

## 2. 日志配置文件格式

见 `spdlog-wrapper/config/log.ini`

```ini
# 基础配置【本节必选】
[basic]
name = log
detailed_min = error
# full_path 或 name_only
detailed_filename_type = full_path
flush_every = 1
flush_on = warning

# 1. 打印到控制台 【本节可选】
# 以 console 开头即可，如果有多个，可以后缀任意内容区分（如-1, _1）
[console]
level = info
# %^ 和 %$ 之间的内容会用彩色显示（根据级别选择相应的颜色，如error用红色显示）
pattern = [%H:%M:%S.%e] %^[%l]%$ %v

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

## 3. 编译

需要安装`CMake`工具。

### 3.1 Linux平台

(a) 编译`spdlog`

```shell
cd spdlog

# x86_64
./build-linux.sh

# aarch64 (arm64)
# 自行修改 spdlog/cmake/toolchain.aarch64.cmake 中的配置
./build-linux.sh aarch64
```

(b) 编译`spdlog-wrapper`

```shell
cd spdlog-wrapper

# x86_64
make

# aarch64 (arm64)
make CC=aarch64-linux-gnu-g++ AR=aarch64-linux-gnu-ar
```

得到静态库:

+ lib/linux/libspdlog.a
+ lib/linux/libspdlog_wrapper.a

以及测试程序:

+ bin/example.out

### 3.2 Windows平台

(a) 编译 `spdlog`

需要安装`cmake`。安装后，进入`spdlog`目录，修改`build-windows.bat`，选择需要使用的`Visual Studio`版本。（默认是VS2019）

`REM`表示注释，选择电脑上安装的VS版本，注释掉其他版本的。

```bat
set VS_VERSION="Visual Studio 16 2019"
REM 可选其他VS版本，例如：
REM set VS_VERSION="Visual Studio 14 2015"
REM set VS_VERSION="Visual Studio 15 2017"
REM set VS_VERSION="Visual Studio 17 2022"
```

运行该脚本，如果没有报错，生成的静态库已被复制到`spdlog-wrapper/lib/windows`目录下。

(b) 编译 `spdlog-wrapper`

使用`Visual Studio 2019`打开`spdlog-wrapper\spdlog-wrapper.sln`工程文件，直接编辑即可。得到静态库：

```txt
spdlog-wrapper/lib/windows
├── Win32
│   ├── Debug
│   │   ├── spdlog.lib
│   │   ├── spdlog.pdb
│   │   └── spdlog_wrapper.lib
│   └── Release
│       ├── spdlog.lib
│       └── spdlog_wrapper.lib
└── x64
    ├── Debug
    │   ├── spdlog.lib
    │   ├── spdlog.pdb
    │   └── spdlog_wrapper.lib
    └── Release
        ├── spdlog.lib
        └── spdlog_wrapper.lib
```

以及测试程序

+ spdlog-wrapper/bin/example.exe

## END
