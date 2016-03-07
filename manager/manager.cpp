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

void
Manager::error_message( const std::string& errormsg ) const {
   assert( current_comp_  && instances_.find(current_comp_) != instances_.end() );
   std::string phase;
   switch( current_step_ ) {
      case ANALYSIS:
         phase = "analysis";
         break;
      case ELABORATION:
         phase = "elaboration";
         break;
      case SIMULATION:
         phase = "simulation";
         break;
      default:
         assert(false);
   }
   assert( !phase.empty() );
   switch( instances_.find(current_comp_)->second ) {
      case Compiler::VERILOG:
         std::cerr << "Error with the Verilog compiler during " << phase;
         break;
      case Compiler::VHDL:
         std::cerr << "Error with the VHDL compiler during " << phase;
         break;
      default:
         assert(false);
   }
   if( !errormsg.empty() )
      std::cerr << ": " << errormsg;
   std::cerr << std::endl;
}

int
Manager::do_simulation() {
   return 0;
}

int
Manager::do_analysis() {
   int res = 0;

   for( auto it = instances_.begin(); it != instances_.end(); it++ ) {
      current_comp_ = it->first;
      res = current_comp_->analyze();
      if( res ) {
         error_message();
         return res;
      }
   }
   current_comp_ = nullptr;
   return 0;
}

int
Manager::elaborate( ModuleInstance* mod_inst ) {
   int res = 0;
   ModuleInstance * found = mod_inst;
   ModuleSpec * look_for = NULL;

   do {
      look_for = NULL;
      auto it = instances_.begin();
      for(; it != instances_.end() && !look_for; it++ ) {
         current_comp_ = it->first;
         look_for = current_comp_->elaborate( found );
      }
      if( look_for ) {
         found = NULL;
         for( it = instances_.begin(); it != instances_.end() && !found; it++ ) {
            current_comp_ = it->first;
            found = current_comp_->instantiate( *look_for );
         }
         res = elaborate( found );
      }
      if( res ) {
         error_message( "Not able to find " + look_for->name() );
         return res;
      }
   } while( look_for );

   return 0;
}

int
Manager::do_elaboration() {
   int res = elaborate();

   if( res ) {
      return res;
   }

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
      error_message();
      return 1;
   }
   // Emit code
   for( auto it = instances_.begin(); it != instances_.end(); it++ ) {
      current_comp_ = it->first;
      res = current_comp_->emit_code();
      if( res ) {
         error_message("In emit_code");
         return res;
      }
   }
   current_comp_ = nullptr;

   return 0;
}

int
Manager::run( CompilerStep step ) {
   assert( instances_.size() );
   int res = 0;

   if( step >= ANALYSIS ) {
      current_step_ = ANALYSIS;
      res = do_analysis();
      if( res ) {
         return res;
      }
   }
   if( step >= ELABORATION ) {
      current_step_ = ELABORATION;
      res = do_elaboration();
      if( res ) {
         return res;
      }
   }
   if( step >= SIMULATION ) {
      current_step_ = SIMULATION;
      res = do_simulation();
      if( res ) {
         return res;
      }
   }

   return 0;
}
