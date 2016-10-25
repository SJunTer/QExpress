#include "des.h"
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <memory.h>
#include <ctime>
#include <cstdio>
#include <memory.h>

using namespace std;

//--------------------------DES算法部分----------------------------
//des加密宏定义
#define PLAIN_FILE_OPEN_ERROR -1
#define KEY_FILE_OPEN_ERROR -2
#define CIPHER_FILE_OPEN_ERROR -3
#define OK 1
typedef char ElemType;
//初始化置换表IP
int IP_Table[64] = {
    57,49,41,33,25,17,9,1,
    59,51,43,35,27,19,11,3,
    61,53,45,37,29,21,13,5,
    63,55,47,39,31,23,15,7,
    56,48,40,32,24,16,8,0,
    58,50,42,34,26,18,10,2,
    60,52,44,36,28,20,12,4,
    62,54,46,38,30,22,14,6
};
//逆初始置换表IP^-1
int IP_1_Table[64] =
{
                 39,7,47,15,55,23,63,31,
                 38,6,46,14,54,22,62,30,
                 37,5,45,13,53,21,61,29,
                 36,4,44,12,52,20,60,28,
                 35,3,43,11,51,19,59,27,
                 34,2,42,10,50,18,58,26,
                 33,1,41,9,49,17,57,25,
                 32,0,40,8,48,16,56,24
};
//扩充置换表E
int E_Table[48] = {31, 0, 1, 2, 3, 4,
                    3,  4, 5, 6, 7, 8,
                    7,  8,9,10,11,12,
                    11,12,13,14,15,16,
                    15,16,17,18,19,20,
                    19,20,21,22,23,24,
                    23,24,25,26,27,28,
                    27,28,29,30,31, 0};
//置换函数P
int P_Table[32] = {15,6,19,20,28,11,27,16,
                0,14,22,25,4,17,30,9,
                1,7,23,13,31,26,2,8,
                18,12,29,5,21,10,3,24};
//S盒
int S[8][4][16] =//S1
                {{{14,4,13,1,2,15,11,8,3,10,6,12,5,9,0,7},
              {0,15,7,4,14,2,13,1,10,6,12,11,9,5,3,8},
                {4,1,14,8,13,6,2,11,15,12,9,7,3,10,5,0},
                {15,12,8,2,4,9,1,7,5,11,3,14,10,0,6,13}},
                //S2
                  {{15,1,8,14,6,11,3,4,9,7,2,13,12,0,5,10},
                  {3,13,4,7,15,2,8,14,12,0,1,10,6,9,11,5},
                  {0,14,7,11,10,4,13,1,5,8,12,6,9,3,2,15},
                  {13,8,10,1,3,15,4,2,11,6,7,12,0,5,14,9}},
                  //S3
                  {{10,0,9,14,6,3,15,5,1,13,12,7,11,4,2,8},
                  {13,7,0,9,3,4,6,10,2,8,5,14,12,11,15,1},
                {13,6,4,9,8,15,3,0,11,1,2,12,5,10,14,7},
                  {1,10,13,0,6,9,8,7,4,15,14,3,11,5,2,12}},
                  //S4
                  {{7,13,14,3,0,6,9,10,1,2,8,5,11,12,4,15},
                  {13,8,11,5,6,15,0,3,4,7,2,12,1,10,14,9},
                  {10,6,9,0,12,11,7,13,15,1,3,14,5,2,8,4},
                  {3,15,0,6,10,1,13,8,9,4,5,11,12,7,2,14}},
                  //S5
                  {{2,12,4,1,7,10,11,6,8,5,3,15,13,0,14,9},
                  {14,11,2,12,4,7,13,1,5,0,15,10,3,9,8,6},
                  {4,2,1,11,10,13,7,8,15,9,12,5,6,3,0,14},
                  {11,8,12,7,1,14,2,13,6,15,0,9,10,4,5,3}},
                  //S6
                  {{12,1,10,15,9,2,6,8,0,13,3,4,14,7,5,11},
                  {10,15,4,2,7,12,9,5,6,1,13,14,0,11,3,8},
                  {9,14,15,5,2,8,12,3,7,0,4,10,1,13,11,6},
                  {4,3,2,12,9,5,15,10,11,14,1,7,6,0,8,13}},
                  //S7
                  {{4,11,2,14,15,0,8,13,3,12,9,7,5,10,6,1},
                  {13,0,11,7,4,9,1,10,14,3,5,12,2,15,8,6},
                  {1,4,11,13,12,3,7,14,10,15,6,8,0,5,9,2},
                  {6,11,13,8,1,4,10,7,9,5,0,15,14,2,3,12}},
                  //S8
                  {{13,2,8,4,6,15,11,1,10,9,3,14,5,0,12,7},
                  {1,15,13,8,10,3,7,4,12,5,6,11,0,14,9,2},
                  {7,11,4,1,9,12,14,2,0,6,10,13,15,3,5,8},
                  {2,1,14,7,4,10,8,13,15,12,9,0,3,5,6,11}}};
