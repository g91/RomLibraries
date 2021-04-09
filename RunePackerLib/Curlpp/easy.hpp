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

#ifndef CURLPP_EASY_HPP
#define CURLPP_EASY_HPP

#include "curl.hpp"
#include "storage.hpp"


namespace curlpp
{
   /**
    * This class is the class you need to use when you're using standard (not
    * specify to FTP or HTTP) request. If you want to know more about using
    * this class read the "curlpp - the guide" document.
    */
   class easy
   {
      private:
         easy( const easy &other );
         

      public:
         friend class proxy;

         easy();
         
         virtual ~easy();

         void perform();

         /**
          * See curlpp::curl::url( const std::string & ) for more
          * documentation.
          */
         void url( const std::string &link );

		  void port( int port );

         /**
          * See curlpp::curl::verbose( bool ) for more
          * documentation.
          */
         void verbose( bool activate = true );

         /**
          * See curlpp::curl::header( bool ) for more
          * documentation.
          */
         void header( bool include = true );

         /**
          * See curlpp::curl::signal( bool ) for more
          * documentation.
          */
         void signal( bool activate = false );

         /**
          * See curlpp::curl::progress( bool ) for more
          * documentation.
          */
         void progress( bool activate = true );

         /**
          * See curlpp::curl::fail_on_error( bool ) for more
          * documentation.
          */
         void fail_on_error( bool activate = true );

         /**
          * See curlpp::curl::dns_cache_timeout( long ) for more
          * documentation.
          */
         void dns_cache_timeout( long seconds );

         /**
          * See curlpp::curl::dns_use_global_cache( bool ) for more
          * documentation.
          */
         void dns_use_global_cache( bool activate = true );

         /**
          * See curlpp::curl::buffersize( long ) for more
          * documentation.
          */
         void buffersize( long size );

         /**
          * See curlpp::curl::proxy_name( const std::string & ) for more
          * documentation.
          */
         void proxy_name( const std::string &proxy_string );

         /**
          * See curlpp::curl::proxy_port( long ) for more
          * documentation.
          */
         void proxy_port( long port );

         /**
          * See curlpp::curl::proxy_type( curlpp::proxy_type::type ) for more
          * documentation.
          */
         void proxy_type( curlpp::proxy_type::type type );

         /**
          * See curlpp::curl::http_proxy_tunnel( bool ) for more
          * documentation.
          */
         void http_proxy_tunnel( bool activate = true );

         /**
          * See curlpp::curl::network_interface( const std::string & ) for more
          * documentation.
          */
         void network_interface( const std::string &name );

         /**
          * See curlpp::curl::infile_size( long ) for more
          * documentation.
          */
         void infile_size( long size );

         /**
          * See curlpp::curl::netrc( curlpp::netrc::type ) for more
          * documentation.
          */
         void netrc( curlpp::netrc::type netrc_use );

         /**
          * See curlpp::curl::transfert_text( bool ) for more
          * documentation.
          */
         void transfert_text( bool activate = true );

         /**
          * See curlpp::curl::crlf( bool ) for more
          * documentation.
          */
         void crlf( bool activate = true );

         /**
          * See curlpp::curl::range( const std::string & ) for more
          * documentation.
          */
         void range( const std::string &interval );

         /**
          * See curlpp::curl::resume_from( long ) for more
          * documentation.
          */
         void resume_from( long bytes );

         /**
          * See curlpp::curl::custom_request( const std::string & ) for more
          * documentation.
          */
         void custom_request( const std::string &request );

         /**
          * See curlpp::curl::file_time( bool ) for more
          * documentation.
          */
         void file_time( bool activate = true );

         /**
          * See curlpp::curl::no_body( bool ) for more
          * documentation.
          */
         void no_body( bool activate = true );

         /**
          * See curlpp::curl::upload( bool ) for more
          * documentation.
          */
         void upload( bool activate = true );

         /**
          * See curlpp::curl::timeout( long ) for more
          * documentation.
          */
         void timeout( long time );

         /**
          * See curlpp::curl::low_speed_limit( long ) for more
          * documentation.
          */
         void low_speed_limit( long bytes );

