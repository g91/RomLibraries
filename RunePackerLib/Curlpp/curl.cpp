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

#include <string>

#include "curl.hpp"
#include "curlpp.hpp"

static const char *ssl_cert_type_vector[2] = { "PEM", "DER" };

static const char *ssl_key_type_vector[3] = { "PEM", "DER", "ENG" };

static const char *krb4_level_vector[5] = 
{ 
   NULL, 
   "clear",
   "safe",
   "confidential",
   "private"
};


void curlpp::curl::perform()
{
   CURLcode code;

   logic_assert(
         curlpp::is_initialized(),
         "Trying to use curlpp without initializing it before."
         );
   code = curl_easy_perform( m_curl );
   runtime_assert( code == CURLE_OK, m_error_buffer ); //we got an error
	}

curlpp::factory* curlpp::factory::m_instance = 0;

#ifdef CURLPP_DEBUG
curlpp::factory_safe curlpp::factory::m_safe;
#endif

curlpp::factory::factory( long flags )
{
   // load the options for the library
   // this is the place to load SSL and
   // other stuff
   CURLcode code;
   code = curl_global_init( flags );
   if ( code != CURLE_OK )
   {
      throw runtime_error( "cannot initialize curlpp" );
   }
}

curlpp::factory::~factory()
{
   curl_global_cleanup();
}

//\TODO add execption handling
curlpp::curl::curl() 
{
   m_curl = curl_easy_init();
   if ( m_curl == NULL )
   {
      throw runtime_error( m_error_buffer ); //we got an error
   }

   error_buffer( m_error_buffer );
}

curlpp::curl::~curl()
{
   curl_easy_cleanup( m_curl );
}

void 
curlpp::curl::member_string_option(
      std::string &member,
      const std::string &param,
      CURLoption opt
      )
{
   member = param;
   if ( param.size() > 0 )
   {
      option( opt, ( void * ) member.c_str() );
   }
   else
   {
      option( opt, ( void * ) NULL );
   }
}

void 
curlpp::curl::url( const std::string &link)
{
	member_string_option( m_url, link, CURLOPT_URL );
	
   
}
void
curlpp::curl::port( long port)
{
	
	option( CURLOPT_PORT,port  );

}

void 
curlpp::curl::verbose( bool activate )
{
   option( CURLOPT_VERBOSE, activate );
}

void 
curlpp::curl::header( bool include )
{
   option( CURLOPT_HEADER, static_cast< long >( include ) );
}

void 
curlpp::curl::signal( bool activate )
{
   option( CURLOPT_NOSIGNAL, static_cast< long >( !activate ) );
}

void 
curlpp::curl::write_data( curlpp::output_trait *trait )
{
   option( CURLOPT_WRITEDATA, trait );
}

void 
curlpp::curl::write_function( write_function_t function )
{
   option( CURLOPT_WRITEFUNCTION, ( void * ) function );
}

void 
curlpp::curl::write_header_function( write_function_t function )
{
   option( CURLOPT_HEADERFUNCTION, ( void * ) function );
}

void
curlpp::curl::write_header_data( curlpp::output_trait *trait )
{
   option( CURLOPT_WRITEHEADER, trait );
}

void
curlpp::curl::read_data( curlpp::input_trait *trait )
{
   option( CURLOPT_READDATA, trait );
}

void 
curlpp::curl::read_function( read_function_t function )
{
   option( CURLOPT_READFUNCTION, ( void * ) function );
}

void 
curlpp::curl::progress_function( progress_function_t function )
{
   option( CURLOPT_PROGRESSFUNCTION, ( void * ) function );
}

void 
curlpp::curl::progress_data( progress_trait *trait )
{
   option( CURLOPT_PROGRESSDATA, ( void * ) trait );
}

void
curlpp::curl::progress( bool activate )
{
   option( CURLOPT_NOPROGRESS, static_cast< long >( !activate ) );
}

void 
#if LIBCURL_VERSION_NUM < 0x070b01
curlpp::curl::passwd_function( passwd_function_t function )
{
   option( CURLOPT_PASSWDFUNCTION, ( void * ) function );
}
#else
curlpp::curl::passwd_function( passwd_function_t )
{
   runtime_assert( false, "The CURLOPT_PASSWDFUNCTION option has been deprecated." );
}
#endif


void
#if LIBCURL_VERSION_NUM < 0x070b01
curlpp::curl::passwd_data( passwd_trait *trait )
{
   option( CURLOPT_PASSWDDATA, static_cast< void * >( trait ) );
}
#else
curlpp::curl::passwd_data( passwd_trait * )
{
   runtime_assert( false, "The CURLOPT_PASSWDDATA option has been deprecated." );
}
#endif