//置换选择1

int PC_1[56] = {56,48,40,32,24,16,8,
          0,57,49,41,33,25,17,
          9,1,58,50,42,34,26,
          18,10,2,59,51,43,35,
          62,54,46,38,30,22,14,
          6,61,53,45,37,29,21,
          13,5,60,52,44,36,28,
          20,12,4,27,19,11,3};
//置换选择2

int PC_2[48] = {13,16,10,23,0,4,2,27,
          14,5,20,9,22,18,11,3,
          25,7,15,6,26,19,12,1,
          40,51,30,36,46,54,29,39,
          50,44,32,46,43,48,38,55,
          33,52,45,41,49,35,28,31};
//对左移次数的规定
int MOVE_TIMES[16] = {1,1,2,2,2,2,2,2,1,2,2,2,2,2,2,1};

//2）移位操作函数
//字节转换成二进制
int DES::ByteToBit(ElemType ch, ElemType bit[8])
 {
    int cnt;
    for(cnt = 0;cnt < 8; cnt++)
    {
        *(bit+cnt) = (ch>>cnt)&1;
    }
    return 0;
  }
//二进制转换成字节
int DES::BitToByte(ElemType bit[8],ElemType *ch)
{
    int cnt;
    for(cnt = 0;cnt < 8; cnt++)
    {
        *ch |= *(bit + cnt)<<cnt;
     }
    return 0;
}



//将长度为8的字符串转为二进制位串

int DES::Char8ToBit64(ElemType ch[8],ElemType bit[64])
{
    int cnt;
    for(cnt = 0; cnt < 8; cnt++)
    {
        ByteToBit(*(ch+cnt),bit+(cnt<<3));
    }
    return 0;
 }
//将二进制位串转为长度为8的字符串
int DES::Bit64ToChar8(ElemType bit[64],ElemType ch[8])
{
    int cnt;
    memset(ch,0,8);
    for(cnt = 0; cnt < 8; cnt++)
    {
        BitToByte(bit+(cnt<<3),ch+cnt);
    }
    return 0;
}

      //3）密钥相关的函数

//密钥置换1
int DES::DES_PC1_Transform(ElemType key[64], ElemType tempbts[56])
{
    int cnt;
    for(cnt = 0; cnt < 56; cnt++)
   {
        tempbts[cnt] = key[PC_1[cnt]];
   }
    return 0;
}
//密钥置换2

int DES::DES_PC2_Transform(ElemType key[56],ElemType tempbts[48])
{
    int cnt;
   for(cnt = 0; cnt < 48; cnt++)
   {
        tempbts[cnt] = key[PC_2[cnt]];
    }
    return 0;
}
//循环左移
int DES::DES_ROL(ElemType data[56], int time)
{
    ElemType temp[56];
    //保存将要循环移动到右边的位
    memcpy(temp,data,time);
    memcpy(temp+time,data+28,time);
   //前28位移动
   memcpy(data,data+time,28-time);
   memcpy(data+28-time,temp,time);
   //后28位移动
    memcpy(data+28,data+28+time,28-time);
    memcpy(data+56-time,temp+time,time);
    return 0;
}
//生成子密钥
int DES::DES_MakeSubKeys(ElemType key[64],ElemType subKeys[16][48])
{
   ElemType temp[56];
   int cnt;
    DES_PC1_Transform(key,temp);    //PC1置换
   for(cnt = 0; cnt < 16; cnt++)

     {  //16轮跌代，产生16个子密钥

      DES_ROL(temp,MOVE_TIMES[cnt]);//循环左移

     DES_PC2_Transform(temp,subKeys[cnt]);//PC2置换，产生子密钥

      }

    return 0;

 }
//4）IP置换相关函数

//IP置换

int DES::DES_IP_Transform(ElemType data[64])

{

    int cnt;
    ElemType temp[64];
    for(cnt = 0; cnt < 64; cnt++)
 {
        temp[cnt] = data[IP_Table[cnt]];
   }
    memcpy(data,temp,64);
    return 0;

}
//IP逆置换

int DES::DES_IP_1_Transform(ElemType data[64])

{
    int cnt;
    ElemType temp[64];
    for(cnt = 0; cnt < 64; cnt++)
    {
        temp[cnt] = data[IP_1_Table[cnt]];
    }
    memcpy(data,temp,64);
    return 0;
}
//扩展置换

