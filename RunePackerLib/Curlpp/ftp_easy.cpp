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

#include "ftp_easy.hpp"

curlpp::ftp_easy::ftp_easy()
{
}

void 
curlpp::ftp_easy::ftp_port( const std::string &port )
{
   m_data->ftp_port( port );
}

void 
curlpp::ftp_easy::quote( const std::list< std::string > &list )
{
   m_data->quote( list );
}

void 
curlpp::ftp_easy::post_quote( const std::list< std::string > &list )
{
   m_data->post_quote( list );
}

void 
curlpp::ftp_easy::pre_quote( const std::list< std::string > &list )
{
   m_data->pre_quote( list );
}

void 
curlpp::ftp_easy::ftp_list_only( bool activate )
{
   m_data->ftp_list_only( activate );
}

void 
curlpp::ftp_easy::ftp_append( bool activate )
{
   m_data->ftp_append( activate );
}

void
curlpp::ftp_easy::ftp_use_epsv( bool activate )
{
   m_data->ftp_use_epsv( activate );
}

