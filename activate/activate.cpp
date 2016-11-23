#include <cstdio>
#include <cstring>
#include "activate.h"

using namespace std;

bool Activate::WriteToFile()
{
    FILE *fp;

    if((fp=fopen("sec.key", "w"))==NULL)
    {
        return 0;
    }

    fwrite(key, sizeof(key), 1, fp);

    if(fclose(fp))
    {
             return 0;
    }
    return 1;
}


void Activate::GenerateKey()
{
    MD5Init(&md5);

    unsigned char cpuid[17] = {0};
    unsigned char mac[18] = {0};

    memset(key, '\0', 17);
    memset(id, '\0', 17);
    memset(ciphertext, '\0', 17);

    hdInfo.getcpuid(cpuid);
    hdInfo.getMAC(mac);

//    cout << "cpuid = " << cpuid << endl;
 //   cout << "mac = " << mac << endl;

    unsigned char encrypt[34] = {0};

    for(int i = 0, j = 0, t = 15; i < 33; ++i)
    {
        if(i % 2 == 0 && j < 17)
        {
            encrypt[i] = mac[j];
            ++j;
        }
        else if(t > 0)
        {
            encrypt[i] = cpuid[t] + 32;
            --t;
        }

        encrypt[i] = ((encrypt[i] == ':') ? 'g': encrypt[i]);
    }

//    cout << encrypt << endl;

    for(int i = 0; i < 16; ++i)
    {
        id[i] = encrypt[i];
    }
    id[16] = '\0';

    MD5Update(&md5, id, strlen((char*)id));
    MD5Final(ciphertext, &md5);

    char asc[36] = {0};

    for(int i = 0; i < 10; ++i)
    {
        asc[i] = '0' + i;
    }
    for(int i = 10; i < 36; ++i)
    {
        asc[i] = 'A' + i - 10;
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
    if((fp=fopen("sec.key", "r")) == NULL)
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
