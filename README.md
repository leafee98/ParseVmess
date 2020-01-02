# ParseVmess

把Vmess链接导入到配置模板生成可用的配置

## 编译

```g++ -Llib -ljsoncpp -lmyBase64 main.cpp -o ./out/main.out```

只需要把指定静态链接库编译`main.cpp`即可, `jsoncpp`即名为此的C++项目, 用于解析json, `myBase64`即本人自己实现的Base64编解码, 源码见本人的另一个项目.

## 使用

运行编译结果即可, 要求工作目录下有`settings.json`

```./out/main.out```

也可以在运行时直接指定`settings.json`的位置

```./out/main.out ./settings.json```

## 进度

目前支持了tcp配置和ws两种配置的vmess链接, 可以看到`main.cpp`里面的`if else`里面还空了几个, 只要理清楚配置结构后往里面套即可, 如果有想法, 直接提`pull request`即可.

## Vmess协议标准

[分享链接格式说明(ver 2)](https://github.com/2dust/v2rayN/wiki/%E5%88%86%E4%BA%AB%E9%93%BE%E6%8E%A5%E6%A0%BC%E5%BC%8F%E8%AF%B4%E6%98%8E(ver-2))

## 所使用的开源项目

1. [jsoncpp](https://github.com/open-source-parsers/jsoncpp)
2. (本人实现的)[myBase64](https://github.com/leafee98/myBase64)
