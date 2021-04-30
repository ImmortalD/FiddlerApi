package com.immortal.fiddler.core;

import java.util.Properties;

/**
 * Fiddler 消息回调处理
 *
 * @author Immortal
 * @version V1.0
 * @since 2018-02-09
 */
public class Fiddler {

    /**
     * 数据发送阶段,可修改请求数据
     */
    public static final int SEND = 0;

    /**
     * 数据接收阶段,可修改响应数据
     */
    public static final int RECV = 1;

    /**
     * 数据接收阶段,数据不可修改
     */
    public static final int COMPLETE = 2;

    /**
     * 不打印日志
     */
    public static final int CLOSE = 0;

    /**
     * 只打印ERR级别日志
     */
    public static final int ERR = 1;

    /**
     * 打印WARN和ERR级别日志
     */
    public static final int WARN = 3;

    /**
     * 打印INFO WARN ERR 级别日志
     */
    public static final int INFO = 7;

    /**
     * 打印DEBUIG NFO WARN ERR 级别日志
     */
    public static final int DEBUG = 15;

    /**
     * 不修改数据
     */
    public static final int MODIFY_NO_ALL = 0;

    /**
     * 修改Url
     */
    public static final int MODIFY_URL = 1;

    /**
     * 修改http body
     */
    public static final int MODIFY_BODY = 2;

    /**
     * 修改http 头
     */
    public static final int MODIFY_HEADERS = 4;

    /**
     * 修改通知类型
     */
    public static final int MODIFY_NOTIFY = 8;

    /**
     * 修改url,http header,http body以及修改通知类型
     */
    public static final int MODIFY_ALL = 15;

    /**
     * 不通知任何类型
     */
    public static final int NOTIFY_NO_ALL = 0;

    /**
     * 发送之前通知
     */
    public static final int NOTIFY_BEFORE_SEND = 1;

    /**
     * 响应之前通知
     */
    public static final int NOTIFY_BEFORE_RESPONSE = 2;

    /**
     * 响应完成通知
     */
    public static final int NOTIFY_COMPOLE_RESPONSE = 4;

    /**
     * 通知所有类型
     */
    public static final int NOTIFY_ALL = 7;

    static {
//        // 获得系统属性集
//        Properties props = System.getProperties();
//
//        if (props.getProperty("os.arch").contains("64")) {
//            // 64位操作系统
//            System.loadLibrary("FiddlerJNI_64");
//        } else {
//            // 32 bit 操作系统
//            System.loadLibrary("FiddlerJNI_86");
//        }


        System.load("../FiddlerJNI/x64/Release/FiddlerJNI.dll");
    }


    private static native boolean start0();

    private static native boolean close0();

    private static native boolean setCallBack0(final FiddlerCallBack callBack);

    private static native boolean createAndTrustRootCert0();

    private static native boolean removeCert0(final boolean bRemoveRoot);

    private static native void writeLog0(int level, String str);

    private static native FiddlerConfig getFiddlerConfig0();

    private static native boolean setFiddlerConfig0(FiddlerConfig data);

    private static native boolean exportRootCert0(String fileName);

    private static native boolean modifyFiddlerData0(FiddlerData data);

    public static boolean start() {
        return start0();
    }

    public static boolean close() {
        return close0();
    }

    public static boolean setCallBack(final FiddlerCallBack callBack) {
        return setCallBack0(callBack);
    }

    public static boolean createAndTrustRootCert() {
        return createAndTrustRootCert0();
    }

    public static boolean removeCert(final boolean bRemoveRoot) {
        return removeCert0(bRemoveRoot);
    }

    public static void writeLog(int level, String str) {
        writeLog0(level, str);
    }

    public static FiddlerConfig getFiddlerConfig() {
        return getFiddlerConfig0();
    }

    public static boolean setFiddlerConfig(FiddlerConfig data) {
        return setFiddlerConfig0(data);
    }

    public static boolean exportRootCert(String fileName) {
        return exportRootCert0(fileName);
    }

    public static boolean modifyFiddlerData(FiddlerData data) {
        return modifyFiddlerData0(data);
    }

}
