//
//  ServerInstruction.h
//  common
//
//  Created by baowei on 2016/9/24.
//  Copyright (c) 2016 com. All rights reserved.
//

#ifndef ServerInstruction_h
#define ServerInstruction_h

#include "ClientInstruction.h"
#include "system/Delegate.h"

using namespace Diag;

namespace Communication {
    class ServerInstruction : public ClientInstruction {
    public:
        class ValueChangingEventArgs : public HandledEventArgs {
        public:
            ValueChangingEventArgs(MemoryStream *stream, ClientContext *context) {
                this->stream = stream;
                this->context = context;
            }

            ~ValueChangingEventArgs() override {
                this->stream = nullptr;
                this->context = nullptr;
            }

            MemoryStream *stream;
            ClientContext *context;
        };

        ServerInstruction(InstructionDescription *id);

        ~ServerInstruction() override;

        ClientContext *setReceiveBuffer(MemoryStream &ms, ClientContext *context, bool checkFrameId = true) override;

        virtual ClientContext *setValue(ClientContext *context) {
            return context;
        }

    protected:
        InstructionContext *executeSync(Interactive *interactive, Device *device, InstructionContext *context,
                                        const ByteArray *buffer) override;
    };

    template<class T, class K>
    class ServerElementInstruction : public ServerInstruction {
    public:
        ServerElementInstruction(InstructionDescription *id) : ServerInstruction(id) {
        }

        ~ServerElementInstruction() override {
        }

        bool getCommandBuffer(MemoryStream &ms, ClientContext *context) override {
            ElementContext<T, K> *rcontext = dynamic_cast<ElementContext<T, K> *>(context);
            if (rcontext != nullptr) {
                ms.writeByte(command());
                writeVersion(&ms);

                // send the elements.
                rcontext->outputData()->write(&ms);

                return true;
            }
            return false;
        }

        bool setCommandBuffer(MemoryStream &ms, ClientContext *context) override {
            ElementContext<T, K> *rcontext = dynamic_cast<ElementContext<T, K> *>(context);
            if (rcontext != nullptr) {
                ms.readByte();    // skip command
                readVersion(&ms);

                rcontext->inputData()->read(&ms);

                return true;
            }
            return false;
        }

    protected:
        bool autoResponsed() const override {
            return true;
        }
    };

    template<class T>
    class ServerElementAInstruction : public ServerElementInstruction<T, EmptyContext> {
    public:
        ServerElementAInstruction(InstructionDescription *id) : ServerElementInstruction<T, EmptyContext>(id) {
        }

        ~ServerElementAInstruction() override {
        }

    protected:
        bool autoResponsed() const override {
            return false;
        }
    };

    template<class T>
    class ServerElementSInstruction : public ServerElementInstruction<T, StatusContext> {
    public:
        ServerElementSInstruction(InstructionDescription *id) : ServerElementInstruction<T, StatusContext>(id) {
        }

        ~ServerElementSInstruction() override {
        }

    protected:
        bool autoResponsed() const override {
            return true;
        }
    };

    // T is input data, K is output data, P is item of K.
    template<class T, class K, class P>
    class ServerDownloadPacketInstruction : public ServerInstruction {
    public:
        ServerDownloadPacketInstruction(InstructionDescription *id) : ServerInstruction(id) {
        }

        ~ServerDownloadPacketInstruction() override {
        }

        bool getCommandBuffer(MemoryStream &ms, ClientContext *context) override {
            PacketContext<T, K> *rcontext = dynamic_cast<PacketContext<T, K> *>(context);
            if (rcontext != nullptr) {
                ms.writeByte(command());
                writeVersion(&ms);

                uint8_t transfer = rcontext->transfer();
                assert(transfer == BasePacketContext::TransferHeader ||
                       transfer == BasePacketContext::TransferData);
                ms.writeByte(transfer);
                if (transfer == BasePacketContext::TransferHeader) {
                    uint32_t packetCount = this->calcPacketCount(rcontext);
                    if (packetCount == 0) {
                        ms.writeByte(PacketContext<T, K>::PacketNotFound);
                    } else {
                        ms.writeByte(StatusContext::Succeed);
                        rcontext->setPacketCount(packetCount);
                        ms.writeUInt32(packetCount);
                    }
                } else if (transfer == BasePacketContext::TransferData) {
                    // send the elements.
                    uint32_t packetNo = rcontext->getPacketNo();
                    if (packetNo >= 0 && packetNo < this->_ranges.count()) {
                        ms.writeBoolean(true);
                        K *k = this->_ranges.at(packetNo);
                        k->write(&ms);
                    } else {
                        ms.writeBoolean(false);
                    }
                } else {
                    return false;
                }

                return true;
            }
            return false;
        }

