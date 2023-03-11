//
//  Clipboard.cpp
//  common
//
//  Created by baowei on 2016/12/27.
//  Copyright (c) 2016 com. All rights reserved.
//

#include "system/Clipboard.h"
#include "diag/Trace.h"

#ifdef MAC_OS

#include <ApplicationServices/ApplicationServices.h>

#elif WIN_OS

#include <Windows.h>

#elif ANDROID_OS

#include <malloc.h>

#endif

using namespace Diag;

namespace System {
    void *Clipboard::_value = nullptr;
    String Clipboard::_type;
    const String Clipboard::TextType = "common.clipboard.text";
    const String Clipboard::ImageType = "common.clipboard.image";

    Clipboard::Clipboard(bool inner) : _inner(inner), _clipboard(nullptr) {
#ifdef MAC_OS
        PasteboardRef clipboard = nullptr;
        OSStatus err = PasteboardCreate(kPasteboardClipboard, &clipboard);
        if (err != noErr) {
            Debug::writeLine("Failed to create the clipboard.");
        } else {
            _clipboard = clipboard;
        }
#elif WIN_OS
#endif
    }

    Clipboard::~Clipboard() {
        _clipboard = nullptr;
    }

    bool Clipboard::clear() {
        if (_inner) {
            if (_value != nullptr) {
                if (_type == TextType) {
                    auto value = (String *) _value;
                    delete value;
                } else if (_type == ImageType) {
                    auto value = (ByteArray *) _value;
                    delete value;
                } else {
                    free(_value);
                }
                _value = nullptr;
            }
            return true;
        } else {
#ifdef MAC_OS
            OSStatus err = PasteboardClear((PasteboardRef) _clipboard);
            if (err != noErr) {
                Debug::writeLine("Failed to empty the clipboard.");
                return false;
            }
            return true;
#elif WIN_OS
            return EmptyClipboard();
#elif BROWSER_OS
            return false;
#else
            return false;
#endif
        }
    }

    bool Clipboard::isText() const {
        if (_inner) {
            return _type == TextType && _value != nullptr;
        }
        return false;
    }

    bool Clipboard::setText(const String &data) {
        if (data.length() == 0)
            return false;

        if (_inner) {
            // we can only store one object.
            clear();

            _type = TextType;
            auto value = new String(data);
            _value = value;
            return true;
        } else {
#ifdef MAC_OS
            // we can only store one object.
            clear();

            PasteboardSyncFlags syncFlags = PasteboardSynchronize((PasteboardRef) _clipboard);
            //        static_assert(!(syncFlags & kPasteboardModified), "clipboard modified after clear");
            //        static_assert(!(syncFlags & kPasteboardClientIsOwner), "client couldn't own clipboard");

            long itemID = 1;
            CFDataRef temp = CFDataCreate(kCFAllocatorDefault, (uint8_t *) data.c_str(), data.length());
            OSStatus err = PasteboardPutItemFlavor((PasteboardRef) _clipboard, (PasteboardItemID) itemID,
                                                   kUTTypeUTF8PlainText, temp, kPasteboardFlavorNoFlags);
            //            PasteboardPutItemFlavor( pasteboard, (PasteboardItemID) itemID,
            //                                    (CFStringRef) thisFormat.GetFormatId() , data, kPasteboardFlavorNoFlags);
            CFRelease(temp);
            if (err != noErr)   // notPasteboardOwnerErr
            {
                Debug::writeLine("Failed to add data to the clipboard.");
                return false;
            }
            return true;
#elif WIN_OS
            if (!OpenClipboard(NULL))
                return false;
            EmptyClipboard();
            
            String str = String::UTF8toGBK(data);
            size_t cch = str.length();
            HGLOBAL hglbCopy = GlobalAlloc(GMEM_MOVEABLE, (cch + 1) * sizeof(char));
            if (hglbCopy == NULL)
            {
                CloseClipboard();
                return false;
            }
            
            // Lock the handle and copy the text to the buffer.
            LPTSTR lptstrCopy = (LPTSTR)GlobalLock(hglbCopy);
            memcpy(lptstrCopy, str.c_str(), cch * sizeof(char));
            lptstrCopy[cch] = (char)0;    // null character
            GlobalUnlock(hglbCopy);
            
            // Place the handle on the clipboard.
            SetClipboardData(CF_TEXT, hglbCopy);
            
            // Close the clipboard.
            return CloseClipboard();
#elif BROWSER_OS
            return false;
#else
            return false;
#endif
        }
    }

