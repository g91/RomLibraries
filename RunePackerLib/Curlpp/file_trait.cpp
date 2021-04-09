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

using namespace curlpp;

file_trait::file_trait( FILE *file ) :
      m_file( file )
{}

// ============================================================================

file_trait::file_trait( const char *filename ) :
      m_file( NULL )
{
   m_filename = filename;
}

// ============================================================================

file_trait::~file_trait()
{
   ( void ) close();
}

// ============================================================================

bool file_trait::size( long &size )
{
   long thePos;
   
   if ( m_file != NULL )
      if ( ( thePos = ftell( m_file ) ) != -1 )
         if ( fseek( m_file, 0, SEEK_END ) == 0 )
         {
            size = ftell( m_file );
            ( void ) fseek( m_file, thePos, SEEK_SET );
            return true;
         }
         
   size = 0;
   return false;
}

// ============================================================================

bool file_trait::open( const char *open_mode )
{
   if ( m_file != NULL )
      return true;
      
   if ( !m_filename.compare( "" ) )
      return false;
      
   return ( ( m_file = fopen( m_filename.c_str(), open_mode ) ) != NULL );
}

// ============================================================================

bool file_trait::close()
{
   if ( m_file == NULL )
      return true;
      
   if ( !m_filename.compare( "" ) )
      return false;
      
   ( void ) fclose( m_file );
   
   m_file = NULL;
   
   return true;
}

// ============================================================================
// ============================================================================

size_t ifile_trait::read( void *buffer, size_t size )
{
   return fread( buffer, size, 1, m_file );
}

// ============================================================================

ifile_trait::ifile_trait( FILE *file ) :
      file_trait( file )
{}

// ============================================================================

ifile_trait::ifile_trait( const char *filename ) :
      file_trait( filename )
{}

// ============================================================================
// ============================================================================

size_t ofile_trait::write( void *buffer, size_t size )
{
   return fwrite( buffer, 1, size, ( FILE * ) m_file );
}

// ============================================================================

ofile_trait::ofile_trait( FILE *file ) :
      file_trait( file )
{}

// ============================================================================

ofile_trait::ofile_trait( const char *filename ) :
      file_trait( filename )
{}
