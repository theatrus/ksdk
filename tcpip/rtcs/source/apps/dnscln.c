/*HEADER**********************************************************************
*
* Copyright 2013-2014 Freescale Semiconductor, Inc.
*
* This software is owned or controlled by Freescale Semiconductor.
* Use of this software is governed by the Freescale MQX RTOS License
* distributed with this Material.
* See the MQX_RTOS_LICENSE file distributed for more details.
*
* Brief License Summary:
* This software is provided in source form for you to use free of charge,
* but it is not open source software. You are allowed to use this software
* but you cannot redistribute it or derivative works of it in source form.
* The software may be used only in connection with a product containing
* a Freescale microprocessor, microcontroller, or digital signal processor.
* See license agreement file for full license terms including other
* restrictions.
*****************************************************************************
*
* Comments:
*
*   This file contains the Domain Name System client/resolver.
*
*
*END************************************************************************/

#include <rtcs.h>
#include "rtcs_prv.h"
#include <ctype.h>
#include <string.h>
#include "dnscln.h"
#include "ip_prv.h"

#define DNSCLN_TIMEOUT                      (3000) /* sec.*/ /* TBD CFG parameter.*/
#define DNSCLN_MAX_UDP_MESSAGE_SIZE         (512)  /* bytes.*/
/*
** Domain Name Server Response Resource Record Structure
** This is defined to make writing a response easier.  This section of
** the response buffer should have a name first, this information second,
** and then response information of RDLENGTH after the structure.
*/

typedef struct dnscln_response_rr_middle_struct {

   unsigned char         TYPE[2];
   unsigned char         CLASS[2];
   unsigned char         TTL[4];
   unsigned char         RDLENGTH[2];

}DNSCLN_RESPONSE_RR_MIDDLE_STRUCT, * DNSCLN_RESPONSE_RR_MIDDLE_STRUCT_PTR;

/*
** Domain Name Server Message Header Structure
*/
typedef struct dnscln_message_header_struct
{
    unsigned char   ID[2];          /* Identifier used to match replies to outstanding queries */
    unsigned char   CONTROL[2];     /* Bits set to define query or response */
    unsigned char   QDCOUNT[2];     /* Number of entries in question section */
    unsigned char   ANCOUNT[2];     /* Number of RR's in answer section */
    unsigned char   NSCOUNT[2];     /* Number of Name Server RR's in the authority records   */
    unsigned char   ARCOUNT[2];     /* Number of RR's in the additional records */
}DNSCLN_MESSAGE_HEADER_STRUCT, * DNSCLN_MESSAGE_HEADER_STRUCT_PTR;

/*
** Domain Name Server Message Tail Structure
*/
typedef struct dnscln_message_tail_struct
{
    unsigned char   QTYPE[2];
    unsigned char   QCLASS[2];
}DNSCLN_MESSAGE_TAIL_STRUCT, * DNSCLN_MESSAGE_TAIL_STRUCT_PTR;

/* CLASS values */
#define DNSCLN_IN                           0x0001   /* The Internet */

/* QDCOUNT Codes */
#define DNSCLN_SINGLE_QUERY                 0x0001
#define DNSCLN_DOUBLE_QUERY                 0x0002
#define DNSCLN_TRIPLE_QUERY                 0x0003

/* Response Codes  */
#define DNSCLN_RESPONSE                     0x8000
#define DNSCLN_RECURSION_DESIRED            0x0100
#define DNSCLN_RECURSION_AVAILABLE          0x0080
#define DNSCLN_AUTHORITATIVE_ANSWER         0x0400
#define DNSCLN_TRUNCATION                   0x0200
#define DNSCLN_NO_ERROR                     0x0000
#define DNSCLN_FORMAT_ERROR                 0x0001
#define DNSCLN_SERVER_FAILURE               0x0002
#define DNSCLN_NAME_ERROR                   0x0003
#define DNSCLN_AUTHORITATIVE_NAME_ERROR     0x0403
#define DNSCLN_QUERY_NOT_IMPLEMENTED        0x0004
#define DNSCLN_QUERY_REFUSED                0x0005

