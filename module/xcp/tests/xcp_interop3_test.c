/* 小序会话层 ↔ 小微 mock 从机：CONNECT/STATUS/GET_ID/UPLOAD/DOWNLOAD/CHECKSUM/MODIFY_BITS/Seed&Key */
#include "xcp_session.h"
#include "xcp_tl.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static int fails, passes;
#define CK(c,m) do{ if(c) {passes++;printf("PASS %s\n",m);} else {printf("FAIL %s (rc=%d %s)\n",m,rc,xcp_strerror(rc));fails++;} }while(0)
static int sk(void*u,uint8_t r,const uint8_t*seed,int sl,uint8_t*key,int*kl){int i;(void)u;(void)r;for(i=0;i<sl;i++)key[i]=(uint8_t)(seed[i]^0xFF);*kl=sl;return 0;}
int main(int argc,char**v){ int err=0,rc=0; XcpSession*s; XcpSessionCfg cfg; memset(&cfg,0,sizeof cfg);
 int ag=atoi(v[2]), mot=atoi(v[3]); (void)argc;
 XcpTl*tl=xcp_tl_tcp_open("127.0.0.1",atoi(v[1]),1000,&err); if(!tl){puts("FAIL open");return 1;}
 xcp_session_create(&s,tl,&cfg); xcp_set_seed_key(s,sk,NULL);
 XcpConnInfo ci; rc=xcp_connect(s,0,&ci); CK(rc==0&&ci.ag==ag&&ci.byte_order==mot,"CONNECT 格式");
 XcpStatus st; rc=xcp_get_status(s,&st); CK(rc==0,"GET_STATUS");
 uint8_t prot=0xFF; rc=xcp_unlock_all(s,&prot); CK(rc==0&&prot==0,"Seed&Key 全部解锁");
 XcpId id; rc=xcp_get_id(s,1,&id); CK(rc==0&&id.len==13,"GET_ID"); xcp_id_free(&id);
 static uint8_t src[2400], back[2400]; int i; for(i=0;i<2400;i++)src[i]=(uint8_t)(i*31+7);
 uint32_t A=0x20001000, n=2400/ag;
 rc=xcp_download(s,0,A,n,src); CK(rc==0,"DOWNLOAD 2400 B");
 memset(back,0,sizeof back); rc=0; { uint32_t off=0; while(off<n && rc==0){ uint32_t lim=(uint32_t)(200/ag); uint32_t k=n-off>lim?lim:n-off; rc=xcp_upload(s,0,A+off*ag,k,back+off*ag); off+=k; } }
 CK(rc==0&&memcmp(src,back,2400)==0,"UPLOAD 回读 2400 B 一致");
 XcpChecksumResult cr; rc=xcp_checksum_verify(s,0,A,src,n,NULL,NULL,&cr); CK(rc==0,"BUILD_CHECKSUM 校验一致");
 src[1234]^=1; rc=xcp_checksum_verify(s,0,A,src,n,NULL,NULL,&cr); CK(rc!=0,"篡改后 CHECKSUM 不一致"); src[1234]^=1;
 uint8_t sd[4]={1,2,3,4}; rc=xcp_short_download(s,0,0x20002000,(uint8_t)(4/ag),sd); uint8_t sb[4]={0}; if(!rc) rc=xcp_upload(s,0,0x20002000,4/ag,sb); CK(rc==0&&!memcmp(sd,sb,4),"SHORT_DOWNLOAD");
 rc=xcp_disconnect(s); CK(rc==0,"DISCONNECT"); xcp_session_destroy(s);
 printf("%d pass %d fail\n",passes,fails); return fails; }
