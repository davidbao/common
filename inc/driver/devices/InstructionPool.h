#ifndef INSTRUCTIONPOOL_H
#define INSTRUCTIONPOOL_H

#include "thread/Mutex.h"
#include "data/LoopPList.h"
#include "thread/Thread.h"
#include "driver/channels/Channel.h"
#include "driver/channels/ChannelDescription.h"
#include "driver/instructions/InstructionDescription.h"
#include "Device.h"
#include "DeviceDescription.h"

using namespace Data;

namespace Drivers
{
    class DriverManager;
	class InstructionPool
	{
	public:
		class Packet : public IComparable<Packet>
		{
		public:
            enum AutoDelete
            {
                None = 0,
                PacketAndInstruction = 1,
                PacketOnly = 2
            };
            
            Packet(InstructionDescription* id, AutoDelete autoDelete = PacketAndInstruction, uint8_t priority = 0);
            Packet(DeviceDescription* dd, InstructionDescription* id, AutoDelete autoDelete = PacketAndInstruction, uint8_t priority = 0);
            ~Packet() override;
            
            DeviceDescription* deviceDescription() const;
            InstructionDescription* getInstruction() const;
            
            void process(InstructionContext* ic = nullptr);
            bool isProcessed() const;
            
            InstructionContext* getInstructionContext() const;
            
            bool needDeleted() const;
            void setAutoDelete();

            int compareTo(const Packet &other) const override;

		private:
            DeviceDescription* _dd;
			InstructionDescription* _id;
			AutoDelete _autoDelete;
            // 0 is the highest priority.
            uint8_t _priority;
            
			bool _processed;
			InstructionContext* _ic;
		};
//		typedef LoopPList<Packet> LoopInstructions;
        class LoopInstructions : public LoopPList<Packet>
        {
        public:
            LoopInstructions(int maxLength = 512, bool autoDelete = true, bool hasPriority = false);
            
            void copyTo(Packet** value);
            
        private:
            bool _hasPriority;
        };

		InstructionPool(DriverManager* dm, ChannelDescription* cd, DeviceDescription* dd, bool hasPriority = false);
		virtual ~InstructionPool();

		virtual void start();
        virtual void start(Channel* channel, Device* device);
		virtual void stop();

        virtual bool channelConnected() const;
        
        Packet* addInstruction(InstructionDescription* id, Packet::AutoDelete autoDelete = Packet::AutoDelete::PacketAndInstruction, uint8_t priority = 0);
        Packet* addTickInstruction(InstructionDescription* id, const TimeSpan& timeout);
        void addInstruction(const InstructionDescriptions& ids, Packet::AutoDelete autoDelete = Packet::AutoDelete::PacketAndInstruction, uint8_t priority = 0);
        
        Packet* addInstruction(DeviceDescription* dd, InstructionDescription* id, Packet::AutoDelete autoDelete = Packet::AutoDelete::PacketAndInstruction, uint8_t priority = 0);
        void addInstruction(DeviceDescription* dd, const InstructionDescriptions& ids, Packet::AutoDelete autoDelete = Packet::AutoDelete::PacketAndInstruction, uint8_t priority = 0);
        
        const String& deviceName() const;
        const String& channelName() const;

		InstructionContext* executeInstructionSync(InstructionDescription* id);
        InstructionContext* executeInstructionSync(DeviceDescription* dd, InstructionDescription* id);

        ChannelContext* context() const;
        ChannelDescription* channelDescription() const;
        
        void pause();
        void resume();
        
        virtual void reset();
        
        Channel* channel() const;
        Device* device() const;
        
        virtual bool containsDevice(const String& deviceName) const;
        
        void errorHandle(bool error);

	protected:
		virtual InstructionContext* executeInstruction(InstructionDescription* id);
        virtual InstructionContext* executeInstruction(DeviceDescription* dd, InstructionDescription* id);
        
        virtual void errorHandle(const DeviceDescription* dd, const InstructionDescription* id, bool error);

		void processInstructions();
        void processPacket(Packet* packet);

		virtual void instructionProcInner();

		Packet* addInstructionInner(InstructionDescription* id, Packet::AutoDelete autoDelete = Packet::AutoDelete::PacketAndInstruction, uint8_t priority = 0);
        void addInstructionInner(const InstructionDescriptions& ids, Packet::AutoDelete autoDelete = Packet::AutoDelete::PacketAndInstruction, uint8_t priority = 0);
        
        Packet* addInstructionInner(DeviceDescription* dd, InstructionDescription* id, Packet::AutoDelete autoDelete = Packet::AutoDelete::PacketAndInstruction, uint8_t priority = 0);
        void addInstructionInner(DeviceDescription* dd, const InstructionDescriptions& ids, Packet::AutoDelete autoDelete = Packet::AutoDelete::PacketAndInstruction, uint8_t priority = 0);
        
        DriverManager* manager();
        
    private:
        static void instructionProc(void* parameter);
        static bool isPacketFinished(void* parameter);
                
    protected:
        Channel* _channel;
        Device* _device;

		LoopInstructions* _instructions;
		Mutex _instructionsMutex;

		DeviceDescription* _dd;
		ChannelDescription* _cd;

		Thread* _instructionThread;

		bool _pause;
        
        DriverManager* _manager;
        
        // key: instruction name, value: tick count.
        Dictionary<String, uint32_t> _nameTicks;
	};
    typedef PList<InstructionPool> InstructionPools;
}
#endif // INSTRUCTIONPOOL_H
