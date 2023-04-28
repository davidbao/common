//
//  RSAProvider.cpp
//  common
//
//  Created by baowei on 2016/9/4.
//  Copyright (c) 2016 com. All rights reserved.
//

#include "crypto/RSAProvider.h"
#include "diag/Trace.h"
#include "IO/File.h"
#include <openssl/rsa.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/bio.h>
#include <openssl/bn.h>

using namespace Diag;

namespace Crypto {
    // How to generate keys.
    // openssl genrsa -out rsa_private_key.pem 512
    // openssl rsa -in rsa_private_key.pem -out rsa_public_key.pem -pubout

    // 512
    const char *RSAProvider::DefaultPriKey = "-----BEGIN RSA PRIVATE KEY-----\n"
                                             "MIIBOwIBAAJBAPO/ASpvbwqQgpohlMNIc8UFfxxUDpamGH1r/7AQ/b7QFtRfkOOE\n"
                                             "kg2IWd5oONcwpnvEeaEBlF710iCrxruEfAECAwEAAQJBAOwpom2FLmdkAMZpxwM2\n"
                                             "zcSVQMUnxEK+h3fuDfx/WftnKrRkmFAekCv7KZrk89Pl/Ff0hT0q6sc5j0Og5Wys\n"
                                             "3OECIQD+TSOzf0oaW+FAq/A+jAyh0zJAwqRMOm+NPKuuoKIx3QIhAPVf0MIDkAqn\n"
                                             "CT71BDPk19HNx5JjVbHLeP66P4wzjFp1AiBuda1jQ5A5oN1YDRde49B83lL+xE0/\n"
                                             "e0aHL+pXARlS1QIgfI21lzZdwLqHe/i9uGXak7pEKu/XiRfDBHKgXVexUB0CIQDU\n"
                                             "P/rGvBxTNCwiTpyol0tRiiJm9ZWe+S6lfFZ/c9Vjkw==\n"
                                             "-----END RSA PRIVATE KEY-----";
    const char *RSAProvider::DefaultPubKey = "-----BEGIN PUBLIC KEY-----\n"
                                             "MFwwDQYJKoZIhvcNAQEBBQADSwAwSAJBAPO/ASpvbwqQgpohlMNIc8UFfxxUDpam\n"
                                             "GH1r/7AQ/b7QFtRfkOOEkg2IWd5oONcwpnvEeaEBlF710iCrxruEfAECAwEAAQ==\n"
                                             "-----END PUBLIC KEY-----";

    // 256
    const char *RSAProvider::Default256PriKey = "-----BEGIN RSA PRIVATE KEY-----\n"
                                                "MIGsAgEAAiEAx30WnOU7VfIbZQk+gev0Gtmxzz5zPbpammQJeQMxyX8CAwEAAQIh\n"
                                                "AKurm+Ac7LkDhkumbuJDeWZzduFM371IiYQ1s3vHUYzxAhEA9lkNw7Ru2ghpalZn\n"
                                                "h9I1BQIRAM9OBY8cxvoSmm7SC8JJi7MCEQCB/Z/rwwwzdPjbKdjbrKpZAhBRv77J\n"
                                                "dNuyeeuUkBg7Aa2pAhEAi38ShgdPJDlhDo2DBao+hg==\n"
                                                "-----END RSA PRIVATE KEY-----";
    const char *RSAProvider::Default256PubKey = "-----BEGIN PUBLIC KEY-----\n"
                                                "MDwwDQYJKoZIhvcNAQEBBQADKwAwKAIhAMd9FpzlO1XyG2UJPoHr9BrZsc8+cz26\n"
                                                "WppkCXkDMcl/AgMBAAE=\n"
                                                "-----END PUBLIC KEY-----";

