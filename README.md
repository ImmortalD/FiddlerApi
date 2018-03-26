# FiddlerApi

* Fiddler数据抓包软件能记录所有客户端和服务器的http和https请求，允许用户监视，设置断点，甚至修改输入输出数据。

- Fiddler是一个http协议调试代理工具，Fiddler4数据抓包软件能够记录并检查所有你的电脑和互联网之间的http通讯，设置断点，
查看所有的“进出”Fiddler的数据（指cookie,html,js,css等文件，这些都可以让你胡乱修改的意思）。Fiddler要比其他的网络调
试器要更加简单，因为它不仅仅暴露http通讯，还提供了一个友好的用户格式。Fiddler不仅功能多.强大。而且还提供了APi供编程着
调用.但是Fiddler本身是C#编写,使得一些编程语言不能直接调用.因此有了FiddlerApi

* FiddlerAPi是基于FiddlerCore4 Api开发的C++版本.这是由于一些语言无法直接调用FiddlerCore4 Api(C# dll).

# 更多资源

-  [FiddlerApi](/FiddlerAPi/README.md)

-  [FiddlerJNI(java sdk jni部分)](./FiddlerJNI/README.md)

-  [FiddlerCore(java sdk)](FiddlerApi/README.md)

-  [FDTest(FiddlerApi C++调用例子)](./FDTest/README.md)

-  [E调用(易语言调用例子)](./E调用/README.md)