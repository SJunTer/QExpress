#ifndef HDINFO_H
#define HDINFO_H

typedef unsigned long DWORD;
typedef unsigned short WORD;

class HDInfo
{
public:
    HDInfo();
    int getcpuid(unsigned char *id); // 获取CPUID
    bool getMAC(unsigned char *mac);

private:
    void execCpuid(DWORD in);
    DWORD m_eax;
    DWORD m_ebx;
    DWORD m_ecx;
    DWORD m_edx;
};

#endif // HDINFO_H