        bool setCommandBuffer(MemoryStream &ms, ClientContext *context) override {
            PacketContext<T, K> *rcontext = dynamic_cast<PacketContext<T, K> *>(context);
            if (rcontext != nullptr) {
                ms.readByte();    // skip command
                readVersion(&ms);
                setVersion(ms.version());

                uint8_t transfer = ms.readByte();
                assert(transfer == BasePacketContext::TransferHeader ||
                       transfer == BasePacketContext::TransferData);
                rcontext->setTransfer(transfer);
                if (transfer == BasePacketContext::TransferHeader) {
                    rcontext->inputData()->read(&ms);
                    rcontext->setPacketLength(ms.readUInt32());
                    return true;
                } else if (transfer == BasePacketContext::TransferData) {
                    rcontext->setPacketNo(ms.readUInt32());
                    return true;
                }
                return false;
            }
            return false;
        }

    protected:
        bool autoResponsed() const override {
            return true;
        }

    private:
        uint32_t calcPacketCount(PacketContext<T, K> *context) {
            const K *outputData = context->outputData();
            assert(outputData);
            MemoryStream ms;
            ms.setVersion(version());
            int count = 0;
            _ranges.clear();
            for (uint32_t i = 0; i < outputData->count(); i++) {
                P *p = outputData->at(i);
                p->write(&ms);
                if (ms.length() > context->packetLength()) {
                    count++;
                    // full, so seek the zero position, and write the last one.
                    ms.clear();
                    p->write(&ms);
                }
                K *k;
                if (_ranges.count() == (uint32_t) count) {
                    k = new K(context->autoDelete());
                    k->copyContextFrom(outputData);
                    _ranges.add(k);
                } else {
                    k = _ranges.at(count);
                }
                if (context->autoDelete()) {
                    P *np = new P();
                    np->copyFrom(p);
                    k->add(np);
                } else {
                    k->add(p);
                }
            }
            return _ranges.count();
        }

        typedef PList<K> Ranges;

        Ranges _ranges;
    };

    // T is input data, K is output data.
    template<class T, class K>
    class ServerUploadPacketInstruction : public ServerInstruction {
    public:
        ServerUploadPacketInstruction(InstructionDescription *id, bool syncSendReceive = true) : ServerInstruction(id) {
        }

        ~ServerUploadPacketInstruction() override {
        }

        bool getCommandBuffer(MemoryStream &ms, ClientContext *context) override {
            PacketContext<T, K> *rcontext = dynamic_cast<PacketContext<T, K> *>(context);
            if (rcontext != nullptr) {
                ms.writeByte(command());
                writeVersion(&ms);

                uint8_t transfer = rcontext->transfer();
                assert(transfer == BasePacketContext::TransferHeader ||
                       transfer == BasePacketContext::TransferData);
                if (!(transfer == BasePacketContext::TransferHeader ||
                      transfer == BasePacketContext::TransferData)) {
                    return false;
                }
                ms.writeByte(transfer);
                ms.writeByte(rcontext->status);
                if (rcontext->status == StatusContext::Succeed) {
                    rcontext->outputData()->write(&ms);
                }

                return true;
            }
            return false;
        }

        bool setCommandBuffer(MemoryStream &ms, ClientContext *context) override {
            PacketContext<T, K> *rcontext = dynamic_cast<PacketContext<T, K> *>(context);
            if (rcontext != nullptr) {
                ms.readByte();    // skip command
                readVersion(&ms);

                uint8_t transfer = ms.readByte();
                assert(transfer == BasePacketContext::TransferHeader ||
                       transfer == BasePacketContext::TransferData);
                rcontext->setTransfer(transfer);
                if (transfer == BasePacketContext::TransferHeader) {
                    rcontext->setPacketCount(ms.readUInt32());
                    rcontext->setCount(ms.readUInt32());
                    return true;
                } else if (transfer == BasePacketContext::TransferData) {
                    rcontext->inputData()->clear();
                    rcontext->inputData()->read(&ms);
                    return true;
                }
                return false;
            }
            return false;
        }

    protected:
        bool autoResponsed() const override {
            return _syncSendReceive;
        }

    private:
        bool _syncSendReceive;
    };

    // T is input data, K is EmptyContext.
    template<class T>
    class ServerUploadPacketEInstruction : public ServerUploadPacketInstruction<T, EmptyContext> {
    public:
        ServerUploadPacketEInstruction(InstructionDescription *id, bool syncSendReceive = true)
                : ServerUploadPacketInstruction<T, EmptyContext>(id, syncSendReceive) {
        }

        ~ServerUploadPacketEInstruction() override {
        }
    };

    // T is input data.
    template<class T>
    class ServerDownloadFileInstruction : public ServerInstruction, public FileInstruction<T> {
    public:
        ServerDownloadFileInstruction(InstructionDescription *id) : ServerInstruction(id) {
        }

        ~ServerDownloadFileInstruction() override {
        }

