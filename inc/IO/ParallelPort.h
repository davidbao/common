//
//  ParallelPort.h
//  common
//
//  Created by baowei on 2018/12/10.
//  Copyright (c) 2018 com. All rights reserved.
//

#ifndef ParallelPort_h
#define ParallelPort_h

#include "IOPort.h"

namespace IO {
    class ParallelPort final : public IOPort {
    public:
        explicit ParallelPort(const String &name);

        ~ParallelPort() override;

        const String &name() const override;

    private:
        String _name;
    };
}
#endif  // ParallelPort_h
