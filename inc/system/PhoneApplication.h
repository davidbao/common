//
//  PhoneApplication.h
//  common
//
//  Created by baowei on 2015/10/27.
//  Copyright © 2015 com. All rights reserved.
//

#ifndef PhoneApplication_h
#define PhoneApplication_h

#include "Application.h"

namespace Common
{
    class PhoneApplication : public Application
    {
    public:
        typedef StringMap Arguments;
        
        enum NetStatus
        {
            Unknown = 0,
            NotReachable = 1,
            ViaWiFi = 2,
            ViaWWAN = 3
        };
        class NetworkChangedEventArgs : public EventArgs
        {
        public:
            NetworkChangedEventArgs(NetStatus oldStatus, NetStatus newStatus)
            {
                this->oldStatus = oldStatus;
                this->newStatus = newStatus;
            }
            
            NetStatus oldStatus;
            NetStatus newStatus;
        };
        
        PhoneApplication();
        ~PhoneApplication() override;
        
        static PhoneApplication* instance();
        
        void onCreate(const String& rootPath);
        void onDestroy();
        
        void onActived();
        void onInactived();
        void onActiving();
        void onInactiving();
        
        void onNetworkChanged(NetStatus status);
        
        void addEventCreate(const Delegate& delegate);
        void removeEventCreate(const Delegate& delegate);
        void addEventActived(const Delegate& delegate);
        void removeEventActived(const Delegate& delegate);
        void addEventInactived(const Delegate& delegate);
        void removeEventInactived(const Delegate& delegate);
        
        void addEventNetworkChanged(const Delegate& delegate);
        void removeEventNetworkChanged(const Delegate& delegate);
        
        const String& phoneModel() const;
        void setPhoneModel(const String& phoneModel);
        const PositionCoord& coord() const;
        void setCoord(const PositionCoord& coord);
        
        const Arguments& arguments() const;
        
        void addArgument(const String& key, const String& value);
        
    private:
        enum EventType
        {
            EventCreate = 0,
            EventDestroy,
            EventActived,
            EventActiving,
            EventInactived,
            EventInactiving,
            EventNetworkChanged
        };
        
    private:
        void addEvent(EventType type, const Delegate& delegate);
        void removeEvent(EventType type, const Delegate& delegate);
        void invokeEvent(EventType type, EventArgs* args = nullptr);
        
        void initLog(const String& rootPath, const String& logPath = "logs");
        
        static const String toNetStatusStr(NetStatus status);
        
    private:
        static const int EventCount = 7;
        Delegates _delegates[EventCount];
        Mutex _delegatesMutex[EventCount];
        
        bool _actived;
        bool _activing;
        
        NetStatus _netStatus;
        
        String _phoneModel;
        PositionCoord _coord;
        
        Arguments _arguments;
    };
}

#endif // PhoneApplication_h
