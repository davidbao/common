#include "diag/Trace.h"
#include "diag/Trace.h"
#include "IO/Path.h"
#include "IO/Directory.h"
#include "crypto/Md5Provider.h"
#include "driver/channels/Channel.h"
#include "driver/devices/Device.h"
#include "communication/ServerInstruction.h"

namespace Communication
{
    ServerInstruction::ServerInstruction(InstructionDescription* id) : ClientInstruction(id)
    {
    }
    ServerInstruction::~ServerInstruction()
    {
    }
    InstructionContext* ServerInstruction::executeSync(Interactive* interactive, Device* device, InstructionContext* context, const ByteArray* buffer)
    {
        return receive(interactive, device, context, buffer);
    }
    ClientContext* ServerInstruction::setReceiveBuffer(MemoryStream& ms, ClientContext* context, bool checkFrameId)
    {
        ClientContext* result = ClientInstruction::setReceiveBuffer(ms, context, checkFrameId);
        if(result != nullptr)
        {
            return setValue(result);
        }
        return nullptr;
    }
}
