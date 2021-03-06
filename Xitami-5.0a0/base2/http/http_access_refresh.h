/*---------------------------------------------------------------------------
    http_access_refresh.h - http_access_refresh component

    Password authentication refresh module.  If this module is loaded, it
    will attempt to recover from certain authentication failures by calling
    a shell command to update the password file(s).  The shell command can
    do whatever is needed - e.g. query a credentials database - to get new
    credentials for the specified user.

    The module issues a system command with this syntax:

        command-name mechanism username realm

    E.g.

        http_refresh Digest mufasa testrealm@host.com

    The module does not wait for a response.  The shell command should update
    the web server password file appropriately; Xitami will reload that data
    and use it.  The user may get a message asking them to retry later.
    Generated from http_access_refresh.icl by smt_object_gen using GSL/4.
    
    Copyright (c) 1996-2009 iMatix Corporation
    All rights reserved.
    
    This file is licensed under the BSD license as follows:
    
    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:
    
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in
      the documentation and/or other materials provided with the
      distribution.
    * Neither the name of iMatix Corporation nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.
    
    THIS SOFTWARE IS PROVIDED BY IMATIX CORPORATION "AS IS" AND ANY
    EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
    PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL IMATIX CORPORATION BE
    LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
    CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
    SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
    BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
    LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
    NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

    These header files use macros to implement a split inclusion in which all
    safe definitions (those that do not depend on the presence of other
    definitions) are done first, and all unsafe definitions are done in a
    second pass through the same headers.

    The first header file included from the main C program defines itself as
    the "root" and thus controls the inclusion of the safe/unsafe pieces of
    the other headers.
 *---------------------------------------------------------------------------*/

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 1)
# ifndef INCLUDE_HTTP_ACCESS_REFRESH_SAFE
#   define INCLUDE_HTTP_ACCESS_REFRESH_SAFE
#   define INCLUDE_HTTP_ACCESS_REFRESH_ACTIVE
#   if !defined (ICL_IMPORT_HEADERS)
#       define ICL_IMPORT_HTTP_ACCESS_REFRESH_ROOT
#       define ICL_IMPORT_HEADERS 1
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif

//  Our own class typedef

typedef struct _http_access_refresh_t http_access_refresh_t;
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_HTTP_ACCESS_REFRESH)
#    define HTTP_ACCESS_REFRESH_HISTORY_LENGTH 32
#endif

#define HTTP_ACCESS_REFRESH_ALIVE       0xFABB
#define HTTP_ACCESS_REFRESH_DEAD        0xDEAD

#define HTTP_ACCESS_REFRESH_ASSERT_SANE(self)\
{\
    if (!self) {\
        fprintf (stderr, "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        fprintf (stderr, "Attempting to work with a NULL http_access_refresh\n");\
        fprintf (stderr, "Please report this to openamq-dev@lists.openamq.org\n");\
        fflush (stderr);\
        abort ();\
    }\
    if (self->object_tag != HTTP_ACCESS_REFRESH_ALIVE) {\
        fprintf (stderr, "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        fprintf (stderr, "http_access_refresh at %p expected object_tag=0x%x, actual object_tag=0x%x\n", self, HTTP_ACCESS_REFRESH_ALIVE, self->object_tag);\
        http_access_refresh_show (self, ICL_CALLBACK_DUMP, stderr);\
        fprintf (stderr, "Please report this to openamq-dev@lists.openamq.org\n");\
        fflush (stderr);\
        abort ();\
    }\
}
#   ifdef __cplusplus
}
#   endif

#   include "http.h"
#   include "http_access_module.h"
#   include "icl_mem.h"
#   ifdef __cplusplus
extern "C" {
#   endif
#include "icl.h"
#include "ipr.h"
#include "smt.h"
#include "http_access_refresh_agent.h"

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_HTTP_ACCESS_REFRESH_ACTIVE
#   if defined (ICL_IMPORT_HTTP_ACCESS_REFRESH_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 2
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 2)
# ifndef INCLUDE_HTTP_ACCESS_REFRESH_UNSAFE
#   define INCLUDE_HTTP_ACCESS_REFRESH_UNSAFE
#   define INCLUDE_HTTP_ACCESS_REFRESH_ACTIVE

#   include "http.h"
#   include "http_access_module.h"
#   include "icl_mem.h"

