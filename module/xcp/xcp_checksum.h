#ifndef XCP_CHECKSUM_H
#define XCP_CHECKSUM_H
/* BUILD_CHECKSUM 本地算法（ASAM XCP 1.3 §7.5.1.13，Table 71/73/75）。
 * 用于和从机返回值比对。WORD/DWORD 元素按从机字节序读取（影响 ADD_22/24/44）。 */
#include <stdint.h>

#define XCP_CKS_ADD_11       0x01u
#define XCP_CKS_ADD_12       0x02u
#define XCP_CKS_ADD_14       0x03u
#define XCP_CKS_ADD_22       0x04u
#define XCP_CKS_ADD_24       0x05u
#define XCP_CKS_ADD_44       0x06u
#define XCP_CKS_CRC_16       0x07u
#define XCP_CKS_CRC_16_CITT  0x08u
#define XCP_CKS_CRC_32       0x09u
#define XCP_CKS_USER_DEFINED 0xFFu

/* 返回 0 成功；-1 类型未知/USER_DEFINED；-2 长度不满足对齐（ADD_2x 需 2 的倍数，ADD_44 需 4 的倍数） */
int xcp_checksum_compute(uint8_t type, const uint8_t* data, uint32_t len, int big_endian, uint32_t* out);
/* 该算法要求的字节对齐（1/2/4），未知类型返回 1 */
int xcp_checksum_align(uint8_t type);
const char* xcp_checksum_name(uint8_t type);

#endif