    // 1024
    const char *RSAProvider::Default1024PriKey = "-----BEGIN RSA PRIVATE KEY-----\n"
                                                 "MIICWwIBAAKBgQDRuRXQ1Iy0PL2puUIazZw5DC4I6Hhqjz171UmSzS/d52reT1fx\n"
                                                 "foYh+33ndOzRZJXw/KO8sSJEOBcezHueEaW30SJgqQAfderY7qT0t4QFLW9W24/6\n"
                                                 "DVophSoAQPmVb/7ufA3z7GVUrHkQXYLN1I5/X6hvyXKlHz5pOGLVM3eABwIDAQAB\n"
                                                 "AoGAJwgARGZabWzki4LcB6r+iSt4IdZKGuLfuX0/fR/M6cqY/BjeMCaxBgANNZdw\n"
                                                 "P6Qtt7T9MC/zRaj+vI+n7gowwgfcMbGg/qiY6JM/vgdxLt+MaPri+30ic2SdjxRt\n"
                                                 "7vxztp198+1ZTaRAZiktfKRYKc/B8McGSe7BjaJ1v9NuWMkCQQDqdq78q2yW1KUl\n"
                                                 "DqZArC9TqJ9U+cAntJxxVs7xT8Ev1v8f5Vzo96q7FOpEL5Ii2Leq7J/IEQuibNBC\n"
                                                 "iUN3qzmlAkEA5Pyi1nubGYdOoPeaxohy/81L2N0TjFHbmFP8cNNLT9Hmj7dOAhSL\n"
                                                 "tRxRDQMaEt8FDs040vPkZUJXSUWtKYCrOwJAG11oxQK49M2yzKVuEARIrcnzdraD\n"
                                                 "j97Ll4rbMlztnkjIlA9wDZzCZ31kbjrjIoAKx79AUthydC2hDPL6VeA9JQJAbx61\n"
                                                 "+rVS58mEe5KMdkli3QWIB/4IbXCPytT+zNWMV3i5lQZUAahsv3+1SUY3Ee9N6gfm\n"
                                                 "yl/2drkB2JnKVTiOXwJAW0lHZolBx5wQUXgiNjKjYJoTXX+kqcBZ7m1yyifPbw5i\n"
                                                 "Fc0jOcHV8Haeitr8w2kXXmAkJ6nVTc99WuYUEH6trA==\n"
                                                 "-----END RSA PRIVATE KEY-----";
    const char *RSAProvider::Default1024PubKey = "-----BEGIN PUBLIC KEY-----\n"
                                                 "MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQDRuRXQ1Iy0PL2puUIazZw5DC4I\n"
                                                 "6Hhqjz171UmSzS/d52reT1fxfoYh+33ndOzRZJXw/KO8sSJEOBcezHueEaW30SJg\n"
                                                 "qQAfderY7qT0t4QFLW9W24/6DVophSoAQPmVb/7ufA3z7GVUrHkQXYLN1I5/X6hv\n"
                                                 "yXKlHz5pOGLVM3eABwIDAQAB\n"
                                                 "-----END PUBLIC KEY-----";

