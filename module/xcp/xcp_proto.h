#ifndef XCP_PROTO_H
#define XCP_PROTO_H

/* ASAM XCP 协议层 V1.3（命令号与 Vector XCPlite 一致）。
 * 本文件只放常量，主从双方共用。字节序为 Intel（小端）。 */

#include <stdint.h>

#define XCP_PID_RES   0xFFu
#define XCP_PID_ERR   0xFEu
#define XCP_PID_EV    0xFDu
#define XCP_PID_SERV  0xFCu

#define XCP_CMD_CONNECT                 0xFFu
#define XCP_CMD_DISCONNECT              0xFEu
#define XCP_CMD_GET_STATUS              0xFDu
#define XCP_CMD_SYNCH                   0xFCu
#define XCP_CMD_GET_COMM_MODE_INFO      0xFBu
#define XCP_CMD_GET_ID                  0xFAu
#define XCP_CMD_SET_REQUEST             0xF9u
#define XCP_CMD_GET_SEED                0xF8u
#define XCP_CMD_UNLOCK                  0xF7u
#define XCP_CMD_SET_MTA                 0xF6u
#define XCP_CMD_UPLOAD                  0xF5u
#define XCP_CMD_SHORT_UPLOAD            0xF4u
#define XCP_CMD_BUILD_CHECKSUM          0xF3u
#define XCP_CMD_TRANSPORT_LAYER_CMD     0xF2u
#define XCP_CMD_USER_CMD                0xF1u
#define XCP_CMD_DOWNLOAD                0xF0u
#define XCP_CMD_DOWNLOAD_NEXT           0xEFu
#define XCP_CMD_DOWNLOAD_MAX            0xEEu
#define XCP_CMD_SHORT_DOWNLOAD          0xEDu
#define XCP_CMD_MODIFY_BITS             0xECu
#define XCP_CMD_SET_CAL_PAGE            0xEBu
#define XCP_CMD_GET_CAL_PAGE            0xEAu
#define XCP_CMD_GET_PAG_PROCESSOR_INFO  0xE9u
#define XCP_CMD_GET_SEGMENT_INFO        0xE8u
#define XCP_CMD_GET_PAGE_INFO           0xE7u
#define XCP_CMD_SET_SEGMENT_MODE        0xE6u
#define XCP_CMD_GET_SEGMENT_MODE        0xE5u
#define XCP_CMD_COPY_CAL_PAGE           0xE4u
#define XCP_CMD_CLEAR_DAQ_LIST          0xE3u
#define XCP_CMD_SET_DAQ_PTR             0xE2u
#define XCP_CMD_WRITE_DAQ               0xE1u
#define XCP_CMD_SET_DAQ_LIST_MODE       0xE0u
#define XCP_CMD_GET_DAQ_LIST_MODE       0xDFu
#define XCP_CMD_START_STOP_DAQ_LIST     0xDEu
#define XCP_CMD_START_STOP_SYNCH        0xDDu
#define XCP_CMD_GET_DAQ_CLOCK           0xDCu
#define XCP_CMD_READ_DAQ                0xDBu
#define XCP_CMD_GET_DAQ_PROCESSOR_INFO  0xDAu
#define XCP_CMD_GET_DAQ_RESOLUTION_INFO 0xD9u
#define XCP_CMD_GET_DAQ_LIST_INFO       0xD8u
#define XCP_CMD_GET_DAQ_EVENT_INFO      0xD7u
#define XCP_CMD_FREE_DAQ                0xD6u
#define XCP_CMD_ALLOC_DAQ               0xD5u
#define XCP_CMD_ALLOC_ODT               0xD4u
#define XCP_CMD_ALLOC_ODT_ENTRY         0xD3u
#define XCP_CMD_PROGRAM_START           0xD2u
#define XCP_CMD_PROGRAM_CLEAR           0xD1u
#define XCP_CMD_PROGRAM                 0xD0u
#define XCP_CMD_PROGRAM_RESET           0xCFu
#define XCP_CMD_GET_PGM_PROCESSOR_INFO  0xCEu
#define XCP_CMD_GET_SECTOR_INFO         0xCDu
#define XCP_CMD_PROGRAM_PREPARE         0xCCu
#define XCP_CMD_PROGRAM_FORMAT          0xCBu
#define XCP_CMD_PROGRAM_NEXT            0xCAu
#define XCP_CMD_PROGRAM_MAX             0xC9u
#define XCP_CMD_PROGRAM_VERIFY          0xC8u
#define XCP_CMD_WRITE_DAQ_MULTIPLE      0xC7u

