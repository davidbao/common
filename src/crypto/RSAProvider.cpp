//
//  RSAProvider.cpp
//  common
//
//  Created by baowei on 2016/9/4.
//  Copyright © 2016 com. All rights reserved.
//

#include "crypto/RSAProvider.h"
#include "diag/Trace.h"
#include <openssl/rsa.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/bio.h>
#include <openssl/bn.h>

namespace Common {
    // openssl genrsa -out rsa_private_key.pem 512, rsa -in rsa_private_key.pem -pubout -out rsa_public_key.pem
    // 512
    const char *RSAProvider::DefaultPriKey = "-----BEGIN RSA PRIVATE KEY-----\nMIIBOwIBAAJBAN3R9PYMqXz3B9lx1x18xSuYXtCnhPHlXkSRdQo8m+6IYEttEBz7\nBDbnl3QpdEQiN95lvpiiWD+SXYuOWcKGwcUCAwEAAQJAfaDUqKmhMCpEYs2LtKKe\nlMM2J9xIeiuaiSEMdWDAO4YpLRRqodJhlKJvlX8Uga4aokBG3BB+L+sb0BMjHw0B\ngQIhAPooGY7g7tZVAS1LLsxzXafTHoUgK45+DJbuOxe9DpLhAiEA4wBosBXBNjbK\nIM2KIRF+M/4nVN0QFN6ZRpweWxA/r2UCIQCILCn1f/YjUDSKzmFUQ/yYbbEwcJml\nQHJj+pzKSEzVoQIgWX+8xsgdAHIDqYFAV59dAcTP3w61m3TUO9mrwj2azl0CIQDS\n8wzFdQSxO6Gpdp37mZrQEr0K21fx3xzO4gqAaDW/AA==\n-----END RSA PRIVATE KEY-----\n";
    const char *RSAProvider::DefaultPubKey = "-----BEGIN PUBLIC KEY-----\nMFwwDQYJKoZIhvcNAQEBBQADSwAwSAJBAN3R9PYMqXz3B9lx1x18xSuYXtCnhPHl\nXkSRdQo8m+6IYEttEBz7BDbnl3QpdEQiN95lvpiiWD+SXYuOWcKGwcUCAwEAAQ==\n-----END PUBLIC KEY-----\n";

    // 256
    const char *RSAProvider::Default256PriKey = "-----BEGIN RSA PRIVATE KEY-----\nMIGqAgEAAiEAy+UCTHF8hgDTrKgtSnZANwtyc3M69fgKhdpu+G+o2/UCAwEAAQIg\nPNHSDrVnfJ1whrcf029Ulc59QnZtxruRjaVAIkNe3v0CEQDobG8IyhuHTDMMJtRk\nT1v7AhEA4JO8Nl7sd9ZdFpIcAqO0zwIRAONorR9BeLvhr5S1LVzUVR8CEFuwVYaJ\nSNhAf+8IJkF5nwcCEGsQHv29kl/y5+BfbvRgwv0=\n-----END RSA PRIVATE KEY-----\n";
    const char *RSAProvider::Default256PubKey = "-----BEGIN PUBLIC KEY-----\nMDwwDQYJKoZIhvcNAQEBBQADKwAwKAIhAMvlAkxxfIYA06yoLUp2QDcLcnNzOvX4\nCoXabvhvqNv1AgMBAAE=\n-----END PUBLIC KEY-----\n";

