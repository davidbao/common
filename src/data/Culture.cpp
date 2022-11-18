//
//  Culture.cpp
//  common
//
//  Created by baowei on 15/8/29.
//  Copyright (c) 2015 com. All rights reserved.
//

#include "data/Culture.h"
#include "diag/Trace.h"
#include <clocale>

namespace Common {
    NumberFormatInfo::NumberFormatInfo() = default;

    NumberFormatInfo::~NumberFormatInfo() = default;

    const NumberFormatInfo *NumberFormatInfo::getFormat(const char *typeName) const {
        if (String(typeName) == typeid(NumberFormatInfo).name()) {
            return this;
        }
        return nullptr;
    }

    const NumberFormatInfo *NumberFormatInfo::getInstance(const IFormatProvider<NumberFormatInfo> *provider) {
        const auto *culture = dynamic_cast<const Culture *>(provider);
        if (culture != nullptr) {
            return &culture->numberFormatInfo();
        }
        const NumberFormatInfo *numberFormat = nullptr;
        numberFormat = dynamic_cast<const NumberFormatInfo *>(provider);
        if (numberFormat != nullptr) {
            return numberFormat;
        }
        if (provider != nullptr) {
            numberFormat = provider->getFormat("NumberFormatInfo");
            if (numberFormat != nullptr) {
                return numberFormat;
            }
        }
        return &NumberFormatInfo::currentInfo();
    }

    const NumberFormatInfo &NumberFormatInfo::currentInfo() {
        return Culture::currentCulture().numberFormatInfo();
    }

    Culture::Data::Data(uint32_t lcid, const String &name, const String &region, const String &language,
                        uint32_t ansiPageCode,
                        uint32_t oemPageCode) {
        this->lcid = lcid;
        this->name = name;
        this->region = region;
        this->language = language;
        this->ansiPageCode = ansiPageCode;
        this->oemPageCode = oemPageCode;
    }

    Culture::Data::Data(const Data &data) {
        this->operator=(data);
    }

    Culture::Data::~Data() = default;

    bool Culture::Data::equals(const Culture::Data &other) const {
        return this->lcid == other.lcid &&
               this->name == other.name &&
               this->region == other.region &&
               this->language == other.language &&
               this->ansiPageCode == other.ansiPageCode &&
               this->oemPageCode == other.oemPageCode;
    }

    void Culture::Data::evaluates(const Culture::Data &other) {
        this->lcid = other.lcid;
        this->name = other.name;
        this->region = other.region;
        this->language = other.language;
        this->ansiPageCode = other.ansiPageCode;
        this->oemPageCode = other.oemPageCode;
    }

    Culture::Data &Culture::Data::operator=(const Data &value) {
        evaluates(value);
        return *this;
    }