    // 2048
    const char *RSAProvider::Default2048PubKey = "-----BEGIN PUBLIC KEY-----\n"
                                                 "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEA7sfvdVol58B4wjrcMXcS\n"
                                                 "SpOoUEw2qQqhfwW0cZ93D5YcrRMt51SMYkVxRc67Gq+mDK/chpMhGWD7n33I3kOY\n"
                                                 "gwH02zcJs1zJg2Ktn2EeRdQuA0bQ/LXKMx9wqXcKmNL2IpQ8vIiTDQs1v7sUadmE\n"
                                                 "UU9rZphO816xS9yIQy5Z6Oc9sS+/sgnr67/aedlFJF4rR6GzVG4VpkGxaUmjMa79\n"
                                                 "H4GPSRw2uXzmmmlZ3yvMmp5zC+qWoiVgMnFNoCE0iHCiTEEyE3atriVj2H1tKxE2\n"
                                                 "oOsy+0qsixu1LQgIAuy2H/UB6Il4MAxm7lqbt1oCiXhlOWlVCn1mne8eUjXBbH+4\n"
                                                 "1wIDAQAB\n"
                                                 "-----END PUBLIC KEY-----";
    const char *RSAProvider::Default2048PriKey = "-----BEGIN RSA PRIVATE KEY-----\n"
                                                 "MIIEowIBAAKCAQEA7sfvdVol58B4wjrcMXcSSpOoUEw2qQqhfwW0cZ93D5YcrRMt\n"
                                                 "51SMYkVxRc67Gq+mDK/chpMhGWD7n33I3kOYgwH02zcJs1zJg2Ktn2EeRdQuA0bQ\n"
                                                 "/LXKMx9wqXcKmNL2IpQ8vIiTDQs1v7sUadmEUU9rZphO816xS9yIQy5Z6Oc9sS+/\n"
                                                 "sgnr67/aedlFJF4rR6GzVG4VpkGxaUmjMa79H4GPSRw2uXzmmmlZ3yvMmp5zC+qW\n"
                                                 "oiVgMnFNoCE0iHCiTEEyE3atriVj2H1tKxE2oOsy+0qsixu1LQgIAuy2H/UB6Il4\n"
                                                 "MAxm7lqbt1oCiXhlOWlVCn1mne8eUjXBbH+41wIDAQABAoIBAENNR0kkZ5qyvVbB\n"
                                                 "kF7A2P78L2yyjpWG0Y4j5YBBGiXigaKmzUFuq0Lnn821Pzd+WqcoyleEEMg+da22\n"
                                                 "nm9HTi1e180R7kIH3SSTI6zQit/TtkRmKXGRK7TNH8wkKXB5xbZpYdUiCZCboEKy\n"
                                                 "UEalcI4afIMmWtptzjs7pYyJ+/2Q5HjuOLzcfPPx5QEmxQxW0eOdyr/AGXxOJpi+\n"
                                                 "74MB3qOZQdH1mZgXLQpUrBQT/c552h6R5I8ZP6DsUnaoW2GCvb0m5DD3XBCYCVKp\n"
                                                 "kvjDvp8Tr+LQwVS9x+Trn8SL9vGZiWKSp589sHY8dSZcIKl1LEoIjLJipk1ItqUP\n"
                                                 "Ol2r8AECgYEA/yjtO/nTO5LyfepMJ0HDjIbC2h5ZkkDrz5IHIiRMQbX64SOZhd4e\n"
                                                 "k1lH0XzHmnvSdvEBUSSynp2ZCWnRd1ZMtY89J/zzarp5Z3NX/PDaUwZ6Ay/YvYHo\n"
                                                 "ovkQy1wNQgViVCMBXLfte6rA+JEIzUag5o1F2BE5SUEUOIimIuuUqdcCgYEA75Ez\n"
                                                 "96W8mGSSCF4pW7U0T6tBUvsrHRy4Z+YKLaaW7eS0ZQnNCSRPgiG7HEZzeERi+28e\n"
                                                 "AKAuXeD1RIDjY+tR8kiJnqgJWFm6CjdtbRcydne1Kik7jLp+ZhgzCsH8An7LEg1u\n"
                                                 "PnpgHW8apMYQXMO0DCd4br3ZOdt7mvsJ6tNRiQECgYBMxP2LnADH3dCDveinrwus\n"
                                                 "zib2BUehz1KjoyXuScl/w8b10gYw1sGQR75p8wcDHdoW2zAwP8BWJMiUonfZVefC\n"
                                                 "gJQ/4cVDCVPZokSI39ZwQHSZXN3zNpDN5+37oXiDsqcA9X9FbDlRifCBWPGzAVsL\n"
                                                 "wtmOxq/bsG7rC4Ig44xZBQKBgQDA6LblE0Ojn1Bsho5dN7QQ90FZ9cEI50ii+X12\n"
                                                 "ty/QDpsoxVSiqOeRTWIjaaOv07m06AxLyrCrjetSp0rz+dj59WlEJeGokFAby/hE\n"
                                                 "oSpUdR6f/aXjM4V25sOrRPuOctVJCyGcE+TzrrTQvGEOodSI+PukiSGz0kZdTQr2\n"
                                                 "WgXNAQKBgEcOIS7irOd6IaBs3Em3s9muZ2ANsZiHIcfpVG0hb3c/MYNKEvjwuXtt\n"
                                                 "aPChOsig1GWWm3rIv1YLYrGTcyJR9bRAWPzGizApVxccbpawWoIzRtTrN1/U8pgT\n"
                                                 "LGaM75Pz1evBI+SpdVtl9RXL+8K+1SU09yvZRp6ofY2NBaFRKxnt\n"
                                                 "-----END RSA PRIVATE KEY-----";

    // data length > (tlen-11) then RSA_R_DATA_TOO_LARGE_FOR_KEY_SIZE
    // 512: tlen = 64, then data length must less than 64 - 11 = 53.
    bool RSAProvider::encrypt(const ByteArray &data, ByteArray &out, KeySize size) {
        return encryptByPubKey(defaultPubKey(size), data, out);
    }