    // 1024
    const char *RSAProvider::Default1024PriKey = "-----BEGIN RSA PRIVATE KEY-----\nMIICXQIBAAKBgQDOS9MkcqbWUUZFQqWAKi2CpHQBFvUxLieqA89dhNpIB40k21dG\nXmhDkuzt3oKqI0lH+DiQRpMBfDg1J1QBx6LgO2m/5L0kztiw/tdgr7YQ3cemelfI\nPzEeOJE1xpDlimujDYVh5h3UMUWDgM3juZNxl5Ihvtm7SCDsrZRy0I5U0wIDAQAB\nAoGBAJfLpWZ5Upf6n8lQ2/StlGU/2vf+S5bPjnIEca4vlmpP3CSVnOdziibG0r3I\n775/Tmm9lOlcdMkv/S/na2IP4qDm9YSz89Gnn2k40K54nYLuPLbnxJvZxpWdp6ql\n3JsBzMiRUvkZOULV0dS8fsOBy0wkQWiKoljWFcglUV2/G6f5AkEA+Lfo1ayMTu8a\no/S1paynCqlfRfIkNiVRNF+As/7Rcu/c0UYvYSWbK9aJ0wnSLsldn7ppwHxwqflZ\njoPo8eTWHQJBANRV+FNmi49hrk55aV6PvxKYjqrc0ork0+v0Ckoxin/dGgAR+dED\nsGxkQRAO/r2+NqWQ4t8biSmJJvIwDOwiI68CQCHlztHwZl8Lr3HRFEC0Oz/0vYtm\n6RCIHX0p1vyxKhKkJlBPZH21BmT/JGGi4QaHYz81xNv7Gc/QsCgASmtNaPECQCQX\nyPJESAzDSeHE+Yg9XvlGPVoF1zQM2YkOHoGmEijR6I2CgLhqrB6AnB6ireKK5rcV\nbp278XDrn8Uc93fn7kCQQC9aMGFvDSneLLyrD5/obexyRzRPxEz8fuF+jsgBPdP\n3NE52avHehQVKYh9IFoWWV9QEms0Ua/qBsq847MxiskW\n-----END RSA PRIVATE KEY-----\n";
    const char *RSAProvider::Default1024PubKey = "-----BEGIN PUBLIC KEY-----\nMIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQDOS9MkcqbWUUZFQqWAKi2CpHQB\nFvUxLieqA89dhNpIB40k21dGXmhDkuzt3oKqI0lH+DiQRpMBfDg1J1QBx6LgO2m/\n5L0kztiw/tdgr7YQ3cemelfIPzEeOJE1xpDlimujDYVh5h3UMUWDgM3juZNxl5Ih\nvtm7SCDsrZRy0I5U0wIDAQAB\n-----END PUBLIC KEY-----\n";

