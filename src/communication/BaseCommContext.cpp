#include "IO/Path.h"
#include "IO/File.h"
#include "IO/Directory.h"
#include "IO/FileStream.h"
#include "crypto/Md5Provider.h"
#include "communication/BaseCommContext.h"

using namespace Crypto;

namespace Communication {
    ClientContext::ClientContext() : _state(0) {
    }

    ClientContext::~ClientContext() {
    }

    const Endpoint &ClientContext::peerEndpoint() const {
        return _peerEndpoint;
    }

    void ClientContext::setPeerEndpoint(const Endpoint &endpoint) {
        _peerEndpoint = endpoint;
    }

    uint8_t ClientContext::state() const {
        return _state;
    }

    void ClientContext::setState(uint8_t state) {
        _state = state;
    }

    StatusContext::StatusContext(uint8_t status) {
        this->status = status;
    }

    StatusContext::StatusContext(const StatusContext &context) {
        this->status = context.status;
    }

    StatusContext::~StatusContext() {
    }

    void StatusContext::write(Stream *stream) const {
        stream->writeByte(status);
    }

    void StatusContext::read(Stream *stream) {
        status = stream->readByte();
    }

    void StatusContext::copyFrom(const StatusContext *value) {
        this->status = value->status;
    }

    bool StatusContext::isSuccessful() const {
        return status == Succeed;
    }

    void StatusContext::operator=(const uint8_t &value) {
        this->status = value;
    }

    void StatusContext::operator=(const StatusContext &value) {
        this->status = value.status;
    }

    bool StatusContext::operator==(const StatusContext &value) const {
        return this->status == value.status;
    }

    bool StatusContext::operator!=(const StatusContext &value) const {
        return !operator==(value);
    }

    BasePacketContext::BasePacketContext() : _packetLength(DefaultPacketLength), _packetCount(0), _packetNo(0),
                                             _transfer(0) {
    }

    BasePacketContext::~BasePacketContext() {
    }

    size_t BasePacketContext::packetCount() const {
        return _packetCount;
    }

    void BasePacketContext::setPacketCount(size_t count) {
        _packetCount = count;
    }

    size_t BasePacketContext::getPacketNo() const {
        return _packetNo;
    }

    void BasePacketContext::setPacketNo(size_t no) {
        _packetNo = no;
    }

    bool BasePacketContext::isLastPacketNo() const {
        return (isTransferHeader() && _packetCount == 0) ||
               (isTransferData() && _packetCount > 0 &&
                _packetNo == _packetCount - 1);
    }

    void BasePacketContext::transferHeader() {
        _transfer = TransferHeader;
    }

    void BasePacketContext::transferData() {
        _transfer = TransferData;
    }

    void BasePacketContext::transferStatus() {
        _transfer = TransferStatus;
    }

    void BasePacketContext::setTransfer(uint8_t transfer) {
        assert(transfer == TransferHeader ||
               transfer == TransferData ||
               transfer == TransferStatus);
        _transfer = transfer;
    }

    uint8_t BasePacketContext::transfer() const {
        return _transfer;
    }

    bool BasePacketContext::isTransferHeader() const {
        return _transfer == TransferHeader;
    }

    bool BasePacketContext::isTransferData() const {
        return _transfer == TransferData;
    }

    bool BasePacketContext::isTransferStatus() const {
        return _transfer == TransferStatus;
    }

    uint32_t BasePacketContext::packetLength() const {
        return _packetLength < MaxPacketLength ? _packetLength : MaxPacketLength;
    }

    void BasePacketContext::setPacketLength(uint32_t packetLength) {
        if (packetLength < MinPacketLength)
            _packetLength = MinPacketLength;
        else if (packetLength > MaxPacketLength)
            _packetLength = MaxPacketLength;
        else {
            _packetLength = packetLength;
        }
    }

    FileHeader::FileHeader() {
        fileLength = 0;
        memset(filemd5, 0, MD5_COUNT);
        packetCount = 0;
        path = Directory::getTempPath();
    }

    FileHeader::~FileHeader() {
    }

    void FileHeader::write(Stream *stream) const {
        stream->writeUInt32(fileLength);
        stream->write(filemd5, 0, MD5_COUNT);
        stream->writeStr(file_name, String::StreamLength1);
        stream->writeUInt32(packetCount);
    }

    void FileHeader::read(Stream *stream) {
        fileLength = stream->readUInt32();
        stream->read(filemd5, 0, MD5_COUNT);
        file_name = stream->readStr(String::StreamLength1);
        packetCount = stream->readUInt32();
    }

