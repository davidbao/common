#ifndef DRIVERMANAGER_H
#define DRIVERMANAGER_H

#include "thread/Mutex.h"
#include "data/PList.h"
#include "data/StringArray.h"
#include "thread/Locker.h"
#include "DriverDescription.h"
#include "channels/ChannelDescription.h"
#include "channels/Channel.h"
#include "devices/Device.h"
#include "devices/InstructionPool.h"

using namespace Data;

namespace Drivers
{
	class DriverManager
	{
	public:
        DriverManager();
        ~DriverManager();

		void open();
        void openAsync();
        
		void close();
        
        void reset();
        void resetAsync();
        void pause();
        void resume();
		void reopenAll(bool allowConnected = false);

		InstructionContext* executeInstruction(const String& deviceName, InstructionDescription* id);
        
        DriverDescription* description() const;        

		bool hasDevice(const String& deviceName);
        Device* getDevice(const String& deviceName) const;
        Device* getDevice(const Channel* channel) const;
        void getDevices(const Channel* channel, Devices& devices) const;
        Device* getDevice(DeviceDescription* dd) const;
        bool containsDevice(DeviceDescription* dd) const;

        bool hasChannel(ChannelDescription* cd);
        Channel* getChannel(const String& channelName) const;
        Channel* getChannel(ChannelDescription* cd) const;
        
        void addPool(const InstructionPool* ip);
        InstructionPool* getPool(const String& deviceName) const;
        InstructionPool* getPoolByChannelName(const String& channelName) const;
        InstructionPool* getPoolByDeviceName(const String& deviceName) const;
        void getPools(const StringArray& deviceNames, InstructionPools& ips) const;
        void getPoolsWithoutDevice(const StringArray& deviceNames, InstructionPools& ips) const;
        const InstructionPools* getPools() const;

        bool opened() const;
        
        Delegates* channelOpenedDelegates();
        Delegates* channelClosedDelegates();

	private:
		void createDevices();

        void reopen(const String& channelName = String::Empty, bool allowConnected = false);
        void reopen(Channel* channel, bool allowConnected = false);        
        
    private:
//        static void resetAction(ThreadHolder* holder);

        static void channelOpened(void* owner, void* sender, EventArgs* args);
        static void channelClosed(void* owner, void* sender, EventArgs* args);

	private:
		DriverDescription* _description;
		Mutex _mutex;
		Devices* _devices;
		Channels* _channels;
		InstructionPools* _pools;

		bool _opened;
        
        Delegates _channelOpenedDelegates;
        Delegates _channelClosedDelegates;
	};
}
#endif // DRIVERMANAGER_H
