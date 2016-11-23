#include "sha256.h"

using namespace std;

inline uint rrot(uint val, int pos)
{
    pos %= 32;
    return ( val >> pos ) | ( val << (32 - pos) );
}

void sha_256(char *digest, char *str)
{
    uint orilen = strlen(str);
    uint chunks_count = (orilen+9)/64;
    if( (orilen+9)%64!=0 ) ++chunks_count;
    ull total_count = orilen * 8;
    uint *filldata = new uint[ chunks_count*16 ];
    memset(filldata, 0, sizeof(uint)*chunks_count*16);

    ubyte *pdata = (ubyte *)filldata;
    uint idx;
    for (idx = 0; idx < orilen; ++idx)
    {
        pdata[idx+3-idx%4-idx%4] = str[idx];
    }
    pdata[idx+3-idx%4-idx%4] = 0x80;
    ++idx;

    ubyte *pbyte = (ubyte *)&total_count;
    for (uint i = 0, j = 64*chunks_count - 4; i < 8; ++i)
    {
        if (i==4) j -= 8;
        pdata[j + i] = pbyte[i];
    }

    uint H[8],a,b,c,d,e,f,g,h, w[80], s[2];
    H[0] = 0x6a09e667;
    H[1] = 0xbb67ae85;
    H[2] = 0x3c6ef372;
    H[3] = 0xa54ff53a;
    H[4] = 0x510e527f;
    H[5] = 0x9b05688c;
    H[6] = 0x1f83d9ab;
    H[7] = 0x5be0cd19;

    for (uint i = 0; i < chunks_count*16; i += 16)
    {
        uint *puint = &filldata[i];
        for (uint j = 0; j < 16; ++j)
        {
            w[j] = puint[j];
        }
        for (uint j = 16; j < 64; ++j)
        {
            s[0] = ( rrot(w[j - 15], 7) ) ^ ( rrot(w[j - 15], 18) ) ^ ( w[j - 15]>>3 );
            s[1] = ( rrot(w[j - 2], 17) ) ^ ( rrot(w[j - 2], 19) ) ^ ( w[j - 2]>>10 );
            w[j] = w[j - 16] + s[0] + w[j - 7] + s[1];
        }
        a = H[0], b = H[1], c = H[2], d = H[3], e = H[4], f = H[5], g = H[6], h = H[7];

        for (uint j = 0; j < 64; ++j)
        {
            uint maj, t[3], ch;
            s[0] = ( rrot(a,2) ) ^ (rrot(a,13)) ^ (rrot(a,22));
            maj = (a & b) ^ (a & c) ^ (b & c);
            t[2] = s[0] + maj;
            s[1] = rrot(e, 6) ^ rrot(e, 11) ^ rrot(e, 25);
            ch = (e & f) ^ ((~e) & g);
            t[1] = h + s[1] + ch + k[j] + w[j];

            h = g, g = f, f = e, e = d + t[1], d = c, c = b, b = a, a = t[1] + t[2];
        }
        H[0] += a, H[1] += b, H[2] += c, H[3] += d, H[4] += e, H[5] += f, H[6] += g, H[7] += h;
    }
    for (int i = 0; i < 8; ++i)
    {
        sprintf(digest+i*8, "%08x", H[i]);
    }
    delete[] filldata;
}