#   ifdef __cplusplus
extern "C" {
#   endif
#include "icl.h"
#include "ipr.h"
#include "smt.h"
#include "http_access_refresh_agent.h"
//  Global variables

extern Bool
    http_access_refresh_animating;

//  Structure of the http_access_refresh object

struct _http_access_refresh_t {
    volatile int  
        links;                          //  Number of links to the item
    volatile qbyte
        zombie;                         //  Object destroyed but not freed
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_HTTP_ACCESS_REFRESH)
    //  Possession history, for tracing
    volatile qbyte
        history_last;
    char
        *history_file [HTTP_ACCESS_REFRESH_HISTORY_LENGTH];
    int
        history_line  [HTTP_ACCESS_REFRESH_HISTORY_LENGTH];
    char
        *history_type [HTTP_ACCESS_REFRESH_HISTORY_LENGTH];
    int
        history_links [HTTP_ACCESS_REFRESH_HISTORY_LENGTH];
#endif
    dbyte
        object_tag;                     //  Object validity marker
smt_thread_t
    *thread;
ipr_looseref_list_t
    *http_access_module_list;       //  List of portals
Bool
    http_access_module_stopped;     //  Is this portal stopped?
};

#   ifdef __cplusplus
}
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif
//  Method prototypes
int
    http_access_refresh_announce (
http_access_refresh_t * self,           //  Reference to object
http_access_module_t * portal,          //  Portal back to client
smt_log_t * log                         //  Not documented
);

int
    http_access_refresh_before (
http_access_refresh_t * self,           //  Reference to object
http_access_module_t * portal,          //  Portal back to client
http_access_context_t * context         //  Not documented
);

#define http_access_refresh_new()       http_access_refresh_new_ (__FILE__, __LINE__)
http_access_refresh_t *
    http_access_refresh_new_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

void
    http_access_refresh_terminate (
void);

http_access_module_t *
    http_access_refresh__http_access_module_new (
void * self_v                           //  Untyped server reference
);

http_access_module_t *
    http_access_refresh__http_access_module_factory (
void);

int
    http_access_refresh__http_access_module_unbind (
http_access_refresh_t * self,           //  Reference to object
http_access_module_t * portal           //  Portal reference
);

int
    http_access_refresh_open (
http_access_refresh_t * self,           //  Reference to object
http_access_module_t * portal,          //  Portal back to client
http_access_context_t * context         //  Not documented
);

int
    http_access_refresh_after (
http_access_refresh_t * self,           //  Reference to object
http_access_module_t * portal,          //  Portal back to client
http_access_context_t * context         //  Not documented
);

int
    http_access_refresh_close (
http_access_refresh_t * self,           //  Reference to object
http_access_module_t * portal,          //  Portal back to client
http_access_context_t * context         //  Not documented
);

void
    http_access_refresh_selftest (
void);

#define http_access_refresh_destroy(self)  http_access_refresh_destroy_ (self, __FILE__, __LINE__)
int
    http_access_refresh_destroy_ (
http_access_refresh_t * ( * self_p ),   //  Reference to object reference
char * file,                            //  Source file
size_t line                             //  Line number
);

