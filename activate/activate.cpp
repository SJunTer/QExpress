#include "activate.h"
#include "../security/sha256.h"
#include <math.h>
#include <cstdio>
#include <cstring>
using namespace std;

bool IfPrime(int i)
{
    bool temp = true;
    for(int j = 2; j <= pow(i, 0.5); ++j)
    {
        if(i % j == 0)
        {
            temp = false;
        }
    }
    if(i != 1)
    {
        return temp;
    }
}

int Activate::WriteToFile()
{
    FILE *fp;

    if((fp=fopen("sec.key", "wb"))==NULL)
    {
        return -1;
    }

    fwrite(key, sizeof(key), 1, fp);

    if(fclose(fp))
    {
             return -1;
    }
    return 0;
}

void Activate::GenerateKey()
{
    unsigned char cpuid[17] = {0};
    unsigned char mac[18] = {0};

    memset(key, '\0', 17);
    memset(id, '\0', 17);
    memset(ciphertext, '\0', 17);

    hdInfo.getcpuid(cpuid);
    hdInfo.getMAC(mac);

    char encrypt[34] = {0};

    for(int i = 0, j = 0, t = 15; i < 33; ++i)
    {
        if(i % 2 == 0)
        {
            encrypt[i] = mac[j];
            ++j;
        }
        else
        {
            encrypt[i] = cpuid[t] + 32;
            --t;
        }

        encrypt[i] = ((encrypt[i] == ':') ? 'g': encrypt[i]);
    }

    for(int i = 0, t = 0; (i < 33) && (t < 16); ++i)
    {
        if((i == 0) || (i == 8) || (i == 15) || (i == 20) || (i == 28))
        {
            id[t] = encrypt[i];
            ++t;
        }
        else if(IfPrime(i))
        {
            id[t] = encrypt[i];
            ++t;
        }
    }
    id[16] = '\0';

    char ciphertext[65] = {0};
    sha_256(ciphertext, id);

    char asc[36] = {0};

    for(int i = 0; i < 10; ++i)
    {
        asc[i] = 48 + i;
    }
    for(int i = 10; i < 36; ++i)
    {
        asc[i] = 65 + i - 10;
    }

    for(int i = 0; i < 16; ++i)
    {
        key[i] = asc[ciphertext[i] % 36];
    }
}

bool Activate::KeyIsCorrect()
{
    FILE *fp;
    bool flag = 0;

    unsigned char strInFile[17] = {0};
    if((fp=fopen("sec.key", "rb")) == NULL)
    {
        return 0;
    }

    while(!feof(fp))
    {
        fread(strInFile, sizeof(strInFile), 1, fp);
        if(!strncmp((char *)key, (char *)strInFile, 17))
        {
            flag = 1;
        }
    }

    if(fclose(fp))
    {
        return 0;
    }

    return flag;
}

string Activate::ReturnId()
{
    return (char *)id;
}

string Activate::ReturnKey()
{
    return (char *)key;
}

bool Activate::Act_codeIsAgreed(string &input)
{
    if(!strncmp((char *)key,(char *)input.c_str(), 17))
    {
        return 1;
    }

    return 0;
}

Activate::Activate()
{
    GenerateKey();
}

Activate::~Activate()
{

}
