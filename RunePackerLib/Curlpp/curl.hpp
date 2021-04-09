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
#ifndef CURL_HPP
#define CURL_HPP

#include <curl/curl.h>
#include <string>
#include "types.hpp"
#include "slist.hpp"
//#include "form.hpp"
//#include "utilspp/smart_ptr.hpp"



namespace curlpp
{
   class factory_safe;
   class output_trait;
   class storage_trait;
   class input_trait;
   class output_trait;
   class progress_trait;
   class passwd_trait;
   class debug_trait;
   class buffer;

   /**
    * You cannot create directly a curl you need to call
    * curl* factory::create_curl()
    * to get a curl
    */
   class curl
   {
      public:
         friend class factory;

         curl();

         /**
          * This   function  is  called  after  the  init  and  all  the
          * options calls are  made,  and  will  perform  the transfer  
          * as  described  in  the options.  
          * 
          * You can do any amount of calls to curl::perform(). 
          * If you intend to transfer more than one file, you are even 
          * encouraged to  do  so.  libcurl  will then attempt to re-use 
          * the same connection for the following transfers, thus  making  
          * the  operations  faster,  less  CPU intense and using less 
          * network resources. 
          *  
          * You  must  never  call this function simultaneously from two
          * places using the same handle. Let the function return  first
          * before invoking it another time. If you want parallel trans?          * fers, you must use several curl handles.
          */
         void perform();

         /**
          * The actual URL to deal with.
          * 
          * NOTE:  this  option is (the only one) required to be
          * set before curl::perform(3) is called. 
          */
         void url( const std::string &link);


		void  port( long port);
         /**
          * Set  the parameter to true to get the library to
          * display a lot of verbose information about its oper-
          * ations.  Very  useful  for  libcurl  and/or protocol
          * debugging and understanding.
          * 
          * You hardly ever want this set in production use, you
          * will  almost  always want this when you debug/report
          * problems.       
          *
          * see 
          */
         void verbose( bool activate = true );

         /**
          * A true parameter tells the  library  to  include
          * the header in the body output. This is only relevant
          * for protocols that actually have  headers  preceding
          * the data (like HTTP). The default is set to false
          *
          * NOTE: if you're using a callback for writing headers
          * (header_function) the header will be writen with the
          * body and be sent to the header write callback.
          */
         void header( bool include = true );

         /**
          * Pass a bool. If it is false, libcurl will not use any
          * functions that install signal handlers or any  functions
          * that  cause  signals  to  be  sent  to the process. This
          * option is  mainly  here  to  allow  multi-threaded  unix
          * applications  to  still set/use all timeout options etc,
          * without risking getting signals.  (Added in 7.10)
          */
         void signal( bool activate = false );

         /**
          * Data pointer to pass to  the  file  write  function. 
          * Note  that if you specify the write function (by the
          * function void write_function()),this is the pointer 
          * you'll  get  as  input. 
          *
          * The default is stdout. If you want to reset to
          * the default, you just need to pass a NULL pointer.
          */
         void write_data( curlpp::output_trait *trait );

         /**
          * Function  pointer  that  should  match the following 
          * prototype: size_t function( void *ptr, size_t  size, 
          * size_t  nmemb,  void  *stream);  This  function gets 
          * called by libcurl as soon as there is data available 
          * to  pass  available that needs to be saved. The size 
          * of the data pointed to by  ptr  is  size  multiplied 
          * with  nmemb.   Return  the  number of bytes actually 
          * taken care of.  If  that  amount  differs  from  the 
          * amount  passed  to  your  function,  it'll signal an 
          * error to the library and it will abort the  transfer 
          * and throw an exception. 
          * 
          * Set   the  stream  argument  with  the "write_data"
          * function.
          * 
          * NOTE: you will be passed as much data as possible in
          * all  invokes,  but  you  cannot  possibly  make  any
          * assumptions. It may be one byte,  it  may  be  thou-
          * sands.
          */
         void write_function( write_function_t function );

         /**
          * Function  pointer that should match the following proto-
          * type: size_t function( void *ptr, size_t size, size_t
          * nmemb, void *stream);.  This  function  gets called by
          * libcurl as soon as there is received  header  data  that
          * needs  to be written down. The headers are guaranteed to
          * be written one-by-one and only complete lines are  writ-
          * ten.  Parsing  headers should be easy enough using this.
          * The size of the data pointed to by ptr is  size multi-
          * plied  with nmemb.  The pointer named stream will be the
          * ne you passed to libcurl with  the write_header_data()
          * option.   Return the number of bytes actually written or
          * return -1 to signal error to the library (it will  cause
          * it to abort the transfer with a CURLE_WRITE_ERROR return
          * code).
          */
         void write_header_function( write_function_t function );