void
curlpp::curl::debug_function( debug_function_t function )
{
   option( CURLOPT_DEBUGFUNCTION, ( void * ) function );
}

void 
curlpp::curl::debug_data( debug_trait *trait )
{
   option( CURLOPT_DEBUGDATA, ( void * ) trait );
}

void
curlpp::curl::fail_on_error( bool activate )
{
   option( CURLOPT_FAILONERROR, static_cast< long >( activate ) );
}

void 
curlpp::curl::dns_cache_timeout( long seconds )
{
   option( CURLOPT_DNS_CACHE_TIMEOUT, seconds );
}

void
curlpp::curl::dns_use_global_cache( bool activate )
{
   option( CURLOPT_DNS_USE_GLOBAL_CACHE, static_cast< long >( activate ) );
}

void
curlpp::curl::buffersize( long size )
{
   option( CURLOPT_BUFFERSIZE, size );
}

void 
curlpp::curl::proxy_name( const std::string &proxy_string )
{
   member_string_option( m_proxy, proxy_string, CURLOPT_PROXY );
}

void
curlpp::curl::proxy_port( long port )
{
   option( CURLOPT_PROXYPORT, port );
}

void 
curlpp::curl::proxy_type( curlpp::proxy_type::type type )
{
   option( CURLOPT_PROXYTYPE, type );
}

void
curlpp::curl::http_proxy_tunnel( bool activate )
{
   option( CURLOPT_HTTPPROXYTUNNEL, static_cast< long >( activate ) );
}

void
curlpp::curl::network_interface( const std::string &name )
{
   member_string_option( m_interface, name, CURLOPT_INTERFACE );
}

void
curlpp::curl::infile_size( long size )
{
   option( CURLOPT_INFILESIZE, size );
}

void
curlpp::curl::netrc( curlpp::netrc::type netrc_use )
{
   option( CURLOPT_NETRC, static_cast< long >( netrc_use ) );
}

void 
curlpp::curl::user_pwd( const std::string &auth )
{
   member_string_option( m_user_pwd, auth, CURLOPT_USERPWD );
}

void 
curlpp::curl::proxy_user_pwd( const std::string &auth )
{
   member_string_option( m_proxy_user_pwd, auth, CURLOPT_PROXYUSERPWD );
}

void 
curlpp::curl::encoding( const std::string &algorithm )
{
   member_string_option( m_encoding, algorithm, CURLOPT_ENCODING );
}

void 
curlpp::curl::follow_location( bool activate )
{
   option( CURLOPT_FOLLOWLOCATION, static_cast< long >( activate ) );
}

void 
curlpp::curl::max_redir( long nb )
{
   option( CURLOPT_MAXREDIRS, nb );
}

void
curlpp::curl::put( bool activate )
{
   option( CURLOPT_PUT, static_cast< long >( activate ) );
}

void 
curlpp::curl::post_fields( const std::string &data )
{
   member_string_option( m_post_fields, data, CURLOPT_POSTFIELDS );
}

void 
curlpp::curl::post_fields( const char *data, long length )
{
   option( CURLOPT_POSTFIELDS, (void *)data );
   option( CURLOPT_POSTFIELDSIZE, length );
}

// void 
// curlpp::curl::http_post( const std::list<curlpp::form *> &posts )
// {
//   m_http_posts = posts;
//   option( CURLOPT_HTTPPOST, m_http_posts.c_http_post() );
// }

void
curlpp::curl::referer( const std::string &name )
{
   member_string_option( m_referer, name, CURLOPT_REFERER );
}

void
curlpp::curl::user_agent( const std::string &name )
{
   member_string_option( m_user_agent, name, CURLOPT_USERAGENT );
}

void 
curlpp::curl::http_header( const std::list< std::string > &list )
{
   m_http_headers = list;
   option( CURLOPT_HTTPHEADER, m_http_headers.c_slist() );
}

void
curlpp::curl::cookie( const std::string &cookie_string )
{
   member_string_option( m_cookie, cookie_string, CURLOPT_COOKIE );
}

void
curlpp::curl::cookie_file( const std::string &file_name )
{
   member_string_option( m_cookie_file, file_name, CURLOPT_COOKIEFILE );
}

void 
curlpp::curl::cookie_jar( const std::string &file_name )
{
   member_string_option( m_cookie_jar, file_name, CURLOPT_COOKIEJAR );
}

void 
curlpp::curl::time_condition( curlpp::time_condition::type since )
{
   option( CURLOPT_TIMECONDITION, static_cast< long >( since ) );
}

