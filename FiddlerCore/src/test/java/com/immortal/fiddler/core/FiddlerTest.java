package com.immortal.fiddler.core;

import static com.immortal.fiddler.core.Fiddler.DEBUG;
import static com.immortal.fiddler.core.Fiddler.MODIFY_BODY;
import static com.immortal.fiddler.core.Fiddler.MODIFY_NOTIFY;
import static com.immortal.fiddler.core.Fiddler.NOTIFY_ALL;
import static com.immortal.fiddler.core.Fiddler.NOTIFY_COMPOLE_RESPONSE;
import static com.immortal.fiddler.core.Fiddler.RECV;
import static com.immortal.fiddler.core.Fiddler.SEND;

/**
 * @author Immortal
 * @version V1.0
 * @since 2018-02-10
 */
public class FiddlerTest {

    public static void main(String[] args) {

        FiddlerConfig fiddlerConfig = Fiddler.getFiddlerConfig();
        System.out.println(fiddlerConfig.iListenPort);
        System.out.println(fiddlerConfig.toString());
        System.out.println("00");


        fiddlerConfig.szLogFileName = "------------------------";
        fiddlerConfig = Fiddler.getFiddlerConfig();
        System.out.println(fiddlerConfig.toString());

        fiddlerConfig.logLevel = DEBUG;
        fiddlerConfig.notifyType = NOTIFY_ALL;
        fiddlerConfig.iListenPort = 1111;
        fiddlerConfig.szRegexProcessId = "-------------";
        Fiddler.setFiddlerConfig(fiddlerConfig);

        fiddlerConfig = Fiddler.getFiddlerConfig();
        System.out.println("set ==================> " + fiddlerConfig);


        System.out.println(Fiddler.start());

        // 所有的http https请求将回调到这里.
        Fiddler.setCallBack(new FiddlerCallBack() {
            @Override
            public boolean callBack(FiddlerData data) {
                // https 建立阶段.不能修改
                if ("CONNECT".equals(data.httpMethod)) {
                    // 以后也不在通知握手包
                    System.out.println("connet " + data.event + " notifytype " + data.notifyType);
                    data.notifyType = NOTIFY_COMPOLE_RESPONSE;
                    data.modifyFlags = MODIFY_NOTIFY;
                    Fiddler.modifyFiddlerData(data);
                    return true;
                }

                if (data.event == SEND) {
                    System.out.println("----------send");
                    data.bModifyResponseBody = true;
                    Fiddler.modifyFiddlerData(data);
                    return true;
                } else if (data.event == RECV) {
                    data.body = "test";
                    System.out.println("----body");
                    data.modifyFlags = MODIFY_BODY;
                    Fiddler.modifyFiddlerData(data);
                    return true;
                }

                //    System.out.println(data.event + " ------  " + data.httpMethod);
                return !false;
            }
         });

        try {
            Thread.sleep(10 * 1000);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }

        System.out.println(Fiddler.close());


    }


}