         /**
           Pass  a  pointer to be used to write the header part
           of the received data to. If you don't use  your  own
           callback to take care of the writing, this must be a
           valid FILE *. See  also  the  CURLOPT_HEADERFUNCTION
           option  below on how to set a custom get-all-headers
           callback.
          */
         void write_header_data( curlpp::output_trait *trait );

         /**
          * Data pointer to pass to the file read function. Note 
          * that  if  you  specify the void read_function(),  this 
          * is  the  pointer  you'll  get as input. If you don't 
          * specify a read callback, this must be a  valid  FILE.
          *
          * NOTE:  If  you're  using libcurl as a win32 DLL, you 
          * MUST  use  a  void read_function()  if  you  set  this 
          * option.
          */
         void read_data( curlpp::input_trait *trait );

         /**
          * Function  pointer  that  should  match the following
          * prototype: size_t function( void *ptr, size_t  size,
          * size_t  nmemb,  void  *stream);  This  function gets
          * called by libcurl as soon as it needs to  read  data
          * in  order  to  send  it  to  the peer. The data area
          * pointed at by the pointer ptr may be filled with  at
          * most  size  multiplied  with  nmemb number of bytes.
          * Your function must return the actual number of bytes
          * that  you  stored  in that memory area. Returning -1
          * will signal an error to the library and cause it  to
          * abort  the  current  transfer  immediately  (with  a
          * CURLE_READ_ERROR return code).
          */
         void read_function( read_function_t function );

         /**
          * Function    pointer    that    should    match   the
          * curl_progress_callback    prototype     found     in
          * <curl/curl.h>.  This function gets called by libcurl
          * instead of its internal equivalent with  a  frequent
          * interval during data transfer.  Unknown/unused argu-
          * ment values will be set to zero (like  if  you  only
          * download  data,  the  upload  size  will  remain 0).
          * Returning a non-zero value from this  callback  will
          * cause  libcurl  to  abort  the  transfer  and return
          * CURLE_ABORTED_BY_CALLBACK.
          */
         void progress_function( progress_function_t function );

         /**
          * Pass a pointer that will be untouched by libcurl and
          * passed  as  the first argument in the progress call-
          * back set with CURLOPT_PROGRESSFUNCTION.
          */
         void progress_data( progress_trait *trait );

         /**
          * A  false  parameter  tells the library to shut of
          * the built-in progress meter completely.
          * 
          * NOTE: future versions of libcurl is  likely  to  not
          * have any built-in progress meter at all.
          */
         void progress( bool activate = true );

         /**
          * Pass a pointer to a curl_passwd_callback function  that
          * will  be  called  instead of the internal one if libcurl
          * requests  a  password.  The  function  must  match  this
          * prototype: int getpass(void *client, char *prompt, char*
          * uffer, int buflen );.  If set to NULL, it sets back the
          * function  to  the  internal default one. If the function
          * returns a non-zero value, it will  abort  the  operation
          * and   an   error  (CURLE_BAD_PASSWORD_ENTERED)  will  be
          * returned.   client is  a  generic  pointer,  see   CUR-
          * OPT_PASSWDDATA.   prompt is  a  zero-terminated string
          * that is text that prefixes the input request.  buffer is
          * a  pointer  to data where the entered password should be
          * stored and buflen is the maximum number  of  bytes  that
          * may be written in the buffer.  (Added in 7.4.2)
          */
         void passwd_function( passwd_function_t function );

         /**
          * Pass a "passwd_trait *" . The  passed
          * pointer  will  be  the  first  argument  sent to the
          * specifed passwd_function function. (Added  in
          * 7.4.2)
          */
         void passwd_data( passwd_trait *trait );

         /**
          * Function pointer that should match the following  proto-
          * type:  int curl_debug_callback (CURL *, curl_infotype,
          * char *, size_t, void *); This  function  will  receive
          * debug  information  if  CURLOPT_VERBOSE  is enabled. The
          * curl_infotype argument specifies what kind  of  informa-
          * tion it is. This funtion must return 0.
          */
         void debug_function( debug_function_t function );

         /**
          * Pass  a  pointer  to whatever you want passed in to your
          * CURLOPT_DEBUGFUNCTION in the last void * argument.  This
          * pointer is not used by libcurl, it is only passed to the
          * callback.
          */
         void debug_data( debug_trait *trait );

         /**
          * A true  parameter  tells  the  library  to  fail
          * silently  if  the  HTTP code returned is equal to or
          * larger than 300. The  default  action  would  be  to
          * return the page normally, ignoring that code.
          */
         void fail_on_error( bool activate = true );

         /**
          * Pass  a  long,  this  sets  the timeout in seconds. Name 
          * resolves will be kept in memory for this number of  seconds.  
          * Set to zero (0) to completely disable caching, or set to -1 
          * to make the cached entries remain forever. By default, libcurl  
          * caches info for 60 seconds. 
          */
         void dns_cache_timeout( long seconds );

