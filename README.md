# ParseVmess

把Vmess链接导入到配置模板生成可用的配置

## 编译

请**预先安装jsoncpp的依赖项**,
如arch Linux的发行版可以使用`pacman -S jsoncpp`进行安装.

在依赖项安装正确的前提下,
可以在克隆整个项目以后使用`make all`进行编译,
`makefile`中会递归克隆子模块.

## 使用

几乎完全重写了整个代码,
重写后的项目在以命令行参数的形式接收vmess链接和制定输入输出位置.

使用时需要有配置模板文件,
配置模板与普通的v2ray配置文件类似,
可以根据自己需要进行修改,
但是outbound的第一个对象会被vmess分享链接的内容覆盖,
其他项的内容则会保留.

程序帮助如下:

```
usage:
        parsevmess [-t <template file>] [-o <output file>] <vmess link>
                   [-h] [-p] [-e]
    -t, --template <template file>
              Specify where the template file is.
              Use /etc/v2ray/config_template.json if not specified.
    -o, --output <output file>
              Specify where to output the filled the config.
              Use /etc/v2ray/config.json if not specified.
              If specified dash(-), will print to stdout.
    -e, --export
              Export the vmess link used by <output file> now.
              Take care that this option is only display the vmess link
              stored as comment in <output file>.
    -p, --ps
              Display the ps of vmess link now using.
              Take care that this option is only display the ps
              stored as comment in <output file>.
              This is DEFAULT option if no parameter specified
    -h, --help
              Display this help

    <vmess link>
              Specify the vmesslink to be used..
```

## 安装

可以使用`make all`或者`make insall`来安装文件到合适的位置.

如果需要卸载, 可以执行`make uninstall`.

注意在安装和卸载时, 可能需要使用sudo权限.

## 进度

已经基本完成vmess分享链接的支持, 如果有发现任何bug, 可以提issue告知.

## Vmess协议标准

遵循v2rayNg所使用的分享链接格式[分享链接格式说明(ver 2)](https://github.com/2dust/v2rayN/wiki/%E5%88%86%E4%BA%AB%E9%93%BE%E6%8E%A5%E6%A0%BC%E5%BC%8F%E8%AF%B4%E6%98%8E(ver-2))

## 使用和借鉴的开源项目

1. 用于解析json的[jsoncpp](https://github.com/open-source-parsers/jsoncpp)
2. 用于base64解码的(本人实现的)[myBase64](https://github.com/leafee98/myBase64)
3. `src/fills.cpp`中关于vmess分享链接的行为借鉴自[boypt/vmess2json](https://github.com/boypt/vmess2json)

## 许可

[MIT](/LICENSE)