    bool Clipboard::getText(String &data) {
        if (_inner) {
            if (!isText())
                return false;

            auto value = (String *) _value;
            data = *value;
            return true;
        } else {
#ifdef MAC_OS
            //        wxWidget/datobj.cpp
            //        m_format = (NativeFormat) CFStringCreateCopy(NULL, (CFStringRef)format);
            //        if ( UTTypeConformsTo( (CFStringRef)format, kUTTypeHTML ) )
            //        {
            //            m_type = wxDF_HTML;
            //        }
            //        if (  UTTypeConformsTo( (CFStringRef)format, kUTTypeUTF16PlainText ) )
            //        {
            //            m_type = wxDF_UNICODETEXT;
            //        }
            //        else if (  UTTypeConformsTo( (CFStringRef)format,kUTTypeUTF16ExternalPlainText ) )
            //        {
            //            m_type = wxDF_UNICODETEXT;
            //        }
            //        else if (  UTTypeConformsTo( (CFStringRef)format,kUTTypeUTF8PlainText ) )
            //        {
            //            m_type = wxDF_UNICODETEXT;
            //        }
            //        else if ( UTTypeConformsTo( (CFStringRef)format, kUTTypePlainText ) )
            //        {
            //            m_type = wxDF_TEXT;
            //        }
            //        else if (  UTTypeConformsTo( (CFStringRef)format, kUTTypeImage ) )
            //        {
            //            m_type = wxDF_BITMAP;
            //        }
            //        else if (  UTTypeConformsTo( (CFStringRef)format, kUTTypePDF ) )
            //        {
            //            m_type = wxDF_METAFILE;
            //        }
            //        else if (  UTTypeConformsTo( (CFStringRef)format, kUTTypeFileURL ) ||
            //                 UTTypeConformsTo( (CFStringRef)format, kPasteboardTypeFileURLPromise))
            //        {
            //            m_type = wxDF_FILENAME;
            //        }
            //        else
            //        {
            //            m_type = wxDF_PRIVATE;
            //            m_id = wxCFStringRef( (CFStringRef) CFRetain((CFStringRef) format )).AsString();
            //        }
            auto pasteboard = (PasteboardRef) _clipboard;

            ItemCount itemCount = 0;
            // we synchronize here once again, so we don't mind which flags get returned
            PasteboardSynchronize(pasteboard);

            OSStatus err = PasteboardGetItemCount(pasteboard, &itemCount);
            if (err == noErr) {
                for (size_t itemIndex = 1; itemIndex <= itemCount; itemIndex++) {
                    PasteboardItemID itemID = nullptr;
                    CFArrayRef flavorTypeArray = nullptr;
                    CFIndex flavorCount = 0;

                    err = PasteboardGetItemIdentifier(pasteboard, itemIndex, &itemID);
                    if (err != noErr)
                        continue;

                    err = PasteboardCopyItemFlavors(pasteboard, itemID, &flavorTypeArray);
                    if (err != noErr)
                        continue;

                    flavorCount = CFArrayGetCount(flavorTypeArray);
                    for (CFIndex flavorIndex = 0; flavorIndex < flavorCount; flavorIndex++) {
                        CFStringRef flavorType;
                        CFDataRef flavorData;
                        CFIndex flavorDataSize;

                        flavorType = (CFStringRef) CFArrayGetValueAtIndex(flavorTypeArray, flavorIndex);
                        if (UTTypeConformsTo(flavorType, kUTTypeUTF8PlainText)) {
                            err = PasteboardCopyItemFlavorData(pasteboard, itemID, flavorType, &flavorData);
                            if (err == noErr) {
                                flavorDataSize = CFDataGetLength(flavorData);
                                // because some data implementation expect trailing a trailing NUL, we add some headroom
                                void *buf = malloc(flavorDataSize + 4);
                                if (buf) {
                                    memset(buf, 0, flavorDataSize + 4);
                                    memcpy(buf, CFDataGetBytePtr(flavorData), flavorDataSize);
                                    data = (char *) buf;

                                    //                            if (dataFormat.GetType() == wxDF_TEXT)
                                    //                                wxMacConvertNewlines10To13( (char*) buf );
                                    //                            SetData( flavorFormat, flavorDataSize, buf );
                                    //                            transferred = true;
                                    free(buf);
                                }
                                CFRelease(flavorData);
                                return true;
                            }
                        }
                    }
                }
            }
            return false;
#elif WIN_OS
            if (!IsClipboardFormatAvailable(CF_TEXT))
                return false;
            if (!OpenClipboard(NULL))
                return false;
            
            HGLOBAL hglb = GetClipboardData(CF_TEXT);
            if (hglb != NULL)
            {
                LPTSTR lptstr = (LPTSTR)GlobalLock(hglb);
                if (lptstr != NULL)
                {
                    data = lptstr;
                    GlobalUnlock(hglb);
                }
            }
            return CloseClipboard();
#elif BROWSER_OS
            return false;
#else
            return false;
#endif
        }
    }