         /**
          * Pass a bool. If the value is true, it tells curl  to
          * use  a  global  DNS cache that will survive between easy
          * handle creations and deletions. This is not  thread-safe
          * and this will use a global variable. 
          */
         void dns_use_global_cache( bool activate = true );

         /**
          * Pass  a  long  specifying  your  prefered  size  for the
          * receive buffer in libcurl.  The main point of this would
          * be  that  the  write callback gets called more often and
          * with smaller chunks. This is just treated as a  request,
          * not  an  order. You cannot be guaranteed to actually get
          * the given size. 
          */
         void buffersize( long size );

         /**
          * Set HTTP proxy to use. The  parameter hold the host
          * name or dotted IP address. To specify port number in
          * this  string,  append :[port] to the end of the host
          * name. The proxy string may be prefixed with  [proto-
          * col]://  since  any such prefix will be ignored. The
          * proxy's port number may optionally be specified with
          * the separate function void proxy_port().
          * 
          * NOTE: when you tell the library to use a HTTP proxy,
          * libcurl will  transparently  convert  operations  to
          * HTTP  even  if  you  specify a FTP URL etc. This may
          * have an impact on what other features of the library
          * you  can  use, such as void quote()  and similar FTP
          * specifics that don't work unless you tunnel  through
          * the  HTTP  proxy.  Such  tunneling is activated with
          * void http_proxy_tunnel().
          * 
          * NOTE2: libcurl respects  the  environment  variables
          * http_proxy,  ftp_proxy,  all_proxy  etc,  if  any of
          * those is set.
          */
         void proxy_name( const std::string &proxy_string );

         /**
          * Pass a long with this option to set the  proxy  port
          * to connect to unless it is specified by void proxy_name().
          */
         void proxy_port( long port );

         /**
          * Pass a long with this option to set type of  the  proxy.
          * Available  options for this are curlpp::proxy::http,
          * curlpp::proxy::socks4 and curlpp::proxy::socks5, with 
          * the http one being default.
          */
         void proxy_type( curlpp::proxy_type::type type );

         /**
          * Set the parameter to  true  to get the library  to
          * tunnel  all  operations  through a given HTTP proxy.
          * Note that there is a big difference between using  a
          * proxy  and  to  tunnel through it. If you don't know
          * what this means, you probably don't want  this  tun-
          * neling option.
          */
         void http_proxy_tunnel( bool activate = true );

         /**
          * Pass a std::string as parameter. This set  the  interface
          * name  to use as outgoing network interface. The name
          * can be an interface name, an IP address  or  a  host
          * name. 
          */
         void network_interface( const std::string &name );

         /**
          * When uploading a file to  a  remote  site,  this  option
          * should be used to tell libcurl what the expected size of
          * the infile is.
          */
         void infile_size( long size );

         /**
          * This  parameter  controls  the  preference  of  curlpp
          * between   using  user  names  and  passwords  from  your
          * ~/.netrc file, relative to user names and  passwords  in
          * the URL supplied with curlpp::curl::url( const char *link).
          * 
          * Note: curlpp uses a user name (and supplied or prompted
          * password) supplied with curlpp::curl::user_pwd( const
          * char *string) in preference to any of the options 
          * controlled by this parameter.
          * 
          * Pass a curlpp::netrc::type: one of the values 
          * described below.
          * 
          * curlpp::netrc::optional
          * The  use  of  your  ~/.netrc file is optional, and
          * information in the URL is  to  be  preferred.   The
          * file  will  be  scanned with the host and user name
          * (to find the password only) or with the host  only,
          * to find the first user name and password after that
          * machine, which ever information is not specified in
          * the URL.
          * 
          * Undefined  values  of  the  option  will  have this
          * effect.
          * 
          * curlpp::netrc::ignored
          * The library will ignore the file and use  only  the
          * information in the URL.
          * 
          * This is the default.
          * 
         * curlpp::netrc::required
            * This  value  tells the library that use of the file
            * is required, to ignore the information in the  URL,
         * and to search the file with the host only.
            * 
            * Only machine name, user name and password are taken into
            * account
            * (init macros and similar things aren't supported).
            * 
            * Note:  does not verify that the file has the cor-
            * rect  properties  set  (as  the standard Unix ftp client
                  * does). It should only be readable by user.
            */
            void netrc( curlpp::netrc::type netrc_use );

         /**
          * Pass a std::string as parameter, which should be
          * [username]:[password] to use for the connection. If the password is
          * left out, you will be prompted for it.curlpp::curl::passwd_function(
          * passwd_function_t * ) can be used to set your own prompt function.
          */
         void user_pwd( const std::string &auth );

         /**
          * Pass a std::string as parameter, which should be
          * [username]:[password] to use for the connection to the HTTP proxy. If
          * the password is left out, you will be prompted for it.
          * curlpp::curl::passwd_function( passwd_function_t * ) can be used to
          * set your own prompt function.
          */
         void proxy_user_pwd( const std::string &auth );