    const Culture::Data *Culture::AllCultures[AllCulturesCount] = {
            new Culture::Data(0x0036, "af", "Afrikaans", "Afrikaans", 1252, 850),
            new Culture::Data(0x0436, "af-ZA", "Afrikaans (South Africa)", "Afrikaans", 1252, 850),
            new Culture::Data(0x001C, "sq", "Albanian", "Albanian", 1250, 852),
            new Culture::Data(0x041C, "sq-AL", "Albanian (Albania)", "Albanian", 1250, 852),
            new Culture::Data(0x0484, "gsw-FR", "Alsatian (France)", "Alsatian", 1252, 850),
            new Culture::Data(0x045E, "am-ET", "Amharic (Ethiopia)", "Amharic", 0, 1),
            new Culture::Data(0x0001, "ar", "Arabic?", "Arabic", 1256, 720),
            new Culture::Data(0x1401, "ar-DZ", "Arabic (Algeria)?", "Arabic", 1256, 720),
            new Culture::Data(0x3C01, "ar-BH", "Arabic (Bahrain)?", "Arabic", 1256, 720),
            new Culture::Data(0x0C01, "ar-EG", "Arabic (Egypt)?", "Arabic", 1256, 720),
            new Culture::Data(0x0801, "ar-IQ", "Arabic (Iraq)?", "Arabic", 1256, 720),
            new Culture::Data(0x2C01, "ar-JO", "Arabic (Jordan)?", "Arabic", 1256, 720),
            new Culture::Data(0x3401, "ar-KW", "Arabic (Kuwait)?", "Arabic", 1256, 720),
            new Culture::Data(0x3001, "ar-LB", "Arabic (Lebanon)?", "Arabic", 1256, 720),
            new Culture::Data(0x1001, "ar-LY", "Arabic (Libya)?", "Arabic", 1256, 720),
            new Culture::Data(0x1801, "ar-MA", "Arabic (Morocco)?", "Arabic", 1256, 720),
            new Culture::Data(0x2001, "ar-OM", "Arabic (Oman)?", "Arabic", 1256, 720),
            new Culture::Data(0x4001, "ar-QA", "Arabic (Qatar)?", "Arabic", 1256, 720),
            new Culture::Data(0x0401, "ar-SA", "Arabic (Saudi Arabia)?", "Arabic", 1256, 720),
            new Culture::Data(0x2801, "ar-SY", "Arabic (Syria)?", "Arabic", 1256, 720),
            new Culture::Data(0x1C01, "ar-TN", "Arabic (Tunisia)?", "Arabic", 1256, 720),
            new Culture::Data(0x3801, "ar-AE", "Arabic (U.A.E.)?", "Arabic", 1256, 720),
            new Culture::Data(0x2401, "ar-YE", "Arabic (Yemen)?", "Arabic", 1256, 720),
            new Culture::Data(0x002B, "hy", "Armenian", "Armenian", 0, 1),
            new Culture::Data(0x042B, "hy-AM", "Armenian (Armenia)", "Armenian", 0, 1),
            new Culture::Data(0x044D, "as-IN", "Assamese (India)", "Assamese", 0, 1),
            new Culture::Data(0x002C, "az", "Azeri", "Azeri (Latin)", 1254, 857),
            new Culture::Data(0x082C, "az-Cyrl-AZ", "Azeri (Cyrillic, Azerbaijan)", "Azeri (Cyrillic)", 1251,
                              866),
            new Culture::Data(0x042C, "az-Latn-AZ", "Azeri (Latin, Azerbaijan)", "Azeri (Latin)", 1254, 857),
            new Culture::Data(0x046D, "ba-RU", "Bashkir (Russia)", "Bashkir", 1251, 866),
            new Culture::Data(0x002D, "eu", "Basque", "Basque", 1252, 850),
            new Culture::Data(0x042D, "eu-ES", "Basque (Basque)", "Basque", 1252, 850),
            new Culture::Data(0x0023, "be", "Belarusian", "Belarusian", 1251, 866),
            new Culture::Data(0x0423, "be-BY", "Belarusian (Belarus)", "Belarusian", 1251, 866),
            new Culture::Data(0x0845, "bn-BD", "Bengali (Bangladesh)", "Bengali", 0, 1),
            new Culture::Data(0x0445, "bn-IN", "Bengali (India)", "Bengali", 0, 1),
            new Culture::Data(0x201A, "bs-Cyrl-BA", "Bosnian (Cyrillic, Bosnia and Herzegovina)",
                              "Bosnian (Cyrillic)", 1251, 855),
            new Culture::Data(0x141A, "bs-Latn-BA", "Bosnian (Latin, Bosnia and Herzegovina)",
                              "Bosnian (Latin)", 1250, 852),
            new Culture::Data(0x047E, "br-FR", "Breton (France)", "Breton", 1252, 850),
            new Culture::Data(0x0002, "bg", "Bulgarian", "Bulgarian", 1251, 866),
            new Culture::Data(0x0402, "bg-BG", "Bulgarian (Bulgaria)", "Bulgarian", 1251, 866),
            new Culture::Data(0x0003, "ca", "Catalan", "Catalan", 1252, 850),
            new Culture::Data(0x0403, "ca-ES", "Catalan (Catalan)", "Catalan", 1252, 850),
            new Culture::Data(0x0C04, "zh-HK", "Chinese (Hong Kong S.A.R.)", "Chinese", 950, 950),
            new Culture::Data(0x1404, "zh-MO", "Chinese (Macao S.A.R.)", "Chinese", 950, 950),
            new Culture::Data(0x0804, "zh-CN", "Chinese (People's Republic of China)", "Chinese", 936, 936),
            new Culture::Data(0x0004, "zh-Hans", "Chinese (Simplified)", "Chinese", 936, 936),
            new Culture::Data(0x1004, "zh-SG", "Chinese (Singapore)", "Chinese", 936, 936),
            new Culture::Data(0x0404, "zh-TW", "Chinese (Taiwan)", "Chinese", 950, 950),
            new Culture::Data(0x7C04, "zh-Hant", "Chinese (Traditional)", "Chinese", 950, 950),
            new Culture::Data(0x0483, "co-FR", "Corsican (France)", "Corsican", 1252, 850),
            new Culture::Data(0x001A, "hr", "Croatian", "Croatian", 1250, 852),
            new Culture::Data(0x041A, "hr-HR", "Croatian (Croatia)", "Croatian", 1250, 852),
            new Culture::Data(0x101A, "hr-BA", "Croatian (Latin, Bosnia and Herzegovina)", "Croatian (Latin)",
                              1250, 852),
            new Culture::Data(0x0005, "cs", "Czech", "Czech", 1250, 852),
            new Culture::Data(0x0405, "cs-CZ", "Czech (Czech Republic)", "Czech", 1250, 852),
            new Culture::Data(0x0006, "da", "Danish", "Danish", 1252, 850),
            new Culture::Data(0x0406, "da-DK", "Danish (Denmark)", "Danish", 1252, 850),
            new Culture::Data(0x048C, "prs-AF", "Dari (Afghanistan)", "Dari", 1256, 720),
            new Culture::Data(0x0065, "div", "Divehi?", "Divehi", 0, 1),
            new Culture::Data(0x0465, "div-MV", "Divehi (Maldives)?", "Divehi", 0, 1),
            new Culture::Data(0x0013, "nl", "Dutch", "Dutch", 1252, 850),
            new Culture::Data(0x0813, "nl-BE", "Dutch (Belgium)", "Dutch", 1252, 850),
            new Culture::Data(0x0413, "nl-NL", "Dutch (Netherlands)", "Dutch", 1252, 850),
            new Culture::Data(0x0009, "en", "English", "English", 1252, 437),
            new Culture::Data(0x0C09, "en-AU", "English (Australia)", "English", 1252, 850),
            new Culture::Data(0x2809, "en-BZ", "English (Belize)", "English", 1252, 850),
            new Culture::Data(0x1009, "en-CA", "English (Canada)", "English", 1252, 850),
            new Culture::Data(0x2409, "en-029", "English (Caribbean)", "English", 1252, 850),
            new Culture::Data(0x4009, "en-IN", "English (India)", "English", 1252, 437),
            new Culture::Data(0x1809, "en-IE", "English (Ireland)", "English", 1252, 850),
            new Culture::Data(0x2009, "en-JM", "English (Jamaica)", "English", 1252, 850),
            new Culture::Data(0x4409, "en-MY", "English (Malaysia)", "English", 1252, 437),
            new Culture::Data(0x1409, "en-NZ", "English (New Zealand)", "English", 1252, 850),
            new Culture::Data(0x3409, "en-PH", "English (Republic of the Philippines)", "English", 1252, 437),
            new Culture::Data(0x4809, "en-SG", "English (Singapore)", "English", 1252, 437),
            new Culture::Data(0x1C09, "en-ZA", "English (South Africa)", "English", 1252, 437),
            new Culture::Data(0x2C09, "en-TT", "English (Trinidad and Tobago)", "English", 1252, 850),
            new Culture::Data(0x0809, "en-GB", "English (United Kingdom)", "English", 1252, 850),
            new Culture::Data(0x0409, "en-US", "English (United States)", "English", 1252, 437),
            new Culture::Data(0x3009, "en-ZW", "English (Zimbabwe)", "English", 1252, 437),
            new Culture::Data(0x0025, "et", "Estonian", "Estonian", 1257, 775),
            new Culture::Data(0x0425, "et-EE", "Estonian (Estonia)", "Estonian", 1257, 775),
            new Culture::Data(0x0038, "fo", "Faroese", "Faroese", 1252, 850),
            new Culture::Data(0x0438, "fo-FO", "Faroese (Faroe Islands)", "Faroese", 1252, 850),
            new Culture::Data(0x0464, "fil-PH", "Filipino (Philippines)", "Filipino", 1252, 437),
            new Culture::Data(0x000B, "fi", "Finnish", "Finnish", 1252, 850),
            new Culture::Data(0x040B, "fi-FI", "Finnish (Finland)", "Finnish", 1252, 850),
            new Culture::Data(0x000C, "fr", "French", "French", 1252, 850),
            new Culture::Data(0x080C, "fr-BE", "French (Belgium)", "French", 1252, 850),
            new Culture::Data(0x0C0C, "fr-CA", "French (Canada)", "French", 1252, 850),
            new Culture::Data(0x040C, "fr-FR", "French (France)", "French", 1252, 850),
            new Culture::Data(0x140C, "fr-LU", "French (Luxembourg)", "French", 1252, 850),
            new Culture::Data(0x180C, "fr-MC", "French (Principality of Monaco)", "French", 1252, 850),
            new Culture::Data(0x100C, "fr-CH", "French (Switzerland)", "French", 1252, 850),
            new Culture::Data(0x0462, "fy-NL", "Frisian (Netherlands)", "Frisian", 1252, 850),
            new Culture::Data(0x0056, "gl", "Galician", "Galician", 1252, 850),
            new Culture::Data(0x0456, "gl-ES", "Galician (Galician)", "Galician", 1252, 850),
            new Culture::Data(0x0037, "ka", "Georgian", "Georgian", 0, 1),
            new Culture::Data(0x0437, "ka-GE", "Georgian (Georgia)", "Georgian", 0, 1),
            new Culture::Data(0x0007, "de", "German", "German", 1252, 850),
            new Culture::Data(0x0C07, "de-AT", "German (Austria)", "German", 1252, 850),
            new Culture::Data(0x0407, "de-DE", "German (Germany)", "German", 1252, 850),
            new Culture::Data(0x1407, "de-LI", "German (Liechtenstein)", "German", 1252, 850),
            new Culture::Data(0x1007, "de-LU", "German (Luxembourg)", "German", 1252, 850),
            new Culture::Data(0x0807, "de-CH", "German (Switzerland)", "German", 1252, 850),
            new Culture::Data(0x0008, "el", "Greek", "Greek", 1253, 737),
            new Culture::Data(0x0408, "el-GR", "Greek (Greece)", "Greek", 1253, 737),
            new Culture::Data(0x046F, "kl-GL", "Greenlandic (Greenland)", "Greenlandic", 1252, 850),
            new Culture::Data(0x0047, "gu", "Gujarati", "Gujarati", 0, 1),
            new Culture::Data(0x0447, "gu-IN", "Gujarati (India)", "Gujarati", 0, 1),
            new Culture::Data(0x0468, "ha-Latn-NG", "Hausa (Latin, Nigeria)", "Hausa (Latin)", 1252, 437),
            new Culture::Data(0x000D, "he", "Hebrew?", "Hebrew", 1255, 862),
            new Culture::Data(0x040D, "he-IL", "Hebrew (Israel)?", "Hebrew", 1255, 862),
            new Culture::Data(0x0039, "hi", "Hindi", "Hindi", 0, 1),
            new Culture::Data(0x0439, "hi-IN", "Hindi (India)", "Hindi", 0, 1),
            new Culture::Data(0x000E, "hu", "Hungarian", "Hungarian", 1250, 852),
            new Culture::Data(0x040E, "hu-HU", "Hungarian (Hungary)", "Hungarian", 1250, 852),
            new Culture::Data(0x000F, "is", "Icelandic", "Icelandic", 1252, 850),
            new Culture::Data(0x040F, "is-IS", "Icelandic (Iceland)", "Icelandic", 1252, 850),
            new Culture::Data(0x0470, "ig-NG", "Igbo (Nigeria)", "Igbo", 1252, 437),
            new Culture::Data(0x0021, "id", "Indonesian", "Indonesian", 1252, 850),
            new Culture::Data(0x0421, "id-ID", "Indonesian (Indonesia)", "Indonesian", 1252, 850),
            new Culture::Data(0x085D, "iu-Latn-CA", "Inuktitut (Latin, Canada)", "Inuktitut (Latin)", 1252,
                              437),
            new Culture::Data(0x045D, "iu-Cans-CA", "Inuktitut (Syllabics, Canada)", "Inuktitut", 0, 1),
            new Culture::Data(0x083C, "ga-IE", "Irish (Ireland)", "Irish", 1252, 850),
            new Culture::Data(0x0434, "xh-ZA", "isiXhosa (South Africa)", "isiXhosa", 1252, 850),
            new Culture::Data(0x0435, "zu-ZA", "isiZulu (South Africa)", "isiZulu", 1252, 850),
            new Culture::Data(0x0010, "it", "Italian", "Italian", 1252, 850),
            new Culture::Data(0x0410, "it-IT", "Italian (Italy)", "Italian", 1252, 850),
            new Culture::Data(0x0810, "it-CH", "Italian (Switzerland)", "Italian", 1252, 850),
            new Culture::Data(0x0011, "ja", "Japanese", "Japanese", 932, 932),
            new Culture::Data(0x0411, "ja-JP", "Japanese (Japan)", "Japanese", 932, 932),
            new Culture::Data(0x004B, "kn", "Kannada", "Kannada", 0, 1),
            new Culture::Data(0x044B, "kn-IN", "Kannada (India)", "Kannada", 0, 1),
            new Culture::Data(0x003F, "kk", "Kazakh", "Kazakh", 1251, 866),
            new Culture::Data(0x043F, "kk-KZ", "Kazakh (Kazakhstan)", "Kazakh", 1251, 866),
            new Culture::Data(0x0453, "km-KH", "Khmer (Cambodia)", "Khmer", 0, 1),
            new Culture::Data(0x0486, "qut-GT", "K'iche (Guatemala)", "K'iche", 1252, 850),
            new Culture::Data(0x0487, "rw-RW", "Kinyarwanda (Rwanda)", "Kinyarwanda", 1252, 437),
            new Culture::Data(0x0041, "sw", "Kiswahili", "Kiswahili", 1252, 437),
            new Culture::Data(0x0441, "sw-KE", "Kiswahili (Kenya)", "Kiswahili", 1252, 437),
            new Culture::Data(0x0057, "kok", "Konkani", "Konkani", 0, 1),
            new Culture::Data(0x0457, "kok-IN", "Konkani (India)", "Konkani", 0, 1),
            new Culture::Data(0x0012, "ko", "Korean", "Korean", 949, 949),
            new Culture::Data(0x0412, "ko-KR", "Korean (Korea)", "Korean", 949, 949),
            new Culture::Data(0x0040, "ky", "Kyrgyz", "Kyrgyz", 1251, 866),
            new Culture::Data(0x0440, "ky-KG", "Kyrgyz (Kyrgyzstan)", "Kyrgyz", 1251, 866),
            new Culture::Data(0x0454, "lo-LA", "Lao (Lao P.D.R.)", "Lao", 0, 1),
            new Culture::Data(0x0026, "lv", "Latvian", "Latvian", 1257, 775),
            new Culture::Data(0x0426, "lv-LV", "Latvian (Latvia)", "Latvian", 1257, 775),
            new Culture::Data(0x0027, "lt", "Lithuanian", "Lithuanian", 1257, 775),
            new Culture::Data(0x0427, "lt-LT", "Lithuanian (Lithuania)", "Lithuanian", 1257, 775),
            new Culture::Data(0x082E, "wee-DE", "Lower Sorbian (Germany)", "Lower Sorbian", 1252, 850),
            new Culture::Data(0x046E, "lb-LU", "Luxembourgish (Luxembourg)", "Luxembourgish", 1252, 850),
            new Culture::Data(0x002F, "mk", "Macedonian", "Macedonian (FYROM)", 1251, 866),
            new Culture::Data(0x042F, "mk-MK", "Macedonian (Former Yugoslav Republic of Macedonia)",
                              "Macedonian (FYROM)", 1251, 866),
            new Culture::Data(0x003E, "ms", "Malay", "Malay", 1252, 850),
            new Culture::Data(0x083E, "ms-BN", "Malay (Brunei Darussalam)", "Malay", 1252, 850),
            new Culture::Data(0x043E, "ms-MY", "Malay (Malaysia)", "Malay", 1252, 850),
            new Culture::Data(0x044C, "ml-IN", "Malayalam (India)", "Malayalam", 0, 1),
            new Culture::Data(0x043A, "mt-MT", "Maltese (Malta)", "Maltese", 0, 1),
            new Culture::Data(0x0481, "mi-NZ", "Maori (New Zealand)", "Maori", 0, 1),
            new Culture::Data(0x047A, "arn-CL", "Mapudungun (Chile)", "Mapudungun", 1252, 850),
            new Culture::Data(0x004E, "mr", "Marathi", "Marathi", 0, 1),
            new Culture::Data(0x044E, "mr-IN", "Marathi (India)", "Marathi", 0, 1),
            new Culture::Data(0x047C, "moh-CA", "Mohawk (Mohawk)", "Mohawk", 1252, 850),
            new Culture::Data(0x0050, "mn", "Mongolian", "Mongolian (Cyrillic)", 1251, 866),
            new Culture::Data(0x0450, "mn-MN", "Mongolian (Cyrillic, Mongolia)", "Mongolian (Cyrillic)", 1251,
                              866),
            new Culture::Data(0x0850, "mn-Mong-CN", "Mongolian (Traditional Mongolian, PRC)",
                              "Mongolian (Traditional Mongolian)", 0, 1),
            new Culture::Data(0x0461, "ne-NP", "Nepali (Nepal)", "Nepali", 0, 1),
            new Culture::Data(0x0014, "no", "Norwegian", "Norwegian (Bokm?l)", 1252, 850),
            new Culture::Data(0x0414, "nb-NO", "Norwegian, Bokm?l (Norway)", "Norwegian (Bokm?l)", 1252, 850),
            new Culture::Data(0x0814, "nn-NO", "Norwegian, Nynorsk (Norway)", "Norwegian (Nynorsk)", 1252, 850),
            new Culture::Data(0x0482, "oc-FR", "Occitan (France)", "Occitan", 1252, 850),
            new Culture::Data(0x0448, "or-IN", "Oriya (India)", "Oriya", 0, 1),
            new Culture::Data(0x0463, "ps-AF", "Pashto (Afghanistan)", "Pashto", 0, 1),
            new Culture::Data(0x0029, "fa", "Persian?", "Persian", 1256, 720),
            new Culture::Data(0x0429, "fa-IR", "Persian?", "Persian", 1256, 720),
            new Culture::Data(0x0015, "pl", "Polish", "Polish", 1250, 852),
            new Culture::Data(0x0415, "pl-PL", "Polish (Poland)", "Polish", 1250, 852),
            new Culture::Data(0x0016, "pt", "Portuguese", "Portuguese", 1252, 850),
            new Culture::Data(0x0416, "pt-BR", "Portuguese (Brazil)", "Portuguese", 1252, 850),
            new Culture::Data(0x0816, "pt-PT", "Portuguese (Portugal)", "Portuguese", 1252, 850),
            new Culture::Data(0x0046, "pa", "Punjabi", "Punjabi", 0, 1),
            new Culture::Data(0x0446, "pa-IN", "Punjabi (India)", "Punjabi", 0, 1),
            new Culture::Data(0x046B, "quz-BO", "Quechua (Bolivia)", "Quechua", 1252, 850),
            new Culture::Data(0x086B, "quz-EC", "Quechua (Ecuador)", "Quechua", 1252, 850),
            new Culture::Data(0x0C6B, "quz-PE", "Quechua (Peru)", "Quechua", 1252, 850),
            new Culture::Data(0x0018, "ro", "Romanian", "Romanian", 1250, 852),
            new Culture::Data(0x0418, "ro-RO", "Romanian (Romania)", "Romanian", 1250, 852),
            new Culture::Data(0x0417, "rm-CH", "Romansh (Switzerland)", "Romansh", 1252, 850),
            new Culture::Data(0x0019, "ru", "Russian", "Russian", 1251, 866),
            new Culture::Data(0x0419, "ru-RU", "Russian (Russia)", "Russian", 1251, 866),
            new Culture::Data(0x243B, "smn-FI", "Sami, Inari (Finland)", "Sami (Inari)", 1252, 850),
            new Culture::Data(0x103B, "smj-NO", "Sami, Lule (Norway)", "Sami (Lule)", 1252, 850),
            new Culture::Data(0x143B, "smj-SE", "Sami, Lule (Sweden)", "Sami (Lule)", 1252, 850),
            new Culture::Data(0x0C3B, "se-FI", "Sami, Northern (Finland)", "Sami (Northern)", 1252, 850),
            new Culture::Data(0x043B, "se-NO", "Sami, Northern (Norway)", "Sami (Northern)", 1252, 850),
            new Culture::Data(0x083B, "se-SE", "Sami, Northern (Sweden)", "Sami (Northern)", 1252, 850),
            new Culture::Data(0x203B, "sms-FI", "Sami, Skolt (Finland)", "Sami (Skolt)", 1252, 850),
            new Culture::Data(0x183B, "sma-NO", "Sami, Southern (Norway)", "Sami (Southern)", 1252, 850),
            new Culture::Data(0x1C3B, "sma-SE", "Sami, Southern (Sweden)", "Sami (Southern)", 1252, 850),
            new Culture::Data(0x004F, "sa", "Sanskrit", "Sanskrit", 0, 1),
            new Culture::Data(0x044F, "sa-IN", "Sanskrit (India)", "Sanskrit", 0, 1),
            new Culture::Data(0x7C1A, "sr", "Serbian", "Serbian (Latin)", 1251, 855),
            new Culture::Data(0x1C1A, "sr-Cyrl-BA", "Serbian (Cyrillic, Bosnia and Herzegovina)",
                              "Serbian (Cyrillic)", 1251, 855),
            new Culture::Data(0x0C1A, "sr-Cyrl-SP", "Serbian (Cyrillic, Serbia)", "Serbian (Cyrillic)", 1251,
                              855),
            new Culture::Data(0x181A, "sr-Latn-BA", "Serbian (Latin, Bosnia and Herzegovina)",
                              "Serbian (Latin)", 1250, 852),
            new Culture::Data(0x081A, "sr-Latn-SP", "Serbian (Latin, Serbia)", "Serbian (Latin)", 1250, 852),
            new Culture::Data(0x046C, "nso-ZA", "Sesotho sa Leboa (South Africa)", "Sesotho sa Leboa", 1252,
                              850),
            new Culture::Data(0x0432, "tn-ZA", "Setswana (South Africa)", "Setswana", 1252, 850),
            new Culture::Data(0x045B, "si-LK", "Sinhala (Sri Lanka)", "Sinhala", 0, 1),
            new Culture::Data(0x001B, "sk", "Slovak", "Slovak", 1250, 852),
            new Culture::Data(0x041B, "sk-SK", "Slovak (Slovakia)", "Slovak", 1250, 852),
            new Culture::Data(0x0024, "sl", "Slovenian", "Slovenian", 1250, 852),
            new Culture::Data(0x0424, "sl-SI", "Slovenian (Slovenia)", "Slovenian", 1250, 852),
            new Culture::Data(0x000A, "es", "Spanish", "Spanish", 1252, 850),
            new Culture::Data(0x2C0A, "es-AR", "Spanish (Argentina)", "Spanish", 1252, 850),
            new Culture::Data(0x400A, "es-BO", "Spanish (Bolivia)", "Spanish", 1252, 850),
            new Culture::Data(0x340A, "es-CL", "Spanish (Chile)", "Spanish", 1252, 850),
            new Culture::Data(0x240A, "es-CO", "Spanish (Colombia)", "Spanish", 1252, 850),
            new Culture::Data(0x140A, "es-CR", "Spanish (Costa Rica)", "Spanish", 1252, 850),
            new Culture::Data(0x1C0A, "es-DO", "Spanish (Dominican Republic)", "Spanish", 1252, 850),
            new Culture::Data(0x300A, "es-EC", "Spanish (Ecuador)", "Spanish", 1252, 850),
            new Culture::Data(0x440A, "es-SV", "Spanish (El Salvador)", "Spanish", 1252, 850),
            new Culture::Data(0x100A, "es-GT", "Spanish (Guatemala)", "Spanish", 1252, 850),
            new Culture::Data(0x480A, "es-HN", "Spanish (Honduras)", "Spanish", 1252, 850),
            new Culture::Data(0x080A, "es-MX", "Spanish (Mexico)", "Spanish", 1252, 850),
            new Culture::Data(0x4C0A, "es-NI", "Spanish (Nicaragua)", "Spanish", 1252, 850),
            new Culture::Data(0x180A, "es-PA", "Spanish (Panama)", "Spanish", 1252, 850),
            new Culture::Data(0x3C0A, "es-PY", "Spanish (Paraguay)", "Spanish", 1252, 850),
            new Culture::Data(0x280A, "es-PE", "Spanish (Peru)", "Spanish", 1252, 850),
            new Culture::Data(0x500A, "es-PR", "Spanish (Puerto Rico)", "Spanish", 1252, 850),
            new Culture::Data(0x0C0A, "es-ES", "Spanish (Spain)", "Spanish", 1252, 850),
            new Culture::Data(0x540A, "es-US", "Spanish (United States)", "Spanish", 1252, 850),
            new Culture::Data(0x380A, "es-UY", "Spanish (Uruguay)", "Spanish", 1252, 850),
            new Culture::Data(0x200A, "es-VE", "Spanish (Venezuela)", "Spanish", 1252, 850),
            new Culture::Data(0x001D, "sv", "Swedish", "Swedish", 1252, 850),
            new Culture::Data(0x081D, "sv-FI", "Swedish (Finland)", "Swedish", 1252, 850),
            new Culture::Data(0x041D, "sv-SE", "Swedish (Sweden)", "Swedish", 1252, 850),
            new Culture::Data(0x005A, "syr", "Syriac?", "Syriac", 0, 1),
            new Culture::Data(0x045A, "syr-SY", "Syriac (Syria)?", "Syriac", 0, 1),
            new Culture::Data(0x0428, "tg-Cyrl-TJ", "Tajik (Cyrillic, Tajikistan)", "Tajik (Cyrillic)", 1251,
                              866),
            new Culture::Data(0x085F, "tzm-Latn-DZ", "Tamazight (Latin, Algeria)", "Tamazight (Latin)", 1252,
                              850),
            new Culture::Data(0x0049, "ta", "Tamil", "Tamil", 0, 1),
            new Culture::Data(0x0449, "ta-IN", "Tamil (India)", "Tamil", 0, 1),
            new Culture::Data(0x0044, "tt", "Tatar", "Tatar", 1251, 866),
            new Culture::Data(0x0444, "tt-RU", "Tatar (Russia)", "Tatar", 1251, 866),
            new Culture::Data(0x004A, "te", "Telugu", "Telugu", 0, 1),
            new Culture::Data(0x044A, "te-IN", "Telugu (India)", "Telugu", 0, 1),
            new Culture::Data(0x001E, "th", "Thai", "Thai", 874, 874),
            new Culture::Data(0x041E, "th-TH", "Thai (Thailand)", "Thai", 874, 874),
            new Culture::Data(0x0451, "bo-CN", "Tibetan (PRC)", "Tibetan", 0, 1),
            new Culture::Data(0x001F, "tr", "Turkish", "Turkish", 1254, 857),
            new Culture::Data(0x041F, "tr-TR", "Turkish (Turkey)", "Turkish", 1254, 857),
            new Culture::Data(0x0442, "tk-TM", "Turkmen (Turkmenistan)", "Turkmen", 1250, 852),
            new Culture::Data(0x0480, "ug-CN", "Uighur (PRC)", "Uighur", 1256, 720),
            new Culture::Data(0x0022, "uk", "Ukrainian", "Ukrainian", 1251, 866),
            new Culture::Data(0x0422, "uk-UA", "Ukrainian (Ukraine)", "Ukrainian", 1251, 866),
            new Culture::Data(0x042E, "wen-DE", "Upper Sorbian (Germany)", "Upper Sorbian", 1252, 850),
            new Culture::Data(0x0020, "ur", "Urdu?", "Urdu", 1256, 720),
            new Culture::Data(0x0420, "ur-PK", "Urdu (Islamic Republic of Pakistan)?", "Urdu", 1256, 720),
            new Culture::Data(0x0043, "uz", "Uzbek", "Uzbek (Latin)", 1254, 857),
            new Culture::Data(0x0843, "uz-Cyrl-UZ", "Uzbek (Cyrillic, Uzbekistan)", "Uzbek (Cyrillic)", 1251,
                              866),
            new Culture::Data(0x0443, "uz-Latn-UZ", "Uzbek (Latin, Uzbekistan)", "Uzbek (Latin)", 1254, 857),
            new Culture::Data(0x002A, "vi", "Vietnamese", "Vietnamese", 1258, 1258),
            new Culture::Data(0x042A, "vi-VN", "Vietnamese (Vietnam)", "Vietnamese", 1258, 1258),
            new Culture::Data(0x0452, "cy-GB", "Welsh (United Kingdom)", "Welsh", 1252, 850),
            new Culture::Data(0x0488, "wo-SN", "Wolof (Senegal)", "Wolof", 1252, 850),
            new Culture::Data(0x0485, "sah-RU", "Yakut (Russia)", "Yakut", 1251, 866),
            new Culture::Data(0x0478, "ii-CN", "Yi (PRC)", "Yi", 0, 1),
            new Culture::Data(0x046A, "yo-NG", "Yoruba (Nigeria)", "Yoruba", 1252, 437)
    };