    bool Clipboard::isImage() const {
        if (_inner) {
            return _type == ImageType && _value != nullptr;
        }
        return false;
    }

    bool Clipboard::setImage(const ByteArray &data) {
        if (data.count() == 0)
            return false;

        if (_inner) {
            // we can only store one object.
            clear();

            _type = ImageType;
            auto value = new ByteArray(data);
            _value = value;
            return true;
        } else {
#ifdef MAC_OS
            // we can only store one object.
            clear();

            PasteboardSyncFlags syncFlags = PasteboardSynchronize((PasteboardRef) _clipboard);
            //        static_assert(!(syncFlags & kPasteboardModified), "clipboard modified after clear");
            //        static_assert(!(syncFlags & kPasteboardClientIsOwner), "client couldn't own clipboard");

            long itemID = 1;
            CFDataRef temp = CFDataCreate(kCFAllocatorDefault, (uint8_t *) data.data(), data.count());
            OSStatus err = PasteboardPutItemFlavor((PasteboardRef) _clipboard, (PasteboardItemID) itemID,
                                                   kUTTypePNG, temp, kPasteboardFlavorNoFlags);
            CFRelease(temp);
            if (err != noErr)   // notPasteboardOwnerErr
            {
                Debug::writeLine("Failed to add data to the clipboard.");
                return false;
            }
            return true;
#elif WIN_OS
            return false;
#elif BROWSER_OS
            return false;
#else
            return false;
#endif
        }
    }