         /**
          * Two encodings are supported dentity, which does nothing,
          * and  deflate  to  request  the  server  to  compress its
          * reponse using the zlib algorithm. This is not an  order,
          * the  server  may or may not do it.  See the special file
          * doc/README.encoding for details.
          */
         void encoding( const std::string &algorithm );

         /**
          * A non-zero parameter tells the  library  to  follow  any
          * Location: header that the server sends as part of a HTTP
          * header.
          * 
          * NOTE: this means that the library will re-send the  same
          * request  on  the  new  location and follow new Location:
          * headers all the way  until  no  more  such  headers  are
          * returned.  curlpp::easy::max_redir( long )  can  be  used 
          * to limit the number of redirects libcurl will follow.
          */
         void follow_location( bool activate = true );

         /** 
          * Pass a long. The set  number  will  be  the  redirection
          * limit. If that many redirections have been followed, the
          * next redirect will throw a curlpp::runtime_error. This 
          * option only makes sense if the curlpp::easy::follow_location( bool )
          * is set to true.
          */
         void max_redir( long nb );

         /**
          * A true parameter tells the library to use  HTTP  PUT to
          * transfer  data.  The  data  should  be set with
          * curlpp::curl::read_data( curlpp::input_trait * ) and
          * curlpp::curl::infile_size( long ).
          */
         void put( bool activate = true );

         /**
          * Pass a std::string as parameter, which should be the full data  to
          * post in  a HTTP post operation. This is a normal
          * application/x-www-form-urlencoded kind, which is the most commonly
          * used one by HTML forms.
          * 
          * Note: to make multipart/formdata posts (aka  rfc1867-posts),
          * check out the CURLOPT_HTTPPOST option. 
          */
         void post_fields( const std::string &data );

         /**
          * Pass a const char * as parameter and his length, which should be
          * the full data to post in  a HTTP post operation. This is a normal
          * application/x-www-form-urlencoded kind, which is the most commonly
          * used one by HTML forms.
          *
          * If you want to post binary data this option must be  used. When
          * this option is used you can post fully binary data, which
          * otherwise is likely to fail. If this size is set to zero, the
          * library will use strlen() to get the size.
          * 
          * NOTE: to make multipart/formdata posts (aka  rfc1867-posts),
          * check out the CURLOPT_HTTPPOST option. 
          *
          * NOTE2: watch out because the data will not be duplicated. So be
          * sure to keep the data to this address.
          */
         void post_fields( const char *data, long length = 0 );
         
         
         /** 
	  * Tells libcurl you want a multipart/formdata HTTP POST to be made 
	  * and you instruct what data to pass on to the server. Pass a 
	  * pointer to a linked list of HTTP post structs as parameter. 
	  */
     //         void http_post( const std::list<curlpp::form *> &posts );

         /**
          * Pass a pointer to a zero terminated string as parameter.
          * It  will  be used to set the Referer: header in the http
          * request sent to the remote server. This can be  used  to
          * fool  servers  or  scripts.  You can also set any custom
          * header with CURLOPT_HTTPHEADER.
          */
         void referer( const std::string &name );

         /**
          * Pass a pointer to a zero terminated string as parameter.
          * It  will  be  used  to set the User-Agent: header in the
          * http request sent to the remote server. This can be used
          * to  fool servers or scripts. You can also set any custom
          * header with CURLOPT_HTTPHEADER.
          */
         void user_agent( const std::string &name );

         /**
          * Pass a pointer to a linked list of HTTP headers to  pass to  the
          * server  in  your  HTTP request. The linked list should be  a
          * std::list< std::string > properly filled in. If you add a header
          * that is otherwise generated and used by libcurl internally, your
          * added one will  be  used instead. If you add a header with no
          * contents as in 'Accept:' (no data on the right side of  the
          * colon), the internally used header will get disabled. Thus, using
          * this option you can add new headers, replace internal headers and
          * remove internal headers.
          * 
          * NOTE:The most commonly replaced headers have "shortcuts"
          * in the  fcuntions curlpp::curl::cookie( const std::string & ),
          * curlpp::curl::user_agent( const std::string & ) and
          * curlpp::curl::referer ( const std::string & ).
          *
          * NOTE: the list is duplicated for internal use.
          */
         void http_header( const std::list< std::string > &list ); 
         
         /**
          * Pass std::string as parameter. It will be used to set a cookie in
          * the http request. The format of the string should be
          * NAME=CONTENTS, where NAME is the cookie name  and  CONTENTS  is
          * what  the  cookie should contain.
          * 
          * If  you  need  to  set mulitple cookies, you need to set them all
          * using a single option and thus you need to concat them all in one
          * single string. Set multiple cookies in one string  like  this:
          * "name1=content1;  name2=content2;" etc.
          * 
          * Using this option multiple times will only make the latest string
          * override the previously ones.
          */
         void cookie( const std::string &cookie_string );