/* Masks */
#define DNSCLN_RCODE_MASK                   0x000F
#define DNSCLN_OPCODE_MASK                  0x7800
#define DNSCLN_COMPRESSED_NAME_MASK         0xC0
#define DNSCLN_COMPRESSED_LOCATION_MASK     0x3FFF


/******************************************************************
* Function Prototypes
*******************************************************************/
static uint32_t dnscln_send (uint32_t sock, const char *name_to_resolve, uint16_t type, char *tx_buffer, uint32_t *id_ptr);
static bool dnscln_domains_equal(const char *req_domain, char *res_domain);
static uint32_t dnscln_domain_name_size(uint32_t start, char *rx_buffer);
static bool dnscln_domain_name_extract(uint32_t start, char *rx_buffer, char *ipname, uint32_t ipnamesize);
static uint32_t dnscln_domain_name_fragment_length(unsigned char *name);
static DNSCLN_RECORD_STRUCT_PTR dnscln_get_response(uint32_t  sock, const char *name_to_resolve, uint16_t type, char *rx_buffer, uint32_t req_id);

/************************************************************************
* NAME: dnscln_send
* RETURNS: DNS Resource Record List or NULL if failed to resolve.
* DESCRIPTION: DNS Query.
*************************************************************************/
static uint32_t dnscln_send (uint32_t sock, const char *name_to_resolve, uint16_t type, char *tx_buffer, uint32_t *id_ptr)
{
    DNSCLN_MESSAGE_HEADER_STRUCT_PTR    message_head_ptr;
    DNSCLN_MESSAGE_TAIL_STRUCT_PTR      message_tail_ptr;
    int32_t                             error;
    uint32_t                            i;
    uint32_t                            name_size;
    char                                *name_ptr;
    char                                *len_ptr;
    uint16_t                            id;
    unsigned char                       len;

    name_size = strlen((char *)name_to_resolve);

    /* Build message.*/

    message_head_ptr = (DNSCLN_MESSAGE_HEADER_STRUCT *)tx_buffer;
    name_ptr  = &tx_buffer[sizeof(DNSCLN_MESSAGE_HEADER_STRUCT)];
    id = RTCS_rand(); /* or  TASK_NUMBER_FROM_TASKID(_task_get_id()) ;*/
    *id_ptr = id;

    mqx_htons(message_head_ptr->ID, id );
    mqx_htons(message_head_ptr->CONTROL, DNSCLN_RECURSION_DESIRED);
    mqx_htons(message_head_ptr->QDCOUNT, DNSCLN_SINGLE_QUERY);
    mqx_htons(message_head_ptr->ANCOUNT, 0);
    mqx_htons(message_head_ptr->NSCOUNT, 0);
    mqx_htons(message_head_ptr->ARCOUNT, 0);
    
    /* Replace '.' by zero.*/
    len_ptr=name_ptr;
    len=0;
    if (*name_ptr != '.')
       name_ptr++;

    for (i=0; i<name_size; i++)
    {
        if (name_to_resolve[i] == '.')
        {
            *len_ptr = len;
            len = 0;
            len_ptr = name_ptr;
        } 
        else
        {
            (void) mqx_htonc(name_ptr, name_to_resolve[i]);
            len++;
        }
        name_ptr++;
   }
   *len_ptr = len;  /* Set length, before label.*/
   *name_ptr++ = 0; /* End of string. */

    /* Add tail.*/
    message_tail_ptr = (DNSCLN_MESSAGE_TAIL_STRUCT *) name_ptr;
    name_ptr += sizeof(DNSCLN_MESSAGE_TAIL_STRUCT);

    mqx_htons(message_tail_ptr->QTYPE, (uint16_t)type);
    mqx_htons(message_tail_ptr->QCLASS, DNSCLN_IN);

    /* Send message.*/
    error = send(sock, tx_buffer, (name_ptr-tx_buffer), 0);

    if (error == RTCS_ERROR)
    {
        error = RTCS_geterror(sock);
    } 
    else
    {
        error = RTCS_OK;
    }

    return( error );
}

