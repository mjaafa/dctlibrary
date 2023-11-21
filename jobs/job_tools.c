
/**
 * \file     job_tools.c
 * \brief    job tools (multithreading) 
 * \author   mohamet.jaafar\@gmail.com
 * \version  0.8
 * \date     first release 
 * \date     modification  mohamet.jaafar\@gmail.com 2008
 *
 * This file describe public multithreading approche structure and functions.
 *
 * ----------------------------------------
 * Copyright (C) 2004 Mohamed JAAFAR
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
 *****************************************************************************/
/**
 * \defgroup JPEG JPEG
 * \{ */

/**
 * \defgroup JPEG_CODEC JPEG_CODEC
 * \{ */
/* =================================80======================================= */
/*                               INCLUDES                                     */
/* =================================80======================================= */

#include "job_tools.h"
#include <stdio.h>
#include <string.h>
#include <sys/queue.h>

/* =================================80======================================= */
/*                               MACRO's                                      */
/* =================================80======================================= */

/* =================================80======================================= */
/*                               TYPEDEF & STRUCTURES & ENUMS                 */
/* =================================80======================================= */

/* =================================80======================================= */
/*                               VARIABLES                                    */
/* =================================80======================================= */

TAILQ_HEAD(tailhead, JOB_frame_entry)	pool = TAILQ_HEAD_INITIALIZER(pool);

struct tailhead             *headp;		 
struct JOB_frame_entry {
    TAILQ_ENTRY(JOB_frame_entry) entries;	     /*	List. */
    int msgId;
    int data1;
    int data2;
} *frameEntry;
/* =================================80======================================= */
/*                               FUNCTIONS                                    */
/* =================================80======================================= */
 
int JTOOLS_createJob(pthread_t *threadId, int threadPriority, void *jobFunction, void *threadData)
{
   if((threadId == NULL)||(jobFunction == NULL))
   {
      return (ERROR_7);
   }
    
   int                 z_ret;
   static pthread_attr_t   attr;
   struct sched_param  sched_param;
        
    
   /* initialisation du descripteur de pthread */
   z_ret = pthread_attr_init(&attr);
   if (z_ret != OK)
   {
      return (ERROR_8);
   } 
    

   /* pthread priority fixing */
   z_ret = pthread_attr_getschedparam (&attr, &sched_param);
   sched_param.sched_priority  = threadPriority;
   if (z_ret != OK)
   {
       printf("error = %d %s \n", z_ret ,strerror(z_ret));
      return (ERROR_8);
   }
   z_ret = pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
   if (z_ret != OK)
   {
       printf("error = %d \n", z_ret);
      return (ERROR_8);
   }
   z_ret = pthread_attr_setschedpolicy(&attr, SCHED_RR);
   if (z_ret != OK)
   {
       printf("error = %d \n", z_ret);
      return (ERROR_8);
   }
   z_ret = pthread_attr_setschedparam(&attr,&sched_param);
   if (z_ret != OK)
   {
       printf("error = %d %s \n", z_ret ,strerror(z_ret));
      return (ERROR_8);
   }

   /* Creation de la pthread */
   z_ret = (pthread_create((pthread_t *) threadId,
                          &attr,
                          (void*) jobFunction,
                          (void*) threadData) < 0);
   if (z_ret != OK)
   {
       printf(" error = %d \n", z_ret);
      return (ERROR_8);
   }

   /* destruction du descripteur */
   z_ret = pthread_attr_destroy(&attr);
   if (z_ret != OK)
   {
      return (ERROR_8);
   }

   return z_ret;    
}

int JTOOLS_exitJob(pthread_t *threadId)
{
   if(threadId == NULL)
   {
      return ERROR_7;
   }
   int z_ret;
   //z_ret = pthread_join(*threadId, NULL);
   z_ret = pthread_cancel(*threadId);
   
   return z_ret;
}

int JTOOLS_msgQueueInit(JOB_msgQueue_ts *msgQueue)
{
   pthread_mutexattr_t     mutexAttr;
   pthread_condattr_t      condAttr;
   int                     z_ret;

    z_ret = (pthread_mutexattr_init( &mutexAttr ));
    if(0 != z_ret)
    {
       printf("Cannot initialize mutex attributes (err = %d) \n", z_ret);
       return z_ret;
    }

    z_ret = pthread_mutex_init(&(msgQueue->locker), &mutexAttr);
    if(0 != z_ret)
    {
       printf("Cannot initialize mutex (err = %d)\n", z_ret);
       return ERROR_8;
    }

    pthread_mutexattr_destroy(&mutexAttr);

    z_ret = (pthread_cond_init(&(msgQueue->trigger), &condAttr));
    if(0 != z_ret)
    {
       printf("Cannot initialize cond (err = %d)\n", z_ret);
       return ERROR_8;
    }
    pthread_condattr_destroy(&condAttr);
   if(msgQueue == NULL)
   {
      return ERROR_9;
   }
   
   msgQueue->length = 0;
   msgQueue->read   = 0;
   msgQueue->write  = 0;
   
   TAILQ_INIT(&pool);
   
   return OK;
}

int JTOOLS_msgQueuePush(JOB_msgQueue_ts *msgQueue, long msgId, long data1, long data2)
{
  //printf("%s msgId = %d \n", __FUNCTION__, msgId);
    	pthread_mutex_lock(&msgQueue->locker);
        frameEntry	= malloc(sizeof(struct JOB_frame_entry));	     /*	Insert at the head. */
        TAILQ_INSERT_TAIL(&pool, frameEntry, entries);
        frameEntry->msgId = msgId;
        frameEntry->data1 = data1;
        frameEntry->data2 = data2;
// TODO : implement 	msgqueue.

    	pthread_cond_signal(&msgQueue->trigger);
       pthread_mutex_unlock(&msgQueue->locker);
   
   return OK;
}


int JTOOLS_msgQueueWait(JOB_msgQueue_ts *msgQueue, JOB_msg_ts *msg)
{
    pthread_mutex_lock(&msgQueue->locker);
#if 0
            if(!TAILQ_EMPTY(&pool)){
             TAILQ_FOREACH_REVERSE(frameEntry, &pool, tailhead, entries) {
                if (NULL != frameEntry)	
                {
			msg->data1 = frameEntry->data1;
			msg->data2 = frameEntry->data2;
			msg->msgId = frameEntry->msgId;
  printf("%s msgId = %d [I=%d/J=%d]\n", __FUNCTION__, msg->msgId, msg->data1, msg->data2);
	//		free(frameEntry);
		}
	     }
	     }
#endif
	        while (!TAILQ_EMPTY(&pool)) {
             frameEntry = TAILQ_FIRST(&pool);
             TAILQ_REMOVE(&pool, frameEntry, entries);
			msg->data1 = frameEntry->data1;
			msg->data2 = frameEntry->data2;
			msg->msgId = frameEntry->msgId;

             free(frameEntry);
	     break;
     }

// TODO : implmenent msgqueue.   
   
    pthread_cond_wait(&(msgQueue->trigger), &(msgQueue->locker));
    pthread_mutex_unlock(&msgQueue->locker);


  
}


