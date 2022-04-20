#include "crypto.h"
#include "base64.h"
#include <iostream>
#include <string.h>

#define AES_KEY_LEN 128

static const char aes_filename[] = "../be1/en_aes.ini";
static char aes_key[AES_KEY_LEN]={0};
static char aes_iv[AES_KEY_LEN]={0};

char* hex_to_base64(const char* hex,int size)
{
    int size64 = (size * 2) / 3.0;
    size64 += 1;
    char *base64 = (char*)calloc(size64,1);
    size64 -= 1;

    for(int i = size -1; i >= 0; i -= 3, size64 -=2)
    {
        base64[size64] |= hex[i];
        if(i > 0)
        {
            base64[size64] |= ((hex[i-1] << 4) & 0x3F);
            base64[size64 -1] |= (hex[i - 1] >> 2);
        }
        if( i >1)
        {
            base64[size64 -1] |= ((hex[i -2] << 2));
        }
    }
    printf("%s\n",base64);
    return base64;
}


void init_kv()
{
    char* token;
    char* search = "=";

    FILE* fp = fopen(aes_filename,"r");
    if(NULL != fp)
    {
        char line[256];
        while(fgets(line,sizeof line, fp) != NULL)
        {
            token = strtok(line,search);
            if(strcmp(token , "key") == 0)
            {
                token = strtok(NULL,search);
                strcpy(aes_key,token);
            }
            else if(strcmp(token, "iv") == 0)
            {
                token = strtok(NULL,search);
                strcpy(aes_iv,token);
            }

        }
        fclose(fp);
    }

}
extern "C" {
char* EncryptPassword(const char* szInput)
{
    if(aes_key[0] == 0 && aes_iv[0] == 0)
    {
        init_kv();
    }


  AESCrypter ldar_aes(aes_key,aes_iv);
  if(!szInput){

    std::cout <<" need a string to be encrypted."<< std::endl;
    return NULL;
  }

  std::string input(szInput);
  std::string output;
  std::string tmp;

  if(!ldar_aes.Encrypt(input,output)) {
    std::cout << "encrypt error "<< std::endl;
    return NULL;
  }
/*
  if(!ldar_aes.Decrypt(output,tmp)){
    std::cout <<" Decrypt error " << std::endl;
    return NULL;
  }
*/
   std::string encoded = base64_encode(output);
  static char tmpS[256];
  strcpy(tmpS,encoded.c_str());
  return tmpS;
}
}
#if 0 
int main()
{
    printf("%s\n",EncryptPassword("123456"));
}
#endif

