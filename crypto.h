#ifndef _LDAR_CRYPTO_H
#define _LDAR_CRYPTO_H






#include <string>
#include <openssl/evp.h>


class AESCrypter{
  public:
    AESCrypter(const std::string& key, const std::string& iv);
    AESCrypter(const char* key,const char* iv);
    ~AESCrypter();

    bool Encrypt(const std::string &input, std::string& output);
    bool Decrypt(const std::string& input, std::string& output);

  private:
    int BlockSize() const;
    void Init();
    void Setup(int dir);

    bool Update(const std::string &in, std::string& out);
    bool Final(std::string &out);

    int dir_;
    std::string key_;
    std::string iv_;
    //EVP_CIPHER_CTX ctx_;
    EVP_CIPHER_CTX *ctx_;

    const EVP_CIPHER *cryptoAlgorithm_;

};




#endif 
