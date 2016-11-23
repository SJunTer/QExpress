#ifndef ACTIVATE_H
#define ACTIVATE_H

#include "../security/md5.h"
#include "hdinfo.h"
#include <string>

class Activate
{
private:
    HDInfo hdInfo;
    MD5_CTX md5;

    unsigned char key[17];
    unsigned char id[17];
    unsigned char ciphertext[17];

    void GenerateKey();

public:
    bool WriteToFile();     // 添加激活标识
    std::string ReturnId(); // 返回本机标识
    std::string ReturnKey();
    bool KeyIsCorrect();    // 判断是否激活
    bool Act_codeIsAgreed(std::string &input);   // 密钥输入是否正确

    Activate();
    ~Activate();
};

#endif // ACTIVATE_H
