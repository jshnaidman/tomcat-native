/* Copyright 1999-2004 The Apache Software Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef AJP_H
#define AJP_H

#include "apr_version.h"
#include "apr.h"

#include "apr_hooks.h"
#include "apr_lib.h"
#include "apr_strings.h"
#include "apr_buckets.h"
#include "apr_md5.h"
#include "apr_network_io.h"
#include "apr_pools.h"
#include "apr_strings.h"
#include "apr_uri.h"
#include "apr_date.h"
#include "apr_fnmatch.h"
#define APR_WANT_STRFUNC
#include "apr_want.h"

#if APR_HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#if APR_HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif

#if APR_CHARSET_EBCDIC

#define USE_CHARSET_EBCDIC
#define ajp_xlate_to_ascii(b, l) ap_xlate_proto_to_ascii(b, l)
#define ajp_xlate_from_ascii(b, l) ap_xlate_proto_from_ascii(b, l)

#else                           /* APR_CHARSET_EBCDIC */

#define ajp_xlate_to_ascii(b, l) 
#define ajp_xlate_from_ascii(b, l) 

#endif

#ifdef AJP_USE_HTTPD_WRAP
#include "httpd_wrap.h"
#else
#include "httpd.h"
#include "http_config.h"
#include "http_request.h"
#include "http_core.h"
#include "http_protocol.h"
#include "http_main.h"
#include "http_log.h"
#endif


/** AJP Specific error codes
 */
/** Buffer overflow exception */
#define AJP_EOVERFLOW           (APR_OS_START_USERERR + 1) 
/** Destination Buffer is to small */
#define AJP_ETOSMALL            (APR_OS_START_USERERR + 2) 
/** Invalid input parameters */
#define AJP_EINVAL              (APR_OS_START_USERERR + 3) 
/** Bad message signature */
#define AJP_EBAD_SIGNATURE      (APR_OS_START_USERERR + 4) 
/** Incoming message too bg */
#define AJP_ETOBIG              (APR_OS_START_USERERR + 5) 
/** Missing message header */
#define AJP_ENO_HEADER          (APR_OS_START_USERERR + 6) 
/** Bad message header */
#define AJP_EBAD_HEADER         (APR_OS_START_USERERR + 7) 
/** Bad message */
#define AJP_EBAD_MESSAGE        (APR_OS_START_USERERR + 8) 

/** A structure that represents ajp message */ 
typedef struct ajp_msg ajp_msg_t;

/** A structure that represents ajp message */ 
struct ajp_msg
{
    apr_byte_t  *buf;
    apr_size_t  header_len;
    apr_size_t  len;
    apr_size_t  pos;
    int         server_side;
};

#define AJP_HEADER_LEN              4
#define AJP_HEADER_SZ_LEN           2
#define AJP_MSG_BUFFER_SZ           (8*1024)
#define AJP13_MAX_SEND_BODY_SZ      (AJP_DEF_BUFFER_SZ - 6)

/* Webserver ask container to take control (logon phase) */
#define CMD_AJP13_PING              (unsigned char)8

/* Webserver check if container is alive, since container should respond by cpong */
#define CMD_AJP13_CPING             (unsigned char)10

/* Container response to cping request */
#define CMD_AJP13_CPONG             (unsigned char)9

/**
 * Check a new AJP Message by looking at signature and return its size
 *
 * @param msg       AJP Message to check
 * @param len       Pointer to returned len
 * @return          APR_SUCCESS or error
 */
apr_status_t ajp_msg_check_header(ajp_msg_t *msg, apr_size_t *len);

/**
 * Reset an AJP Message
 *
 * @param msg       AJP Message to reset
 * @return          APR_SUCCESS or error
 */
apr_status_t ajp_msg_reset(ajp_msg_t *msg);

/**
 * Mark the end of an AJP Message
 *
 * @param msg       AJP Message to end
 * @return          APR_SUCCESS or error
 */
apr_status_t ajp_msg_end(ajp_msg_t *msg);

/**
 * Add an unsigned 32bits value to AJP Message
 *
 * @param msg       AJP Message to get value from
 * @param value     value to add to AJP Message
 * @return          APR_SUCCESS or error
 */
apr_status_t ajp_msg_append_uint32(ajp_msg_t *msg, apr_uint32_t value);

/**
 * Add an unsigned 16bits value to AJP Message
 *
 * @param msg       AJP Message to get value from
 * @param value     value to add to AJP Message
 * @return          APR_SUCCESS or error
 */
apr_status_t ajp_msg_append_uint16(ajp_msg_t *msg, apr_uint16_t value);

/**
 * Add an unsigned 8bits value to AJP Message
 *
 * @param msg       AJP Message to get value from
 * @param value     value to add to AJP Message
 * @return          APR_SUCCESS or error
 */
apr_status_t ajp_msg_append_uint8(ajp_msg_t *msg, apr_byte_t value);

/**
 *  Add a String in AJP message, and transform the String in ASCII 
 *  if convert is set and we're on an EBCDIC machine    
 *
 * @param msg       AJP Message to get value from
 * @param value     Pointer to String
 * @param convert   When set told to convert String to ASCII
 * @return          APR_SUCCESS or error
 */
