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

#include "http_easy.hpp"
//#include "form.hpp"

curlpp::http_easy::http_easy()
{
}

void 
curlpp::http_easy::encoding( const std::string &algorithm )
{
   m_data->encoding( algorithm );
}

void 
curlpp::http_easy::follow_location( bool activate )
{
   m_data->follow_location( activate );
}

void 
curlpp::http_easy::max_redir( long nb )
{
   m_data->max_redir( nb );
}

void 
curlpp::http_easy::put( bool activate )
{
   m_data->put( activate );
}

void 
curlpp::http_easy::post_fields( const std::string &data )
{
   m_data->post_fields( data );
}

void 
curlpp::http_easy::post_fields( const char *data, long length )
{
   m_data->post_fields( data, length );
}

/*
void 
curlpp::http_easy::http_post( const std::list<curlpp::form *> &posts )
{
   m_data->http_post( posts );
}
*/
void 
curlpp::http_easy::referer( const std::string &name )
{
   m_data->referer( name );
}

void 
curlpp::http_easy::user_agent( const std::string &name )
{
   m_data->user_agent( name );
}

void 
curlpp::http_easy::http_header( const std::list< std::string > &list )
{
   m_data->http_header( list );
}

void 
curlpp::http_easy::cookie( const std::string &cookie_string )
{
   m_data->cookie( cookie_string );
}

void 
curlpp::http_easy::cookie_file( const std::string &file_name )
{
   m_data->cookie_file( file_name );
}

void 
curlpp::http_easy::cookie_jar( const std::string &file_name )
{
   m_data->cookie_jar( file_name );
}

void 
curlpp::http_easy::time_condition( curlpp::time_condition::type since )
{
   m_data->time_condition( since );
}

void 
curlpp::http_easy::time_value( long seconds )
{
   m_data->time_value( seconds );
}

void 
curlpp::http_easy::http_get( bool activate )
{
   m_data->http_get( activate );
}

void 
curlpp::http_easy::http_version( curlpp::http_version::type version )
{
   m_data->http_version( version );
}


