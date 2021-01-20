## wstone

### 用途说明
+ 项目是根据自己工作多年的编程经验封装而来，__力求易用与稳定__。
+ 主要针对 **windows** 平台使用C++开发，迁移至 **Linux** 需要修改OS层面API

### 目录布局说明
1. **helper** 目录存放一些较为基础的功能接口
2. 每个目录中 **bin** 文件夹存放编译产生的库文件或者二进制文件
3. 每个目录中 **build** 文件夹存放VS工程文件
4. 每个目录中 **include** 文件夹存放供外部使用的接口文件
5. 每个目录中 **src** 文件夹存放工程源代码文件  
---

## 代码库注意事项
1. **SDK所有工程必须配置为Unicode环境**
2. **SDK所有std::string,char类型的都存储为UTF8**
3. 使用VS2019插件ForceUTF8（with-out BOM） 让所有被VS使用的文件都转换为UTF8）
4. 修改系统 Microsoft.Cpp.Win32.user(VS2019需要每个享目自己添加) 属性页，添加 **/utf-8** 至 C/C++ commandline中，让所有win32工程都支持UTF8执行代码集 (VS2019废弃了user属性页，自定义就行)
5. 添加控制台打印UTF8功能， 添加 
    1. _SetConsoleCP(65001)_。
    2. _SetConsoleOutputCP(65001)_。
    3. _修改控制台字体为 Lucida Console。_
6. vld_x86.dll 官方推荐使用6.11.1.404版本的dbghelp.dll,dbghelp.dll分32位和64位版本, bin目录下的dbghelp.dll为32位版本，匹配vld_x86.dll  
---
	
## 加载指定DLL目录
1. 在Linker选项Manifest File键附加清单中添加 "type='win32' name='runtime' version='1.0.0.0' processorArchitecture='x86'" 
2. 创建exe同级目录rumtime，放入需要的DLL信息  
---

## 库功能介绍

### 测试框架
+ 本项目使用 **catch2** 进行单元测试，具体使用请自行Google。

### Foundation

> Base
+ 封装 **Win32 API**,实现线程池，事件，注册表，服务，文件，路径，内存池，信号量，Timer等各种基础接口

> Bugtrap
+ 使用开源Dump收集处理库 **bugtrap**，使用者可结合 _foundation/bin/bugtarp/bugtrapwebserver.zip_ 部署dump收集服务器。
+ 调用 _foundation/include/bugtrap/IBugTrap.h 可实现程序在crash时候采集数据，使用示范。以下代码就会在exe运行目录下面产生dump文件夹
	```C++
	int main(void) {
		ws::IBugTrap::install();
		auto zero = 0;
		auto p = 100 / zero;
		return 0;
	}
	```

> CURL
+ 基于libcurl开发，支持SSL加密，支持Gzip，
+ 实现ftp文件下载功能
+ 支持task cancel

> Archie
+ 易于使用的Tar功能，Zip字符串压缩，Z4算法压缩
  
> Crypt
+ 基于Crypto++8.2封装，方便使用的各种加解密算法，RSA支持超长文本加解密

> Mysql, Redis, Logger, Regex, Sqlite, ZXing 等各种丰富的库