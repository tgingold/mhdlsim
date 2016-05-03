/*
 * Copyright (c) 2016 CERN
 * @author Michele Castellana <michele.castellana@cern.ch>
 *
 * This source code is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This source code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "GhdlHandler.hpp"
#include "compiler.h"
#include "ghdl.h"
#include <cassert>

GhdlHandler::GhdlHandler()
{
  mhdlsim_vhdl_init ();
}

GhdlHandler::~GhdlHandler() {}

void
GhdlHandler::add_files( std::vector<std::string>& in_files ) {
   assert( in_files.size() );
   files_.insert(in_files.begin(), in_files.end());
};

void
GhdlHandler::add_file( const std::string& newfile ) {
   assert( !newfile.empty() );
   auto result = files_.insert(newfile);
   assert( result.second );
}

void
GhdlHandler::processParams( std::vector<std::string>& params_ ) {
   for(auto it = params_.begin(); it != params_.end(); it++) {
     std::string p(*it);
     if (mhdlsim_vhdl_process_param (p.c_str(), p.size())) {
       // This should never happen. The only 2 possible reasons are:
       // 1 - The ArgumentParser did a wrong translation of parameters.
       // 2 - There is an unhandled parameter.
       assert(false);
       std::cerr << "ERROR: paramenter not recognized" << std::endl;
       abort();
     }
   }
}
