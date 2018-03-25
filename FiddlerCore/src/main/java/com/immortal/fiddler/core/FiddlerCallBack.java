package com.immortal.fiddler.core;

/**
 * Fiddler 消息回调处理
 *
 * @author Immortal
 * @version V1.0
 * @since 2018-02-09
 */
public interface FiddlerCallBack {
    /**
     * 回调通知
     *
     * @param data 回调数据
     * @return
     */
    boolean callBack(final FiddlerData data);
}
