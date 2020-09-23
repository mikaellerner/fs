/*
 * Copyright (c) 2020 NVI, Inc.
 *
 * This file is part of VLBI Field System
 * (see http://github.com/nvi-inc/fs).
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
/* dbbc3 core3h_modex SNAP command display */

#include <stdio.h>
#include <string.h>
#include <sys/types.h>

#include "../include/params.h"
#include "../include/fs_types.h"
#include "../include/fscom.h"
#include "../include/shm_addr.h"

#define MAX_OUT 256
#define BUFSIZE 2048

void dbbc3_core3h_modex_dis(command,itask,ip)
    struct cmd_ds *command;
    int itask;
    int ip[5];
{
    int ierr, count, i;
    char output[MAX_OUT];
    int rtn1;    /* argument for cls_rcv - unused */
    int rtn2;    /* argument for cls_rcv - unused */
    int msgflg=0;  /* argument for cls_rcv - unused */
    int save=0;    /* argument for cls_rcv - unused */
    int nchars;
    int out_class=0;
    int out_recs=0;
    char inbuf[BUFSIZE];
    int kcom;
    int iclass, nrecs;
    struct dbbc3_core3h_modex_cmd lclc;
    struct dbbc3_core3h_modex_mon lclm;

    kcom= command->argv[0] != NULL &&
        *command->argv[0] == '?' && command->argv[1] == NULL;

    if((!kcom) && command->equal == '=') {
        ierr=logmsg_dbbc3(output,command,ip);
        if(ierr!=0) {
            ierr+=-450;
            goto error2;
        }
        return;
    } else if(kcom) {
        memcpy(&lclc,&shm_addr->dbbc3_core3h_modex[itask-30],sizeof(lclc));
    } else {
        int mask = 0;
        int rate = 0;
        int output = 0;
        iclass=ip[0];
        nrecs=ip[1];
        for (i=0;i<nrecs;i++) {
            char *ptr;
            if ((nchars =
                        cls_rcv(iclass,inbuf,BUFSIZE,&rtn1,&rtn2,msgflg,save)) <= 0) {
                ierr = -401;
                goto error;
            }
            if(!mask && NULL != strstr(inbuf,"VSI input bitmask")) {
                if(0!=dbbc3_core3h_2_vsi_bitmask(inbuf,&lclc)) {
                    ierr=-501;
                    goto error;
                }
                mask=TRUE;
            } else if(!rate && NULL != strstr(inbuf,"VSI sample rate")) {
                if(0!=dbbc3_core3h_2_vsi_samplerate(inbuf,&lclc,&lclm)) {
                    ierr=-502;
                    goto error;
                }
                rate = TRUE;
            } else if(!output && NULL != strstr(inbuf," Output      ")) {
                if(0!=dbbc3_core3h_2_output(inbuf,&lclc)) {
                    ierr=-503;
                    goto error;
                }
                output = TRUE;
            }
        }
        if (!mask) {
            ierr = -511;
            goto error;
        } else if (!rate) {
            ierr = -512;
            goto error;
        } else if (!output) {
            ierr = -513;
            goto error;
        }
    }
    /* format output buffer */

    strcpy(output,command->name);
    strcat(output,"/");

    if(0 == lclc.set) {
        strcat(output,"stopped");
        goto send;
    }
    count=0;
    while( count>= 0) {
        if (count > 0) strcat(output,",");
        count++;
        dbbc3_core3h_modex_enc(output,&count,&lclc,itask-30);
    }

    /* this a rare command that has a monitor '?' value from shared memory */

    if(kcom) {
        m5state_init(&lclm.clockrate.state);
        lclm.clockrate.clockrate=shm_addr->m5b_crate*1.0e6+0.5;
        lclm.clockrate.state.known=1;
    }
    count=0;
    while( count>= 0) {
        if (count > 0) strcat(output,",");
        count++;
        dbbc3_core3h_modex_mon(output,&count,&lclm);
    }

    if(strlen(output)>0) output[strlen(output)-1]='\0';

send:
    for (i=0;i<5;i++)
        ip[i]=0;
    cls_snd(&ip[0],output,strlen(output),0,0);
    ip[1]=1;
    return;

error:
    if(i!=nrecs-1)
        cls_clr(iclass);
    ip[0]=0;
    ip[1]=0;
error2:
    ip[2]=ierr;
    ip[4]=0;
    memcpy(ip+3,"dr",2);
    return;
}


