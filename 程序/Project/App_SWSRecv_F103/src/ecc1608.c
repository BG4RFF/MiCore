/* ecc1608.c - (16,8) error correction code test program. Rev1.0 Nov.13,2004
 * Copyright (c) 2004 Takayuki Hosoda. All rights reserved.
 * http://www.finetune.co.jp/~lyuka/ mailto:lyuka@finetune.co.jp
 */
/* modified by eleqian 2014-5-23
 */
#include "ecc1608.h"

#define GENPOLY     0xeb80
#define BITMASK     0x8000
#define DATAMASK    0xff00
#define ECCMASK     0x00ff
#define CODEMASK    0xffff
#define CODELEN     16
#define DATALEN     8
#define ECCLEN      8
#define N2D         0x0100
#define N2E         0x0100

static const unsigned char ecc1608_enctbl[N2D] = {
    0x00, 0xd7, 0x79, 0xae, 0xf2, 0x25, 0x8b, 0x5c,
    0x33, 0xe4, 0x4a, 0x9d, 0xc1, 0x16, 0xb8, 0x6f,
    0x66, 0xb1, 0x1f, 0xc8, 0x94, 0x43, 0xed, 0x3a,
    0x55, 0x82, 0x2c, 0xfb, 0xa7, 0x70, 0xde, 0x09,
    0xcc, 0x1b, 0xb5, 0x62, 0x3e, 0xe9, 0x47, 0x90,
    0xff, 0x28, 0x86, 0x51, 0x0d, 0xda, 0x74, 0xa3,
    0xaa, 0x7d, 0xd3, 0x04, 0x58, 0x8f, 0x21, 0xf6,
    0x99, 0x4e, 0xe0, 0x37, 0x6b, 0xbc, 0x12, 0xc5,
    0x4f, 0x98, 0x36, 0xe1, 0xbd, 0x6a, 0xc4, 0x13,
    0x7c, 0xab, 0x05, 0xd2, 0x8e, 0x59, 0xf7, 0x20,
    0x29, 0xfe, 0x50, 0x87, 0xdb, 0x0c, 0xa2, 0x75,
    0x1a, 0xcd, 0x63, 0xb4, 0xe8, 0x3f, 0x91, 0x46,
    0x83, 0x54, 0xfa, 0x2d, 0x71, 0xa6, 0x08, 0xdf,
    0xb0, 0x67, 0xc9, 0x1e, 0x42, 0x95, 0x3b, 0xec,
    0xe5, 0x32, 0x9c, 0x4b, 0x17, 0xc0, 0x6e, 0xb9,
    0xd6, 0x01, 0xaf, 0x78, 0x24, 0xf3, 0x5d, 0x8a,
    0x9e, 0x49, 0xe7, 0x30, 0x6c, 0xbb, 0x15, 0xc2,
    0xad, 0x7a, 0xd4, 0x03, 0x5f, 0x88, 0x26, 0xf1,
    0xf8, 0x2f, 0x81, 0x56, 0x0a, 0xdd, 0x73, 0xa4,
    0xcb, 0x1c, 0xb2, 0x65, 0x39, 0xee, 0x40, 0x97,
    0x52, 0x85, 0x2b, 0xfc, 0xa0, 0x77, 0xd9, 0x0e,
    0x61, 0xb6, 0x18, 0xcf, 0x93, 0x44, 0xea, 0x3d,
    0x34, 0xe3, 0x4d, 0x9a, 0xc6, 0x11, 0xbf, 0x68,
    0x07, 0xd0, 0x7e, 0xa9, 0xf5, 0x22, 0x8c, 0x5b,
    0xd1, 0x06, 0xa8, 0x7f, 0x23, 0xf4, 0x5a, 0x8d,
    0xe2, 0x35, 0x9b, 0x4c, 0x10, 0xc7, 0x69, 0xbe,
    0xb7, 0x60, 0xce, 0x19, 0x45, 0x92, 0x3c, 0xeb,
    0x84, 0x53, 0xfd, 0x2a, 0x76, 0xa1, 0x0f, 0xd8,
    0x1d, 0xca, 0x64, 0xb3, 0xef, 0x38, 0x96, 0x41,
    0x2e, 0xf9, 0x57, 0x80, 0xdc, 0x0b, 0xa5, 0x72,
    0x7b, 0xac, 0x02, 0xd5, 0x89, 0x5e, 0xf0, 0x27,
    0x48, 0x9f, 0x31, 0xe6, 0xba, 0x6d, 0xc3, 0x14,
};