    bool Clipboard::getImage(ByteArray &data) {
        if (_inner) {
            if (!isText())
                return false;

            auto value = (ByteArray *) _value;
            data = *value;
            return true;
        } else {
#ifdef MAC_OS
            auto pasteboard = (PasteboardRef) _clipboard;

            ItemCount itemCount = 0;
            // we synchronize here once again, so we don't mind which flags get returned
            PasteboardSynchronize(pasteboard);

            OSStatus err = PasteboardGetItemCount(pasteboard, &itemCount);
            if (err == noErr) {
                for (size_t itemIndex = 1; itemIndex <= itemCount; itemIndex++) {
                    PasteboardItemID itemID = nullptr;
                    CFArrayRef flavorTypeArray = nullptr;
                    CFIndex flavorCount = 0;

                    err = PasteboardGetItemIdentifier(pasteboard, itemIndex, &itemID);
                    if (err != noErr)
                        continue;

                    err = PasteboardCopyItemFlavors(pasteboard, itemID, &flavorTypeArray);
                    if (err != noErr)
                        continue;

                    flavorCount = CFArrayGetCount(flavorTypeArray);
                    for (CFIndex flavorIndex = 0; flavorIndex < flavorCount; flavorIndex++) {
                        CFStringRef flavorType;
                        CFDataRef flavorData;
                        CFIndex flavorDataSize;

                        flavorType = (CFStringRef) CFArrayGetValueAtIndex(flavorTypeArray, flavorIndex);
                        if (UTTypeConformsTo(flavorType, kUTTypeImage)) {
                            err = PasteboardCopyItemFlavorData(pasteboard, itemID, flavorType, &flavorData);
                            if (err == noErr) {
                                flavorDataSize = CFDataGetLength(flavorData);
                                // because some data implementation expect trailing a trailing NUL, we add some headroom
                                void *buf = malloc(flavorDataSize + 4);
                                if (buf) {
                                    memset(buf, 0, flavorDataSize + 4);
                                    memcpy(buf, CFDataGetBytePtr(flavorData), flavorDataSize);
                                    data.addRange((const uint8_t *) buf, flavorDataSize);

                                    free(buf);
                                }
                                CFRelease(flavorData);
                                return true;
                            }
                        }
                    }
                }
            }
            return false;
#elif WIN_OS
            return false;
#elif BROWSER_OS
            return false;
#else
            return false;
#endif
        }
    }

    bool Clipboard::isValue(const String &typeName) const {
        if (_inner) {
            if (typeName.isNullOrEmpty())
                return false;

            if (_type == typeName && _value != nullptr) {
                return true;
            }
            return false;
        } else {
            // todo: maybe can do it.
            return false;
        }
    }

//#ifdef WIN_OS
//    bool Clipboard::setBitmap(void* hbitmap)
//    {
//        if (!OpenClipboard(NULL))
//            return false;
//        EmptyClipboard();
//
//        BITMAP bmp;
//        GetObject((HBITMAP)hbitmap, sizeof(BITMAP), &bmp);
//
//        HDC hdcMem = CreateCompatibleDC((HDC)NULL);
//        HDC hdcSrc = CreateCompatibleDC((HDC)NULL);
//        HBITMAP old = (HBITMAP)::SelectObject(hdcSrc, (HBITMAP)hbitmap);
//        HBITMAP hBitmap = CreateCompatibleBitmap(hdcSrc, bmp.bmWidth, bmp.bmHeight);
//        if (!hBitmap)
//        {
//            SelectObject(hdcSrc, old);
//            DeleteDC(hdcMem);
//            DeleteDC(hdcSrc);
//            return false;
//        }
//
//        HBITMAP old1 = (HBITMAP)SelectObject(hdcMem, hBitmap);
//        BitBlt(hdcMem, 0, 0, bmp.bmWidth, bmp.bmHeight, hdcSrc, 0, 0, SRCCOPY);
//
//        // Select new bitmap out of memory DC
//        SelectObject(hdcMem, old1);
//
//        // Set the data
//        HANDLE handle = ::SetClipboardData(CF_BITMAP, hBitmap);
//        if (!handle)
//        {
//            DWORD dwError = GetLastError();
//            Debug::writeFormatLine("Clipboard::setBitmap error, code: %d", dwError);
//        }
//
//        // Clean up
//        SelectObject(hdcSrc, old);
//        DeleteDC(hdcSrc);
//        DeleteDC(hdcMem);
//
//        return CloseClipboard();
//    }
//    bool Clipboard::getBitmap(void*& hBitmap)
//    {
//        return false;
//    }
//#endif
}
