#include <string.h>
#include <string>
#include "slist.hpp"

curlpp::slist::slist() : m_list( 0 )
{}

curlpp::slist::~slist()
{
   if( m_list != 0 )
   {
      curl_slist_free_all( m_list );
      m_list = 0;
   }
}

curlpp::slist &
curlpp::slist::operator=( const std::list< std::string > &list )
{
   if( m_list != 0 )
   {
      curl_slist_free_all( m_list );
      m_list = 0;
   }

   for(
         std::list< std::string >::const_iterator pos = list.begin();
         pos != list.end();
         pos++
      )
   {
      char *str;
      size_t len = (*pos).size();

      str = new char[len + 1];
      memset( str, 0, len + 1 );
      strncpy( str, (*pos).c_str(), len );
      m_list = curl_slist_append( m_list, str );
   }

   return (*this);
}

curl_slist *
curlpp::slist::c_slist()
{
   return m_list;
}
