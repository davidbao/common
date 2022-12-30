//
//  StreamAccessor.h
//  common
//
//  Created by baowei on 2015/7/20.
//  Copyright © 2015 com. All rights reserved.
//

#ifndef StreamAccessor_h
#define StreamAccessor_h

#include "exception/Exception.h"
#include "IO/Stream.h"
#include "data/Vector.h"
#include "data/PList.h"

using namespace System;

namespace Data {
    class StreamAccessor {
    public:
        StreamAccessor() {
        }

        virtual ~StreamAccessor() {
        }

        virtual void write(Stream *stream) const = 0;

        virtual void read(Stream *stream) = 0;
    };

    template<class type>
    class StreamVector : public CopyPList<type>, public StreamAccessor {
    public:
        StreamVector(bool autoDelete = true, uint32_t capacity = CopyPList<type>::DefaultCapacity) : CopyPList<type>(
                autoDelete, capacity) {
        }

        ~StreamVector() override {
        }

        void write(Stream *stream) const override {
            size_t cl = countLength();
            if (!(cl == 0 || cl == 1 || cl == 2 || cl == 4)) {
                throw ArgumentException("countLength must be equal to 0, 1, 2, 4.");
            }

            size_t c = fixedCount();
            switch (cl) {
                case 0:
                    break;
                case 1:
                    stream->writeByte((uint8_t) c);
                    break;
                case 2:
                    stream->writeUInt16((uint16_t) c);
                    break;
                case 4:
                    stream->writeUInt32((uint32_t) c);
                    break;
                default:
                    break;
            }
            for (size_t i = 0; i < c; i++) {
                const type *value = this->at(i);
                value->write(stream);
            }
        }

        void read(Stream *stream) override {
            size_t cl = countLength();
            if (!(cl == 0 || cl == 1 || cl == 2 || cl == 4)) {
                throw ArgumentException("countLength must be equal to 0, 1, 2, 4.");
            }

            size_t c = fixedCount();
            switch (cl) {
                case 0:
                    break;
                case 1:
                    c = stream->readByte();
                    break;
                case 2:
                    c = stream->readUInt16();
                    break;
                case 4:
                    c = stream->readUInt32();
                    break;
                default:
                    break;
            }
            for (size_t i = 0; i < c; i++) {
                type *value = new type();
                assert(value);
                value->read(stream);
                this->add(value);
            }
        }

    protected:
        virtual size_t countLength() const {
            return 2;
        }

        virtual size_t fixedCount() const {
            return CopyPList<type>::count();
        }
    };
}

#endif // StreamAccessor_h