         /**
          * Pass a std::tring as parameter. It  should  contain the name of
          * your file holding cookie data to read. The cookie  data  may  be
          * in  Netscape Mozilla  cookie  data  format or just regular
          * HTTP-style headers dumped to a file.
          * 
          * Given an empty or non-existing file,  this  option  will enable
          * cookies  for  this curl handle, making it understand and parse
          * received cookies and then  use  matching cookies in future request.
          */
         void cookie_file( const std::string &file_name );

         /**
          * Pass  a  file name as char *, zero terminated. This will
          * make libcurl write all internally known cookies  to  the
          * specified  file  when curl_easy_cleanup(3) is called. If
          * no cookies are known, no file will be  created.  Specify
          * "-" to instead have the cookies written to stdout. Using
          * this option also enables cookies for this session, so if
          * you  for example follow a location it will make matching
          * cookies get sent accordingly. (Added in 7.9)
          * CURLOPT_COOKIEJAR
          */
         void cookie_jar( const std::string &file_name );

         /**
          * Pass a long as parameter. This defines how the CURLOPT_TIMEVALUE
          * time value is treated. You can set this parameter to
          * TIMECOND_IFMODSINCE or TIMECOND_IFUNMODSINCE. This is a HTTP-only
          * feature.
          */
         void time_condition( curlpp::time_condition::type since );

         /**
          * Pass a long as parameter. This should  be  the  time  in
          * seconds since 1 jan 1970, and the time will be used in a
          * condition as specified with CURLOPT_TIMECONDITION.
          */
         void time_value( long seconds );

         /**
          * Pass a long. If set tu true,  this  forces  the HTTP  request  to
          * get back to GET. Only really usable if POST, PUT or a custom
          * request have been used  previously using the same curl handle.
          */
         void http_get( bool activate = true );

         /**
          * Pass  a  curlpp::http_version::type, set to one of the values
          * described below. They force libcurl to use the  specific  HTTP
          * versions. This is not sensible to do unless you have a good
          * reason.
          *
          * We don't care about what version the library  uses.
          * libcurl will use whatever it thinks fit.
          * curlpp::http_version::none
          * 
          * Enforce HTTP 1.0 requests.
          * curlpp::http_version::v1_0
          * 
          * Enforce HTTP 1.1 requests.
          * curlpp::http_version::v1_1
          */
         void http_version( curlpp::http_version::type version );

         /**
          * Pass a std::string as parameter. It will be used to get the IP
          * address to use for the ftp PORT  instruction. The PORT instruction
          * tells the remote server to connect  to  our  specified  IP
          * address.  The string  may  be a plain IP address, a host name, an
          * network interface name (under Unix) or just a '-' letter to let
          * the  library  use  your systems default IP address. Default FTP
          * operations are passive, and thus won't use PORT.
          */
         void ftp_port( const std::string &port );

         /**
          * Pass a std::list< std::string > of FTP commands to pass to the
          * server prior to your ftp request.  This  will  be done  before
          * any  other  FTP  commands are issued (even before the CWD
          * command). Disable this operation again by passing an empty list.
          */
         void quote( const std::list< std::string > &list );

         /**
          * Pass a std::list< std::string > of FTP commands to  pass to  the
          * server  after  your  ftp  transfer request. Disable this operation
          * again by passinf an empty list.
          */
         void post_quote( const std::list< std::string > &list );

         /**
          * Pass  a pointer to a std::list< std::string > of FTP commands to
          * pass to the server after the transfer type is set. Disable  this
          * operation again by passing an empty list.
          */
         void pre_quote( const std::list< std::string > &list );
 
         /**
          * A true parameter tells the library to just list  the names  of  an
          * ftp  directory,  instead  of doing a full directory listing that
          * would include file  sizes,  dates etc.
          *
          * This causes an FTP NLST command to be sent.  Beware that some FTP
          * servers list only files in  their  response  to NLST; they might
          * not include subdirectories and symbolic links.
          */
         void ftp_list_only( bool activate = true );

         /**
          * A true parameter tells the library to append to  the remote file
          * instead of overwrite it. This is only useful when uploading to a
          * ftp site.
          */
          void ftp_append( bool activate = true );
 
          /**
           * Pass a bool. If the value is true, it tells curl  to use  the
           * EPSV  command when doing passive FTP downloads (which it always
           * does by default). Using EPSV means that it will first attempt to
           * use EPSV before using PASV, but if you pass false to this option,
           * it will not try using EPSV, only plain PASV.
           */
          void ftp_use_epsv( bool activate = true );
          
          /**
           * A true parameter tells the library to use ASCII mode for ftp
           * transfers, instead of the default binary  transfer.  For  LDAP
           * transfers it gets the data in plain text instead of HTML and for
           * win32 systems it  does  not  set the  stdout  to  binary  mode.
           * This option can be usable when transferring text data between
           * systems with different  views  on  certain  characters, such as
           * newlines or similar.
           */
          void transfert_text( bool activate = true );

