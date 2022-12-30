//---------------------------------------------------------------------------
#ifndef DESProvider_H
#define DESProvider_H
//---------------------------------------------------------------------------

namespace Crypto {
    class DESProvider {
    public:
        // length of k is 16, length of in is 8, length of out is 8.
        static void encrypt(unsigned char *k, unsigned char *in, unsigned char *out);

        // length of k is 16, length of in is 8, length of out is 8.
        static void decrypt(unsigned char *k, unsigned char *in, unsigned char *out);

    private:
        typedef struct {
            unsigned long esk[32];     /*!< DES encryption subkeys */
            unsigned long dsk[32];     /*!< DES decryption subkeys */
        } des_context;

        typedef struct {
            unsigned long esk[96];     /*!< Triple-DES encryption subkeys */
            unsigned long dsk[96];     /*!< Triple-DES decryption subkeys */
        } des3_context;

        static void des_set_key(des_context *ctx, unsigned char key[8]);

        static void des_encrypt(des_context *ctx,
                                unsigned char input[8],
                                unsigned char output[8]);

        static void des_decrypt(des_context *ctx,
                                unsigned char input[8],
                                unsigned char output[8]);

        static void des_cbc_encrypt(des_context *ctx,
                                    unsigned char iv[8],
                                    unsigned char *input,
                                    unsigned char *output,
                                    int len);

        static void des_cbc_decrypt(des_context *ctx,
                                    unsigned char iv[8],
                                    unsigned char *input,
                                    unsigned char *output,
                                    int len);

        static void des3_set_2keys(des3_context *ctx, unsigned char key[16]);

        static void des3_set_3keys(des3_context *ctx, unsigned char key[24]);

        static void des3_encrypt(des3_context *ctx,
                                 unsigned char input[8],
                                 unsigned char output[8]);

        static void des3_decrypt(des3_context *ctx,
                                 unsigned char input[8],
                                 unsigned char output[8]);

        static void des3_cbc_encrypt(des3_context *ctx,
                                     unsigned char iv[8],
                                     unsigned char *input,
                                     unsigned char *output,
                                     int len);

        static void des3_cbc_decrypt(des3_context *ctx,
                                     unsigned char iv[8],
                                     unsigned char *input,
                                     unsigned char *output,
                                     int len);

        static void des_crypt(unsigned long SK[32],
                              unsigned char input[8],
                              unsigned char output[8]);

        static void des_main_ks(unsigned long SK[32], unsigned char key[8]);

        static void des3_crypt(unsigned long SK[96],
                               unsigned char input[8],
                               unsigned char output[8]);

        //---------------------------------------------------------------------------
    };
}
#endif    // DESProvider_H
