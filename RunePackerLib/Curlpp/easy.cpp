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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "easy.hpp"

curlpp::easy::easy() : 
  m_data( curlpp::factory::instance()->create_curl() ),
  m_body_storage( m_data ),
  m_input_storage( m_data ),
  m_header_storage( m_data ),
  m_progress_storage( m_data ),
  m_passwd_storage( m_data ),
  m_debug_storage( m_data )
{}

curlpp::easy::~easy()
{
  delete m_data;
  m_data = NULL;
}

void 
curlpp::easy::perform()
{
   m_data->perform();
}

void 
curlpp::easy::url( const std::string &link)
{
   m_data->url( link );
}
void 
curlpp::easy::port(  int port)
{
   m_data->port( port );
}

void 
curlpp::easy::verbose( bool activate )
{
   m_data->verbose( activate );
}

void 
curlpp::easy::header( bool include )
{
   m_data->header( include );
}

void 
curlpp::easy::signal( bool activate )
{
   m_data->signal( activate );
}

void 
curlpp::easy::progress( bool activate )
{
   m_data->progress( activate );
}

void 
curlpp::easy::fail_on_error( bool activate )
{
   m_data->fail_on_error( activate );
}

void 
curlpp::easy::dns_cache_timeout( long seconds )
{
   m_data->dns_cache_timeout( seconds );
}

void 
curlpp::easy::dns_use_global_cache( bool activate )
{
   m_data->dns_use_global_cache( activate );
}

void 
curlpp::easy::buffersize( long size )
{
   m_data->buffersize( size );
}

void 
curlpp::easy::proxy_name( const std::string &proxy_string )
{
   m_data->proxy_name( proxy_string );
}

void 
curlpp::easy::proxy_port( long port )
{
   m_data->proxy_port( port );
}

void 
curlpp::easy::proxy_type( curlpp::proxy_type::type type )
{
   m_data->proxy_type( type );
}

void 
curlpp::easy::http_proxy_tunnel( bool activate )
{
   m_data->http_proxy_tunnel( activate );
}

void 
curlpp::easy::network_interface( const std::string &name )
{
   m_data->network_interface( name );
}

void 
curlpp::easy::infile_size( long size )
{
   m_data->infile_size( size );
}

void 
curlpp::easy::netrc( curlpp::netrc::type netrc_use )
{
   m_data->netrc( netrc_use );
}
void 

curlpp::easy::transfert_text( bool activate )
{
   m_data->transfert_text( activate );
}

void 
curlpp::easy::crlf( bool activate )
{
   m_data->crlf( activate );
}

void 
curlpp::easy::range( const std::string &interval )
{
   m_data->range( interval );
}

void 
curlpp::easy::resume_from( long bytes )
{
   m_data->resume_from( bytes );
}

void 
curlpp::easy::custom_request( const std::string &request )
{
   m_data->custom_request( request );
}

void 
curlpp::easy::file_time( bool activate )
{
   m_data->file_time( activate );
}

void 
curlpp::easy::no_body( bool activate )
{
   m_data->no_body( activate );
}

void 
curlpp::easy::upload( bool activate )
{
   m_data->upload( activate );
}

void 
curlpp::easy::timeout( long time )
{
   m_data->timeout( time );
}

void 
curlpp::easy::low_speed_limit( long bytes )
{
   m_data->low_speed_limit( bytes );
}

void 
curlpp::easy::low_speed_time( long time )
{
   m_data->low_speed_time( time );
}

void 
curlpp::easy::max_connects( long nb_connects )
{
   m_data->max_connects( nb_connects );
}

void 
curlpp::easy::close_policy( curlpp::close_policy::type policy )
{
   m_data->close_policy( policy );
}

void 
curlpp::easy::fresh_connect( bool new_connect )
{
   m_data->fresh_connect( new_connect );
}

void 
curlpp::easy::forbid_reuse( bool close )
{
   m_data->forbid_reuse( close );
}

void 
curlpp::easy::connect_timeout( long seconds )
{
   m_data->connect_timeout( seconds );
}

void 
curlpp::easy::ssl_cert( const std::string &filename )
{
   m_data->ssl_cert( filename );
}

void 
curlpp::easy::ssl_cert_type( curlpp::ssl_cert_type::type format )
{
   m_data->ssl_cert_type( format );
}

void 
curlpp::easy::ssl_key( const std::string &key )
{
   m_data->ssl_key( key );
}

void 
curlpp::easy::ssl_key_type( curlpp::ssl_key_type::type key_type )
{
   m_data->ssl_key_type( key_type );
}

void 
curlpp::easy::ssl_key_passwd( const std::string &key_passwd )
{
   m_data->ssl_key_passwd( key_passwd );
}

void 
curlpp::easy::ssl_engine( const std::string &engine )
{
   m_data->ssl_engine( engine );
}

void 
curlpp::easy::ssl_engine_default()
{
   m_data->ssl_engine_default();
}

void 
curlpp::easy::ssl_version( curlpp::ssl_version::type version )
{
   m_data->ssl_version( version );
}

void 
curlpp::easy::ssl_verify_peer( bool activate )
{
   m_data->ssl_verify_peer( activate );
}

void 
curlpp::easy::ca_info( const std::string &filename )
{
   m_data->ca_info( filename );
}

void 
curlpp::easy::ca_path( const std::string &directory )
{
   m_data->ca_path( directory );
}

void 
curlpp::easy::random_file( const std::string &filename )
{
   m_data->random_file( filename );
}

void 
curlpp::easy::egd_socket( const std::string &path )
{
   m_data->egd_socket( path );
}

void 
curlpp::easy::ssl_verify_host( curlpp::ssl_verify_host::type verify )
{
   m_data->ssl_verify_host( verify );
}

void 
curlpp::easy::ssl_cipher_list( const std::list< std::string > &list )
{
   m_data->ssl_cipher_list( list );
}

void 
curlpp::easy::krb4_level( curlpp::krb4_level::type level )
{
   m_data->krb4_level( level );
}