/************************************************************************
* NAME: dnscln_domains_equal
* RETURNS: 
* DESCRIPTION: 
*************************************************************************/
static bool dnscln_domains_equal(const char *req_domain, char *res_domain)
{
    int             req_index=0;
    int             res_index=0;
    int             i;
    unsigned char   len;
    unsigned char   c;

    len = mqx_ntohc(res_domain + res_index);
    res_index++;
    do
    {
        if ( len & DNSCLN_COMPRESSED_NAME_MASK )
        {
            return FALSE;
        }
    
        for (i=0;i<len;i++)
        {
            if (req_domain[req_index++] != tolower((int) res_domain[res_index++]))
            {
                return FALSE;
            }
        }
        len = mqx_ntohc(res_domain + res_index);
        res_index++;
        c = mqx_ntohc(req_domain + req_index);
        req_index++;
    } 
    while (len && (c == '.'));

    return (len ==0) && (c==0);
}

/************************************************************************
* NAME: dnscln_domain_name_size
* RETURNS: 
* DESCRIPTION: 
*************************************************************************/
static uint32_t dnscln_domain_name_size(uint32_t start, char *rx_buffer)
{
    uint32_t        s_index=start;
    uint32_t        d_index=0;
    unsigned char   b;

    do
    {
        b = mqx_ntohc(rx_buffer + s_index);
        if ( b & DNSCLN_COMPRESSED_NAME_MASK ) /* Check if compresses.*/
        {
            s_index = mqx_ntohs(rx_buffer + s_index) & DNSCLN_COMPRESSED_LOCATION_MASK;
        }
        else
        {
            s_index = s_index + b + 1/*skip leght.*/;
            d_index = d_index + b + 1/* for '.'*/;
        }
    }
    while(rx_buffer[s_index]);

    return d_index;
}

/************************************************************************
* NAME: dnscln_domain_name_extract
* RETURNS: 
* DESCRIPTION: 
*************************************************************************/
static bool dnscln_domain_name_extract(uint32_t start, char *rx_buffer, char *ipname,uint32_t ipnamesize)
{
    uint32_t        s_index=start;
    uint32_t        d_index=0;
    uint32_t        i;
    unsigned char   b;

    do
    {
        b = mqx_ntohc (rx_buffer + s_index);
        if ( b & DNSCLN_COMPRESSED_NAME_MASK )
        {
            s_index = mqx_ntohs(rx_buffer + s_index) & DNSCLN_COMPRESSED_LOCATION_MASK;
        } 
        else
        {
            s_index++;
            for (i=0; i<b; i++)
            {
                if (d_index >= ipnamesize-1) 
                    break;
                ipname[d_index++] = rx_buffer[s_index++];
            }
            if (d_index >= ipnamesize-1) 
                break;
            ipname[d_index++] = '.';
        }
    } 
    while (rx_buffer[s_index]);

    if (d_index)
    {
        ipname[d_index] = 0;
    }
    else
    {
        ipname[0] = 0;
    }

    return TRUE;
}

/************************************************************************
* NAME: dnscln_domain_name_extract
* RETURNS: 
* DESCRIPTION: 
*************************************************************************/
static uint32_t dnscln_domain_name_fragment_length(unsigned char *name)
{
    uint32_t    i=0;

    while ( (name[i] != 0) && ((mqx_ntohc (name + i) & DNSCLN_COMPRESSED_NAME_MASK) == 0))
    {
        i++;
    }

    return (name[i]?i+2:i+1);
}