    void FileHeader::copyFrom(const FileHeader *obj) {
        fileLength = obj->fileLength;
        memcpy(filemd5, obj->filemd5, MD5_COUNT);
        file_name = obj->file_name;
        packetCount = obj->packetCount;
        path = obj->path;
    }

    bool FileHeader::update() {
        String fileName = fullFileName();
        if (File::exists(fileName)) {
            if (Md5Provider::computeFileHash(fileName, filemd5)) {
                FileStream fs(fileName.c_str(), FileMode::FileOpen, FileAccess::FileRead);
                if (!fs.isOpen())
                    return false;
                fileLength = (uint32_t) fs.length();
            }
            return true;
        }
        return false;
    }

    bool FileHeader::checkmd5() const {
        return checkmd5(fullFileName());
    }

    bool FileHeader::checkmd5(const String &filename) const {
        uint8_t buffer[MD5_COUNT];
        memset(buffer, 0, sizeof(buffer));
        bool result = Md5Provider::computeFileHash(filename, buffer);
        if (result) {
            for (size_t i = 0; i < MD5_COUNT; i++) {
                if (buffer[i] != filemd5[i]) {
                    return false;
                }
            }
            return true;
        }
        return false;
    }

    String FileHeader::md5Str() const {
        // a23ffa1f42606a4f55754648881465e9
        ByteArray array(filemd5, MD5_COUNT);
        return array.toString("%02X", "");
    }

    bool FileHeader::parseMd5Str(const String &file_md5, uint8_t md5[16]) {
        // a23ffa1f42606a4f55754648881465e9
        ByteArray array;
        if (ByteArray::parse(file_md5, array, "") &&
            array.count() == MD5_COUNT) {
            memcpy(md5, array.data(), MD5_COUNT);
            return true;
        }
        return false;
    }

    bool FileHeader::parseMd5Str(const String &file_md5) {
        return parseMd5Str(file_md5, filemd5);
    }

    String FileHeader::fullFileName() const {
        if (path.isNullOrEmpty())
            return String::Empty;
        return Path::combine(path, file_name);
    }

    String FileHeader::tempFullFileName() const {
        if (path.isNullOrEmpty())
            return String::Empty;
        return Path::combine(path, (String) "~" + file_name);
    }

    void FileHeader::clear() {
        fileLength = 0;
        memset(filemd5, 0, MD5_COUNT);
        file_name.empty();
        packetCount = 0;
    }

    FileData::FileData() : packetNo(0) {
    }

    FileData::~FileData() {
    }

    void FileData::write(Stream *stream) const {
        stream->writeUInt32(packetNo);
        stream->writeUInt32((uint32_t) data.count());
        stream->write(data.data(), 0, data.count());
    }

    void FileData::read(Stream *stream) {
        packetNo = stream->readUInt32();
        uint32_t length = stream->readUInt32();
        uint8_t *buffer = new uint8_t[length];
        stream->read(buffer, 0, length);
        data.addRange(buffer, length);
        delete[] buffer;
    }

    void FileData::copyFrom(const FileData *obj, bool append) {
        packetNo = obj->packetNo;
        if (!append) {
            data.clear();
        }
        data.addRange(obj->data);
    }

    FileData *FileData::clone() const {
        FileData *fd = new FileData();
        fd->copyFrom(this);
        return fd;
    }

    bool FileData::isFirstPart() const {
        return packetNo == 0;
    }

    bool FileData::isLastPart(const FileHeader *header) const {
        return packetNo == header->packetCount - 1;
    }

    FileCommInfo::FileCommInfo() {
        packetCount = 0;
        packetNo = 0;
        fileLength = 0;
    }

    FileCommInfo::FileCommInfo(const FileCommInfo &info) {
        evaluates(info);
    }

    bool FileCommInfo::equals(const FileCommInfo &other) const {
        return packetCount == other.packetCount && packetNo == other.packetNo && fileLength == other.fileLength &&
               fileName == other.fileName;
    }

    void FileCommInfo::evaluates(const FileCommInfo &other) {
        packetCount = other.packetCount;
        packetNo = other.packetNo;
        fileLength = other.fileLength;
        fileName = other.fileName;
    }

    bool FileCommInfo::isLastPart() const {
        return packetNo == packetCount - 1;
    }

    bool FileCommInfo::isEmpty() const {
        return packetCount == 0;
    }

    void FileCommInfo::empty() {
        packetCount = 0;
    }

    FileCommEntry::FileCommEntry(filecomm_callback callback, void *owner) : callback(callback), owner(owner) {
    }
}
