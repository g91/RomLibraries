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

#ifndef MEMORY_TRAIT_HPP
#define MEMORY_TRAIT_HPP

namespace curlpp
{
   /**
    * Storage trait using memory to store its content. Memory allocation and
    * deallocation is done using the standard 'malloc', 'realloc' and 'free'
    * functions.
    */
   class memory_trait : public io_trait
   {

      public:
         /**
          * Default constructor, the buffer will be allocated when 'write'
          * will be called and deallocated by the destructor. This is the
          * constructor to use for output (write).
          */
         memory_trait();

         /**
          * Input (read) constructor. If 'own' is true the buffer will be 
          * deallocated (using 'free') in the destructor. If not it's the caller
          * responsibility to deallocate the buffer.
          */
         memory_trait( void *buffer, size_t length, bool own );

         /**
          * Free the buffer if owned.
          */
         virtual ~memory_trait();

         size_t write( void *buffer, size_t length );
         size_t read( void *buffer, size_t length );

         /*
          * Free the buffer, set buffer to NULL and size to zero.
          */
         void reset();

         /**
          * Returns a pointer to the buffer (for read only). Returns NULL
          * after a call to the default constructor or to 'reset'.
          */
         void *buffer()
         {
            return m_buffer;
         }

         /**
          * Returns the buffer size.
          */
         size_t length()
         {
            return m_length;
         }

         /**
          * Convert the buffer into a C string (zero terminated). If the buffer
          * contains binary data, the string will be only made of the data
          * before the first zero in the buffer.
          */
         const char *string();

      protected:
         bool mrealloc( size_t length );

      protected:
         void *m_buffer;
         size_t m_length;
         size_t m_size;
         size_t m_pos;
         size_t m_zero_pos;
         bool m_own;

   };
};

#endif //MEMORY_TRAIT_HPP