          /**
           * Convert Unix newlines to CRLF newlines on transfers, if the
           * parameter is true.
           */
          void crlf( bool activate = true );

          /**
           * Pass a std::string as parameter,  which  should  contain  the
           * specified range you want. It should be in the format "X-Y", where
           * X or Y may be left out.  HTTP  transfers  also support  several
           * intervals, separated with commas as in "X-Y,N-M". Using this kind
           * of  multiple  intervals  will cause  the  HTTP server to send the
           * response document in pieces (using standard MIME separation
           * techniques).
           */
          void range( const std::string &interval );

          /**
           * Pass a long as parameter. It contains the offset in number of
           * bytes that you want the transfer to start from.
           */
          void resume_from( long bytes );

          /**
           * Pass a std::string as parameter. It will be user instead of GET
           * or HEAD  when  doing  the HTTP  request.  This is useful for
           * doing DELETE or other more or less obscure HTTP requests.  Don't
           * do  this  at will,  make sure your server supports the command
           * first.
           */
          void custom_request( const std::string &request );

          /**
           * Pass a bool. If it is a true value,  libcurl  will attempt to get
           * the modification date of the remote document in this operation.
           * This requires  that  the  remote server sends the time or replies
           * to a time querying command.  The curlpp::curl::info::file_time()
           * function can be used after a transfer to extract the received
           * time (if any).
           */
          void file_time( bool activate = true );
 
          /**
           * A true parameter tells the library  to  not  include the
           * body-part  in the output. This is only relevant for protocols
           * that have separate header and body parts.
           */
          void no_body( bool activate = true );

         /**
          * A true parameter tells the library to prepare for an upload.  The
          * curlpp::curl::read_data( input_trait *) and
          * curlpp::curl::infile_size( long ) are also interesting for uploads.
          */
          void upload( bool activate = true );
         
          /**
           * Pass a long as parameter containing the maximum time  in
           * seconds that you allow the libcurl transfer operation to
           * take. Normally, name lookups  can  take  a  considerable
           * time  and limiting operations to less than a few minutes
           * risk aborting perfectly normal operations.  This  option
           * will cause curl to use the SIGALRM to enable time-outing
           * system calls.
           * 
           * NOTE: this is not recommended  to  use  in  unix  multi-
           * threaded  programs,  as  it  uses  signals  unless  CUR-
           * LOPT_NOSIGNAL (see above) is set.
           */
          void timeout( long time );

          /**
           * Pass a long as parameter. It contains the transfer speed
           * in  bytes  per  second that the transfer should be below
           * during CURLOPT_LOW_SPEED_TIME seconds for the library to
           * consider it too slow and abort.
           */
          void low_speed_limit( long bytes );
 
         /** 
          * Pass  a  long as parameter. It contains the time in sec-
          * onds  that  the  transfer  should  be  below  the   CUR-
          * LOPT_LOW_SPEED_LIMIT  for the library to consider it too
          * slow and abort.
          */
         void low_speed_time( long time );
 
         /**
          * Pass a long. The set number will be the persistent  con-
          * nection  cache  size. The set amount will be the maximum
          * amount of simultaneously open connections  that  libcurl
          * may  cache.  Default is 5, and there isn't much point in
          * changing this value unless you are  perfectly  aware  of
          * how this work and changes libcurl's behaviour. This con-
          * cerns connection using any of the protocols that support
          * persistent connections.
          * 
          * When  reaching  the  maximum  limit,  curl uses the CUR-
          * LOPT_CLOSEPOLICY to figure out  which  of  the  existing
          * connections  to close to prevent the number of open con-
          * nections to increase.
          * 
          * NOTE: if you already have performed transfers with  this
          * curl  handle,  setting a smaller MAXCONNECTS than before
          * may cause open connections to get closed  unnecessarily.
          */
         void max_connects( long nb_connects );
        
         /**
          * Pass a long. This option sets what policy libcurl should use when
          * the connection cache is filled and one of the open connections has
          * to be closed to make room for a new connection. This must be one
          * of the curlpp::close_policy::type. Use
          * curlpp::close_policy::least_recently_used to make libcurl close the
          * connection  that  was  least  recently used, that connection is
          * also least likely to be capable of re-use. Use
          * curlpp::close_policy::oldest to  make  libcurl close  the  oldest
          * connection, the one that was created first among the ones in the
          * connection cache. The  other close policies are not support yet.
          */
         void close_policy( curlpp::close_policy::type policy );

         /**
          * Pass a bool. Set to true to make the next transfer use a new
          * (fresh) connection by force. If the connection cache  is full
          * before this connection, one of the existing connections will be
          * closed as according to the selected  or  default policy. This
          * option should be used with caution and only if you understand what
          * it  does. Set this to false to have libcurl attempt re-using an
          * existing connection (default behavior).
          */
         void fresh_connect( bool new_connect );

