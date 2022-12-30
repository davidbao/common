#ifndef PARALLELPORT_H
#define PARALLELPORT_H

#include "IOPort.h"

namespace IO {
    class ParallelPort : public IOPort {
    public:
        ParallelPort(const String &name);

        ~ParallelPort() override;
    };
}
#endif  //PARALLELPORT_H
