//
//  CommInstructionSet.h
//  common
//
//  Created by baowei on 2015/8/21.
//  Copyright © 2015 com. All rights reserved.
//

#ifndef CommInstructionSet_h
#define CommInstructionSet_h

#include "driver/instructions/InstructionSet.h"
#include "ServerInstruction.h"

using namespace Common;

namespace Communication
{
    typedef void(*instructions_callback)(void*, Instructions*);
    
    class TcpInstructionSet : public InstructionSet
    {
    public:
        TcpInstructionSet(void* owner, instructions_callback action);
        ~TcpInstructionSet() override;

        void generateInstructions(Instructions* instructions) override;
        
        bool receive(Device* device, Channel* channel, ByteArray* buffer, int order = 0) override;
        
        bool recombine(const ByteArray& buffer, PList<ByteArray>& buffers) override;
        
        InstructionSet* clone() const override
        {
            return new TcpInstructionSet(_owner, _instructionsCallback);
        }
        
    protected:
        bool receiveWS(Device* device, Channel* channel, ByteArray* buffer, int order = 0);
        bool receiveInner(Device* device, Channel* channel, ByteArray* buffer, int order = 0);
        
    protected:
        instructions_callback _instructionsCallback;
        void* _owner;
        
        Mutex _bufferMutex;
        ByteArray _buffer;
    };

	class SerialInstructionSet : public InstructionSet
	{
	public:
		SerialInstructionSet(void* owner, instructions_callback action);
		~SerialInstructionSet() override;

		void generateInstructions(Instructions* instructions) override;

		bool receive(Device* device, Channel* channel, ByteArray* buffer, int order = 0) override;

		InstructionSet* clone() const override
		{
			return new SerialInstructionSet(_owner, _instructionsCallback);
		}

	private:
		instructions_callback _instructionsCallback;
		void* _owner;
	};
    
    class UdpInstructionSet : public InstructionSet
    {
    public:
        UdpInstructionSet(void* owner, instructions_callback action);
        ~UdpInstructionSet() override;
        
        void generateInstructions(Instructions* instructions) override;
        
        bool receive(Device* device, Channel* channel, ByteArray* buffer, int order = 0) override;
        
        InstructionSet* clone() const override
        {
            return new UdpInstructionSet(_owner, _instructionsCallback);
        }
        
    private:
        instructions_callback _instructionsCallback;
        void* _owner;
    };
}
#endif // CommInstructionSet_h