apr_status_t ajp_msg_append_cvt_string(ajp_msg_t *msg, const char *value,
                                       int convert);
/**
 *  Add a String in AJP message, and transform 
 *  the String in ASCII if we're on an EBCDIC machine    
 *
 * @param msg       AJP Message to get value from
 * @param value     Pointer to String
 * @return          APR_SUCCESS or error
 */
apr_status_t ajp_msg_append_string(ajp_msg_t *msg, const char *value);

/**
 *  Add a String in AJP message. 
 *
 * @param msg       AJP Message to get value from
 * @param value     Pointer to String
 * @return          APR_SUCCESS or error
 */
apr_status_t ajp_msg_append_ascii_string(ajp_msg_t *msg, const char *value);

/**
 * Add a Byte array to AJP Message
 *
 * @param msg       AJP Message to get value from
 * @param value     Pointer to Byte array
 * @param valuelen  Byte array len
 * @return          APR_SUCCESS or error
 */
apr_status_t ajp_msg_append_bytes(ajp_msg_t *msg, const apr_byte_t *value,
                                  apr_size_t valuelen);

/**
 * Get a 32bits unsigned value from AJP Message
 *
 * @param msg       AJP Message to get value from
 * @param rvalue    Pointer where value will be returned
 * @return          APR_SUCCESS or error
 */
apr_status_t ajp_msg_get_uint32(ajp_msg_t *msg, apr_uint32_t *rvalue);

/**
 * Get a 16bits unsigned value from AJP Message
 *
 * @param msg       AJP Message to get value from
 * @param rvalue    Pointer where value will be returned
 * @return          APR_SUCCESS or error
 */
apr_status_t ajp_msg_get_uint16(ajp_msg_t *msg, apr_uint16_t *rvalue);

/**
 * Peek a 16bits unsigned value from AJP Message, position in message
 * is not updated
 *
 * @param msg       AJP Message to get value from
 * @param rvalue    Pointer where value will be returned
 * @return          APR_SUCCESS or error
 */
apr_status_t ajp_msg_peek_uint16(ajp_msg_t *msg, apr_uint16_t *rvalue);

/**
 * Get a 8bits unsigned value from AJP Message
 *
 * @param msg       AJP Message to get value from
 * @param rvalue    Pointer where value will be returned
 * @return          APR_SUCCESS or error
 */
apr_status_t ajp_msg_get_byte(ajp_msg_t *msg, apr_byte_t *rvalue);

/**
 * Get a String value from AJP Message
 *
 * @param msg       AJP Message to get value from
 * @param rvalue    Pointer where value will be returned
 * @return          APR_SUCCESS or error
 */
apr_status_t ajp_msg_get_string(ajp_msg_t *msg, char **rvalue);


/**
 * Get a Byte array from AJP Message
 *
 * @param msg       AJP Message to get value from
 * @param rvalue    Pointer where value will be returned
 * @param rvalueLen Pointer where Byte array len will be returned
 * @return          APR_SUCCESS or error
 */
apr_status_t ajp_msg_get_bytes(ajp_msg_t *msg, apr_byte_t **rvalue,
                               apr_size_t *rvalueLen);

/**
 * Create an AJP Message from pool
 *
 * @param pool      memory pool to allocate AJP message from
 * @param rmsg      Pointer to newly created AJP message
 * @return          APR_SUCCESS or error
 */
apr_status_t ajp_msg_create(apr_pool_t *pool, ajp_msg_t **rmsg);

/**
 * Recopy an AJP Message to another
 *
 * @param smsg      source AJP message
 * @param dmsg      destination AJP message
 * @return          APR_SUCCESS or error
 */
apr_status_t ajp_msg_copy(ajp_msg_t *smsg, ajp_msg_t *dmsg);

/**
 * Serialize in an AJP Message a PING command
 *
 * +-----------------------+
 * | PING CMD (1 byte)     |
 * +-----------------------+
 *
 * @param smsg      AJP message to put serialized message
 * @return          APR_SUCCESS or error
 */
apr_status_t ajp_msg_serialize_ping(ajp_msg_t *msg);

/** 
 * Serialize in an AJP Message a CPING command
 *
 * +-----------------------+
 * | CPING CMD (1 byte)    |
 * +-----------------------+
 *
 * @param smsg      AJP message to put serialized message
 * @return          APR_SUCCESS or error
 */
apr_status_t ajp_msg_serialize_cping(ajp_msg_t *msg);

/** 
 * Send an AJP message to backend
 *
 * @param soct      backend socket
 * @param smsg      AJP message to put serialized message
 * @return          APR_SUCCESS or error
 */
apr_status_t ajp_ilink_send(apr_socket_t *sock, ajp_msg_t *msg);

/** 
 * Receive an AJP message from backend
 *
 * @param soct      backend socket
 * @param smsg      AJP message to put serialized message
 * @return          APR_SUCCESS or error
 */
apr_status_t ajp_ilink_receive(apr_socket_t *sock, ajp_msg_t *msg);



#endif /* AJP_H */