void 
curlpp::curl::time_value( long seconds )
{
   option( CURLOPT_TIMEVALUE, seconds );
}

void 
curlpp::curl::http_get( bool activate )
{
   option( CURLOPT_HTTPGET, static_cast< long >( activate ) );
}

void 
curlpp::curl::http_version( curlpp::http_version::type version )
{
   option( CURLOPT_HTTP_VERSION, static_cast< long >( version ) );
}

void 
curlpp::curl::ftp_port( const std::string &port )
{
   member_string_option( m_ftp_port, port, CURLOPT_FTPPORT );
}

void 
curlpp::curl::quote( const std::list< std::string > &list )
{
   m_quote = list;
   option( CURLOPT_QUOTE, (void *)m_quote.c_slist() );
}

void 
curlpp::curl::post_quote( const std::list< std::string > &list )
{
   m_post_quote = list;
   option( CURLOPT_POSTQUOTE, (void *)m_post_quote.c_slist() );
}

void 
curlpp::curl::pre_quote( const std::list< std::string > &list )
{
   m_pre_quote = list;
   option( CURLOPT_PREQUOTE, m_pre_quote.c_slist() );
}

void 
curlpp::curl::ftp_list_only( bool activate )
{
   option( CURLOPT_FTPLISTONLY, static_cast< long >( activate ) );
}

void 
curlpp::curl::ftp_append( bool activate )
{
   option( CURLOPT_FTPAPPEND, static_cast< long >( activate ) );
}

void
curlpp::curl::ftp_use_epsv( bool activate )
{
   option( CURLOPT_FTP_USE_EPSV, static_cast< long >( activate ) );
}

void 
curlpp::curl::transfert_text( bool activate )
{
   option( CURLOPT_TRANSFERTEXT, static_cast< long >( activate ) );
}

void 
curlpp::curl::crlf( bool activate )
{
   option( CURLOPT_CRLF, static_cast< long >( activate ) );
}

void 
curlpp::curl::range( const std::string &interval )
{
   member_string_option( m_range, interval, CURLOPT_RANGE );
}

void 
curlpp::curl::resume_from( long bytes )
{
   option( CURLOPT_RESUME_FROM, bytes );
}

void 
curlpp::curl::custom_request( const std::string &request )
{
   member_string_option( m_custom_request, request, CURLOPT_CUSTOMREQUEST );
}

void 
curlpp::curl::file_time( bool activate )
{
   option( CURLOPT_FILETIME, static_cast< long >( activate ) );
}

void 
curlpp::curl::no_body( bool activate )
{
   option( CURLOPT_NOBODY, static_cast< long >( activate ) );
}

void 
curlpp::curl::upload( bool activate )
{
   option( CURLOPT_UPLOAD, static_cast< long >( activate ) );
}

void 
curlpp::curl::timeout( long time )
{
   option( CURLOPT_TIMEOUT, time );
}

void 
curlpp::curl::low_speed_limit( long bytes )
{
   option( CURLOPT_LOW_SPEED_LIMIT, bytes );
}

void 
curlpp::curl::low_speed_time( long time )
{
   option( CURLOPT_LOW_SPEED_TIME, time );
}

void 
curlpp::curl::max_connects( long nb_connects )
{
   option( CURLOPT_MAXCONNECTS, nb_connects );
}

void 
curlpp::curl::close_policy( curlpp::close_policy::type policy )
{
   option( CURLOPT_CLOSEPOLICY, static_cast< long >( policy ) );
}

void 
curlpp::curl::fresh_connect( bool new_connect )
{
   option( CURLOPT_FRESH_CONNECT, static_cast< long >( new_connect ) );
}

void 
curlpp::curl::forbid_reuse( bool close )
{
   option( CURLOPT_FORBID_REUSE, static_cast< long >( close ) );
}

void 
curlpp::curl::connect_timeout( long seconds )
{
   option( CURLOPT_CONNECTTIMEOUT, seconds );
}

void 
curlpp::curl::ssl_cert( const std::string &filename )
{
   member_string_option( m_ssl_cert, filename, CURLOPT_SSLCERT );
}

void 
curlpp::curl::ssl_cert_type( curlpp::ssl_cert_type::type format )
{
   option( CURLOPT_SSLCERTTYPE, ssl_cert_type_vector[ format ] );
}

void 
curlpp::curl::ssl_key( const std::string &key )
{
   member_string_option( m_ssl_key, key, CURLOPT_SSLKEY );
}

void 
curlpp::curl::ssl_key_type( curlpp::ssl_key_type::type key_type )
{
   option( CURLOPT_SSLKEYTYPE, ssl_key_type_vector[ key_type ] );
}

