//
//  Locker.h
//  common
//
//  Created by baowei on 2018/12/8.
//  Copyright © 2018 com. All rights reserved.
//

#ifndef Locker_h
#define Locker_h

namespace Threading {
    class IMutex;

    class ISyncRoot;

    class Locker {
    public:
        Locker(IMutex *mutexp);

        ~Locker();

    private:
        IMutex *_mutex;
    };
}

#endif // Locker_h