    Culture Culture::_current;

    Culture::Culture() = default;

    Culture::Culture(const String &name) {
        for (auto data: AllCultures) {
            if (String::equals(data->name, name, true)) {
                _data = *data;
                updateNumberFormatInfo(_data.name);
                break;
            }
        }
    }

    Culture::Culture(uint32_t lcid) {
        for (auto data: AllCultures) {
            if (data->lcid == lcid) {
                _data = *data;
                updateNumberFormatInfo(_data.name);
                break;
            }
        }
    }

    Culture::Culture(const Culture &culture) {
        _data = culture._data;
        _numberFormat = culture._numberFormat;
    }

    Culture::~Culture() = default;

    bool Culture::equals(const Culture &other) const {
        return _data == other._data;
    }

    void Culture::evaluates(const Culture &other) {
        _data = other._data;
    }

    const NumberFormatInfo *Culture::getFormat(const char *typeName) const {
        if (typeName == typeid(NumberFormatInfo).name()) {
            return &_numberFormat;
        }
        return nullptr;
    }

    const Culture &Culture::currentCulture() {
        return _current;
    }

    const String &Culture::name() const {
        return _data.name;
    }

    uint32_t Culture::lcid() const {
        return _data.lcid;
    }

    const String &Culture::region() const {
        return _data.region;
    }

