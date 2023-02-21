//
//  Locker.h
//  common
//
//  Created by baowei on 2018/12/8.
//  Copyright (c) 2018 com. All rights reserved.
//

#ifndef Locker_h
#define Locker_h

namespace Threading {
    class IMutex;

    class Locker {
    public:
        Locker(IMutex *m);

        ~Locker();

    private:
        IMutex *_mutex;
    };
}

#endif // Locker_h
