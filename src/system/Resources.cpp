//
//  Resources.h
//  common
//
//  Created by baowei on 2015/7/14.
//  Copyright (c) 2015 com. All rights reserved.
//

#include "system/Resources.h"
#include "system/Application.h"
#include "data/StringArray.h"

namespace System {
    class DefaultStrings {
    public:
        DefaultStrings() {
        }

        virtual ~DefaultStrings() {
        }

        String getString(const String &name) {
            String value;
            _map.at(name, value);
            return value;
        }

        bool findString(const String &name) {
            return _map.contains(name);
        }

    protected:
        void add(const StringMap::KeyValue *kvs) {
            const StringMap::KeyValue *kv = kvs;
            while (kv != nullptr && !kv->key.isNullOrEmpty()) {
#ifdef WIN32
                _map.add(kv->key, String::GBKtoUTF8(kv->value));
#else
                _map.add(kv->key, kv->value);
#endif
                kv++;
            }
        }

    private:
        StringMap _map;
    };

    class DefaultStrings_cn : public DefaultStrings {
    public:
        DefaultStrings_cn() {
            const StringMap::KeyValue kvs[] = {
                    {"Empty", ""},
                    {"Null", "-"},
                    {"Error", "错误"},
                    {"Warning", "警告"},
                    {"Ok", "确定"},
                    {"Cancel", "取消"},
                    {"Yes", "是"},
                    {"No", "否"},
                    {"Prompt", "提示"},

                    {"RemoveLogFilesSuccessfully", "移除日志文件'%s'成功！"},
                    {"FailedToRemoveLogFiles", "移除日志文件'%s'失败！"},

                    {"SocketListenFailed", "监听IP地址'%s'，端口'%d'失败！端口可能被占用。"},
                    {"SocketBindingFailed", "绑定IP地址'%s'，端口'%d'失败！端口可能被占用。"},
                    {"BluetoothListenFailed", "监听蓝牙失败！"},
                    {"BluetoothBindingFailed", "绑定蓝牙失败！"},

                    {"UnableToConnectDevice", "无法连接设备, 设备名称: %s"},
                    {"DeviceFailure", "设备失效, 设备名称: %s"},
                    {"RetryingConnectDevice", "重新尝试连接设备, 设备名称: %s"},

                    {"SocketBindingFailedStr2", "绑定 UDP 端口'%d'失败！端口可能被占用。"},

                    {"NetSendInfo", "send: %s"},
                    {"NetReceivedInfo", "recv: %s"},

                    {"NetStateChanged", "服务端'%s'状态从'%s'变为'%s'."},

                    {"CreateTcpChannelInfo", "创建以太网通道: 名称: %s, IP 地址: %s, 端口: %d"},
                    {"CreateTcpServerChannelInfo", "创建以太网服务通道: 名称: %s, IP 地址: %s, 端口: %d"},

                    {"FailedToSendInstruction", "发送网络指令失败，名称: '%s', 异常: %s"},
                    {"CommunicationException", "通讯异常"},

                    {"Yesterday", "昨天"},
                    {"Monday", "星期一"},
                    {"Tuesday", "星期二"},
                    {"Wednesday", "星期三"},
                    {"Thursday", "星期四"},
                    {"Friday", "星期五"},
                    {"Saturday", "星期六"},
                    {"Sunday", "星期日"},
                    {"Year", "年"},
                    {"Month", "月"},
                    {"Day", "日"},
                    nullptr
            };
            add(kvs);
        }
    };

    class DefaultStrings_en : public DefaultStrings {
    public:
        DefaultStrings_en() {
            const StringMap::KeyValue kvs[] =
                    {
                            {"Empty", ""},
                            {"Null", "-"},
                            {"Error", "Error"},
                            {"Warning", "Warning"},
                            {"Ok", "Ok"},
                            {"Cancel", "Cancel"},
                            {"Yes", "Yes"},
                            {"No", "No"},
                            {"Prompt", "Prompt"},

                            {"RemoveLogFilesSuccessfully", "Remove log file'%s' successfully!"},
                            {"FailedToRemoveLogFiles", "Failed to remove log file'%s'!"},

                            {"SocketListenFailed", "Failed to listen IP address'%s', port'%d'! The port may be used."},
                            {"SocketBindingFailed", "Failed to bind IP address'%s', port'%d'! The port may be used."},
                            {"BluetoothListenFailed", "Failed to listen bluetooth!"},
                            {"BluetoothBindingFailed", "Failed to bind bluetooth!"},

                            {"UnableToConnectDevice", "Unable to connect the device, name: %s"},
                            {"DeviceFailure", "The device is failure, name: %s"},
                            {"RetryingConnectDevice", "Retry to connect the device, name: %s"},

                            {"SocketBindingFailedStr2", "Failed to bind UDP, port'%d'! The port may be used."},

                            {"NetSendInfo", "send: %s"},
                            {"NetReceivedInfo", "recv: %s"},

                            {"NetStateChanged", "The server'%s' status changed, from'%s'to'%s'."},

                            {"CreateTcpChannelInfo", "Create tcp channel: name: %s, IP address: %s, port: %d"},
                            {"CreateTcpServerChannelInfo",
                             "Create tcp server channel: name: %s, IP address: %s, port: %d"},

                            {"FailedToSendInstruction", "Failed to send the instruction, name: '%s', exception: %s"},
                            {"CommunicationException", "Communication exception"},

                            {"Yesterday", "Yesterday"},
                            {"Monday", "Monday"},
                            {"Tuesday", "Tuesday"},
                            {"Wednesday", "Wednesday"},
                            {"Thursday", "Thursday"},
                            {"Friday", "Friday"},
                            {"Saturday", "Saturday"},
                            {"Sunday", "Sunday"},

                            {"Year", "Year"},
                            {"Month", "Month"},
                            {"Day", "Day"},
                            nullptr
                    };
            add(kvs);
        }
    };

    String Resources::getString(const String &name) {
        // TODO: search string from yml file.

        return getStaticString(name);
    }

    String Resources::getStaticString(const String &name) {
        const Culture &culture = Application::instance()->culture();
        bool isChinese = culture.isChinese();
        if (isChinese) {
            static DefaultStrings_cn cn;
            return cn.getString(name);
        } else {
            static DefaultStrings_en en;
            return en.getString(name);
        }
    }
}
