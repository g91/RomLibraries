/*
*    Copyright (c) <2002-2005> <Jean-Philippe Barrette-LaPierre>
*    
*    Permission is hereby granted, free of charge, to any person obtaining
*    a copy of this software and associated documentation files 
*    (curlpp), to deal in the Software without restriction, 
*    including without limitation the rights to use, copy, modify, merge,
*    publish, distribute, sublicense, and/or sell copies of the Software,
*    and to permit persons to whom the Software is furnished to do so, 
*    subject to the following conditions:
*    
*    The above copyright notice and this permission notice shall be included
*    in all copies or substantial portions of the Software.
*    
*    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
*    OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
*    IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY 
*    CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, 
*    TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
*    SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <iostream>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif //HAVE_CONFIG

#include "curlpp.hpp"


curlpp::body_storage::body_storage(
   curlpp::curl *parent,
   curlpp::output_trait *trait,
   bool own
)
      :
      storage_base< curlpp::output_trait >( parent, trait, &stdout_trait, own )
{}

size_t curlpp::body_storage::write_callback(
   void *buffer,
   size_t size,
   size_t nitems,
   void *outstream
)
{
   curlpp::output_trait * trait = static_cast< curlpp::output_trait *>( outstream );
   return trait->write( buffer, size * nitems );
}

void curlpp::body_storage::set_callback()
{
   m_parent->write_function( &write_callback );
}

void curlpp::body_storage::set_callback_data()
{
   m_parent->write_data( m_trait );
}

void curlpp::header_storage::set_callback()
{
   m_parent->write_header_function( &write_callback );
}

void curlpp::header_storage::set_callback_data()
{
   m_parent->write_header_data( m_trait );
}

size_t curlpp::header_storage::write_callback(
   void *buffer,
   size_t size,
   size_t nitems,
   void *outstream
)
{
   curlpp::output_trait * trait = static_cast< curlpp::output_trait *>( outstream );
   if(trait != NULL) {
     return trait->write( buffer, size * nitems );
   }
   else {
     return size * nitems;
   }
}

void curlpp::input_storage::set_callback_data()
{
   m_parent->read_data( m_trait );
}

void curlpp::input_storage::set_callback()
{
   m_parent->read_function( &read_callback );
}

size_t curlpp::input_storage::read_callback(
   void *buffer,
   size_t size,
   size_t nmemb,
   void *instream
)
{
   curlpp::input_trait * trait = static_cast< curlpp::input_trait *>( instream );
   return trait->read( buffer, size * nmemb );
}

void curlpp::progress_storage::set_callback()
{
   m_parent->progress_function( &progress_callback );
}

void curlpp::progress_storage::set_callback_data()
{
   m_parent->progress_data( m_trait );
}

int curlpp::progress_storage::progress_callback(
   void *clientp,
   double dltotal,
   double dlnow,
   double ultotal,
   double ulnow
)
{
   curlpp::progress_trait * trait = static_cast< curlpp::progress_trait *>( clientp );
   return !( trait->progress_function( dltotal, dlnow, ultotal, ulnow ) );
}

void curlpp::passwd_storage::set_callback()
{
   m_parent->passwd_function( &passwd_callback );
}

void curlpp::passwd_storage::set_callback_data()
{
   m_parent->passwd_data( m_trait );
}

int curlpp::passwd_storage::passwd_callback(
   void *client,
   char *prompt,
   char *buffer,
   int buflen
)
{
   curlpp::passwd_trait * trait = static_cast< curlpp::passwd_trait * >( client );
   return !( trait->passwd_function( prompt, buffer, buflen ) );
}

void curlpp::debug_storage::set_callback()
{
   m_parent->debug_function( &debug_callback );
}

void curlpp::debug_storage::set_callback_data()
{
   m_parent->debug_data( m_trait );
}

int curlpp::debug_storage::debug_callback(
   CURL *,
   curlpp::debug::type type,
   char *buffer,
   size_t len,
   void *ptr
)
{
   curlpp::debug_trait * trait = static_cast< curlpp::debug_trait * >( ptr );
   trait->debug_function( type, buffer, len );
   return 0;
}

#if LIBCURL_VERSION_NUM >= 0x070b01
curlpp::storage< curlpp::passwd_storage >::storage( curl * )
{}

void curlpp::storage< curlpp::passwd_storage >::trait( curlpp::passwd_trait *, bool )
{
   runtime_assert( false, "The CURLOPT_PASSWDFUNCTION option has been deprecated." );
}
#endif

