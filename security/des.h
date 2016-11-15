#ifndef DES_H
#define DES_H

#include <string>


int DES_Encrypt(std::string &Data);     // 加密算法接口
int DES_Decrypt(std::string &Data);     // 解密算法接口


#endif // DES_H
