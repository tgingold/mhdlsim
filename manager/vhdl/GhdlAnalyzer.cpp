/*
 * Copyright (c) 2016 CERN
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

#include "GhdlAnalyzer.hpp"
#include "ghdl.h"
#include <iostream>
#include <cassert>
#include <string>


GhdlAnalyzer::GhdlAnalyzer() {}

GhdlAnalyzer::~GhdlAnalyzer() {}

int
GhdlAnalyzer::analyze() {
   assert( files_.size() );

   mhdlsim_vhdl_analyze_init();

   for(auto it = files_.begin(); it != files_.end(); it++) {
     int result = mhdlsim_vhdl_analyze_file(it->c_str(), it->size());
     if (result) {
       std::cerr << "Analyze error!" << std::endl;
       return result;
     }
   }

   return 0;
}