    const char *RSAProvider::Default2048PubKey = "-----BEGIN PUBLIC KEY-----\nMIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEA3sXwk+/yiT2BhrL9PYcd\nooFh+xtEY2WGXSDSISXBPNN4QqdiljsJD/QW0N/7qo9JUtRNSjAvLXuVPRKVxH7L\nmSnp4fbocQhnakhMdGCnIppQPKQkOuYB8ldI3F5XssPf+EKQacfrxi/v1BEumgT5\nDK5WUYcoE0qZ4VN6LPdWCcx93vFTsVS2+pz8GqMmy3ntQS+JOnxvfEUCiTyJOdzn\nYjcbCwsPBqir4vwsmliFB5lGn/d9wxh31YuElhE500Qknma/GanA9sFTsU/K3daQ\nOFnrmNM61TwtrpyYHs1ggqrYU4TA1z2GQ3TXt46NQzEu7tvDZvxZootvE7aRQ619\n+wIDAQAB\n-----END PUBLIC KEY-----\n";
    const char *RSAProvider::Default2048PriKey = "-----BEGIN RSA PRIVATE KEY-----\nMIIEpAIBAAKCAQEA3sXwk+/yiT2BhrL9PYcdooFh+xtEY2WGXSDSISXBPNN4Qqdi\nljsJD/QW0N/7qo9JUtRNSjAvLXuVPRKVxH7LmSnp4fbocQhnakhMdGCnIppQPKQk\nOuYB8ldI3F5XssPf+EKQacfrxi/v1BEumgT5DK5WUYcoE0qZ4VN6LPdWCcx93vFT\nsVS2+pz8GqMmy3ntQS+JOnxvfEUCiTyJOdznYjcbCwsPBqir4vwsmliFB5lGn/d9\nwxh31YuElhE500Qknma/GanA9sFTsU/K3daQOFnrmNM61TwtrpyYHs1ggqrYU4TA\n1z2GQ3TXt46NQzEu7tvDZvxZootvE7aRQ619+wIDAQABAoIBAFb4zC3VdcjivFjM\nNFX7FDTHcmtIf/CWSFi/YYoTZVfL752GH9UrAyS5VNcDOwuhmSy/OdWG7kRBhUgs\no0/teKs5U9gmPyUx9L3nsAGXYmj9+elS49oixgvIy+dM5cwi8KPJyPujU4cyW91d\nP+ZdnqW2wnhd3eV6MI1pu78maO61rnoeGhlh41EfiH1GSq2ksYIH7JNR11g5g7RY\nbDdbmYIRUUqDLbuwpcLKxEunU+fkvmBn5fM9JVp4eWZmqaD8cfWRvAvJ/UqNrldP\naShs+8SV3m2zhwvuCOL/QsBBYfXUfxqZ0QCKyE/Bw1LvjDmGC3XKLYwvDmwZ79Nr\nsmZF6QECgYEA/XXQNQuUGnxkjnJljM6gDSIwpw3w4PcoeZSjJzhWd79mBB5scaO+\nECt4KFnKfu+1v/GIbi4jzdnyobMY7JYyVJCGykz8xQHPYSyI6n5zMNA0cQUExTVd\npQN2EZhBKfm7kp1oqWqgfRShmKAbdux0R3bWOC2yWwg0sR9imvD2bDsCgYEA4QFo\nKAmqytkpYk2LEHkgFRUyKlxtWoLIFuoctYOJv82cpbQ+etGc+Kc9080s6kQRAh4N\nWAe1H5IpSVqYP7b0m3cdjcSXSBKxEBrchgt7Go2wyOys7wQsXkZp9wMN8RrUcsPZ\nGU09COrY5hNwS0wMo1Au+R7J28CTfvdk+So62UECgYEA9HH5eg9OqTSrLo65zdFB\nnykITzUsyWrViFyH9b1p91eKphJJeFf5aFLkMdZXa5D4csQZVln9gfze+oBmW1aH\n9/LjbJxPdBlIzmlLKw0FP2CuEwpKDYBobksmRdXXkwodk0tos2v4mGmbaD2R/2IQ\nyxObitbXmuARIXopeVFH+B0CgYEAihJq7+a+v6tzEbez1m/9l/JhzkzZbelDD4yq\nAViudtyde7GXhZaLVKqL7iTBoyARr9LxLUvNgYoXe8P0GWUTxTLbF9vDWZ7euJtD\nJ55z9F9uVF2n4SQWdeOPstxU+dcU9zfnSPd0tLwoTUcwaYRJXQ4fRQ/3nroVyJvX\ngm6/ngECgYBEj0KntJorYNQGV3YPnEuxMGC3L2XxrzIM5YzRZBtaNwcHdDQLBuYn\n6zR7FU4rurbiiNq23KpgEX9HPRQNfJtg1siMJP3NTQFylxTF5hUCx/ZzQL/YutLd\nAQQxAqWDDHn0JdRwkJtJpaqhfKoJDuU1HFL8hXW2Hg4uRHfXnClXuw==\n-----END RSA PRIVATE KEY-----\n";

