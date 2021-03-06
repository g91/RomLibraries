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

#include "curlpp.hpp"
#include <stdio.h>


bool curlpp::output_progress_trait::progress_function(
   double dltotal,
   double dlnow,
   double ultotal,
   double ulnow
)
{
   if ( dltotal != 0 )
   {
      printf( "%.0f/%.0f KBytes\n", dlnow / 1024, dltotal / 1024 );
   }
   else
   {
      printf( "%f/%f KBytes\n", ulnow / 1024, ultotal / 1024 );
   }
   
   return true;
}

//\TODO add exception throwing
bool curlpp::input_passwd_trait::passwd_function(
   char *prompt,
   char *buffer,
   int buflen
)
{
   printf( "%s", prompt );
   if ( fgets( buffer, buflen, stdin ) == NULL )
   {
      return false;
   }
   
   return true;
}

void curlpp::output_debug_trait::debug_function(
   curlpp::debug::type type,
   char *buffer,
   int buflen
)
{
   if ( type == curlpp::debug::text )
   {
      fwrite( "* ", 2, 1, stdout );
      fwrite( buffer, buflen, 1, stdout );
   }
   else if ( type == curlpp::debug::header_out )
   {
      fwrite( "> ", 2, 1, stdout );
      fwrite( buffer, buflen, 1, stdout );
   }
};

