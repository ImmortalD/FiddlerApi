package com.immortal.fiddler.core;

/**
 * Fiddler配置信息
 *
 * @author Immortal
 * @version V1.0
 * @since 2018-02-09
 */
public class FiddlerConfig {

    /**
     * Fiddler代理端口
     */
    public int iListenPort;
    /**
     * 是否开启系统代理
     */
    public boolean bRegisterAsSystemProxy;
    /**
     * 默认是否都修改ResponseBody
     */
    public boolean bAlwaysModifyResponseBody;
    /**
     * 是否解密https的流量,这个需要信任Fiddler的根证书
     */
    public boolean bDecryptSSL;
    /**
     * 是否允许远程连接
     */
    public boolean bAllowRemote;
    /**
     * https流量解密使用
     */
    public int iSecureEndpointPort;
    /**
     * 日志级别
     */
    public int logLevel;
    /**
     * 通知类型
     */
    public int notifyType;
    /**
     * 回调
     */
    public FiddlerCallBack callBack;
    /**
     * szSecureEndpointHostname
     */
    public String szSecureEndpointHostname;
    /**
     * 日志文件全路径
     */
    public String szLogFileName;
    /**
     * 进程PID过滤正则表达式,和进程名过滤正则表达式只要有一个匹配上就表示匹配上
     */
    public String szRegexProcessId;
    /**
     * 进程名过滤正则表达式
     */
    public String szRegexProcessName;

    @Override
    public String toString() {
        final StringBuilder sb = new StringBuilder("FiddlerConfig{");
        sb.append("iListenPort=").append(iListenPort);
        sb.append(", bRegisterAsSystemProxy=").append(bRegisterAsSystemProxy);
        sb.append(", bAlwaysModifyResponseBody=").append(bAlwaysModifyResponseBody);
        sb.append(", bDecryptSSL=").append(bDecryptSSL);
        sb.append(", bAllowRemote=").append(bAllowRemote);
        sb.append(", iSecureEndpointPort=").append(iSecureEndpointPort);
        sb.append(", logLevel=").append(logLevel);
        sb.append(", notifyType=").append(notifyType);
        sb.append(", callBack=").append(callBack);
        sb.append(", szSecureEndpointHostname='").append(szSecureEndpointHostname).append('\'');
        sb.append(", szLogFileName='").append(szLogFileName).append('\'');
        sb.append(", szRegexProcessId='").append(szRegexProcessId).append('\'');
        sb.append(", szRegexProcessName='").append(szRegexProcessName).append('\'');
        sb.append('}');
        return sb.toString();
    }
}
