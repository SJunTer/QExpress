#ifndef DES_H
#define DES_H

#include <string>


//des加密宏定义

#define KEYSTR "W5Vwn4s6"

#define PLAIN_FILE_OPEN_ERROR -1
#define KEY_FILE_OPEN_ERROR -2
#define CIPHER_FILE_OPEN_ERROR -3
#define OK 1



class DES
{
public:
    int DES_Encrypt(std::string &Data);
    int DES_Decrypt(std::string &Data);

private:
    int ByteToBit(char ch, char bit[]);         //字节转换成二进制
    int BitToByte(char bit[],char *ch);         //二进制转换成字节
    int Char8ToBit64(char ch[],char bit[]);   //将长度为8的字符串转为二进制位串
    int Bit64ToChar8(char bit[],char ch[]);  //将二进制位串转为长度为8的字符串
    int DES_PC1_Transform(char key[], char tempbts[]);   //密钥置换1
    int DES_PC2_Transform(char key[],char tempbts[]);    //密钥置换2

    int DES_MakeSubKeys(char key[],char subKeys[][48]);  //生成子密钥
    int DES_ROL(char data[], int time);                       //循环左移
    int DES_IP_Transform(char data[]);                        //IP置换
    int DES_IP_1_Transform(char data[]);                       //IP逆置换
    int DES_E_Transform(char data[]);                         //扩展置换
    int DES_P_Transform(char data[]);                         //P置换
    int DES_XOR(char R[], char L[] ,int count);         //异或
    int DES_SBOX(char data[]);                                //S盒置换
    int DES_Swap(char left[], char right[]);            //交换
    int DES_EncryptBlock(char plainBlock[], char subKeys[][48], char cipherBlock[]);     //加密单个分组
    int DES_DecryptBlock(char cipherBlock[], char subKeys[][48],char plainBlock[]);     //解密单个分组

};

#endif // DES_H