         /**
          * See curlpp::curl::low_speed_time( long ) for more
          * documentation.
          */
         void low_speed_time( long time );

         /**
          * See curlpp::curl::max_connects( long ) for more
          * documentation.
          */
         void max_connects( long nb_connects );

         /**
          * See curlpp::curl::close_policy( curlpp::close_policy::type ) for more
          * documentation.
          */
         void close_policy( curlpp::close_policy::type policy );

         /**
          * See curlpp::curl::fresh_connect( bool ) for more
          * documentation.
          */
         void fresh_connect( bool new_connect );

         /**
          * See curlpp::curl::forbid_reuse( bool ) for more
          * documentation.
          */
         void forbid_reuse( bool close );

         /**
          * See curlpp::curl::connect_timeout( long ) for more
          * documentation.
          */
         void connect_timeout( long seconds );

         /**
          * See curlpp::curl::ssl_cert( const std::string & ) for more
          * documentation.
          */
         void ssl_cert( const std::string &filename );

         /**
          * See curlpp::curl::ssl_cert_type( curlpp::ssl_cert_type::type ) for
          * more documentation.
          */
         void ssl_cert_type( curlpp::ssl_cert_type::type format );

         /**
          * See curlpp::curl::ssl_key( const std::string & ) for more
          * documentation.
          */
         void ssl_key( const std::string &key );

         /**
          * See curlpp::curl::ssl_key_type( curlpp::ssl_key_type::type ) for more
          * documentation.
          */
         void ssl_key_type( curlpp::ssl_key_type::type key_type );

         /**
          * See curlpp::curl::ssl_key_passwd( const std::string & ) for more
          * documentation.
          */
         void ssl_key_passwd( const std::string &key_passwd );

         /**
          * See curlpp::curl::ssl_engine( const std::string & ) for more
          * documentation.
          */
         void ssl_engine( const std::string &engine );

         /**
          * See curlpp::curl::ssl_engine_default() for more
          * documentation.
          */
         void ssl_engine_default();

         /**
          * See curlpp::curl::ssl_version( curlpp:ssl_version::type ) for more
          * documentation.
          */
         void ssl_version( curlpp::ssl_version::type version );

         /**
          * See curlpp::curl::ssl_verify_peer( bool ) for more
          * documentation.
          */
         void ssl_verify_peer( bool activate );

         /**
          * See curlpp::curl::ca_info( const std::string & ) for more
          * documentation.
          */
         void ca_info( const std::string &filename );

         /**
          * See curlpp::curl::ca_path( const std::string & ) for more
          * documentation.
          */
         void ca_path( const std::string &directory );

         /**
          * See curlpp::curl::random_file( const std::string & ) for more
          * documentation.
          */
         void random_file( const std::string &filename );

         /**
          * See curlpp::curl::egd_socket( const std::string & ) for more
          * documentation.
          */
         void egd_socket( const std::string &path );

         /**
          * See curlpp::curl::ssl_verify_host( curlpp::ssl_verify_host::type )
          * for more documentation.
          */
         void ssl_verify_host( curlpp::ssl_verify_host::type verify );

         /**
          * See curlpp::curl::ssl_cipher_list( const std::list< std:string > &
          * ) for more documentation.
          */
         void ssl_cipher_list( const std::list< std::string > &list );

         /**
          * See curlpp::curl::krb4_level( curlpp::krb4_level::type ) for more
          * documentation.
          */
         void krb4_level( curlpp::krb4_level::type level );

         bool operator==( const curlpp::easy &other ) const
         {
            return m_data == other.m_data;
         }

         bool operator!=( const curlpp::easy &other ) const
         {
            return m_data != other.m_data;
         }

         bool operator<( const curlpp::easy &other ) const
         {
            return m_data < other.m_data;
         }

      protected:
         curlpp::curl *m_data;

      public:
          storage< curlpp::body_storage > m_body_storage;
          storage< curlpp::input_storage > m_input_storage;
          storage< curlpp::header_storage > m_header_storage;
          storage< curlpp::progress_storage > m_progress_storage;
          storage< curlpp::passwd_storage > m_passwd_storage;
          storage< curlpp::debug_storage > m_debug_storage;
   };
};
#endif
