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

#include "manager.h"
#include <iostream>

Manager::Manager() :
   current_comp_(nullptr),
   current_step_(ANALYSIS) {}

Manager::~Manager() {};

void
Manager::add_instance(Compiler::Type type, Compiler* comp) {
   assert( comp );
   for( auto it = instances_.begin(); it != instances_.end(); it++ ) {
      assert( type != it->second );
   }
   instances_[comp] = type;
   assert( instances_[comp] == type );
}

int
Manager::run( CompilerStep type ) {
   assert( instances_.size() );
   int res = 0;

   for( auto it = instances_.begin(); it != instances_.end(); it++ ) {
      assert( it->first );
      current_comp_ = it->first;
      res = current_comp_->analyze();
      // An error message has been already printed, just return.
      if( res )
         return res;
   }
   current_comp_ = nullptr;

   ModuleSpec * tmp = nullptr;
   for( auto it = instances_.begin(); it != instances_.end(); it++ ) {
      assert( it->first );
      current_comp_ = it->first;
      tmp = current_comp_->elaborate();
      if( tmp )
         break;
   }
   current_comp_ = nullptr;

   // If we do not need any module for the elaboration
   // we have already finished
   if( !tmp  ) {
      // Can we go ahead?
      for( auto it = instances_.begin(); it != instances_.end(); it++ ) {
         current_comp_ = it->first;
         if ( !current_comp_->can_continue() )
            break;
      }
      // At least one compiler had a problem.
      if( !current_comp_->can_continue() ) {
         auto it = instances_.find( reinterpret_cast<Compiler*>( current_comp_ ) );
         assert( it != instances_.end() );
         switch( it->second ) {
            case Compiler::VERILOG:
               std::cerr << "Error with the Verilog compiler." << std::endl;
               break;
            case Compiler::VHDL:
               std::cerr << "Error with the VHDL compiler." << std::endl;
               break;
            default:
               std::cerr << "Something bad happened, you should never see this message." << std::endl;
         }
         return 1;
      }
      // Emit code
      for( auto it = instances_.begin(); it != instances_.end(); it++ ) {
         current_comp_ = it->first;
         res = current_comp_->emit_code();
         // An error message has been already printed, just return.
         if( res )
            return res;
      }
      current_comp_ = nullptr;
   }

   return 0;
}
