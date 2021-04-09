#ifndef TYPES_HPP
#define TYPES_HPP

namespace curlpp
{

   /**
    * See curlpp::curl::netrc( curlpp::netrc::type use_netrc ) for
    * more documentation
    */
   namespace netrc
   {
      enum type { 
         optional = CURL_NETRC_OPTIONAL, 
         ignored = CURL_NETRC_IGNORED, 
         required = CURL_NETRC_REQUIRED 
      };
   };
   
   namespace debug
   {
      enum type { 
         text = CURLINFO_TEXT, 
         header_in = CURLINFO_HEADER_IN,
         header_out = CURLINFO_HEADER_OUT, 
         data_in = CURLINFO_DATA_IN,
         data_out = CURLINFO_DATA_OUT 
      };
   };

   /**
    * See "void curlpp::curl::proxy_type( curlpp::proxy_type::type ) for more documentation.
    */
   namespace proxy_type
   {
      enum type { 
         http = CURLPROXY_HTTP, 
         socks4 = CURLPROXY_SOCKS4, 
         socks5 = CURLPROXY_SOCKS5 
      };
   };

   /**
    * See curlpp::curl::time_condition( curlpp::time_condition::type ) for more
    * documentation.
    */
#ifndef TIMECOND_IFMODSINCE
#define TIMECOND_IFMODSINCE CURL_TIMECOND_IFMODSINCE
#endif 

#ifndef TIMECOND_IFUNMODSINCE
#define TIMECOND_IFUNMODSINCE CURL_TIMECOND_IFUNMODSINCE
#endif 

#ifndef TIMECOND_LASTMOD
#define TIMECOND_LASTMOD CURL_TIMECOND_LASTMOD
#endif

   namespace time_condition
   {
      enum type { 
         if_mod_since = TIMECOND_IFMODSINCE, 
         if_unmod_sice = TIMECOND_IFUNMODSINCE,
	 if_lastmod = TIMECOND_LASTMOD
      };
   };

   /**
    * See curlpp::curl::http_version( curlpp::http_version::type ) for more
    * documentation.
    */
   namespace http_version
   {
      enum type { 
         none = CURL_HTTP_VERSION_NONE,
         v1_0 = CURL_HTTP_VERSION_1_0,
         v1_1 = CURL_HTTP_VERSION_1_1 
      };
   };

   /**
    * See curlpp::curl::close_policy( curlpp::close_policy::type ) for more
    * documentation.
    */
   namespace close_policy
   {
      enum type {
         oldest = CURLCLOSEPOLICY_OLDEST,
         least_recently_used = CURLCLOSEPOLICY_LEAST_RECENTLY_USED,
         least_traffic = CURLCLOSEPOLICY_LEAST_TRAFFIC,
         slowest = CURLCLOSEPOLICY_SLOWEST,
         callback = CURLCLOSEPOLICY_CALLBACK
      };
   };

   /**
    * See curlpp::curl::ssl_cert_type( curlpp::ssl_cert_type::type ) for more
    * documentation.
    */
   namespace ssl_cert_type
   {
      enum type {
         pem = 0,
         der
      };
   };
   
   /**
    * See curlpp::curl::ssl_key_type( curlpp::ssl_key_type::type ) for more
    * documentation.
    */
   namespace ssl_key_type
   {
      enum type {
         pem = 0,
         der,
         eng
      };
   };

   /**
    * See curlpp::curl::ssl_version( curlpp::ssl_version::type ) for more
    * documentation.
    */
   namespace ssl_version
   {
      enum type {
         v2 = 2,
         v3 = 3
      };
   };

   /**
    * See curlpp::curl::ssl_verify_host( curlpp::ssl_verify_host::type ) for more
    * documentation.
    */
   namespace ssl_verify_host
   {
      enum type {
         disable = 0,
         check_existence = 1,
         match = 2
      };
   };

   /**
    * See curlpp::curl::krb4_level( curlpp::krb4_level::type ) for more
    * documentation.
    */
   namespace krb4_level
   {
      enum type {
         disable = 0,
         clear,
         safe,
         confidential,
         priv
      };
   };
   
   /**
    * the typedef that defines the type of the write callback for storage.
    */
   typedef size_t ( *write_function_t ) ( void *, size_t, size_t, void * );
   /**
    * the typedef that defines the type of the read callback for storage.
    */
   typedef size_t ( *read_function_t ) ( void *, size_t, size_t, void * );
   /**
    * the typedef that defines the type of the progress callback for our "home made" progess.
    */
   typedef int ( *progress_function_t ) ( void *, double, double, double, double );
   /**
    * the typedef that defines the type of the passwd callback for getting
    * "home made " passwords.
    */
   typedef int ( *passwd_function_t ) ( void *, char *, char *, int );
   /**
    * the typedef that defines the type of the write debug for getting
    * "home made" debug function.
    */
   typedef int ( *debug_function_t ) ( CURL *, curlpp::debug::type, char *, size_t, void * );
};


#endif
