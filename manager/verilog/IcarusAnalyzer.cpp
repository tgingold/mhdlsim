/*
 * Copyright (C) 2016 Michele Castellana <michele.castellana@cern.ch>
 * 
 * This source code is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This source code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "IcarusAnalyzer.hpp"
#include "parse_api.h"
#include "pp_globals.h"
#include <iostream>
#include <cassert>
#include <string>
#include <cstdio>
#include <cstring>

#define OUTPUT_PP_FILENAME "pp_out.v"

IcarusAnalyzer::IcarusAnalyzer() {}

IcarusAnalyzer::~IcarusAnalyzer() {}

int
IcarusAnalyzer::analyze() {
   assert( files_.size() );

   // Create output file
   FILE* pp_out = fopen( OUTPUT_PP_FILENAME , "w");
   if( !pp_out ) {
      std::cerr << "Impossible to open the file " << OUTPUT_PP_FILENAME << ". Abort!" << std::endl;
      return 1;
   }
   // Create file list for the preprocessor:
   // it takes a null terminated char ** as input
   char ** file_list = new (std::nothrow) char*[files_.size() + 1]();
   if( !file_list ) {
      std::cerr << "No more space. Abort!" << std::endl;
      return 1;
   }

   unsigned i = 0;
   for(auto it = files_.begin(); it != files_.end(); it++, i++) {
      file_list[i] = strdup(it->c_str());
      assert( file_list[i] );
   }

   // Preprocessor
   reset_lexor( pp_out, file_list );
   int result = yylex();
   if ( result ) {
      std::cerr << "An error occured with yylex(). Abort!" << std::endl;
      cleanup( file_list );
      return result;
   }
   result = fclose( pp_out );
   assert( std::char_traits<wchar_t>::eof() != result );
   // pform creation
   pform_set_timescale(def_ts_units, def_ts_prec, 0, 0);
   result = pform_parse( OUTPUT_PP_FILENAME, NULL );
   if( result ) {
      std::cerr << "An error occured with pform_parse(). Abort!" << std::endl;
      cleanup( file_list );
      return result;
   }

   cleanup( file_list );
   return 0;
}

void
IcarusAnalyzer::cleanup(char ** file_list) {
   for( unsigned i = 0; i < files_.size(); i++ )
      free( file_list[i] );
   delete[] file_list;
}