/************************************************************************
* NAME: dnscln_get_response
* RETURNS: DNS Resource Record List or NULL if failed to resolve.
* DESCRIPTION: Handle DNS response.
*************************************************************************/
static DNSCLN_RECORD_STRUCT_PTR dnscln_get_response(uint32_t  sock, const char *name_to_resolve, uint16_t type, char *rx_buffer, uint32_t req_id)
{
    DNSCLN_MESSAGE_HEADER_STRUCT_PTR       message_head_ptr;
    DNSCLN_RESPONSE_RR_MIDDLE_STRUCT_PTR   answer_middle_ptr;
    unsigned char                           *answer_ptr;
    uint16_t                                answer_count;
    uint16_t                                query_count;
    uint16_t                                rdlength;
    uint16_t                                answer_type;
    uint16_t                                id;
    int32_t                                 response_size;
    char                                    *temp_ptr;
    uint32_t                                i;
    uint32_t                                offset;
    DNSCLN_RECORD_STRUCT_PTR                record_list = NULL;
    DNSCLN_RECORD_STRUCT_PTR                record_prev = NULL;
    DNSCLN_RECORD_STRUCT_PTR                record = NULL;
    uint32_t                                name_size;

    response_size = recv(sock, rx_buffer, DNSCLN_MAX_UDP_MESSAGE_SIZE, 0);
    if ((response_size != RTCS_ERROR) && (response_size >= sizeof(DNSCLN_MESSAGE_HEADER_STRUCT)))
    {
        message_head_ptr = (DNSCLN_MESSAGE_HEADER_STRUCT_PTR) rx_buffer;
        id           = mqx_ntohs( message_head_ptr->ID );
        query_count  = mqx_ntohs( message_head_ptr->QDCOUNT );
        answer_count = mqx_ntohs( message_head_ptr->ANCOUNT );

        if ( (id == req_id ) && (query_count==1) && answer_count)
        {
            temp_ptr = &rx_buffer[sizeof(DNSCLN_MESSAGE_HEADER_STRUCT)];

            if (dnscln_domains_equal(name_to_resolve, temp_ptr))
            {
                temp_ptr +=strlen(temp_ptr) + 1 + sizeof(DNSCLN_MESSAGE_TAIL_STRUCT);
                
                /* Parse answers.*/
                for (i=0; i<answer_count; i++)
                {
                    answer_ptr =  (unsigned char *)temp_ptr;

                    offset = dnscln_domain_name_fragment_length(answer_ptr);

                    answer_middle_ptr = (DNSCLN_RESPONSE_RR_MIDDLE_STRUCT_PTR) &answer_ptr[offset];
                    answer_type = mqx_ntohs( answer_middle_ptr->TYPE );
                    rdlength = mqx_ntohs( answer_middle_ptr->RDLENGTH );

                    /* Sanity check of resource length. */
                    if((temp_ptr - rx_buffer + offset + sizeof(DNSCLN_RESPONSE_RR_MIDDLE_STRUCT) + rdlength)  > response_size)
                    {
                        /* Wrong packet*/
                        break;
                    }

                    if ((answer_type == type) && (rdlength > 0))
                    {
                        record_prev = record; /* Save previous record.*/

                        /* Allocate response structure.*/
                        record = RTCS_mem_alloc_zero(sizeof(DNSCLN_RECORD_STRUCT));
                        if(record)
                        {
                            uint32_t        data_size;
                            
                            switch(answer_type)
                            {
                                /* <domain-name> */
                                case DNSCLN_TYPE_PTR:
                                case DNSCLN_TYPE_NS:
                                case DNSCLN_TYPE_CNAME:
                                    data_size = dnscln_domain_name_size((char *)&answer_ptr[offset  + sizeof(DNSCLN_RESPONSE_RR_MIDDLE_STRUCT)]-rx_buffer, rx_buffer);
                                    break;
                                default:
                                    data_size = rdlength;
                                    break;
                            }

                            record->data = RTCS_mem_alloc_zero(data_size);

                            if(record->data)
                            {
                                record->data_length = data_size;                     /* Length of DNS Resource Record data, in bytes.*/

                                switch(answer_type)
                                {
                                    /* <domain-name> */
                                    case DNSCLN_TYPE_PTR:
                                    case DNSCLN_TYPE_NS:
                                    case DNSCLN_TYPE_CNAME:
                                        dnscln_domain_name_extract((char *)&answer_ptr[offset  + sizeof(DNSCLN_RESPONSE_RR_MIDDLE_STRUCT)]-rx_buffer, rx_buffer, record->data, data_size);
                                        break;
                                    default:
                                        _mem_copy(&answer_ptr[offset  + sizeof(DNSCLN_RESPONSE_RR_MIDDLE_STRUCT)], record->data, data_size); /* Pointer to DNS Resource Record data in network byte order.*/
                                        break;
                                }

                                record->type = (DNSCLN_TYPE)answer_type;            /* Type of DNS Resource Record. */
                                record->ttl = mqx_ntohl(answer_middle_ptr->TTL);    /* Time to Live of DNS Resource Record, in seconds.*/

                                if((name_size =  dnscln_domain_name_size(temp_ptr-rx_buffer, rx_buffer) )> 0)
                                {
                                    record->name = RTCS_mem_alloc_zero(name_size); /* Pointer to a host name string (null-terminated). */
                                    if(record->name)
                                        dnscln_domain_name_extract(temp_ptr-rx_buffer, rx_buffer, record->name, name_size);
                                }

                                if(record_list == NULL) /* Save pointer to the first record.*/
                                    record_list = record;

                                if(record_prev != NULL)
                                    record_prev->next = record;
                            }
                            else
                            {
                                _mem_free(record);
                                record = NULL;
                            }
                        }
                    }

                    temp_ptr += offset + sizeof(DNSCLN_RESPONSE_RR_MIDDLE_STRUCT) + rdlength;
                }
            }
        }
   }

   return record_list;
}

