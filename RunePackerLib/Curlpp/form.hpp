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

#ifndef FORM_HPP
#define FORM_HPP


#include <list>
#include <string>

struct curl_httppost;

namespace curlpp
{
  class form;
  
  /**
   * This class is used internally to wrap over curl_httppost
   * class.
   */
  class http_post
  {
  public:
    http_post();
    ~http_post();

    /**
     * initialize the HTTP post with the list of forms. 
     * Each form will be duplicated for the http_post use.
     */
    http_post &operator=( const std::list< curlpp::form * > &posts );

    
    /**
     * return the curl_httppost representation of this HTTP Post.
     */
    curl_httppost *c_http_post();

    /**
     * Free all HTTP posts.
     */
    void clear();

  private:
    curl_httppost *m_first;
    curl_httppost *m_last;
  };

  /**
   * This class is the base representation of a post. You need
   * to inherit from it to define a type of post.
   */
  class form
  {
    friend class http_post;

  public:
    /**
     * initialize the form. "name" is the name of the field.
     */
    form( const char *name );

    /**
     * initialize the form. "name" is the name of the field.
     */
    form( const std::string &name );

    virtual ~form(){};

    virtual form *clone() = 0;

  protected:
    /**
     * it will add himself to the curl_httppost *first.
     */
    virtual void form_add( curl_httppost **first, 
			   curl_httppost **last ) = 0;

    /**
     * Contain the name of the field.
     */
    const std::string m_name;
  };
  
  /**
   * This class is a file post. It will send a file in the
   * HTTP post.
   */
  class file_form : public form
  {
  public:
    /**
     * initialize the form. "name" is the name of the field. 
     * "filename" is the string that holds the filename.
     */
    file_form( const char *name, 
	       const char *filename );
    /**
     * initialize the form. "name" is the name of the field. 
     * "filename" is the string that holds the filename. 
     * "content_type" is the MIME type of the file.
     */
    file_form( const char *name, 
	       const char *filename, 
	       const char *content_type );
    /**
     * initialize the form. "name" is the name of the field. 
     * "filename" is the string that holds the filename.
     */
    file_form( const std::string &name, 
	       const std::string &filename );
    /**
     * initialize the form. "name" is the name of the field. 
     * "filename" is the string that holds the filename. 
     * "content_type" is the MIME type of the file.
     */
    file_form( const std::string &name, 
	       const std::string &filename,
	       const std::string &content_type );

    virtual ~file_form(){}

    /**
     * This function will return a copy of the instance.
     */
    virtual file_form *clone();

  private:
    void form_add( curl_httppost **first, 
		   curl_httppost **last );

  private:
    const std::string m_filename; 
    const std::string m_content_type;
  };

 /**
   * This class is a file post. It will send a file in the
   * HTTP post.
   */
  class content_form : public form
  {
  public:
    /**
     * initialize the form. "name" is the name of the field. 
     * "content" is the string that holds the filename.
     */
    content_form( const char *name, 
		  const char *content );
    /**
     * initialize the form. "name" is the name of the field. 
     * "content" is the string that holds the filename. 
     * "content_type" is the MIME type of the file.
     */
    content_form( const char *name, 
		  const char *content, 
		  const char *content_type );
    /**
     * initialize the form. "name" is the name of the field. 
     * "content" is the string that holds the content.
     */
    content_form( const std::string &name, 
		  const std::string &content );
    /**
     * initialize the form. "name" is the name of the field. 
     * "content" is the string that holds the content. 
     * "content_type" is the MIME type of the file.
     */
    content_form( const std::string &name, 
		  const std::string &content,
		  const std::string &content_type );

    virtual ~content_form(){}
    
    /**
     * This function will return a copy of the instance.
     */
    virtual content_form *clone();

  private:
    void form_add( curl_httppost **first, 
		   curl_httppost **last );

  private:
    const std::string m_content; 
    const std::string m_content_type;
  };
}

#endif //FORM_HPP
