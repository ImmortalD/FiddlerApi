package com.immortal.fiddler.core;

/**
 * Fiddler回调数据类型
 *
 * @author Immortal
 * @version V1.0
 * @since 2018-02-09
 */
public class FiddlerData {

    /**
     * id,每一次http请求的唯一标识,在消息回调中不要修改
     */
    public int id;
    /**
     * 回调消息类型 0表示数据发送阶段,1数据接收阶段,2表示数据处理完成阶段.这个阶段的数据不能在修改了,在回调中不要修改
     */
    public int event;
    /**
     * Http请求的进程ID,在回调中不要修改
     */
    public int processId;
    /**
     * 是否要修改Http响应body,如果需要修改需要在回调函数event=SEND(0)的时候把bModifyResponseBody修改为true
     */
    public boolean bModifyResponseBody;
    /**
     * 修改的数据标志取值是`ModifyFlag`的枚举值,需要修改多项的值其取值可以是`MODIFY_BODY|MODIFY_HEADERS`这样的
     */
    public int modifyFlags;
    /**
     * 通知类型
     */
    public int notifyType;
    /**
     * http请求的url
     */
    public String url;
    /**
     * http请求方式,GET POST CONNECT等
     */
    public String httpMethod;
    /**
     * http协议版本,如 HTTP/1.1
     */
    public String httpVersion;
    /**
     * http头信息,enevt为SEND时表示请求头,否则是响应头
     */
    public String header;
    /**
     * http 正文数据
     */
    public String body;

    @Override
    public String toString() {
        final StringBuilder sb = new StringBuilder("FiddlerData{");
        sb.append("id=").append(id);
        sb.append(", event=").append(event);
        sb.append(", processId=").append(processId);
        sb.append(", bModifyResponseBody=").append(bModifyResponseBody);
        sb.append(", modifyFlags=").append(modifyFlags);
        sb.append(", notifyType=").append(notifyType);
        sb.append(", url='").append(url).append('\'');
        sb.append(", httpMethod='").append(httpMethod).append('\'');
        sb.append(", httpVersion='").append(httpVersion).append('\'');
        sb.append(", header='").append(header).append('\'');
        sb.append(", body='").append(body).append('\'');
        sb.append('}');
        return sb.toString();
    }
}
