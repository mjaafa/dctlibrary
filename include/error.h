#ifndef _ERROR_H_
#define _ERROR_H_

/**
 * \file     error.h
 * \brief    error type
 * \author   mohamet.jaafar\@gmail.com
 * \version  0.8
 * \date     first release 
 * \date     modification  mohamed.jaafar\@gmail.com 2008
 *
 * This file describe public error code structure and functions.
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

/* =================================80======================================= */
/*                               DEFINES                                      */
/* =================================80======================================= */

/** \brief  ERROR read / write definition                                     */
/** \brief  ERROR_1  opening file error : file does not exist                 */
#define ERROR_1             (-1)
/** \brief  ERROR_2  invalid file name (path)                                 */
#define ERROR_2             (-2)
/** \brief  ERROR_3  memory allocation error                                  */
#define ERROR_3             (-3)
/** \brief  ERROR_4  reading file error                                       */
#define ERROR_4             (-4)
/** \brief  ERROR_5  saving file error                                        */
#define ERROR_5             (-5)
/** \brief  ERROR_6  null pointer error                                       */
#define ERROR_6             (-6)
/** \brief  OK !!!                                                            */
#define OK                  (0)
#ifdef _USE_JOBS
/** \brief  ERROR_7  null pointer (thread creation) error                     */
#define ERROR_7             (-7)
/** \brief  ERROR_8  thread creation error                                    */
#define ERROR_8             (-8)
/** \brief  ERROR_9  msg queue init error                                     */
#define ERROR_9             (-9)
/** \brief  ERROR_10 msg queue error : full msg Queue                         */
#define ERROR_10            (-10)
#endif /*_USE_JOBS*/
/* =================================80======================================= */
/*                               FUNCTIONS                                    */
/* =================================80======================================= */

 
// JPEG_CODEC
/** \} */
// JPEG
/** \} */

#endif // < _ERROR_H_
