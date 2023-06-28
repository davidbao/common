#ifndef INSTRUCTIONSET_H
#define INSTRUCTIONSET_H

#include "data/ByteArray.h"
#include "exception/Exception.h"
#include "Instruction.h"

using namespace Data;

namespace Drivers {
    class Channel;

    class Device;

    class InstructionSet {
    public:
        InstructionSet();

        virtual ~InstructionSet();

        virtual void generateInstructions(Instructions *instructions) = 0;

        virtual bool receive(Device *device, Channel *channel, ByteArray *buffer, int order);

        bool receive(Device* device, Channel* channel, ByteArray* buffer) {
            return receive(device, channel, buffer, 0);
        }

        virtual InstructionSet *clone() const;

        virtual bool recombine(const ByteArray &buffer, PList<ByteArray> &buffers);
    };
}

#endif // INSTRUCTIONSET_H