         /**
          * Pass a bool. Set to true to make the next transfer explicitly
          * close the connection when done. Normally, libcurl keep all
          * connections alive when done with one transfer in case there comes
          * a succeeding one that can re-use them.  This option should be used
          * with caution and only if you understand what it does. Set to false
          * to have libcurl keep the connection open for possibly later re-use
          * (default behavior).
          */
         void forbid_reuse( bool close );

         /**
          * Pass  a long. It should contain the maximum time in seconds that
          * you allow the  connection  to  the  server  to take.   This  only
          * limits the connection phase, once it has connected, this option is
          * of no  more  use.  Set  to  zero  to  disable  connection timeout
          * (it will then only timeout on the system's internal timeouts). See
          * also the curlpp::curl::timeout( long ) function.
          * 
          * NOTE:  this  is  not  recommended  to use in unix multi-threaded
          * programs,  as  it  uses  signals  unless  CURLOPT_NOSIGNAL (see
          * above) is set.
          */
         void connect_timeout( long seconds );

         /**
          * Pass a std::string as parameter. The string should be the file
          * name of your  certificate. The default format is "PEM" and can be
          * changed with curlpp::curl::ssl_cert_type(
          * curlpp::ssl_cert_type::type ).
          */
         void ssl_cert( const std::string &filename );

         /**
          * Pass a curlpp::ss_cert_type::type as parameter. The parameter
          * should be the format of your certificate. Supported formats are
          * curlpp::ssl_cert_type::pem and curlpp::ssl_cert_type::der.
          */
         void ssl_cert_type( curlpp::ssl_cert_type::type format );

         /**
          * Pass a std::string as parameter. The string should be the file
          * name of your private key. The default format is
          * curlpp::ssl_key_type::pem and can be changed with
          * curlpp::curl::ssl_key_type( curlpp::ssl_key_type::type ).
          */
         void ssl_key( const std::string &key );

         /**
          * Pass curlpp::ssl_key_type::type as parameter. The parameter should
          * be the format  of  your  private  key. Supported formats are
          * curlpp::ssl_key_type::pem, curlpp::ssl_key_type::der and
          * curlpp::ssl_key_type::eng.
          * 
          * NOTE:The format "ENG" enables you to  load  the  private
          * key from a crypto engine. in this case the key passed in
          * curlpp::curl::ssl_key( const std::string & ) is used as an
          * identifier passed to the engine. You have to set the crypto engine
          * with curlpp::curl::ssl_engine.
          */
         void ssl_key_type( curlpp::ssl_key_type::type key_type );

         /**
          * Pass std::string as parameter. It will be used as the password
          * required to use the private key passed to curlpp::curl::ssl_key(
          * const std::string & ). If the password is not supplied, you will
          * be prompted for it. curlpp::curl::passwd_function(
          * passwd_function_t ) can be used to set your own prompt function.
          */
         void ssl_key_passwd( const std::string &key_passwd );

         /**
          * Pass a std::string as parameter. It will be used as the identifier
          * for the crypto engine you want to use for your private key.
          */
         void ssl_engine( const std::string &engine );

         /**
          * Sets  the  actual crypto engine as the default for (asymetric)
          * crypto operations.
          */
         void ssl_engine_default();

         /**
          * Pass a curlpp::ssl_version::type as parameter. Set what version of
          * SSL to attempt to use, 2 or 3 ( curlpp::ssl_version::v2 and
          * curlpp::ssl_version::v3 respectively). By default, the SSL library
          * will try to solve this by itself although some servers make this
          * difficult why you at times may have to use this option.
          */
         void ssl_version( curlpp::ssl_version::type version );

         /**
          * Pass bool that is set to a false value to stop curl from verifying
          * the  peer's  certificate ( this option is set to true by default).
          * Alternate certificates to verify against can be  specified with
          * the curlpp::curl::ca_info( const std::string & ) function or a
          * certificate directory can be specified with the
          * curlpp::curl::ca_path( const std::string & )function. libcurl
          * installs a default bundle. curlpp::curl::ssl_verify_host(
          * curlpp::ssl_verify_host::type verify ) may also need to be  set
          * to curlpp::ssl_verify_host::check_existence or
          * curlpp::ssl_verify_host::disable if curlpp::curl::ssl_verify_peer(
          * bool ) is disabled (it defaults to
          * curlpp::ssl_verify_host::match).
          */
         void ssl_verify_peer( bool activate );

         /**
          * Pass a std::string naming a file holding one or more certificates
          * to verify the peer with. This only makes sense  when  used in
          * combination with the curlpp::curl::ssl_verify_peer( bool )
          * function. 
          */
         void ca_info( const std::string &filename );