int DES::DES_E_Transform(ElemType data[48])
{
    int cnt;
    ElemType temp[48];
    for(cnt = 0; cnt < 48; cnt++)
    {
        temp[cnt] = data[E_Table[cnt]];
    }
    memcpy(data,temp,48);
    return 0;
}
//P置换

int DES::DES_P_Transform(ElemType data[32])
{
   int cnt;
    ElemType temp[32];
   for(cnt = 0; cnt < 32; cnt++)
    {
        temp[cnt] = data[P_Table[cnt]];
    }
    memcpy(data,temp,32);
    return 0;
}
//异或
int DES::DES_XOR(ElemType R[48], ElemType L[48] ,int count)
{
    int cnt;
   for(cnt = 0; cnt < count; cnt++)
    {
       R[cnt] ^= L[cnt];
    }
   return 0;
}
//S盒置换

int DES::DES_SBOX(ElemType data[48])

{
   int cnt;
    int line,row,output;
    int cur1,cur2;
    for(cnt = 0; cnt < 8; cnt++)
      {
        cur1 = cnt*6;
        cur2 = cnt<<2;
       //计算在S盒中的行与列
        line = (data[cur1]<<1) + data[cur1+5];
        row = (data[cur1+1]<<3) + (data[cur1+2]<<2) + (data[cur1+3]<<1) + data[cur1+4];
        output = S[cnt][line][row];
       //化为2进制

        data[cur2] = (output&0X08)>>3;

        data[cur2+1] = (output&0X04)>>2;

        data[cur2+2] = (output&0X02)>>1;

        data[cur2+3] = output&0x01;
      }
    return 0;
}
//交换
int DES::DES_Swap(ElemType left[32], ElemType right[32])
{
    ElemType temp[32];
    memcpy(temp,left,32);
    memcpy(left,right,32);
    memcpy(right,temp,32);
    return 0;
 }
      //5）加解密函数
//加密单个分组

int DES::DES_EncryptBlock(ElemType plainBlock[8], ElemType subKeys[16][48], ElemType cipherBlock[8])
{
    ElemType plainBits[64];
    ElemType copyRight[48];
    int cnt;
    Char8ToBit64(plainBlock,plainBits);
    //初始置换（IP置换）
    DES_IP_Transform(plainBits);
    //16轮迭代
    for(cnt = 0; cnt < 16; cnt++)
      {
        memcpy(copyRight,plainBits+32,32);
        //将右半部分进行扩展置换，从32位扩展到48位
       DES_E_Transform(copyRight);
        //将右半部分与子密钥进行异或操作
        DES_XOR(copyRight,subKeys[cnt],48);
        //异或结果进入S盒，输出32位结果
        DES_SBOX(copyRight);
        //P置换
        DES_P_Transform(copyRight);
        //将明文左半部分与右半部分进行异或
        DES_XOR(plainBits,copyRight,32);
        if(cnt != 15)
          {
            //最终完成左右部的交换
            DES_Swap(plainBits,plainBits+32);
           }
      }
    //逆初始置换（IP^1置换）
    DES_IP_1_Transform(plainBits);
    Bit64ToChar8(plainBits,cipherBlock);
    return 0;
}
//解密单个分组
int DES::DES_DecryptBlock(ElemType cipherBlock[8], ElemType subKeys[16][48],ElemType plainBlock[8])
{
    ElemType cipherBits[64];
    ElemType copyRight[48];
    int cnt;

    Char8ToBit64(cipherBlock,cipherBits);
    //初始置换（IP置换）
    DES_IP_Transform(cipherBits);
   //16轮迭代
    for(cnt = 15; cnt >= 0; cnt--)
      {
       memcpy(copyRight,cipherBits+32,32);
        //将右半部分进行扩展置换，从32位扩展到48位
        DES_E_Transform(copyRight);
        //将右半部分与子密钥进行异或操作
        DES_XOR(copyRight,subKeys[cnt],48);
        //异或结果进入S盒，输出32位结果
        DES_SBOX(copyRight);
        //P置换
        DES_P_Transform(copyRight);
        //将明文左半部分与右半部分进行异或
        DES_XOR(cipherBits,copyRight,32);
        if(cnt != 0)
          {
            //最终完成左右部的交换
            DES_Swap(cipherBits,cipherBits+32);
          }
       }
    //逆初始置换（IP^1置换）
    DES_IP_1_Transform(cipherBits);
    Bit64ToChar8(cipherBits, plainBlock);
       return 0;
}
  //---------------------------DES算法部分结束--------------------------
