# FiddlerAPI(JNI)

###  说明

- 所有的`Java Bean`的属性都是`public`的,这不符合标准的`Java Bean`格式,可以自行修改源码,注意:`字段名虽然也是C++风格,但是不可修改,因为这与Jni中的字段名一一对应`

### JDK版本

- 需要JDK8及以上,使用是要把`libs`下的`FiddlerCore4.dll`和`FiddlerApi.dll`拷贝到`JAVA_HOME/bin`目录下,`FiddlerJNI_86.dll`和`FiddlerJNI_64.dll`加入到环境变量


