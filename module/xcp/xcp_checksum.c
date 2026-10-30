#include "xcp_checksum.h"

static uint32_t rd(const uint8_t* p, int n, int be)
{
    if (n == 2) return be ? (uint32_t)((p[0] << 8) | p[1]) : (uint32_t)(p[0] | (p[1] << 8));
    if (be) return ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16) | ((uint32_t)p[2] << 8) | p[3];
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

int xcp_checksum_align(uint8_t type)
{
    if (type == XCP_CKS_ADD_22 || type == XCP_CKS_ADD_24) return 2;
    if (type == XCP_CKS_ADD_44) return 4;
    return 1;
}

const char* xcp_checksum_name(uint8_t t)
{
    switch (t) {
    case XCP_CKS_ADD_11: return "XCP_ADD_11";
    case XCP_CKS_ADD_12: return "XCP_ADD_12";
    case XCP_CKS_ADD_14: return "XCP_ADD_14";
    case XCP_CKS_ADD_22: return "XCP_ADD_22";
    case XCP_CKS_ADD_24: return "XCP_ADD_24";
    case XCP_CKS_ADD_44: return "XCP_ADD_44";
    case XCP_CKS_CRC_16: return "XCP_CRC_16";
    case XCP_CKS_CRC_16_CITT: return "XCP_CRC_16_CITT";
    case XCP_CKS_CRC_32: return "XCP_CRC_32";
    case XCP_CKS_USER_DEFINED: return "XCP_USER_DEFINED";
    default: return "?";
    }
}

int xcp_checksum_compute(uint8_t type, const uint8_t* d, uint32_t len, int be, uint32_t* out)
{
    uint32_t i, s = 0;
    int a = xcp_checksum_align(type), b;
    if ((len % (uint32_t)a) != 0) return -2;
    switch (type) {
    case XCP_CKS_ADD_11: for (i = 0; i < len; i++) s += d[i]; s &= 0xFFu; break;
    case XCP_CKS_ADD_12: for (i = 0; i < len; i++) s += d[i]; s &= 0xFFFFu; break;
    case XCP_CKS_ADD_14: for (i = 0; i < len; i++) s += d[i]; break;
    case XCP_CKS_ADD_22: for (i = 0; i < len; i += 2) s += rd(d + i, 2, be); s &= 0xFFFFu; break;
    case XCP_CKS_ADD_24: for (i = 0; i < len; i += 2) s += rd(d + i, 2, be); break;
    case XCP_CKS_ADD_44: for (i = 0; i < len; i += 4) s += rd(d + i, 4, be); break;
    case XCP_CKS_CRC_16: /* poly 0x8005 reflected = 0xA001, init 0, refin/refout, xorout 0 */
        for (i = 0; i < len; i++) {
            s ^= d[i];
            for (b = 0; b < 8; b++) s = (s & 1u) ? (s >> 1) ^ 0xA001u : (s >> 1);
        }
        break;
    case XCP_CKS_CRC_16_CITT: /* poly 0x1021, init 0xFFFF, 非反射 */
        s = 0xFFFFu;
        for (i = 0; i < len; i++) {
            s ^= (uint32_t)d[i] << 8;
            for (b = 0; b < 8; b++) s = (s & 0x8000u) ? ((s << 1) ^ 0x1021u) : (s << 1);
            s &= 0xFFFFu;
        }
        break;
    case XCP_CKS_CRC_32: /* 0x04C11DB7 反射 = 0xEDB88320 */
        s = 0xFFFFFFFFu;
        for (i = 0; i < len; i++) {
            s ^= d[i];
            for (b = 0; b < 8; b++) s = (s & 1u) ? (s >> 1) ^ 0xEDB88320u : (s >> 1);
        }
        s ^= 0xFFFFFFFFu;
        break;
    default:
        return -1;
    }
    *out = s;
    return 0;
}