#define http_access_refresh_unlink(self)  http_access_refresh_unlink_ (self, __FILE__, __LINE__)
void
    http_access_refresh_unlink_ (
http_access_refresh_t * ( * self_p ),   //  Reference to object reference
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

void
    http_access_refresh_show_animation (
Bool enabled                            //  Are we enabling or disabling animation?
);

#define http_access_refresh_free(self)  http_access_refresh_free_ (self, __FILE__, __LINE__)
void
    http_access_refresh_free_ (
http_access_refresh_t * self,           //  Object reference
char * file,                            //  Source file
size_t line                             //  Line number
);

#define http_access_refresh_show(item,opcode,trace_file)  http_access_refresh_show_ (item, opcode, trace_file, __FILE__, __LINE__)
void
    http_access_refresh_show_ (
void * item,                            //  The opaque pointer
int opcode,                             //  The callback opcode
FILE * trace_file,                      //  File to print to
char * file,                            //  Source file
size_t line                             //  Line number
);

#define http_access_refresh_link(self)  http_access_refresh_link_ (self, __FILE__, __LINE__)
http_access_refresh_t *
    http_access_refresh_link_ (
http_access_refresh_t * self,           //  Not documented
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

#define http_access_refresh_new_in_scope(self_p,_scope)  http_access_refresh_new_in_scope_ (self_p, _scope, __FILE__, __LINE__)
void
    http_access_refresh_new_in_scope_ (
http_access_refresh_t * * self_p,       //  Not documented
icl_scope_t * _scope,                   //  Not documented
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);


#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_HTTP_ACCESS_REFRESH_ACTIVE
#   if defined (ICL_IMPORT_HTTP_ACCESS_REFRESH_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 3
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 3)
# if !defined (INCLUDE_HTTP_ACCESS_REFRESH_INLINE)
#   define INCLUDE_HTTP_ACCESS_REFRESH_INLINE
#   define INCLUDE_HTTP_ACCESS_REFRESH_ACTIVE

#   include "http.h"
#   include "http_access_module.h"
#   include "icl_mem.h"

#   ifdef __cplusplus
extern "C" {
#   endif

#if (defined (BASE_TRACE)   || defined (BASE_TRACE_HTTP_ACCESS_REFRESH)   || defined (BASE_TRACE_HTTP_ACCESS_REFRESH_ANNOUNCE)   || defined (BASE_TRACE_HTTP_ACCESS_REFRESH_BEFORE)   || defined (BASE_TRACE_HTTP_ACCESS_REFRESH_NEW)   || defined (BASE_TRACE_HTTP_ACCESS_REFRESH_DESTROY)   || defined (BASE_TRACE_HTTP_ACCESS_REFRESH_INITIALISE)   || defined (BASE_TRACE_HTTP_ACCESS_REFRESH_TERMINATE)   || defined (BASE_TRACE_HTTP_ACCESS_REFRESH__HTTP_ACCESS_MODULE_NEW)   || defined (BASE_TRACE_HTTP_ACCESS_REFRESH__HTTP_ACCESS_MODULE_FACTORY)   || defined (BASE_TRACE_HTTP_ACCESS_REFRESH__HTTP_ACCESS_MODULE_UNBIND_SHIM)   || defined (BASE_TRACE_HTTP_ACCESS_REFRESH__HTTP_ACCESS_MODULE_UNBIND)   || defined (BASE_TRACE_HTTP_ACCESS_REFRESH_ANNOUNCE_SHIM)   || defined (BASE_TRACE_HTTP_ACCESS_REFRESH_OPEN_SHIM)   || defined (BASE_TRACE_HTTP_ACCESS_REFRESH_OPEN)   || defined (BASE_TRACE_HTTP_ACCESS_REFRESH_BEFORE_SHIM)   || defined (BASE_TRACE_HTTP_ACCESS_REFRESH_AFTER_SHIM)   || defined (BASE_TRACE_HTTP_ACCESS_REFRESH_AFTER)   || defined (BASE_TRACE_HTTP_ACCESS_REFRESH_CLOSE_SHIM)   || defined (BASE_TRACE_HTTP_ACCESS_REFRESH_CLOSE)   || defined (BASE_TRACE_HTTP_ACCESS_REFRESH__HTTP_ACCESS_MODULE_LINK)   || defined (BASE_TRACE_HTTP_ACCESS_REFRESH__HTTP_ACCESS_MODULE_UNLINK)   || defined (BASE_TRACE_HTTP_ACCESS_REFRESH_SELFTEST)   || defined (BASE_TRACE_HTTP_ACCESS_REFRESH_DESTROY_PUBLIC)   || defined (BASE_TRACE_HTTP_ACCESS_REFRESH_UNLINK)   || defined (BASE_TRACE_HTTP_ACCESS_REFRESH_SHOW_ANIMATION)   || defined (BASE_TRACE_HTTP_ACCESS_REFRESH_FREE)   || defined (BASE_TRACE_HTTP_ACCESS_REFRESH_SHOW)   || defined (BASE_TRACE_HTTP_ACCESS_REFRESH_ALLOC)   || defined (BASE_TRACE_HTTP_ACCESS_REFRESH_LINK)   || defined (BASE_TRACE_HTTP_ACCESS_REFRESH_CACHE_INITIALISE)   || defined (BASE_TRACE_HTTP_ACCESS_REFRESH_CACHE_PURGE)   || defined (BASE_TRACE_HTTP_ACCESS_REFRESH_CACHE_TERMINATE)   || defined (BASE_TRACE_HTTP_ACCESS_REFRESH_NEW_IN_SCOPE) )
void
    http_access_refresh_dump (icl_os_thread_t thread, apr_time_t time, qbyte info);
#endif

#include "icl.h"
#include "ipr.h"
#include "smt.h"
#include "http_access_refresh_agent.h"
#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_HTTP_ACCESS_REFRESH_ACTIVE
#   if defined (ICL_IMPORT_HTTP_ACCESS_REFRESH_ROOT)
#       undef ICL_IMPORT_HEADERS
#       undef ICL_IMPORT_HTTP_ACCESS_REFRESH_ROOT
#   endif
# endif
#endif