    // data length > (tlen-11) then RSA_R_DATA_TOO_LARGE_FOR_KEY_SIZE
    // 512: tlen = 64, then data length must less than 64 - 11 = 53.
    bool RSAProvider::encrypt(const ByteArray &data, ByteArray &out, KeySize size) {
        if (data.isEmpty())
            return false;

        BIO *b = nullptr;
        if ((b = BIO_new_mem_buf(defaultPubKey(size), -1)) == nullptr)
            return false;

        RSA *pRSAPublicKey = nullptr;
        if (PEM_read_bio_RSA_PUBKEY(b, &pRSAPublicKey, nullptr, nullptr) == nullptr) {
            ERR_load_crypto_strings();
            char errBuf[512];
            ERR_error_string_n(ERR_get_error(), errBuf, sizeof(errBuf));
            Debug::writeFormatLine("RSAProvider::encrypt error: %s", errBuf);
            BIO_free_all(b);
            return false;
        }

//        int j = BN_num_bytes(pRSAPublicKey->n);

        int nLen = RSA_size(pRSAPublicKey);
        uint8_t *pEncode = new uint8_t[nLen + 1];
        int ret = RSA_public_encrypt(data.count(), data.data(), (uint8_t *) pEncode, pRSAPublicKey, RSA_PKCS1_PADDING);
        if (ret >= 0) {
            out = ByteArray(pEncode, ret);
        } else {
            ERR_load_crypto_strings();
            char errBuf[512];
            ERR_error_string_n(ERR_get_error(), errBuf, sizeof(errBuf));
            Debug::writeFormatLine("RSAProvider::encrypt error: %s", errBuf);
            delete[] pEncode;
            BIO_set_close(b, BIO_CLOSE);
            BIO_free(b);
            RSA_free(pRSAPublicKey);
            CRYPTO_cleanup_all_ex_data();
            return false;
        }
        delete[] pEncode;
        BIO_set_close(b, BIO_CLOSE);
        BIO_free(b);
        RSA_free(pRSAPublicKey);
        CRYPTO_cleanup_all_ex_data();
        return true;
    }

    bool RSAProvider::decrypt(const ByteArray &data, ByteArray &out, KeySize size) {
        if (data.isEmpty())
            return false;

        BIO *b = nullptr;
        if ((b = BIO_new_mem_buf(defaultPriKey(size), -1)) == nullptr)
            return false;

        RSA *pRSAPriKey = RSA_new();
        if (PEM_read_bio_RSAPrivateKey(b, &pRSAPriKey, nullptr, nullptr) == nullptr) {
            ERR_load_crypto_strings();
            char errBuf[512];
            ERR_error_string_n(ERR_get_error(), errBuf, sizeof(errBuf));
            BIO_free_all(b);
            return false;
        }

        int nLen = RSA_size(pRSAPriKey);
        uint8_t *pDecode = new uint8_t[nLen + 1];
        int ret = RSA_private_decrypt(data.count(), data.data(), (uint8_t *) pDecode, pRSAPriKey, RSA_PKCS1_PADDING);
        if (ret >= 0) {
            out = ByteArray(pDecode, ret);
        } else {
            ERR_load_crypto_strings();
            char errBuf[512];
            ERR_error_string_n(ERR_get_error(), errBuf, sizeof(errBuf));
            Debug::writeFormatLine("RSAProvider::decrypt error: %s", errBuf);
            delete[] pDecode;
            BIO_set_close(b, BIO_CLOSE);
            BIO_free(b);
            RSA_free(pRSAPriKey);
            CRYPTO_cleanup_all_ex_data();
            return false;
        }
        delete[] pDecode;
        BIO_set_close(b, BIO_CLOSE);
        BIO_free(b);
        RSA_free(pRSAPriKey);
        CRYPTO_cleanup_all_ex_data();
        return true;
    }

    bool RSAProvider::encrypt(const String &data, String &out, KeySize size) {
        ByteArray array((uint8_t *) data.c_str(), data.length());
        ByteArray buffer;
        if (encrypt(array, buffer, size)) {
            out = buffer.toString("%02X", String::Empty);
            return true;
        }
        return false;
    }

    bool RSAProvider::decrypt(const String &data, String &out, KeySize size) {
        ByteArray array;
        if (ByteArray::parse(data, array, String::Empty)) {
            ByteArray buffer;
            if (decrypt(array, buffer, size)) {
                out = String((const char *) buffer.data(), buffer.count());
                return true;
            }
        }
        return false;
    }