    bool RSAProvider::decrypt(const ByteArray &data, ByteArray &out, KeySize size) {
        return decryptByPriKey(defaultPriKey(size), data, out);
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

    bool RSAProvider::encryptByPubKeyFile(const String &pemFileName, const ByteArray &data, ByteArray &out) {
        String key = File::readAllText(pemFileName);
        if (key.isNullOrEmpty()) {
            return false;
        }

        return encryptByPubKey(key, data, out);
    }

    bool RSAProvider::decryptByPriKeyFile(const String &pemFileName, const ByteArray &data, ByteArray &out) {
        String key = File::readAllText(pemFileName);
        if (key.isNullOrEmpty()) {
            return false;
        }

        return decryptByPriKey(key, data, out);
    }

    bool RSAProvider::encryptByPubKeyFile(const String &pemFileName, const String &data, String &out) {
        ByteArray array((uint8_t *) data.c_str(), data.length());
        ByteArray buffer;
        if (encryptByPubKeyFile(pemFileName, array, buffer)) {
            out = buffer.toString("%02X", String::Empty);
            return true;
        }
        return false;
    }

    bool RSAProvider::decryptByPriKeyFile(const String &pemFileName, const String &data, String &out) {
        ByteArray array;
        if (ByteArray::parse(data, array, String::Empty)) {
            ByteArray buffer;
            if (decryptByPriKeyFile(pemFileName, array, buffer)) {
                out = String((const char *) buffer.data(), buffer.count());
                return true;
            }
        }
        return false;
    }

    bool RSAProvider::encryptByPubKey(const String &key, const ByteArray &data, ByteArray &out) {
        if (data.isEmpty())
            return false;

        BIO *b;
        if ((b = BIO_new_mem_buf(key, -1)) == nullptr)
            return false;

        RSA *pRSAPublicKey = nullptr;
        if (PEM_read_bio_RSA_PUBKEY(b, &pRSAPublicKey, nullptr, nullptr) == nullptr) {
            ERR_load_crypto_strings();
            char errBuf[512];
            ERR_error_string_n(ERR_get_error(), errBuf, sizeof(errBuf));
            Trace::error(errBuf);
            BIO_free_all(b);
            return false;
        }

//        int j = BN_num_bytes(pRSAPublicKey->n);

        int nLen = RSA_size(pRSAPublicKey);
        auto pEncode = new uint8_t[nLen + 1];
        int ret = RSA_public_encrypt((int) data.count(), data.data(), (uint8_t *) pEncode, pRSAPublicKey,
                                     RSA_PKCS1_PADDING);
        if (ret >= 0) {
            out = ByteArray(pEncode, ret);
        } else {
            ERR_load_crypto_strings();
            char errBuf[512];
            ERR_error_string_n(ERR_get_error(), errBuf, sizeof(errBuf));
            Trace::error(errBuf);
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

    bool RSAProvider::decryptByPriKey(const String &key, const ByteArray &data, ByteArray &out) {
        if (data.isEmpty())
            return false;

        BIO *b;
        if ((b = BIO_new_mem_buf(key, -1)) == nullptr)
            return false;

        RSA *pRSAPriKey = RSA_new();
        if (PEM_read_bio_RSAPrivateKey(b, &pRSAPriKey, nullptr, nullptr) == nullptr) {
            ERR_load_crypto_strings();
            char errBuf[512];
            ERR_error_string_n(ERR_get_error(), errBuf, sizeof(errBuf));
            BIO_free_all(b);
            Trace::error(errBuf);
            return false;
        }

        int nLen = RSA_size(pRSAPriKey);
        auto pDecode = new uint8_t[nLen + 1];
        int ret = RSA_private_decrypt((int) data.count(), data.data(), (uint8_t *) pDecode, pRSAPriKey,
                                      RSA_PKCS1_PADDING);
        if (ret >= 0) {
            out = ByteArray(pDecode, ret);
        } else {
            ERR_load_crypto_strings();
            char errBuf[512];
            ERR_error_string_n(ERR_get_error(), errBuf, sizeof(errBuf));
            Trace::error(errBuf);
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

    bool RSAProvider::encryptByPubKey(const String &key, const String &data, String &out) {
        ByteArray array((uint8_t *) data.c_str(), data.length());
        ByteArray buffer;
        if (encryptByPubKey(key, array, buffer)) {
            out = buffer.toString("%02X", String::Empty);
            return true;
        }
        return false;
    }

    bool RSAProvider::decryptByPriKey(const String &key, const String &data, String &out) {
        ByteArray array;
        if (ByteArray::parse(data, array, String::Empty)) {
            ByteArray buffer;
            if (decryptByPriKey(key, array, buffer)) {
                out = String((const char *) buffer.data(), buffer.count());
                return true;
            }
        }
        return false;
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
                return DefaultPubKey;
        }
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
                return DefaultPriKey;
        }
    }
}