int DES::DES_Encrypt(const char *keyStr,const char *cipherFile,const char* Date)
{
    ElemType plainBlock[8],cipherBlock[8],keyBlock[8];
    ElemType bKey[64];
    ElemType subKeys[16][48];
    FILE *cipher;
    int count,m=0;
    int n;      //会出现的8个的次数
   // int num=0;
    if((cipher = fopen(cipherFile,"wb")) == NULL)
    {
        cout << "cannot write " <<cipherFile<<endl;
        return -1;
    }
    //DES算法密钥制造部分   设置密钥 将密钥转换为二进制流 生成子密钥
    memcpy(keyBlock,keyStr,8);
    Char8ToBit64(keyBlock,bKey);
    DES_MakeSubKeys(bKey,subKeys);

    count=strlen(Date);
    //cout<<count<<endl;
    n=count/8;
    for(int i=0;i<n;i++)
    {
        strncpy(plainBlock,Date+i*8,8);
        //cout<<Date+i*8<<endl;
        //cout<<plainBlock<<endl;
        DES_EncryptBlock(plainBlock,subKeys,cipherBlock);
        fwrite(cipherBlock,sizeof(char),8,cipher);
    }
    memset(plainBlock,0,8);
    if((m=count%8)!=0)
    {
        for(int i=0;i<m;i++)
        {
            plainBlock[i]=Date[n*8+i];
        }
        for(int i=m;i<8;i++)
        {
            plainBlock[i]='Z';
            //cout<<plainBlock<<endl;
        }
       // cout<<"plainBlock:"<<plainBlock<<endl;
        DES_EncryptBlock(plainBlock,subKeys,cipherBlock);
        fwrite(cipherBlock,sizeof(char),8,cipher);
    }
    fclose(cipher);
    return 0;
}
int DES::DES_Decrypt(const char *keyStr,const char *cipherFile,std::string &Date)
{
    string str;
    //Date="";
    FILE *cipher;
    ElemType plainBlock[8],cipherBlock[8],keyBlock[8],tempBlock[8]={'0'};
    ElemType bKey[64];
    ElemType subKeys[16][48];
    ElemType TurnBlock[8]={'0'};
    long temp;
    if((cipher = fopen(cipherFile,"rb")) == NULL)
    {
        cout << "cannot read "<<cipherFile<< endl;
        return -1;
    }
    memcpy(keyBlock,keyStr,8);
    Char8ToBit64(keyBlock,bKey);
    DES_MakeSubKeys(bKey,subKeys);
    fread(cipherBlock,sizeof(char),8,cipher);
    while(!feof(cipher))
    {
        //fread(cipherBlock,sizeof(char),8,cipher);
        //密文的字节数一定是8的整数倍
        DES_DecryptBlock(cipherBlock,subKeys,plainBlock);
        //cout<<"\n\n-----plainBlock----"<<plainBlock<<endl<<endl;
        /*for(int i=0;i<8;i++)
        {
            cout<<plainBlock[i];
        }*/
        for(int i=0;i<8;i++)
        {
            if(plainBlock[i]!='Z')
            {
                tempBlock[i]=plainBlock[i];
            }
            else
            {
                for(int m=0;m<i;m++)
                {
                    str+=plainBlock[m];
                }
                //cout<<str<<endl;
                Date+=str;
                str="";
                break;
            }
        }
       // cout<<endl<<endl;
        if(plainBlock[7]!='Z')
        {
            for(int i=0;i<8;i++)
            {
                str+=plainBlock[i];
            }
            //cout<<"\n\n----str-----"<<str<<endl<<endl;
            Date+=str;
            str="";
        }
        memset(tempBlock,0,8);
        memset(TurnBlock,0,8);
        //cout<<Date<<endl;
        fread(cipherBlock,sizeof(char),8,cipher);
    }
    return 0;
}
int DES::DES_Main(const char* Date,string &cstr)
  {
    char* LockFile="plainFile.txt";
    int count;
    count=strlen(Date);

    DES_Lock(Date,LockFile);
    DES_Unlock(cstr,LockFile);
    //cout<<cstr<<endl;
    //cout<<"------"<<endl;
    return 0;
  }
int DES::DES_Lock(const char* str,const char * CipherFile)
 {
     //cout<<"error1"<<endl<<endl;
     char *keystr="12345678";
     return DES_Encrypt(keystr,CipherFile,str);
 }
int DES::DES_Unlock(string &cstr,const char *CipherFile)
{
    char *keystr="12345678";
     //解密后，产生新的明文文件Pro_New.txt
     return DES_Decrypt(keystr,CipherFile,cstr);
}
