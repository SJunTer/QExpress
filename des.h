#ifndef DES_H
#define DES_H
#include <QVector>
#include <iostream>

typedef char ElemType;
class DES
{
public:
    int DES_Main(const char* Date,std::string &cstr);
    int DES_Lock(const char* str,const char * CipherFile);             //加密
    int DES_Unlock(std::string &cstr,const char *CipherFile);                //解密
private:
    int ByteToBit(ElemType ch, ElemType bit[]);         //字节转换成二进制
    int BitToByte(ElemType bit[],ElemType *ch);         //二进制转换成字节
    int Char8ToBit64(ElemType ch[],ElemType bit[]);   //将长度为8的字符串转为二进制位串
    int Bit64ToChar8(ElemType bit[],ElemType ch[]);  //将二进制位串转为长度为8的字符串
    int DES_PC1_Transform(ElemType key[], ElemType tempbts[]);   //密钥置换1
    int DES_PC2_Transform(ElemType key[],ElemType tempbts[]);    //密钥置换2

    int DES_MakeSubKeys(ElemType key[],ElemType subKeys[][48]);  //生成子密钥
    int DES_ROL(ElemType data[], int time);                       //循环左移
    int DES_IP_Transform(ElemType data[]);                        //IP置换
    int DES_IP_1_Transform(ElemType data[]);                       //IP逆置换
    int DES_E_Transform(ElemType data[]);                         //扩展置换
    int DES_P_Transform(ElemType data[]);                         //P置换
    int DES_XOR(ElemType R[], ElemType L[] ,int count);         //异或
    int DES_SBOX(ElemType data[]);                                //S盒置换
    int DES_Swap(ElemType left[], ElemType right[]);            //交换
    int DES_EncryptBlock(ElemType plainBlock[], ElemType subKeys[][48], ElemType cipherBlock[]);     //加密单个分组
    int DES_DecryptBlock(ElemType cipherBlock[], ElemType subKeys[][48],ElemType plainBlock[]);     //解密单个分组
    //int DES_Encrypt(const char *plainFile, const char *keyStr, const char *cipherFile);                 //加密文件
    //int DES_Decrypt(const char *cipherFile, const char *keyStr,const char *plainFile);                  //解密文件
    int DES_Encrypt(const char *keyStr,const char *cipherFile,const char* Date);
    int DES_Decrypt(const char *keyStr,const char *cipherFile,std::string &Date);
};

#endif // DES_H
