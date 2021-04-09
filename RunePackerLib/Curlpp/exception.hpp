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

#ifndef CURLPP_EXCEPTION_HPP
#define CURLPP_EXCEPTION_HPP

#include <string>

namespace curlpp
{
   /**
    * Very simple exception class.
    */
   class exception
   {
      public:
         // Copy constructor
         exception( const exception& except ) throw() : m_what( except.m_what )
         {}

         // Return the reason of the exception
         const char *what() const throw()
         {
            return m_what.c_str();
         }

         virtual ~exception()
         {}

      protected:
         exception( const char *what ) throw() : m_what( what )
         {}

      private:
         // We can't instatiate an empty exception
         exception();

         std::string m_what;
   };

   class runtime_error : public exception
   {
      public:
         runtime_error( const char *what ) : exception( what )
         {}
   }
   ;

   class logic_error : public exception
   {
      public:
         logic_error( const char *what ) : exception( what )
         {}
   }
   ;

   class invalid_argument : public logic_error
   {
      public:
         invalid_argument( const char *what ) : logic_error( what )
         {}
   }
   ;

   /**
    * if is_true is false, it throws a curlpp::runtime_error with 
    * the reason. It's used in inline fonction, because throwing 
    * an execption is heavy in binary code, something we don't 
    * want in an inline fonction.
    */
   void runtime_assert( bool is_true, const char *reason );

   /**
    * if is_true is false, it throws a curlpp::logic_error with 
    * the reason. It's used in inline fonction, because throwing 
    * an execption is heavy in binary code, something we don't 
    * want in an inline fonction.
    */
   void logic_assert( bool is_true, const char *reason );
}
#endif