    bool RSAProvider::encryptToBase64(const String &data, String &out, KeySize size) {
        ByteArray array((uint8_t *) data.c_str(), data.length());
        ByteArray buffer;
        if (encrypt(array, buffer, size)) {
            out = String::toBase64(buffer.data(), 0, buffer.count());
            return true;
        }
        return false;
    }

    bool RSAProvider::decryptFromBase64(const String &data, String &out, KeySize size) {
        ByteArray array;
        if (String::fromBase64(data, array)) {
            ByteArray buffer;
            if (decrypt(array, buffer, size)) {
                out = String((const char *) buffer.data(), buffer.count());
                return true;
            }
        }
        return false;
    }

    bool RSAProvider::encryptByPubKeyFile(const String &pemFileName, const String &data, String &out) {
        if (pemFileName.isNullOrEmpty())
            return false;
        if (data.isNullOrEmpty())
            return false;

        FILE *hKeyFile = fopen(pemFileName, "rb");
        if (hKeyFile == nullptr)
            return false;

        RSA *pRSAPublicKey = RSA_new();
        if (PEM_read_RSA_PUBKEY(hKeyFile, &pRSAPublicKey, 0, 0) == nullptr) {
            RSA_free(pRSAPublicKey);
            return false;
        }

        int nLen = RSA_size(pRSAPublicKey);
        char *pEncode = new char[nLen + 1];
        int ret = RSA_public_encrypt(data.length(), (const unsigned char *) data.c_str(), (unsigned char *) pEncode,
                                     pRSAPublicKey, RSA_PKCS1_PADDING);
        if (ret >= 0) {
            out = String(pEncode, ret);
        }
        delete[] pEncode;
        RSA_free(pRSAPublicKey);
        fclose(hKeyFile);
        CRYPTO_cleanup_all_ex_data();
        return true;
    }

    bool RSAProvider::decryptByPriKeyFile(const String &pemFileName, const String &data, String &out) {
        if (pemFileName.isNullOrEmpty())
            return false;
        if (data.isNullOrEmpty())
            return false;

        FILE *hKeyFile = fopen(pemFileName, "rb");
        if (hKeyFile == nullptr)
            return false;

        std::string strRet;
        RSA *pRSAPriKey = RSA_new();
        if (PEM_read_RSAPrivateKey(hKeyFile, &pRSAPriKey, 0, 0) == nullptr) {
            RSA_free(pRSAPriKey);
            return false;
        }

        int nLen = RSA_size(pRSAPriKey);
        char *pDecode = new char[nLen + 1];
        int ret = RSA_private_decrypt(data.length(), (const unsigned char *) data.c_str(), (unsigned char *) pDecode,
                                      pRSAPriKey, RSA_PKCS1_PADDING);
        if (ret >= 0) {
            out = String(pDecode, ret);
        }
        delete[] pDecode;
        RSA_free(pRSAPriKey);
        fclose(hKeyFile);
        CRYPTO_cleanup_all_ex_data();
        return true;
    }

    const char *RSAProvider::defaultPubKey(KeySize size) {
        switch (size) {
            case KeySize::Key512:
                return DefaultPubKey;
            case KeySize::Key1024:
                return Default1024PubKey;
            case KeySize::Key2048:
                return Default2048PubKey;
            case KeySize::Key256:
                return Default256PubKey;
            default:
                assert(false);
                break;
        }
        return nullptr;
    }

    const char *RSAProvider::defaultPriKey(KeySize size) {
        switch (size) {
            case KeySize::Key512:
                return DefaultPriKey;
            case KeySize::Key1024:
                return Default1024PriKey;
            case KeySize::Key2048:
                return Default2048PriKey;
            case KeySize::Key256:
                return Default256PriKey;
            default:
                assert(false);
                break;
        }
        return nullptr;
    }
}
