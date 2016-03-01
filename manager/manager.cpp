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

#include "manager.h"

Manager::Manager() :
   current_sim_(nullptr),
   current_ana_(nullptr),
   current_elab_(nullptr) {}

Manager::~Manager() {};

void
Manager::add_instance(Compiler::Type type, Compiler* comp) {
   assert( comp );
   assert( instances_.find(type) == instances_.end() );
   instances_[type] = comp;
   assert( instances_[type] == comp );
}

int
Manager::run() {
   assert( instances_.size() );
   int res = 0;

   for( auto it = instances_.begin(); it != instances_.end(); it++ ) {
      assert( it->second );
      current_ana_ = it->second;
      res = it->second->analyze();
      // An error message has been already printed, just return.
      if( res )
         return res;
   }
   // no active analyzer anymore
   current_ana_ = nullptr;
   return 0;
}