void 
curlpp::curl::ssl_key_passwd( const std::string &key_passwd )
{
   member_string_option( m_ssl_key_passwd, key_passwd, CURLOPT_SSLKEYPASSWD );
}

void 
curlpp::curl::ssl_engine( const std::string &engine )
{
   member_string_option( m_ssl_engine, engine, CURLOPT_SSLENGINE );
}

void 
curlpp::curl::ssl_engine_default()
{
   option( CURLOPT_SSLENGINE_DEFAULT );
}

void 
curlpp::curl::ssl_version( curlpp::ssl_version::type version )
{
   option( CURLOPT_SSLVERSION, static_cast< long >( version ) );
}

void 
curlpp::curl::ssl_verify_peer( bool activate )
{
   option( CURLOPT_SSL_VERIFYPEER, static_cast< bool >( activate ) );
}

void 
curlpp::curl::ca_info( const std::string &filename )
{
   member_string_option( m_ca_info, filename, CURLOPT_CAINFO );
}

void 
curlpp::curl::ca_path( const std::string &directory )
{
   member_string_option( m_ca_path, directory, CURLOPT_CAPATH );
}

void 
curlpp::curl::random_file( const std::string &filename )
{
   member_string_option( m_random_file, filename, CURLOPT_RANDOM_FILE );
}

void 
curlpp::curl::egd_socket( const std::string &path )
{
   member_string_option( m_egd_socket, path, CURLOPT_EGDSOCKET );
}

void 
curlpp::curl::ssl_verify_host( curlpp::ssl_verify_host::type verify )
{
   option( CURLOPT_SSL_VERIFYHOST, static_cast< long >( verify ) );
}

void 
curlpp::curl::ssl_cipher_list( const std::list< std::string > &list )
{
   std::string ciphers;

   if( list.size() > 0 )
   {
         std::list< std::string >::const_iterator pos = list.begin();
         ciphers.append( (*pos) );
         pos++;
         
         while( pos != list.end() )
         {
            ciphers.append( "," );
            ciphers.append( (*pos) );
            pos++;
         }
   }
   else
   {
      ciphers.append( "DEFAULT" );
   }

   member_string_option( 
         m_ssl_cipher_list, 
         ciphers, 
         CURLOPT_SSL_CIPHER_LIST
         );
}

void 
curlpp::curl::krb4_level( curlpp::krb4_level::type level )
{
   option( CURLOPT_KRB4LEVEL, krb4_level_vector[ level ] );
}

void 
curlpp::curl::error_buffer( char* buffer )
{
   option( CURLOPT_ERRORBUFFER, buffer );
}

void 
curlpp::curl::option( CURLoption curl_option, const char *curl_data )
{
   CURLcode code;
   code = curl_easy_setopt( m_curl, curl_option, (void *)curl_data );

   curlpp::runtime_assert( code == CURLE_OK , m_error_buffer );
}

void 
curlpp::curl::option( CURLoption curl_option, void *curl_data )
{
   CURLcode code;
   code = curl_easy_setopt( m_curl, curl_option, curl_data );

   curlpp::runtime_assert( code == CURLE_OK , m_error_buffer );
}

void 
curlpp::curl::option( CURLoption curl_option, long curl_data )
{
   CURLcode code;
   code = curl_easy_setopt( m_curl, curl_option, curl_data );

   runtime_assert( code == CURLE_OK , m_error_buffer );
}

void 
curlpp::curl::option( CURLoption curl_option )
{
   CURLcode code;
   code = curl_easy_setopt( m_curl, curl_option );

   runtime_assert( code == CURLE_OK , m_error_buffer );
}

bool 
curlpp::factory::is_initialized()
{
   return m_instance;
}

curlpp::factory *
curlpp::factory::instance()
{
   curlpp::runtime_assert( m_instance, "The curlpp library is not loaded" );

   return m_instance;
}

void 
curlpp::factory::initialize( long flags )
{
   if ( m_instance == NULL )
   {
      m_instance = new factory( flags );
   }
}

void 
curlpp::factory::terminate()
{
   logic_assert( is_initialized(), "Trying to terminate curlpp when it's not loaded" );
   if ( m_instance != NULL )
   {
      delete m_instance;
      m_instance = NULL;
   }
}

curlpp::curl* 
curlpp::factory::create_curl()
{
   logic_assert( is_initialized(), "Trying to use curlpp without initializing it before." );
   curlpp::curl *new_curl = new curlpp::curl;

   return new_curl;
}

curlpp::factory_safe::~factory_safe()
{
   if ( factory::m_instance != NULL )
   {
      std::cout << "Error: curl Library was not unloaded" << std::endl;
      //terminate();
   }
}