        bool getCommandBuffer(MemoryStream &ms, ClientContext *context) override {
            FileContext<T> *rcontext = dynamic_cast<FileContext<T> *>(context);
            if (rcontext != nullptr) {
                ms.writeByte(command());
                writeVersion(&ms);

                uint8_t transfer = rcontext->transfer();
                assert(transfer == BasePacketContext::TransferHeader ||
                       transfer == BasePacketContext::TransferData);
                ms.writeByte(transfer);
                if (transfer == BasePacketContext::TransferHeader) {
                    ms.writeByte(rcontext->status);
                    if (rcontext->status == FileContext<T>::Succeed) {
                        uint32_t packetCount = this->calcPacketCount(rcontext);
                        if (packetCount == 0) {
                            ms.writeByte(FileContext<T>::FileNotFound);
                        } else {
                            Trace::writeFormatLine("Incoming file download, name: %s",
                                                   rcontext->header()->file_name.c_str());
                            rcontext->setPacketCount(packetCount);
                            ms.writeUInt32(packetCount);
                            rcontext->header()->write(&ms);
                        }
                    }

                    return true;
                } else if (transfer == BasePacketContext::TransferData) {
                    // send the elements.
                    if (this->readFile(rcontext, &ms)) {
                        return true;
                    }
                    return false;
                }
                return false;
            }
            return false;
        }

        bool setCommandBuffer(MemoryStream &ms, ClientContext *context) override {
            FileContext<T> *rcontext = dynamic_cast<FileContext<T> *>(context);
            if (rcontext != nullptr) {
                ms.readByte();    // skip command
                readVersion(&ms);

                uint8_t transfer = ms.readByte();
                assert(transfer == BasePacketContext::TransferHeader ||
                       transfer == BasePacketContext::TransferData);
                rcontext->setTransfer(transfer);
                if (transfer == BasePacketContext::TransferHeader) {
                    rcontext->inputData()->read(&ms);
                    rcontext->setPacketLength(ms.readUInt32());
                    return true;
                } else if (transfer == BasePacketContext::TransferData) {
                    rcontext->header()->read(&ms);
                    rcontext->setPacketNo(ms.readUInt32());
                    return true;
                }
                return false;
            }
            return false;
        }

    protected:
        bool autoResponsed() const override {
            return true;
        }

        bool allowLogMessage() const override {
            return false;
        }
    };

    // T is input data.
    template<class T>
    class ServerUploadFileInstruction : public ServerInstruction, public FileInstruction<T> {
    public:
        ServerUploadFileInstruction(InstructionDescription *id, bool syncSendReceive = true) : ServerInstruction(id) {
            _syncSendReceive = syncSendReceive;
        }

        ~ServerUploadFileInstruction() override {
        }

        bool getCommandBuffer(MemoryStream &ms, ClientContext *context) override {
            FileContext<T> *rcontext = dynamic_cast<FileContext<T> *>(context);
            if (rcontext != nullptr) {
                ms.writeByte(command());
                writeVersion(&ms);

                uint8_t transfer = rcontext->transfer();
                assert(transfer == BasePacketContext::TransferHeader ||
                       transfer == BasePacketContext::TransferData ||
                       transfer == BasePacketContext::TransferStatus);
                if (!(transfer == BasePacketContext::TransferHeader ||
                      transfer == BasePacketContext::TransferData ||
                      transfer == BasePacketContext::TransferStatus)) {
                    return false;
                }
                ms.writeByte(transfer);
                ms.writeByte(rcontext->status);

                return true;
            }
            return false;
        }

        bool setCommandBuffer(MemoryStream &ms, ClientContext *context) override {
            FileContext<T> *rcontext = dynamic_cast<FileContext<T> *>(context);
            if (rcontext != nullptr) {
                ms.readByte();    // skip command
                readVersion(&ms);

                uint8_t transfer = ms.readByte();
                assert(transfer == BasePacketContext::TransferHeader ||
                       transfer == BasePacketContext::TransferData ||
                       transfer == BasePacketContext::TransferStatus);
                rcontext->setTransfer(transfer);
                if (transfer == BasePacketContext::TransferHeader) {
                    rcontext->inputData()->read(&ms);
                    rcontext->header()->read(&ms);
                    return true;
                } else if (transfer == BasePacketContext::TransferData) {
                    rcontext->outputData()->clear();
                    rcontext->outputData()->read(&ms);
                    return this->saveFile(rcontext);
                } else if (transfer == BasePacketContext::TransferStatus) {
                    rcontext->status = ms.readByte();
                    rcontext->inputData()->read(&ms);
                    return true;
                }
                return false;
            }
            return false;
        }

    protected:
        bool autoResponsed() const override {
            return _syncSendReceive;
        }

    private:
        bool _syncSendReceive;
    };
}
#endif // ServerInstruction_h
