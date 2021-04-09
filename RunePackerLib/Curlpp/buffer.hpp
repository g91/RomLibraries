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

#ifndef CURLPP_BUFFER_HPP
#define CURLPP_BUFFER_HPP

#include <stdio.h>

namespace curlpp
{
   /**
    * Very simple class to manage a char buffer.
    */
   class buffer
   {

      public:
         // Default constructor. Creates an invalid buffer.
         buffer();

         buffer( const char *data, size_t length );
         
         buffer( const buffer& other );
         
         // Destructor. Frees the buffer.
         virtual ~buffer();

         // Returns a pointer to the buffer data. Retruns NULL if the buffer
         // is not valid.
         const char *get() const;

         // Returns the buffer length.
         size_t length() const;
         
         // Sets the buffer content. Passing NULL to 'buffer' will result in
         // an invalid buffer.
         void set( const char *buffer, size_t length );

         // Free the buffer and set its size to zero. The buffer becomes
         // invalid.
         void reset();

         // Returns true if the buffer is valid (ie: the buffer is allocated)
         // This can be used to test if a call to 'Set' has failed due to
         // a memory allocation failure.
         bool is_valid() const;

      protected:
         char *m_buffer;
         size_t m_length;
   };
};

#endif
