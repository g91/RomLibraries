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

#include "form.hpp"

#include <curl/curl.h>


curlpp::http_post::http_post()
  : m_first( NULL )
  , m_last( NULL )
{}

curlpp::http_post::~http_post()
{
  clear();
}


curlpp::http_post &
curlpp::http_post::operator=( const std::list< curlpp::form * > &posts )
{
  clear();

  std::list< curlpp::form * >::const_iterator pos;
  for( pos = posts.begin(); pos != posts.end(); pos++) {
    (*pos)->form_add( &m_first, &m_last );
  } 

  return (*this);
}

curl_httppost *
curlpp::http_post::c_http_post()
{
  return m_first;
}

void
curlpp::http_post::clear()
{
  if( m_first != NULL ) {
    curl_formfree( m_first );
    m_first = NULL;
    m_last = NULL;
  }
}

curlpp::form::form( const char *name )
  : m_name( name )
{}

curlpp::form::form( const std::string &name )
  : m_name( name )
{}

curlpp::file_form::file_form( const char *name, const char *filename )
  : form( name )
  , m_filename( filename )
{}

curlpp::file_form::file_form( const char *name, const char *filename, const char *content_type )
  : form( name )
  , m_filename( filename )
  , m_content_type( content_type )
{}

curlpp::file_form::file_form( const std::string &name, 
			      const std::string &filename )
  : form( name )
  , m_filename( filename )
{}

curlpp::file_form::file_form( const std::string &name, 
			      const std::string &filename,
			      const std::string &content_type )
  : form( name )
  , m_filename( filename )
  , m_content_type( content_type )
{}

curlpp::file_form *
curlpp::file_form::clone() 
{
  return new curlpp::file_form(*this);
}


void
curlpp::file_form::form_add( curl_httppost **first, curl_httppost **last )
{
  // We uses options that will copy internally the string (c_str),
  // so we don't need to worry about the memory.
  if(m_content_type.empty()) {
    curl_formadd( first, 
		  last, 
		  CURLFORM_COPYNAME,
		  m_name.c_str(), 
		  CURLFORM_FILE,
		  m_filename.c_str(),
		  CURLFORM_END );
  }
  else {
    curl_formadd( first, 
		  last, 
		  CURLFORM_COPYNAME, 
		  m_name.c_str(), 
		  CURLFORM_FILE,
		  m_filename.c_str(),
		  CURLFORM_CONTENTTYPE,
		  m_content_type.c_str(),
		  CURLFORM_END );
  }
}


curlpp::content_form::content_form( const char *name, const char *content )
  : form( name )
  , m_content( content )
{}

curlpp::content_form::content_form( const char *name, const char *content, const char *content_type )
  : form( name )
  , m_content( content )
  , m_content_type( content_type )
{}

curlpp::content_form::content_form( const std::string &name, 
				    const std::string &content )
  : form( name )
  , m_content( content )
{}

curlpp::content_form::content_form( const std::string &name, 
				    const std::string &content,
				    const std::string &content_type )
  : form( name )
  , m_content( content )
  , m_content_type( content_type )
{}

curlpp::content_form *
curlpp::content_form::clone()
{
  return new curlpp::content_form(*this);
}

void
curlpp::content_form::form_add( curl_httppost **first, curl_httppost **last )
{
  // We uses options that will copy internally the string (c_str),
  // so we don't need to worry about the memory.
  if( m_content_type.empty() ) {
    curl_formadd( first, 
		  last, 
		  CURLFORM_COPYNAME, 
		  m_name.c_str(), 
		  CURLFORM_COPYCONTENTS,
		  m_content.c_str(),
		  CURLFORM_END );
  }
  else {
    curl_formadd( first, 
		  last, 
		  CURLFORM_COPYNAME, 
		  m_name.c_str(), 
		  CURLFORM_COPYCONTENTS,
		  m_content.c_str(),
		  CURLFORM_CONTENTTYPE,
		  m_content_type.c_str(),
		  CURLFORM_END );
  }
}
