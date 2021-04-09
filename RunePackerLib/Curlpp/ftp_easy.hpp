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

#include "easy.hpp"

#ifndef FTP_EASY_HPP
#define FTP_EASY_HPP

namespace curlpp
{
   /**
    * This class is an easy handle, but it offer all the ftp only option in
    * addition.
    */
   class ftp_easy : public curlpp::easy
   {
      public:
         ftp_easy();
         
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

   };
};

#endif //FTP_EASY_HPP
