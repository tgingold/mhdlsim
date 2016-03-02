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

#include "IcarusHandler.hpp"
#include "compiler.h"
#include <cassert>

IcarusHandler::IcarusHandler() {}

IcarusHandler::~IcarusHandler() {}

void
IcarusHandler::set_param( const std::string& newparam ) {
	assert( !newparam.empty() );
   auto result = params_.insert(newparam);
   assert( result.second );
}

void
IcarusHandler::set_param( std::vector<std::string>& in_params ) {
	assert( in_params.size() );
	params_.insert(in_params.begin(), in_params.end());
}

void
IcarusHandler::add_files( std::vector<std::string>& in_files ) {
   assert( in_files.size() );
   files_.insert(in_files.begin(), in_files.end());
};

void
IcarusHandler::add_file( const std::string& newfile ) {
   assert( !newfile.empty() );
   auto result = files_.insert(newfile);
   assert( result.second );
}

void
IcarusHandler::processParams( std::vector<std::string>& ) {
   // Set the default standard
   lexor_keyword_mask = GN_KEYWORDS_1364_1995
                       |GN_KEYWORDS_1364_2001
                       |GN_KEYWORDS_1364_2001_CONFIG
                       |GN_KEYWORDS_1364_2005
                       |GN_KEYWORDS_1800_2005
                       |GN_KEYWORDS_1800_2009
                       |GN_KEYWORDS_1800_2012;
}