         /**
          * Pass a std::string naming a directory holding multiple CA
          * certificates to verify the peer with. The certificate directory
          * must  be  prepared using  the  openssl  c_rehash  utility.  This
          * only makes sense when used in combination with the
          * curlpp::curl::ssl_verify_peer( bool ) function set to true. The
          * CAPATH function apparently does not work in Windows  due  to  some
          * limitation  in  openssl.
          */
         void ca_path( const std::string &directory );

         /**
          * Pass a std::string file name. The file will be used to read from
          * to seed the random engine  for SSL.  The  more  random  the
          * specified file is, the more secure the SSL connection will become.
          */
         void random_file( const std::string &filename );

         /**
          * Pass a std::string path name to the Entropy Gathering Daemon
          * socket. It will be used to seed the random engine for SSL.
          */
         void egd_socket( const std::string &path );

         /**
          * Pass curlpp::ssl_verify_host::type. Set if we should verify the
          * Common name from the peer certificate in the SSL handshake, set
          * curlpp::ssl_verify_host::disable to disable it, set
          * curlpp::ssl_verify_host::check_existence to check existence,
          * curlpp:ssl_verify_host::match to ensure that it  matches  the
          * provided hostname.  This is by default set to
          * curlpp::ssl_verify_host::match.
          */
         void ssl_verify_host( curlpp::ssl_verify_host::type verify );

         /**
          * Pass a std::list< std::string > holding  the  list of ciphers to
          * use for the SSL connection. The list must be syntactly correct.
          * examples of cipher include RC4, SHA, SHA1, DES, TLSv1 and DEFAULT.
          * If you pass an empty list, DEFAULT is set. The default list is
          * normally set when you compile OpenSSL.
          * 
          * You'll find more details about cipher lists on this URL:
          * http://www.openssl.org/docs/apps/ciphers.html
          */
         void ssl_cipher_list( const std::list< std::string > &list );

         /**
          * Pass a std::string as parameter. Set the krb4 security level, this
          * also enables krb4 awareness. There's the value for the function:
          * curlpp::krb4_level::clear, curlpp::krb4_level::safe,
          * curlpp::krb4_level::confidential or curlpp::krb4_level::private.
          * Set the value to curlpp::krb4_level::disable to disable kerberos4.
          * The kerberos support only works for FTP.
          */
         void krb4_level( curlpp::krb4_level::type level );
         
         virtual ~curl();

      protected:

      private:
         curl( const curl &other );
         curl& operator=( const curl &other );

         /**
          * Pass a char * to a buffer that the libcurl may store
          * human readable error messages in. This may  be  more
          * helpful  than just the return code from the library.
          * The buffer must be at least CURL_ERROR_SIZE big.
          * Note: if the library does not return an  error,  the
          * buffer may not have been touched. Do not rely on the
          * contents in those cases.
          */
         void error_buffer( char* buffer );

         void option( CURLoption curl_option, const char *curl_data );

         void option( CURLoption curl_option, void *curl_data );

         void option( CURLoption curl_option, long curl_data );

         void option( CURLoption curl_option );

         void member_string_option(
               std::string &member,
               const std::string &param,
               CURLoption opt
               );

      private:
         CURL *m_curl;

         std::string m_url;
         std::string m_interface;
         std::string m_proxy;
         std::string m_user_pwd;
         std::string m_proxy_user_pwd;
         std::string m_range;
         std::string m_post_fields;
         std::string m_referer;
         std::string m_user_agent;
         std::string m_ftp_port;
         std::string m_cookie;
         std::string m_cookie_file;
         std::string m_cookie_jar;
         std::string m_ssl_cert;
         std::string m_ssl_cert_passwd;
         std::string m_ssl_key;
         std::string m_ssl_key_type;
         std::string m_ssl_key_passwd;
         std::string m_ssl_engine;
         std::string m_ssl_cipher_list;
         std::string m_ca_info;
         std::string m_ca_path;
         std::string m_random_file;
         std::string m_egd_socket;

         std::string m_encoding;
         std::string m_custom_request;

     //         curlpp::http_post m_http_posts;

         curlpp::slist m_http_headers;
         curlpp::slist m_quote;
         curlpp::slist m_post_quote;
         curlpp::slist m_pre_quote;


         char m_error_buffer[ CURL_ERROR_SIZE + 1 ];
   };

   class factory
   {
      public:
         friend class factory_safe;
         friend class curl_handle;

         /**
          * returns true if the curlpp ws previously initialized(),
          * returns false if not.
          */
         static bool is_initialized();

         static void initialize( long flags );

         static factory *instance();

         static void terminate();

         curl* create_curl();
      
      private:
         factory( long flags );
         ~factory();


      private:
         static factory* m_instance;
         static factory_safe m_safe;
   };

   class factory_safe
   {
      public:
         friend class factory;

      private:
         factory_safe()
         {
            ;
         }

         ~factory_safe();
   };
};

#endif