/************************************************************************
* NAME: DNSCLN_query
* RETURNS: DNS Resource Record List or NULL if failed to resolve.
* DESCRIPTION: DNS Query.
*************************************************************************/
DNSCLN_RECORD_STRUCT *DNSCLN_query(DNSCLN_PARAM_STRUCT *params)
{
    DNSCLN_RECORD_STRUCT    *result = NULL;
    sockaddr                local_addr;
    sockaddr                remote_addr;
    uint32_t                sock;
    char                    *buf;
    uint32_t                id;
    int32_t                 error;
    int                     option_udp_nonblock = TRUE; 
    
    /* Check input parameters.*/
    if (params &&  params->name_to_resolve)
    {
        /* Allocate buffer for output and input message.*/
        buf = RTCS_mem_alloc_zero(DNSCLN_MAX_UDP_MESSAGE_SIZE);
        if (buf)
        {
            _mem_zero(&local_addr, sizeof(local_addr));
            local_addr.sa_family = params->dns_server.sa_family;

            remote_addr = params->dns_server;
            if(RTCS_SOCKADDR_PORT(&remote_addr) == 0)
                RTCS_SOCKADDR_PORT(&remote_addr) = DNSCLN_SERVER_PORT; /* Set default DNS port.*/
            
            /* Create UDP socket.*/
            sock = socket(params->dns_server.sa_family, SOCK_DGRAM, 0);
            if (sock !=  RTCS_SOCKET_ERROR)
            {
                if(setsockopt(sock, SOL_UDP, RTCS_SO_UDP_NONBLOCK_TX, &option_udp_nonblock, sizeof(option_udp_nonblock))== RTCS_OK)
                {
                    error = bind(sock, (const sockaddr *)&local_addr, sizeof(local_addr));
                    if ( error == RTCS_OK )
                    {
                        error = connect(sock, (const sockaddr *)(&remote_addr), sizeof(remote_addr));
                        if (error == RTCS_OK )
                        {
                            error = dnscln_send(sock,  params->name_to_resolve, (uint16_t)params->type, buf, &id);
                            if (error == RTCS_OK )
                            {
                                rtcs_fd_set rfds;
                                RTCS_FD_ZERO(&rfds);
                                RTCS_FD_SET(sock, &rfds);

                                if(1 == select(1,&rfds,0,DNSCLN_TIMEOUT))
                                {
                                    result = dnscln_get_response(sock, params->name_to_resolve, (uint16_t)params->type, buf, id);
                                }
                            }
                        }
                    }
                }

                shutdown(sock, 0);
            }

            _mem_free(buf);
        }
    }

    return result;
}

