#include "hdinfo.h"
#include "sys/ioctl.h"
#include "sys/socket.h"
#include "netinet/in.h"
#include "net/if.h"
#include <cstdio>
#include <cstring>
#include <cstdlib>

#define BUF_SIZE 512

HDInfo::HDInfo()
{
    m_eax = m_ebx = m_ecx = m_edx = 0;
}
int HDInfo::getcpuid(unsigned char *id)
{
    //unsigned char id[17];
    memset(id, 0, 17);
    execCpuid(1);
    sprintf((char *)id, "%08X%08X", m_edx, m_eax);
    return 0;
}

void HDInfo::execCpuid(DWORD in)
{
    asm("cpuid"
    : "=a"(m_eax), "=b"(m_ebx), "=c"(m_ecx), "=d"(m_edx)
    : "a"(in)
    );
}

bool HDInfo::getMAC(unsigned char *mac)
{
    int sock;
    struct ifconf ifconf;
    char buf[BUF_SIZE];
    ifconf.ifc_len = BUF_SIZE;
    ifconf.ifc_buf = buf;

    char mac_addr[18];
    memset(mac_addr, 0, 18);

    if((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
//        cout << "创建套接字失败" << endl;
        return 1;
    }
    if(ioctl(sock, SIOCGIFCONF, &ifconf) == -1)
    {
//        cout << "无法获取网络接口列表" << endl;
        return 2;
    }

    struct ifreq *ifreq = (struct ifreq *)buf;

    for(int i = ifconf.ifc_len / (sizeof(struct ifreq)); i > 0; --i)
    {
        if(ioctl(sock, SIOCGIFFLAGS, ifreq) == -1)
        {
//            cout << "无法获取接口类型" << endl;
            return 3;
        }
        if(!(ifreq->ifr_flags & IFF_LOOPBACK))//  跳过loopback
        {
            if(ioctl(sock, SIOCGIFHWADDR, ifreq) == -1)
            {
//                cout << "无法获取MAC地址";
                return 4;
            }
            break;
        }
        ++ifreq;
    }

    // 储存MAC地址
    sprintf(mac_addr, "%02X:%02X:%02X:%02X:%02X:%02X",
            (unsigned char)ifreq->ifr_hwaddr.sa_data[0],
            (unsigned char)ifreq->ifr_hwaddr.sa_data[1],
            (unsigned char)ifreq->ifr_hwaddr.sa_data[2],
            (unsigned char)ifreq->ifr_hwaddr.sa_data[3],
            (unsigned char)ifreq->ifr_hwaddr.sa_data[4],
            (unsigned char)ifreq->ifr_hwaddr.sa_data[5]);
    for(int i = 0; i < 18; ++i)
    {
        mac[i] = (unsigned char)mac_addr[i];
    }
    return 0;
}
