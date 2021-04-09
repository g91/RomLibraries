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

#ifndef CURLPP_STORAGE_HPP
#define CURLPP_STORAGE_HPP

#include "storage_trait.hpp"
#include "file_trait.hpp"

namespace curlpp
{
   /**
    * This is the base class used to manage the writing of the ouput, headers, 
    * input, etc.
    */
   template < class trait_type >
      class storage_base
      {
         public:
            void trait( trait_type *trait, bool own = false );

         public:
            typedef trait_type storage_trait_type;


         protected:
            storage_base(
                  curlpp::curl *parent,
                  trait_type *trait,
                  trait_type *default_trait,
                  bool own
                  );

            virtual ~storage_base();

            virtual void set_callback() = 0;
            virtual void set_callback_data() = 0;

         protected:
            curlpp::curl *m_parent;
            trait_type *m_trait;

         private:
            trait_type *m_default_trait;
            bool m_own;
      };

   template < class trait_type >
      storage_base< trait_type >::storage_base(
            curl *parent,
            trait_type *given_trait,
            trait_type *default_trait,
            bool own
            )
      :
      m_parent( parent ),
   m_trait( NULL ),
   m_default_trait( default_trait ),
   m_own( false )
   {
      trait( given_trait, own );
   }

   template < class trait_type >
      storage_base< trait_type >::~storage_base()
      {
         if ( m_own && ( m_trait != NULL ) )
         {
            delete m_trait;
         }
      }

   template < class trait_type >
      void storage_base< trait_type >::trait( trait_type *trait, bool own )
      {
         if ( m_own && ( m_trait != NULL ) )
         {
            delete m_trait;
         }

         if ( trait == NULL )
         {
            m_trait = m_default_trait;
            m_own = false;
         }
         else
         {
            m_trait = trait;
            m_own = own;
         }
      }

   /**
    * This class manage the writing of the body ouput of the "easy" parent.
    * By default this class is using the "stdout_trait" trait.
    */
   class body_storage : public storage_base< output_trait >
   {
      public:
         body_storage(
               curl *parent,
               output_trait *trait = &stdout_trait,
               bool own = false
               );

         void set_callback();
         void set_callback_data();


      private:
         static size_t write_callback(
               void *buffer,
               size_t size,
               size_t nitems,
               void *outstream
               );
   };


   /**
    * This class manage the writing of the header ouput of the "easy" parent.
    * By default this class is using the "stdout_trait" trait.
    */
   class header_storage : public storage_base< output_trait >
   {
      public:
         header_storage(
               curl *parent,
               output_trait *trait = NULL,
               bool own = false
               ) : storage_base< output_trait >( parent, trait, &stdout_trait, own )
               {}

         void set_callback();
         void set_callback_data();

      private:
         static size_t write_callback(
               void *buffer,
               size_t size,
               size_t nitems,
               void *outstream
               );
   };

   /**
    * This class manage the reading of the "easy" parent. 
    * By default this class is using the "stdin_trait" trait.
    */
   class input_storage : public storage_base< input_trait >
   {
      public:
         input_storage(
               curl *parent,
               input_trait *trait = &stdin_trait,
               bool own = false
               ) : storage_base< input_trait >( parent, trait, &stdin_trait, own )
               {}

         void set_callback();
         void set_callback_data();

      private:
         static size_t read_callback(
               void *buffer,
               size_t size,
               size_t nmemb,
               void *instream
               );
   };

   /**
    * This class manage the writing of the progess ouput of the "easy" parent. 
    * By default this class is using the "default_progress_trait" trait.
    */
   class progress_storage : public storage_base< progress_trait >
   {
      public:
         progress_storage(
               curl *parent,
               progress_trait *trait = &default_progress_trait,
               bool own = false
               ) : storage_base< progress_trait >( parent, trait, &default_progress_trait, own )
               {}

         void set_callback();
         void set_callback_data();

      private:
         static int progress_callback(
               void *clientp,
               double dltotal,
               double dlnow,
               double ultotal,
               double ulnow
               );
   };

   /**
    * This class manage the input of passwords for the "easy" parent. 
    * By default this class is using the "default_passwd_trait" trait.
    */
   class passwd_storage : public storage_base< passwd_trait >
   {
      public:
         passwd_storage(
               curl *parent,
               passwd_trait *trait = &default_passwd_trait,
               bool own = false
               ) : storage_base< passwd_trait >( parent, trait, &default_passwd_trait, own )
               {}

         void set_callback();
         void set_callback_data();

      private:
         static int passwd_callback(
               void *client,
               char *prompt,
               char *buffer,
               int buflen
               );
   };

   /**
    * This class manage the writing of the debug ouput of the "easy" parent. 
    * By default this class is using the "default_debug_trait" trait.
    */
   class debug_storage : public storage_base< debug_trait >
   {
      public:
         debug_storage(
               curl *parent,
               debug_trait *trait = &default_debug_trait,
               bool own = false
               ) : storage_base< debug_trait >( parent, trait, &default_debug_trait, own )
               {}

         void set_callback();
         void set_callback_data();

      private:
         static int debug_callback(
               CURL *handle,
               curlpp::debug::type info,
               char *buffer,
               size_t len,
               void *ptr
               );
   };

   /**
    * This class manage storage classes, who derives from storage_base. 
    * This class calls set_callback() and set_callback_data() when it's
    * needed by the storage.
    */
   template < class storage_type >
      class storage
      {
         public:
            storage( curl *parent );
            void trait( typename storage_type::storage_trait_type *given_trait, bool own = false );

         private:
            storage_type m_storage;
      };

   template < class storage_type >
      storage< storage_type >::storage( curl *parent ) : m_storage( parent )
      {
         m_storage.set_callback();
         m_storage.set_callback_data();
      }

   template < class storage_type >
      void storage< storage_type >::trait( typename storage_type::storage_trait_type *given_trait, bool own ) {
         m_storage.trait( given_trait, own );
         m_storage.set_callback_data();
      }

#if LIBCURL_VERSION_NUM >= 0x070b01
   template <>
      class storage< passwd_storage >
      {
         public:
            storage( curl *parent );
            void trait( passwd_trait *given_trait, bool own = false );
      };
#endif

};
#endif
