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

#ifndef CURLPP_STORAGE_TRAIT_HPP
#define CURLPP_STORAGE_TRAIT_HPP

#include "types.hpp"


/**
 * This is the virtual class that has to be used as the base class to specify
 * any kind of storage trait. Subclasses need to implement the 'setup' method.
 * Storage traits can be used for input (read) or output (write).
 */
namespace curlpp
{
   class storage_trait
   {
      public:
         storage_trait( )
         {}

         virtual ~storage_trait()
         {}

   };

   class input_trait : public storage_trait
   {
      public:
         input_trait( )
         {}

         virtual ~input_trait()
         {}

         virtual size_t read( void *buffer, size_t length ) = 0;

   };

   class output_trait : public storage_trait
   {
      public:
         output_trait( )
         {}

         virtual ~output_trait()
         {}

         virtual size_t write( void *buffer, size_t length ) = 0;
   };

   class io_trait : public input_trait, public output_trait
   {
      public:
         io_trait( )
         {}

         virtual ~io_trait()
         {}
   }
   ;

   class progress_trait : public storage_trait
   {
      public:
         progress_trait()
         {}

         virtual ~progress_trait()
         {}

         virtual bool progress_function(
               double dltotal,
               double dlnow,
               double ultotal,
               double ulnow
               ) = 0;
   };

   class output_progress_trait : public progress_trait
   {
      public:
         output_progress_trait()
         {}

         virtual ~output_progress_trait()
         {}

         bool progress_function(
               double dltotal,
               double dlnow,
               double ultotal,
               double ulnow
               );

   };

   static output_progress_trait default_progress_trait;

   /**
    * This class is the class you need to derive from to 
    * obtain a "home made" passwd callback, in order to 
    * manage by yourself the input of passwords.
    *
    * NOTE:to see how to use this check the 
    * default_passwd_trait, that's an 
    * input_passwd_trait
    */
   class passwd_trait : public storage_trait
   {
      public:
         virtual ~passwd_trait()
         {}

         virtual bool passwd_function(
               char *prompt,
               char *buffer,
               int buflen
               ) = 0;
   };

   /**
    * This class is the default trait for 
    * passwd_storage
    */
   class input_passwd_trait : public passwd_trait
   {
      public:
         virtual ~input_passwd_trait()
         {}

         virtual bool passwd_function(
               char *prompt,
               char *buffer,
               int buflen
               );
   };

   static input_passwd_trait default_passwd_trait;

   /**
    * This class is the class you need to derive from to 
    * obtain a "home made" debug callback, in order to 
    * manage by yourself the output of debug.
    *
    * NOTE:to see how to use this check the 
    * default_debug_trait, that's an 
    * output_debug_trait
    */

   class debug_trait : public storage_trait
   {
      public:
         virtual ~debug_trait()
         {}

         /**
          * You need to implement this function in order
          * to provide a "home made" debug function.
          *
          * NOTE:always use the buflen when using this data, 
          * buffer is treated at this point like binary data,
          * so it will NOT necesserily be NULL terminated.
          */
         virtual void debug_function(
               curlpp::debug::type type,
               char *buffer,
               int buflen
               ) = 0;
   };

   /**
    * This class is the default trait for the
    * debug_storage class
    */
   class output_debug_trait : public debug_trait
   {
      public:
         virtual ~output_debug_trait()
         {}

         virtual void debug_function(
               curlpp::debug::type type,
               char *buffer,
               int buflen
               );
   };

   static output_debug_trait default_debug_trait;
};
#endif
