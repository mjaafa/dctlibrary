#ifndef _JOB_TOOLS_H_
#define _JOB_TOOLS_H_

/**
 * \file     job_tools.h
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
#include <pthread.h>
#include <sched.h>
#include <semaphore.h>
#include "dctlib.h"
#include "error.h"
/* =================================80======================================= */
/*                               DEFINES                                      */
/* =================================80======================================= */
/** \brief  msgQueue size                                                     */
#define JOB_MSGQUEUE_SIZE           (20)
#ifdef _USE_DCT
/** \brief  dct job ID                                                        */
#define DCT_JOB_ID                (0x1)
/** \brief  dct job ID                                                        */
#define DCT_JOB_COMPUTE           (0x001)
#endif /*_USE_DCT*/
#define JOB_EXIT                  (0x000)    

/* =================================80======================================= */
/*                               TYPEDEF                                      */
/* =================================80======================================= */

/**
* \brief  message structure definition.
*/
typedef struct JOB_msg
{
   int        msgId;             /**< Message ID */

   // TODO : https://gist.github.com/vaclavbohac/914625
}JOB_msg_ts;

/**
* \brief  messageQueue structure definition.
*/
typedef struct JOB_msgQueue
{
   int              length;                  /**< Queue length.      */
   int              read;                    /**< Read posision.     */
   int              write;                   /**< Write position.    */
   JOB_msg_ts       queue[JOB_MSGQUEUE_SIZE];/**< Message queue.     */
   sem_t            semaphore;               /**< Waiting semaphore. */
   pthread_mutex_t  locker;
   pthread_cond_t   trigger;
}JOB_msgQueue_ts;

 
/* =================================80======================================= */
/*                               FUNCTIONS                                    */
/* =================================80======================================= */

/**
 *
 * \brief   create a job.
 *
 * \param   pthread_t *threadId                 : job thread id.
 * \param   int threadPriority                  : job thread priority.
 * \param   void *jobFunction                   : job fuction to execute. 
 * \param   void *threadData                    : job data.
 * \return  error code
 * \author  mohamet.jaafar\@gmail.com
 * \date    
 */

int JTOOLS_createJob(pthread_t *threadId, int threadPriority, void *jobFunction, void *threadData);


/**
 *
 * \brief   create a job.
 *
 * \param   pthread_t *threadId                 : job thread id.
 * \return  error code
 * \author  mohamet.jaafar\@gmail.com
 * \date    
 */
 
int JTOOLS_exitJob(pthread_t *threadId);


/**
 *
 * \brief   init a msg queue  .
 *
 * \param   JOB_msgQueue *msgQueue              : message queue.
 * \return  error code
 * \author  mohamet.jaafar\@gmail.com
 * \date    
 */
 
int JTOOLS_msgQueueInit(JOB_msgQueue_ts *msgQueue);



/**
 *
 * \brief   push a message in the msg queue  .
 *
 * \param   JOB_msgQueue *msgQueue              : message queue.
 * \param   long msgId                          : message id.
 * \param   long data1                          : message 1st data. 
 * \param   long data2                          : message 2nd data. 
 * \return  error code
 * \author  mohamet.jaafar\@gmail.com
 * \date    
 */
 
int JTOOLS_msgQueuePush(JOB_msgQueue_ts *msgQueue, long msgId, long data1, long data2);



/**
 *
 * \brief   push a message in the msg queue  .
 *
 * \param   JOB_msgQueue *msgQueue              : message queue.
 * \param   JOB_msg msg                         : message read.
 * \return  error code
 * \author  mohamet.jaafar\@gmail.com
 * \date    
 */
 
int JTOOLS_msgQueueWait(JOB_msgQueue_ts *msgQueue, JOB_msg_ts *msg);

// JPEG_CODEC
/** \} */
// JPEG
/** \} */

#endif /*_JOB_TOOLS_H_*/