#define XCP_ERR_CMD_SYNCH        0x00u
#define XCP_ERR_CMD_BUSY         0x10u
#define XCP_ERR_DAQ_ACTIVE       0x11u
#define XCP_ERR_PGM_ACTIVE       0x12u
#define XCP_ERR_CMD_UNKNOWN      0x20u
#define XCP_ERR_CMD_SYNTAX       0x21u
#define XCP_ERR_OUT_OF_RANGE     0x22u
#define XCP_ERR_WRITE_PROTECTED  0x23u
#define XCP_ERR_ACCESS_DENIED    0x24u
#define XCP_ERR_ACCESS_LOCKED    0x25u
#define XCP_ERR_PAGE_NOT_VALID   0x26u
#define XCP_ERR_MODE_NOT_VALID   0x27u
#define XCP_ERR_SEGMENT_NOT_VALID 0x28u
#define XCP_ERR_SEQUENCE         0x29u
#define XCP_ERR_DAQ_CONFIG       0x2Au
#define XCP_ERR_MEMORY_OVERFLOW  0x30u
#define XCP_ERR_GENERIC          0x31u
#define XCP_ERR_VERIFY           0x32u
#define XCP_ERR_RES_TEMP_NOT_ACC 0x33u
#define XCP_ERR_SUBCMD_UNKNOWN   0x34u

#define XCP_RES_CAL_PAG  0x01u
#define XCP_RES_DAQ      0x04u
#define XCP_RES_STIM     0x08u
#define XCP_RES_PGM      0x10u

/* COMM_MODE_BASIC：小端、字节寻址、从机块传输、可选命令 */
#define XCP_COMM_OPTIONAL     0x80u
#define XCP_COMM_SLAVE_BLOCK  0x40u
#define XCP_CMB               (XCP_COMM_OPTIONAL | XCP_COMM_SLAVE_BLOCK)

#define XCP_PROTOCOL_VERSION  0x13u /* 1.3 */
#define XCP_TRANSPORT_VERSION 0x01u

#define XCP_MAX_CTO           64
#define XCP_MAX_DTO           64
#define XCP_MAX_DAQ           8
#define XCP_MAX_ODT           8
#define XCP_MAX_ODT_ENTRY     8
#define XCP_MAX_EVENT         2
#define XCP_CHECKSUM_ADD_14   0x03u

/* 会话状态 */
#define XCP_SS_STORE_CAL   0x01u
#define XCP_SS_STORE_DAQ   0x04u
#define XCP_SS_CLEAR_DAQ   0x08u
#define XCP_SS_DAQ_RUNNING 0x40u

/* DAQ 列表模式 */
#define XCP_DAQ_FLAG_SELECTED  0x01u
#define XCP_DAQ_FLAG_DIRECTION 0x02u /* 1 = STIM */
#define XCP_DAQ_FLAG_TIMESTAMP 0x10u
#define XCP_DAQ_FLAG_RUNNING   0x40u

#define XCP_DAQ_START_STOP     0x00u
#define XCP_DAQ_START_START    0x01u
#define XCP_DAQ_START_SELECT   0x02u
#define XCP_DAQ_SYNCH_STOP_ALL 0x00u
#define XCP_DAQ_SYNCH_START_SEL 0x01u
#define XCP_DAQ_SYNCH_STOP_SEL 0x02u

/* 时间戳：4 字节，单位 1µs（size=4，unit=3<<4） */
#define XCP_TS_MODE  (4u | (3u << 4))

#define XCP_ID_TEXT "OpenScope-XCP"

static __inline uint16_t xcp_rd16(const uint8_t* p)
{
    return (uint16_t)(p[0] | ((uint16_t)p[1] << 8));
}
static __inline uint32_t xcp_rd32(const uint8_t* p)
{
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}
static __inline void xcp_wr16(uint8_t* p, uint16_t v)
{
    p[0] = (uint8_t)v;
    p[1] = (uint8_t)(v >> 8);
}
static __inline void xcp_wr32(uint8_t* p, uint32_t v)
{
    p[0] = (uint8_t)v;
    p[1] = (uint8_t)(v >> 8);
    p[2] = (uint8_t)(v >> 16);
    p[3] = (uint8_t)(v >> 24);
}

#endif