/************************************************************************
* NAME: DNSCLN_record_list_free
* DESCRIPTION: Frees memory allocated by DNSCLN_resolve() for DNS records. 
*************************************************************************/
void DNSCLN_record_list_free(DNSCLN_RECORD_STRUCT *record_list)
{
    DNSCLN_RECORD_STRUCT *record_list_tmp;

    while(record_list)
    {
        if(record_list->data)
            _mem_free(record_list->data);
        if(record_list->name)
            _mem_free(record_list->name);

        record_list_tmp = record_list;
        record_list = record_list->next;

        _mem_free(record_list_tmp);
    }
}

/************************************************************************
* NAME: DNSCLN_get_dns_addr
* RETURN: TRUE if successful, FALSE otherwise
* DESCRIPTION: Returns N-th DNS IP address from DNS list for given N.
*              If ihandle==0, it return DNS Server Address from all
*              initialised networking interfaces.
*************************************************************************/
bool DNSCLN_get_dns_addr(_rtcs_if_handle ihandle /*Optional.*/, uint32_t n, sockaddr *dns_server)
{
    bool            result = FALSE;
    IP_IF_PTR       ip_if;
    uint32_t        if_n;
    uint32_t        dns_n;

    if(dns_server)
    {
        _mem_zero(dns_server, sizeof(*dns_server));
  
        for(if_n = 0; (((ihandle /* Only for one interface*/)?(ip_if = (IP_IF_PTR)ihandle ) :(ip_if = ip_if_list_get(if_n))) != NULL) ; if_n++) /* Run over all interfaces.*/ 
        {
        #if RTCSCFG_ENABLE_IP4
            {
                _ip_address     dns_addr;

                /* IPv4*/
                for(dns_n = 0; RTCS_if_get_dns_addr((_rtcs_if_handle)ip_if, dns_n, &dns_addr) == TRUE; dns_n++)
                {
                    if(n == 0)  
                    {
                        ((sockaddr_in *)dns_server)->sin_family = AF_INET;
                        ((sockaddr_in *)dns_server)->sin_addr.s_addr = dns_addr;

                        result = TRUE; /* found */
                        goto COMPLETE;
                    }
                    else
                    {
                        n--;
                    }
                }
            }
        #endif
        #if RTCSCFG_ENABLE_IP6
            {
                in6_addr        dns6_addr;

                for(dns_n = 0; RTCS6_if_get_dns_addr((_rtcs_if_handle)ip_if, dns_n, &dns6_addr) == TRUE; dns_n++)
                {
                    if(n == 0)
                    {
                        ((sockaddr_in6 *)dns_server)->sin6_scope_id = RTCS6_if_get_scope_id((_rtcs_if_handle)ip_if);
                        ((sockaddr_in6 *)dns_server)->sin6_family = AF_INET6;
                        IN6_ADDR_COPY(&dns6_addr, &((sockaddr_in6 *)dns_server)->sin6_addr);

                        result = TRUE; /* found */
                        goto COMPLETE;
                    }
                    else
                    {
                        n--;
                    }
                }
            }
        #endif

            if(ihandle) /* Only for one interface.*/
                break;
        }
    }

COMPLETE:
    return result;
}

