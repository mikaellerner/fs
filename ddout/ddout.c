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
#include <memory.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include "../include/params.h"
#include "../include/fs_types.h"
#include "../include/fscom.h"

#define NULLPTR (char *) 0
#define PERMISSIONS 0664
#define MAX_BUF 1024
/* not Y10K compliant */
#define FIRST_CHAR 21

extern struct fscom *shm_addr;

main()
{
    int i;
    int cls_rcv(),fserr_rcv();
    int kp=0, kack=0, kxd=FALSE, kxl=FALSE, fd=-1, kpd=FALSE, knd=FALSE;
    int iwl, iw1, iwm;
    char *llogndx;
    int irga;
    int ip[5];
    char lnamef[65];
    char ibur[150];
    char buf[MAX_BUF+2];
    char buf2[MAX_BUF+2];
    char *iwhs, *iwhe;
    char bul[MAX_BUF+2];
    char llog0[9];
    char sllog[9], sllog0[9];
    int rtn1, rtn2, status, bufl, bull, rtn1f, rtn2f;
    int irgb, iburl;
    char *ich, *cp1, *cp2, ch, iwhat[5], *ptrs, *prtn1;
    int class;
    int offset;
    void dxpm();
    int kdebug;
    char *st;
    int kpcald;
    char ierrch[2];
    int ierrnum;
    struct list {
      char ch[2];
      int num;
      struct list *next;
      struct list *previous;
      char *string;
      char *example;
      int on;
      int count;
    } *last = NULL;
    struct list *first=NULL;
    struct list *ptr;
    int display, count;
    char *offon[ ]= {"off","on"};
    int knl=FALSE;
    unsigned last_sync,now;
    int skd_run_to();
    int serverfd;
    char* serverfdst;

    serverfd = -1;
    serverfdst = getenv("FS_SERVER_LOG_FD");
    if (serverfdst && *serverfdst) {
        serverfd  = atoi(serverfdst);
    }



/* SECTION 1 */
    
    setup_ids();
    skd_set_return_name("ddout");
    lnamef[0]=0;
    umask(0);

/* SECTION 2 */

    llog0[0]=0;
    rte_sleep(100); /* let boss print its messages first */

Messenger:
    /* get next message */
    /* ddout and any programs it waits for, specifically fserr
     * must not call cls_snd() because this to can lead to a deadlock
     * if class system fills
     */
    
    status = cls_rcv(shm_addr->iclbox,buf,MAX_BUF,&rtn1,&rtn2,0,1);
    bufl = status;
    /* set buf up as a string */
    buf[bufl]=0;
    strcpy(bul,buf);
    bull=bufl;
    cp2 = (char *) &rtn2;
    prtn1 = (char *) &rtn1;
    if (memcmp(cp2,"dn",2)==0){ /* extended display on */
      kxd = TRUE;
      goto Messenger;
    }
    if (memcmp(cp2,"df",2)==0){ /* extended display off */
      kxd = FALSE;
      goto Messenger;
    }
    if (memcmp(cp2,"ln",2)==0){ /* extended logging on */
      kxl = TRUE;
      goto Messenger;
    }
    if (memcmp(cp2,"lf",2)==0){ /* extended logging off */
      kxl = FALSE;
      goto Messenger;
    }
    if (memcmp(cp2,"to",2)==0){ /* raw display output with new-line */
      printf("%s\n", buf);
      goto Messenger;
    }
    if (memcmp(cp2,"tr",2)==0){ /* raw display output without new-line */
      printf("%s", buf);
      goto Messenger;
    }
    if (memcmp(cp2,"pn",2)==0) { /* tpicd display on */
      kpd = TRUE;
      goto Messenger;
    }
    if (memcmp(cp2,"pf",2)==0) { /* tpocd display off */
      kpd = FALSE;
      goto Messenger;
    }
    if (memcmp(cp2,"tn",2)==0) { /* TNX on */
      short ix, iy;
      memcpy(&ix,buf+2,2);
      memcpy(&iy,buf+4,2);
      for(ptr=last;ptr!=NULL;ptr=ptr->previous) {
	if(ptr->num == ix && memcmp(ptr->ch,buf,2)==0) {
	  if(iy == 0) {
	    if(ptr->count==1) {
	      if(ptr->on == 1) {
		logit(NULL,-311,"bo");
		goto Messenger;
	      }
	      ptr->on=1;
	      break;
	    } else {
	      sprintf(buf2,"tnx/more than one %2.2s,%d occurred, use 'tnx=%2.2s,%d,on,#num' to select from list below",ptr->ch,ix,ptr->ch,ix);
	      logitf(buf2);
	      for(ptr=first;ptr!=NULL;ptr=ptr->next) {
		if(ptr->num == ix && memcmp(ptr->ch,buf,2)==0) {
		  if(ptr->example==NULL)
		    sprintf(buf2,"tnx/%2.2s,%d,%s,#%d,%s",
			    ptr->ch,ptr->num,offon[ptr->on],
			    ptr->count,ptr->string);
		  else
		    sprintf(buf2,"tnx/%2.2s,%d,%s,#%d,%s,%s",
			    ptr->ch,ptr->num,offon[ptr->on],
			    ptr->count,ptr->string,ptr->example);
		  logitf(buf2);
		}
	      }
	      goto Messenger;
	    }
	  } else if(ptr->count == iy || iy < 0) {
	      if(iy > 0 && ptr->on == 1) {
		logit(NULL,-311,"bo");
		goto Messenger;
	      }
	      ptr->on=1;
	      if(iy > 0 || ptr->count == 1)
		break;
	  }
	}
      }
      if(ptr == NULL) { /* not found */
	logit(NULL,-304,"bo");
	goto Messenger;
      }
      goto Messenger;
    }
    if (memcmp(cp2,"tf",2)==0) { /* TNX off */
      short ix, iy;
      memcpy(&ix,buf+2,2);
      memcpy(&iy,buf+4,2);
      for(ptr=last;ptr!=NULL;ptr=ptr->previous) {
	if(ptr->num == ix && memcmp(ptr->ch,buf,2)==0) {
	  if(iy == 0) {     
	    if(ptr->count==1) {
	      if(ptr->on == 0) {
		logit(NULL,-312,"bo");
		goto Messenger;
	      }
	      ptr->on=0;
	      break;
	    } else {
	      sprintf(buf2,"tnx/more than one %2.2s,%d, exists, use 'tnx=%2.2s,%d,off,#num' to select from list below",ptr->ch,ix,ptr->ch,ix);
	      logitf(buf2);
	      for(ptr=first;ptr!=NULL;ptr=ptr->next) {
		if(ptr->num == ix && memcmp(ptr->ch,buf,2)==0) {
		  if(ptr->example==NULL)
		    sprintf(buf2,"tnx/%2.2s,%d,%s,#%d,%s",
			    ptr->ch,ptr->num,offon[ptr->on],
			    ptr->count,ptr->string);
		  else
		    sprintf(buf2,"tnx/%2.2s,%d,%s,#%d,%s,%s",
			    ptr->ch,ptr->num,offon[ptr->on],
			    ptr->count,ptr->string,ptr->example);
		  logitf(buf2);
		}
	      }
	      goto Messenger;
	    }
	  } else if(ptr->count == iy || iy < 0) {
	      if(iy > 0 && ptr->on == 0) {
		logit(NULL,-312,"bo");
		goto Messenger;
	      }
	      ptr->on=0;
	      if(iy > 0 || ptr->count == 1)
		break;
	  }
	}
      }
      if(ptr == NULL) { /* not found */
	logit(NULL,-303,"bo");
	goto Messenger;
      }
      goto Messenger;
    }
    if (memcmp(cp2,"tl",2)==0) {  /* TNX list */
      int some=0;
      for(ptr=first;ptr!=NULL;ptr=ptr->next) {
	if(ptr->on == 0) {
	  if(ptr->example==NULL)
	    sprintf(buf,"tnx/%2.2s,%d,%s,#%d,%s",
		    ptr->ch,ptr->num,offon[ptr->on],
		    ptr->count,ptr->string);
	  else
	    sprintf(buf,"tnx/%2.2s,%d,%s,#%d,%s,%s",
		    ptr->ch,ptr->num,offon[ptr->on],
		    ptr->count,ptr->string,ptr->example);
	  logitf(buf);
	  some=1;
	}
      }
      if(some==0)
	logitf("tnx/disabled");

      goto Messenger;
    }
   
/* SECTION 3 */

    if(memcmp(cp2,"nl",2)==0 || rtn2 == -1){
      knl=TRUE;
      clear_rxgain_files_log();
      if (fd >=0) {
	fd=recover_log(lnamef,fd);  /* recover log if necessary */
	if(close(fd) < 0) {
	  shm_addr->abend.other_error=1;
	  perror("!! help! ** closing file, ddout");
          play_wav(1);
	}
      }

      if(rtn2 == -1)
	goto Bye;

      strcpy(lnamef,"/usr2/log/");
      llogndx = memccpy(sllog, shm_addr->LLOG, ' ', 8);
      if(llogndx!=NULLPTR) {
	if(llogndx!=sllog)
	  *(llogndx-1)=0;
	else
	  llogndx[0]=0;
      } else
	sllog[8]=0;

      strcat(lnamef, sllog);
      strcat(lnamef, ".log");
      fd = open(lnamef, O_RDWR|O_CREAT,PERMISSIONS);
      rte_rawt(&last_sync);
      if (fd < 0) {  /* if open/create failed, try recovering */
	shm_addr->abend.other_error=1;
	fprintf(stderr,
		"\007!! help! ** error opening/creating log file %.8s\n",
		sllog);
	play_wav(1);
	perror("!! help! ** ddout");
	  
	/* try previous log file now */
	llogndx = memccpy(sllog0,llog0, ' ', 8);
	if(llogndx!=NULLPTR) {
	  if(llogndx!=sllog0)
	    *(llogndx-1)=0;
	  else
	    llogndx[0]=0;
	} else
	  sllog0[8]=0;
	if (strcmp(sllog, sllog0)!=0 && llog0[0]!=0) {
	  strcpy(sllog, sllog0);
	  strcpy(lnamef, "/usr2/log/");
	  strcat(lnamef, sllog);
	  strcat(lnamef, ".log");
	  fprintf(stderr,
		  "\007!! help! ** now trying to re-open log file %.8s\n",
		  sllog);
	  play_wav(1);
	  fd = open(lnamef, O_RDWR|O_CREAT,PERMISSIONS);
	  rte_rawt(&last_sync);
	  if(fd >=0) {
	    memcpy(shm_addr->LLOG,llog0,8);
	    fprintf(stderr,
		    "\007!! help! ** succesfully re-opened log file %.8s\n",
		    sllog);
	    play_wav(1);
	  } else {
	    fprintf(stderr,
		    "\007!! help! ** error re-opening log file %.8s\n",sllog);
	    play_wav(1);
	    perror("!! help! ** ddout");
	  }
	}
      }
      if(fd >= 0) {
	memcpy(llog0, shm_addr->LLOG,8);
	offset= lseek(fd, 0L, SEEK_END);
	if (offset > 0) {
	  offset=lseek(fd, -1L, SEEK_END);
	  if (offset < 0) {
	    shm_addr->abend.other_error=1;
	    perror("!! help! ** error positioning log file, ddout");
            play_wav(1);
	  }
	  read(fd,&ch,1);
	  if(ch != '\n')
	    write(fd, "\n", 1);
	  if(offset > 1000L*1000L*100L)
	    logit(NULL,-999,"bo");
	} else if(offset < 0) {
	  shm_addr->abend.other_error=1;
	  perror("finding end of log file, ddout");
	  play_wav(1);
	}
      } else {
	fprintf(stderr,"\007!! help! ** no file is now open\n");
      	play_wav(1);
      }
      goto Append;  /* always write first message */
    }
/* SECTION 4 */

    strcpy(buf2,buf);
    kack = (buf[FIRST_CHAR-1] == '/');
    if(kack) {
      ich = memchr(buf+FIRST_CHAR, '/', bufl-FIRST_CHAR);
      /* ich now points to spot '/' */
      kack = (ich != NULLPTR);
      ich++;
      if (kack) kack = ((ich = strtok(ich, ","))!=NULLPTR && strncmp(ich, "ack ",3)==0);
      if(kack) {
Ack:    ich = strtok(NULL, ",");
        if (ich != NULLPTR) {
          if (strncmp(ich, "ack ", 3) == 0) goto Ack;
          else kack = 0;
        }
      }
    }
    strcpy(buf,buf2);


/* SECTION 5 */

    st="/form/debug:";
    kdebug=strncmp(buf+FIRST_CHAR-1,st,strlen(st))==0;
    st="#matcn#debug:";
    kdebug = kdebug || strncmp(buf+FIRST_CHAR-1,st,strlen(st))==0;

    kp = (buf[FIRST_CHAR-1] == '$'); /* procedure execution logging */

    kpcald = strncmp(buf+FIRST_CHAR-1,"#pcald#",7)==0 ||
      strncmp(buf+FIRST_CHAR-1,"#tpicd#",7)==0 || /*phasecal or tsys record */
      strncmp(buf+FIRST_CHAR-1,"#rdtc",5)==0;

    knd= memcmp("nd",prtn1,2)==0;  /* no display */
    if(kxd || !(kp || kack || kdebug || knd || kpcald) || kpcald && kpd ){

      /* process log entry for display if conditions are met,
        all errors get processed (needed for logging), but display of errors
        may be overridden depending on TNX settings,
	everything else is only available for logging */

      /*  error recognition and message expansion */

      ierrnum=0;
      if (*cp2 != 'b') /* then not an error */
	goto Append;

      /*  else it is an error or warning */

      /* does the error log entry have text (additional error info) in parentheses? 
	 no  => iwl == 0
	 yes => iwl is text length up to 4
	 iwhs == pointer to '('
      */

      iwl =  0;
      iwhs = memchr(buf+FIRST_CHAR, '(', bufl-FIRST_CHAR);
      if(iwhs != NULL) {
	iwhe = memchr(iwhs+1, ')',bufl-(iwhs+1-buf));
	if (iwhe != NULL){
	  iwl = 4 < iwhe-iwhs+1 ? 4 : iwhe-iwhs-1;
	  strncpy(iwhat, iwhs+1, iwl);
	  iwhat[iwl]=0;
	}
      }
      
      strncpy(ibur,buf+FIRST_CHAR+8,5);
      ibur[5]='\0';
      sscanf(ibur,"%d",&ierrnum);
      memcpy(&ierrch,buf+FIRST_CHAR+6,2);
      if(strncmp(buf+FIRST_CHAR+6,"un",2)==0) {
	int ierr;
	strncpy(ibur,buf+FIRST_CHAR+8,5);
	ibur[5]='\0';
	if(1==sscanf(ibur,"%d",&ierr)) {
	  strncpy(ibur,strerror(ierr),80);
	  if(strlen(strerror(ierr))>(80-1))
	    ibur[79]='\0';
	} else {
	  ibur[0]='\0';
	  goto Append;
	}
      } else {
	fserr_snd(buf, 80);
	while (skd_clr_ret(ip)) // clear any old ones from possible time-outs
	  ;
	ip[0]=0;
	if(skd_run_to("fserr", 'w', ip,500)==1) {
	  strcpy(ibur,"fserr not responding, if this persists, consider restarting the FS");
	  iburl=strlen(ibur);
	} else {
	  iburl=fserr_rcv(ibur, 118);
	  ibur[iburl]='\0';
	}
	
	if((iburl==4) && (strncmp(ibur, "nono", 4) == 0)) {
	  ibur[0]=0;
	  goto Append;
	}
	
	if(iwl != 0){ /* non-empty "()" */
	  dxpm(ibur, "?W", &ptrs, &irgb); 
	  if(ptrs != NULL) { /* replace ?W... in ibur with non-empty "()" */
	    iwm= irgb < iwl? irgb: iwl;
	    memcpy(ptrs,iwhat,iwm);
	  } else {
	    dxpm(ibur, "?F", &ptrs, &irgb); 
	    if(ptrs != NULL) { /* replace ?F... in ibur with non-empty "()" */
	      int ierr;
	      char *minus;
	      iwm= irgb < iwl? irgb: iwl;
	      minus=memchr(iwhat,'-',iwm);
	      if(NULL != minus)
		*minus=' ';
	      memcpy(ptrs,iwhat,iwm);
	      if(1==sscanf(iwhat,"%d",&ierr)) {
		strcat(ibur,": ");
		strcat(ibur,strerror(ierr));
	      } 
	    }
	  }
	  if(ptrs!=NULL)
	     *iwhs=0;   /* get rid of non-empty "()" if "?W"/"?F" found */
	} else if(NULL!=iwhs) /* get rid of empty "()" */ 
	  *iwhs=0;

      }
      /* append returned info (if not empty) to output message for display, 
	 otherwise we jumped to Append
      */
      
      strcat(buf, " ");
      strcat(buf, ibur);
      
    Append:
      display=1;  /* always display unless tnx overrides for errors */
      
      if(*cp2 == 'b') { /* could have gotten here from outside block for new log
			   or from inside block for non-error, we don't want those
			*/
	
	/* tnx command error filtering */
	
	count=0;
	for(ptr=last;ptr!=NULL;ptr=ptr->previous)  /* look for it */
	  if(ptr->num == ierrnum && memcmp(ptr->ch,ierrch,2)==0) {
	    if(count ==0)
	      count=ptr->count;
	    if(strcmp(ptr->string,ibur)==0) {
	      display=ptr->on;
	      break;
	    }
	  }
	if(ptr == NULL) { /* not found, add it */
	  ptr= (struct list *)malloc(sizeof(struct list));
	  if(ptr!=NULL) {
	    memcpy(ptr->ch,ierrch,2);
	    ptr->num=ierrnum;
	    ptr->previous=last;
	    ptr->next=NULL;
	    ptr->on=1;
	    ptr->count=count+1;
	    
	    if(strlen(ibur) == 0) {
	      ptr->example=strdup(buf+FIRST_CHAR+14);
	      if(ptr->example == NULL) {  /* ptr->example is NULL */
		shm_addr->abend.other_error=1;
		perror("!! help! ** getting tnx structure example, ddout");
		play_wav(1);
	      }
	    } else
	      ptr->example=NULL;
	    
	    ptr->string=strdup(ibur);
	    if(ptr->string != NULL) {
	      if(first == NULL)
		first=ptr;
	      else
		last->next=ptr;
	      last=ptr;
	    } else {  /* get rid of it since we can't add it */
	      if(ptr->example!=NULL)
		free(ptr->example);
	      free(ptr);
	      shm_addr->abend.other_error=1;
	      perror("!! help! ** getting tnx structure string, ddout");
            play_wav(1);
	    }
	  } else {
	    shm_addr->abend.other_error=1;
	    perror("!! help! ** getting tnx structure, ddout");
            play_wav(1);
	  }
	}

	/* send message to station error program */
	if(display && *cp2 == 'b' && shm_addr->sterp !=0) {
	  skd_run_arg("sterp", 'n', ip,buf); 
	}
	
	/* send message to station erchk program */
	if(display && *cp2 == 'b' && shm_addr->erchk !=0) {
	  skd_run_arg("erchk", 'n', ip,buf); 
	}
      }
      { /*trim trailing blanks before output*/
	int iend=strlen(buf+20);
	while(iend>0 && buf[20+iend-1]==' ')
	  buf[20+(iend--)-1]=0;
      }	
      if(display) {
	/* not Y10K compliant */
	printf("%.8s",buf+9);
	/* not Y10K compliant */
	printf("%s",buf+20);
	/* sound bell if an error */
	if (*cp2 == 'b' && ierrnum < 0) {
	  printf("\007");
	  play_wav(1);
	}
	printf("\n");
      }
    }

/* SECTION 6 */
/*  write information to the log file if conditions are met */

    if (kxl || !(kp || kack) || memcmp(cp2,"nl",2)==0) {
      int ret, i, to;
      if (fd <0)
	goto Trouble;
      if(NULL!=strchr(buf,'\e')) { /* remove reverse video escapes */
        bull=strlen(buf);
        for (i=to=0;i<=bull;i++) {
          if(i+3 < bull && !strncmp(buf+i,"\e[7m",4)) {
            buf[to]='(';
            i+=3;
          } else if(i+2 < bull && !strncmp(buf+i,"\e[m",3)) {
            buf[to]=')';
            i+=2;
          } else if (to!=i)
            buf[to]=buf[i];
	  to++;
	}
      }
      strcat(buf,"\n");
      bull = strlen(buf);

      if (serverfd >= 0) {
          write(serverfd, buf, bull);
      }

      ret = write(fd, buf, bull);
      if(bull != ret ) {
	shm_addr->abend.other_error=1;
	if(ret >= 0)
	  fprintf(stderr,"!! wrong length written, probably the disk is full or the log file is too large\n");
        else
          perror("!! help! ** writing file, ddout");
        play_wav(1);
	goto Post;
      }
    }
    rte_rawt(&now);
    if(now-last_sync>100) {
      unsigned diff;
      int ierr;
      diff=now-last_sync;
      rte_rawt(&last_sync);
      ierr=fsync(fd);
      // printf("synced %u ierr %d '%.40s'\n",diff,ierr,buf);
      if(ierr < 0) {
	shm_addr->abend.other_error=1;
	perror("!! help! ** syncing file, ddout");
	play_wav(1);
	goto Post;
      }
    }

/* SECTION 7 */
/*  post message to disk, return to caller or to main loop */

Post:
    goto Messenger;

/* SECTION 8 */
/*  routine called if trouble occurs with log file */

Trouble:
    if(knl) {
      shm_addr->abend.other_error=1;
      fprintf(stderr,
	      "\007!! help! ** log file '%.8s' not open, can't write to disk\n",
	     sllog);
      play_wav(1);
    }

    goto Messenger;

/* SECTION 9 */
/*  exit from program */

Bye:
    ip[0]=-1;
    skd_run("fserr", 'n', ip); 

    exit( -1);
}
void dxpm(ibur, ipt, ptrs, len)
char *ibur, *ipt, **ptrs;
int *len;
/*input:
   ibur - raw error message
   ipt  - substring to find
  output:
   ptrs - location of substring in ibur, NULL if not present
   len  - length of substring in ibur to last repeated end character
*/
{
  char last;

  *len=strlen(ipt);
  last=ipt[(*len)-1];
  *ptrs=NULL;
  while(strlen(ibur) >= *len) {
    *ptrs=strchr(ibur,ipt[0]);
    if( *ptrs == NULL)
      return;
    ibur=*ptrs+*len;          /* next place to start looking */
    if(strncmp(*ptrs,ipt,*len) == 0) {  /* if we match */
      while (*ibur == last){            /*   extend length of match if the */
        (*len)++;                       /*   last character is repeated    */
        ibur++;
      }
      return;
    }
    *ptrs=NULL;
  }
  return;
}
