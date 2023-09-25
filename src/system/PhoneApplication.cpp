//
//  PhoneApplication.cpp
//  common
//
//  Created by baowei on 2015/4/18.
//  Copyright (c) 2015 com. All rights reserved.
//

#include <cassert>
#include "system/PhoneApplication.h"
#include "diag/Trace.h"
#include "IO/Path.h"
#include "thread/TickTimeout.h"
#include "diag/FileTraceListener.h"
#include "diag/MemoryTraceListener.h"

namespace System {
    PhoneApplication::PhoneApplication() : _actived(false), _activing(false), _netStatus(Unknown) {
        _instance = this;
        _startTime = TickTimeout::getCurrentTickCount();
    }

    PhoneApplication::~PhoneApplication() = default;

    PhoneApplication *PhoneApplication::instance() {
        if (_instance == nullptr) {
            _instance = new PhoneApplication();
        }
        return dynamic_cast<PhoneApplication *>(_instance);
    }

    void PhoneApplication::onCreate(const String &rootPath) {
        assert(!rootPath.isNullOrEmpty());
        _rootPath = rootPath;

        initLog(_rootPath);

        invokeEvent(EventCreate);

        _actived = true;
    }

    void PhoneApplication::onDestroy() {
        unInitLog();

        invokeEvent(EventDestroy);
    }

    void PhoneApplication::onActived() {
        if (!_actived) {
            Debug::writeLine("The phone is actived.");

            _actived = true;
            invokeEvent(EventActived);
        }
        _activing = true;
    }

    void PhoneApplication::onInactived() {
        if (_actived) {
            Debug::writeLine("The phone is inactived.");

            _actived = false;
            invokeEvent(EventInactived);
        }
    }

    void PhoneApplication::onActiving() {
        if (!_activing) {
            _activing = true;
            invokeEvent(EventActiving);
        }
    }

    void PhoneApplication::onInactiving() {
        if (_activing) {
            _activing = false;
            invokeEvent(EventInactiving);
        }
    }

    void PhoneApplication::onNetworkChanged(NetStatus status) {
        if (_netStatus != status) {
            NetStatus oldStatus = _netStatus;
            _netStatus = status;

            Debug::writeFormatLine("The phone network status is changed, old status: %s, new status: %s",
                                   toNetStatusStr(oldStatus).c_str(), toNetStatusStr(_netStatus).c_str());
            NetworkChangedEventArgs args(oldStatus, _netStatus);
            invokeEvent(EventNetworkChanged, &args);
        }
    }

    String PhoneApplication::toNetStatusStr(NetStatus status) {
        switch (status) {
            case Unknown:
                return "Unknown";
            case NotReachable:
                return "NotReachable";
            case ViaWiFi:
                return "ViaWiFi";
            case ViaWWAN:
                return "ViaWWAN";
            default:
                return "Unknown";
        }
    }

    void PhoneApplication::addEventCreate(const Delegate &delegate) {
        addEvent(EventCreate, delegate);
    }

    void PhoneApplication::removeEventCreate(const Delegate &delegate) {
        removeEvent(EventCreate, delegate);
    }

    void PhoneApplication::addEventActived(const Delegate &delegate) {
        addEvent(EventActived, delegate);
    }

    void PhoneApplication::removeEventActived(const Delegate &delegate) {
        removeEvent(EventActived, delegate);
    }

    void PhoneApplication::addEventInactived(const Delegate &delegate) {
        addEvent(EventInactived, delegate);
    }

    void PhoneApplication::removeEventInactived(const Delegate &delegate) {
        removeEvent(EventInactived, delegate);
    }

    void PhoneApplication::addEventNetworkChanged(const Delegate &delegate) {
        addEvent(EventNetworkChanged, delegate);
    }

    void PhoneApplication::removeEventNetworkChanged(const Delegate &delegate) {
        removeEvent(EventNetworkChanged, delegate);
    }

    void PhoneApplication::addEvent(EventType type, const Delegate &delegate) {
        _delegates[type].add(delegate);
    }

    void PhoneApplication::removeEvent(EventType type, const Delegate &delegate) {
        _delegates[type].remove(delegate);
    }

    void PhoneApplication::invokeEvent(EventType type, EventArgs *args) {
        _delegates[type].invoke(this, args);
    }

    void PhoneApplication::initLog(const String &rootPath, const String &logPath) {
        auto listener = new FileTraceListener(Path::combine(rootPath, logPath));
        _traceListeners.add(listener);
        Trace::addTraceListener(listener);
        Trace::enableConsoleOutput();
    }

    const String &PhoneApplication::phoneModel() const {
        return _phoneModel;
    }

    void PhoneApplication::setPhoneModel(const String &phoneModel) {
        _phoneModel = phoneModel;
    }

    const PositionCoord &PhoneApplication::coord() const {
        return _coord;
    }

    void PhoneApplication::setCoord(const PositionCoord &coord) {
        _coord = coord;
    }
}
