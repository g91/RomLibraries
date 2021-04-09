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

#ifndef HTTP_EASY_HPP
#define HTTP_EASY_HPP

#include "easy.hpp"

namespace curlpp
{
   /**
    * Use this class if you want to use some specific options for http, but
    * check before if you just need some basic options offered by
    * curlpp::easy.
    */
   class http_easy: public easy
   {
      public:
         http_easy();
         
         /**
          * See curlpp::curl::encoding( const std::string & ) for more
          * documentation.
          */
         void encoding( const std::string &algorithm );

         /**
          * See curlpp::curl::follow_location( bool ) for more documentation.
          */
         void follow_location( bool activate = true );

         /**
          * See curlpp::curl::max_redir( long ) for more documentation.
          */
         void max_redir( long nb );

         /** 
          * See curlpp::curl:put( bool ) for more documentation.
          */
         void put( bool activate = true );

         /**
          * See curlpp::curl::post_fields( const std::string &data ) for more
          * documentation.
          */
         void post_fields( const std::string &data );

         /**
          * See curlpp::curl::post_fields( const char *, long ) for more
          * documentation.
          */
         void post_fields( const char *data, long length = 0 );
         
         /**
          * See curlpp::curl::http_post( const std::list<curlpp::form *> & ) for more
          * documentation.
          */
         //void http_post( const std::list<curlpp::form *> &posts );
        
         /**
          * See curlpp::curl::referer( const std::string & ) for more
          * documentation.
          */
         void referer( const std::string &name );

         /**
          * See curlpp::curl::user_agent( const std::string & ) for more
          * documentation.
          */
         void user_agent( const std::string &name );

         /** 
          * See curlpp::curl::http_header( const std::list< std::string > & )
          * for more documentation.
          */
         void http_header( const std::list< std::string > &list ); 
         
         /**
          * See curlpp::curl::cookie( const std::string & ) for more
          * documentation.
          */
         void cookie( const std::string &cookie_string );

         /**
          * See curlpp::curl::cookie_file( const std::string & ) for more
          * documentation.
          */
         void cookie_file( const std::string &file_name );

         /**
          * See curlpp::curl::cookie_jar( const std::string & ) for more
          * documentation.
          */
         void cookie_jar( const std::string &file_name );

         /**
          * See curlpp::curl::time_condition( curlpp::time_condition::type ) for more
          * documentation.
          */
         void time_condition( curlpp::time_condition::type since );

         /**
          * See curlpp::curl::time_value( long ) for more documentation.
          */
         void time_value( long seconds );

         /**
          * See curlpp::curl::http_get( bool ) for more documentation.
          */
         void http_get( bool activate = true );

         /**
          * See curlpp::curl::http_version( curlpp::http_version::type ) for
          * more documentation.
          */
         void http_version( curlpp::http_version::type version );

   };
};

#endif
