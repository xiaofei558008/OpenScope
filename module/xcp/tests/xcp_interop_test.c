#include "xcp_session.h"
#include "xcp_tl.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static int fails;
#define CK(c,m) do{ if(c) printf("PASS %s\n",m); else {printf("FAIL %s\n",m);fails++;} }while(0)
int main(int c,char**v){ int err=0,rc; XcpSession*s; XcpSessionCfg cfg; memset(&cfg,0,sizeof cfg); int ag=atoi(v[2]), mot=atoi(v[3]); (void)c;
 XcpTl*tl=xcp_tl_tcp_open("127.0.0.1",atoi(v[1]),1000,&err); if(!tl){puts("FAIL open");return 1;}
 xcp_session_create(&s,tl,&cfg);
 XcpConnInfo ci; rc=xcp_connect(s,0,&ci); printf(" connect rc=%d cto=%u dto=%u bo=%u ag=%u blk=%u\n",rc,ci.max_cto,ci.max_dto,ci.byte_order,ci.ag,ci.slave_block_mode);
 CK(rc==0&&ci.ag==ag&&ci.byte_order==mot,"CONNECT 格式");
 XcpStatus st; CK(xcp_get_status(s,&st)==0,"GET_STATUS");
 XcpId id; rc=xcp_get_id(s,1,&id); printf(" getid rc=%d len=%u\n",rc,id.len);CK(rc==0&&id.len>0,"GET_ID"); xcp_id_free(&id);
 unsigned char b[1024]; uint32_t n=200/ag; rc=xcp_upload(s,0,0x20000000,n,b); printf(" upload %u elems rc=%d\n",n,rc); CK(rc==0,"块 UPLOAD 200B");
 uint32_t c1=mot?((uint32_t)b[0]<<24|b[1]<<16|b[2]<<8|b[3]):((uint32_t)b[3]<<24|b[2]<<16|b[1]<<8|b[0]);
 rc=xcp_upload(s,0,0x20000000,4/ag,b); uint32_t c2=mot?((uint32_t)b[0]<<24|b[1]<<16|b[2]<<8|b[3]):((uint32_t)b[3]<<24|b[2]<<16|b[1]<<8|b[0]); printf(" counter %u -> %u\n",c1,c2); CK(rc==0&&c2>=c1&&c2-c1<100000,"计数器字节序合理");
 rc=xcp_short_upload(s,0,0x20000004,(uint8_t)(4/ag),b); CK(rc==0,"SHORT_UPLOAD");
 rc=xcp_upload(s,0,0x10000000,4/ag,b); CK(rc!=0,"越界报错");
 CK(xcp_disconnect(s)==0,"DISCONNECT"); xcp_session_destroy(s); return fails; }