// 随机1bit/随机2bits/连续3bits错误/*，部分随机3bits错误*/
static const unsigned short ecc1608_errtbl[N2E] = {
    0x0000, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007,
    0x0008, 0x0009, 0x000a, 0xffff, 0x000c, 0xffff, 0x000e, 0x4040,
    0x0010, 0x0011, 0x0012, 0x0820, 0x0014, 0xffff, 0xffff, 0x01c0,
    0x0018, 0xffff, 0xffff, 0x2100, 0x001c, 0xe000, 0x8080, 0x1200,
    0x0020, 0x0021, 0x0022, 0x0810, 0x0024, 0x0500, 0x1040, 0xffff,
    0x0028, 0x5000, 0xffff, 0xffff, 0xffff, 0xffff, 0x0380, 0xffff,
    0x0030, 0x0802, 0x0801, 0x0800, 0xffff, 0xffff, 0x4200, 0x0804,
    0x0038, 0x0240, 0xffff, 0x0808, 0xffff, 0xffff, 0x2400, 0xffff,
    0x0040, 0x0041, 0x0042, 0xffff, 0x0044, 0xffff, 0x1020, 0x4008,
    0x0048, 0x8100, 0x0a00, 0x4004, 0x2080, 0x4002, 0x4001, 0x4000,
    0x0050, 0xffff, 0xa000, 0xffff, 0xffff, 0x1800, 0xffff, 0x0180,
    0xffff, 0x0220, 0xffff, 0xffff, 0x0700, 0xffff, 0xffff, 0x4010,
    0x0060, 0xffff, 0x1004, 0xffff, 0x1002, 0xffff, 0x1000, 0x1001,
    0xffff, 0x0210, 0xffff, 0xffff, 0x8400, 0xffff, 0x1008, 0x4020,
    0x0070, 0x0208, 0x0480, 0x0840, 0xffff, 0xffff, 0x1010, 0xffff,
    0x0201, 0x0200, 0xffff, 0x0202, 0x4800, 0x0204, 0xffff, 0xffff,
    0x0080, 0x0081, 0x0082, 0x6000, 0x0084, 0xffff, 0xffff, 0xffff,
    0x0088, 0xffff, 0xffff, 0x0600, 0x2040, 0xffff, 0x8010, 0xffff,
    0x0090, 0xffff, 0xffff, 0xffff, 0x1400, 0xffff, 0x8008, 0x0140,
    0x4100, 0x3800, 0x8004, 0xffff, 0x8002, 0x8003, 0x8000, 0x8001,
    0x00a0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0x1c00,
    0xffff, 0xffff, 0x3000, 0xffff, 0xffff, 0x8800, 0x0300, 0xffff,
    0xffff, 0x1100, 0x0440, 0x0880, 0xffff, 0x2200, 0xffff, 0xffff,
    0x0e00, 0xffff, 0xffff, 0xffff, 0xffff, 0x4400, 0x8020, 0xffff,
    0x00c0, 0x0c00, 0xffff, 0xffff, 0x2008, 0xffff, 0xffff, 0x0110,
    0x2004, 0xffff, 0xffff, 0xffff, 0x2000, 0x2001, 0x2002, 0x4080,
    0xc001, 0xc000, 0x0420, 0x0104, 0xffff, 0x0102, 0x0101, 0x0100,
    0xffff, 0xffff, 0xffff, 0xffff, 0x2010, 0xffff, 0x8040, 0x0108,
    0x00e0, 0xffff, 0x0410, 0xffff, 0x0900, 0x7000, 0x1080, 0x8200,
    0xffff, 0xffff, 0xffff, 0xffff, 0x2020, 0xffff, 0xffff, 0xffff,
    0x0402, 0xffff, 0x0400, 0x0401, 0xffff, 0xffff, 0x0404, 0x0120,
    0x9000, 0x0280, 0x0408, 0xffff, 0xffff, 0xffff, 0xffff, 0x2800,
};

unsigned short ecc1608_encode(unsigned char data)
{
    return (((unsigned short)data << 8) | ecc1608_enctbl[data]);
}

unsigned char ecc1608_getsyn(unsigned short code)
{
    return (code & ECCMASK) ^ ecc1608_enctbl[code >> ECCLEN];
}

unsigned char ecc1608_decode(unsigned char *data, unsigned short code)
{
    unsigned short status;

    status = ecc1608_errtbl[ecc1608_getsyn(code)];
    if (status == 0xffff) {
        return 0;
    }
    *data = (code ^ status) >> ECCLEN;
    if (status == 0x0000) {
        return 1;
    } else {
        return 2;
    }
}
