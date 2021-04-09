#include "curlpp.hpp"
#include "exception.hpp"

void curlpp::initialize( long flags )
{
   curlpp::factory::initialize( flags );
}

bool curlpp::is_initialized()
{
   return curlpp::factory::is_initialized();
}

void curlpp::terminate()
{
   curlpp::factory::terminate();
}

std::string 
curlpp::escape( const std::string &url )
{
   std::string buffer;
   char* p = curl_escape( url.c_str(), url.size() );
   if ( !p )
   {
      throw curlpp::runtime_error( "unable to escape the string" ); //we got an error
   }
   else
   {
      buffer = p;
      curl_free( p );
   }
   return buffer;
}

std::string
curlpp::unescape( const std::string &url )
{
   std::string buffer;
   char* p = curl_unescape( url.c_str(), url.size() );
   if ( !p )
   {
      throw curlpp::runtime_error( "unable to escape the string" ); //we got an error
   }
   else
   {
      buffer = p;
      curl_free( p );
   }
   return buffer;
}

std::string
curlpp::getenv( const std::string &name )
{
   std::string buffer;
   char* p = curl_getenv( name.c_str() );
   if ( !p )
   {
      throw curlpp::runtime_error( "unable to get the environnement string" ); //we got an error
   }
   else
   {
      buffer = p;
      free( p );
   }
   return buffer;
}

std::string
curlpp::libcurl_version()
{
   char* p = curl_version();
   if ( !p )
   {
      throw curlpp::runtime_error( "unable to get the libcurl version" ); //we got an error
   }
      
   return std::string( p );
}

time_t 
curlpp::getdate( const std::string&date, time_t *now )
{
   time_t return_value = curl_getdate( date.c_str(), now );
   if( !return_value )
   {
      throw curlpp::runtime_error( "unable to get the date" );
   }

   return return_value;
}

