#ifndef ACTIVATE_H
#define ACTIVATE_H

#include <string>
#include "hdinfo.h"

class Activate
{
private:
    HDInfo hdInfo;
    void GenerateKey();

public:
    char ciphertext[17];
    char key[17];
    char id[17];

    int WriteToFile();
    std::string ReturnId();
    std::string ReturnKey();
    bool KeyIsCorrect();
    bool Act_codeIsAgreed(std::string &input);

    Activate();
    ~Activate();
};

#endif // ACTIVATE_H
