/*
 *  Copyright (C) 2004 Steve Harris
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public License
 *  as published by the Free Software Foundation; either version 2.1
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  $Id$
 */

#ifndef LOP_TYPES_H
#define LOP_TYPES_H

/**
 * \file lop_types.h The liblo headerfile defining types used by this API.
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "lop/lop_osc_types.h"

/**
 * \brief A reference to an OSC service.
 *
 * Created by calls to lop_address_new() or lop_address_new_from_url().
 */
typedef void *lop_address;

/**
 * \brief A object to store an opaque binary data object.
 *
 * Can be passed over OSC using the 'b' type. Created by calls to lop_blob_new().
 */
typedef void *lop_blob;

/**
 * \brief A low-level object used to represent messages passed over OSC.
 *
 * Created by calls to lop_message_new(), arguments can be added with calls to
 * lop_message_add_*().
 */
typedef void *lop_message;

/**
 * \brief An object representing an method on a server.
 *
 * Returned by calls to lop_server_thread_add_method() and
 * lop_server_add_method().
 */
typedef void *lop_method;

/**
 * \brief An object representing an instance of an OSC server.
 *
 * Created by calls to lop_server_new(). If you with the library to take care of
 * the threading as well you can just use server threads instead.
 */
typedef void *lop_server;

/**
 * \brief A callback function to receive notifcation of an error in a server or
 * server thread.
 *
 * On callback the paramters will be set to the following values:
 *
 * \param num An error number that can be used to identify this condition.
 * \param msg An error message describing the condidtion.
 * \param where A string describing the place the error occured - typically
 * either a function call or method path.
 */
typedef void (*lop_err_handler)(int num, const char *msg, const char *where);

/**
 * \brief A callback function to receive notifcation of matching message
 * arriving in the server or server thread.
 *
 * The return value tells the method dispatcher whether this handler
 * has dealt with the message correctly: a return value of 0 indicates
 * that it has been handled, and it should not attempt to pass it on
 * to any other handlers, non-0 means that it has not been handled and
 * the dispatcher will attempt to find more handlers that match the
 * path and types of the incoming message.
 *
 * On callback the paramters will be set to the following values:
 *
 * \param path That path that the incoming message was sent to
 * \param types If you specided types in your method creation call then this
 * will match those and the incoming types will have been coerced to match,
 * otherwise it will be the types of the arguments of the incoming message.
 * \param argv An array of lop_arg types containing the values, e.g. if the
 * first argument of the incoming message is of type 'f' then the vlaue will be
 * found in argv[0]->f.
 * \param argc The number of argumets received.
 * \param msg A structure containing the original raw message as received. No
 * type coercion will have occured and the data will be in OSC byte order
 * (bigendian).
 * \param user_data This contains the user_data value passed in the call to
 * lop_server_thread_add_method.
 */
typedef int (*lop_method_handler)(const char *path, const char *types,
				 lop_arg **argv, int argc, lop_message msg,
				 void *user_data);

#ifdef __cplusplus
}
#endif

#endif