    const String &Culture::language() const {
        return _data.language;
    }

    uint32_t Culture::ansiPageCode() const {
        return _data.ansiPageCode;
    }

    uint32_t Culture::oemPageCode() const {
        return _data.oemPageCode;
    }

    const NumberFormatInfo &Culture::numberFormatInfo() const {
        return _numberFormat;
    }

    Culture &Culture::operator=(const Culture &value) {
        evaluates(value);
        return *this;
    }

    bool Culture::isChinese() const {
        return lcid() == 0x0C04 ||
               lcid() == 0x1404 ||
               lcid() == 0x0804 ||
               lcid() == 0x0004 ||
               lcid() == 0x1004 ||
               lcid() == 0x0404 ||
               lcid() == 0x7C04;
    }

    void Culture::updateNumberFormatInfo(const String &cultureName) {
        if (cultureName.isNullOrEmpty()) {
            return;
        }

        String localeName = String::format("%s.UTF-8", String::replace(cultureName, "-", "_").c_str());
        String oldLocaleName = setlocale(LC_ALL, nullptr);
        char *result = setlocale(LC_ALL, localeName);
        updateNumberFormatInfo(&_numberFormat);
        setlocale(LC_ALL, oldLocaleName);
    }

    void Culture::updateNumberFormatInfo(NumberFormatInfo *info) {
        struct lconv *lc = localeconv();
        if (lc->currency_symbol[0] != 0) {
            info->currencyDecimalSeparator = lc->mon_decimal_point;
            info->currencyGroupSeparator = lc->mon_thousands_sep;
            info->currencySymbol = lc->currency_symbol;
            info->negativeSign = lc->negative_sign;
            info->numberDecimalSeparator = lc->decimal_point;
            info->numberGroupSeparator = lc->thousands_sep;
            info->percentDecimalSeparator = lc->decimal_point;
            info->percentGroupSeparator = lc->thousands_sep;
            info->positiveSign = lc->positive_sign;
            if (lc->p_cs_precedes == 1) {
                if (lc->p_sep_by_space == 0) {
                    info->currencyPositivePattern = 0;  // $n
                } else {
                    info->currencyPositivePattern = 2;  // $ n
                }
            } else {
                if (lc->p_sep_by_space == 0) {
                    info->currencyPositivePattern = 1;  // n$
                } else {
                    info->currencyPositivePattern = 3;  // n $
                }
            }
            if (lc->n_sign_posn ==
                0) { // parentheses around the value and the currency symbol are used to represent the sign
                if (lc->n_cs_precedes == 1) {    // 1 if currency_symbol is placed before negative value, 0 if after
                    if (lc->n_sep_by_space == 0) {
                        info->currencyNegativePattern = 0;  // ($n)
                    } else {
                        info->currencyNegativePattern = 14;  // ($ n)
                    }
                } else {
                    if (lc->n_sep_by_space == 0) {
                        info->currencyNegativePattern = 4;  // (n$)
                    } else {
                        info->currencyNegativePattern = 15;  // (n $)
                    }
                }
            } else if (lc->n_sign_posn == 1) {   // sign before the value and the currency symbol
                if (lc->n_cs_precedes == 1) {
                    if (lc->n_sep_by_space == 0) {
                        info->currencyNegativePattern = 1;  // -$n
                    } else {
                        info->currencyNegativePattern = 9;  // -$ n
                    }
                } else {
                    if (lc->n_sep_by_space == 0) {
                        info->currencyNegativePattern = 5;  // -n$
                    } else {
                        info->currencyNegativePattern = 8;  // -n $
                    }
                }
            } else if (lc->n_sign_posn == 2) {   // sign after the value and the currency symbol
                if (lc->n_cs_precedes == 1) {
                    if (lc->n_sep_by_space == 0) {
                        info->currencyNegativePattern = 3;  // $n-
                    } else {
                        info->currencyNegativePattern = 11;  // $ n-
                    }
                } else {
                    if (lc->n_sep_by_space == 0) {
                        info->currencyNegativePattern = 7;  // n$-
                    } else {
                        info->currencyNegativePattern = 10;  // n $-
                    }
                }
            } else if (lc->n_sign_posn == 3) {   // sign before the currency symbol
                if (lc->n_cs_precedes == 1) {
                    if (lc->n_sep_by_space == 0) {
                        info->currencyNegativePattern = 6;  // n-$
                    } else {
                        info->currencyNegativePattern = 13;  // n- $
                    }
                } else {
                    if (lc->n_sep_by_space == 0) {
                        info->currencyNegativePattern = 6;  // n-$
                    } else {
                        info->currencyNegativePattern = 13;  // n- $
                    }
                }
            } else if (lc->n_sign_posn == 4) {   // sign after the currency symbol
                if (lc->n_cs_precedes == 1) {
                    if (lc->n_sep_by_space == 0) {
                        info->currencyNegativePattern = 2;  // $-n
                    } else {
                        info->currencyNegativePattern = 12;  // $-n
                    }
                } else {
                    if (lc->n_sep_by_space == 0) {
                        info->currencyNegativePattern = 2;  // $-n
                    } else {
                        info->currencyNegativePattern = 12;  //$- n
                    }
                }
            }
        }
    }
}
