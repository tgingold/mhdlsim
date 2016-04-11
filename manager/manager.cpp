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
#include <stack>

Manager::Manager() :
   current_comp_(nullptr),
   current_step_(CompilerStep::ANALYSIS) {}

Manager::~Manager() {};

inline std::ostream&
operator<<(std::ostream& os, const CompilerStep& step) {
   switch (step) {
      case CompilerStep::ANALYSIS:
         os << "analysis";
         break;
      case CompilerStep::ELABORATION:
         os << "elaboration";
         break;
      case CompilerStep::SIMULATION:
         os << "simulation";
         break;
      default:
         os << "unknown";
         break;
   }
   return os;
}

inline std::ostream&
operator<<(std::ostream& os, const Compiler::Type& type) {
   switch (type) {
      case Compiler::VERILOG:
         os << "Verilog";
         break;
      case Compiler::VHDL:
         os << "VHDL";
         break;
      default:
         os << "unknown";
         break;
   }
   return os;
};

void
Manager::add_instance(Compiler::Type type, Compiler* comp) {
   assert( comp );
   for( auto it = instances_.begin(); it != instances_.end(); ++it ) {
      assert( type != it->second );
   }
   instances_[comp] = type;
   assert( instances_[comp] == type );
}

void
Manager::error_message( const std::string& errormsg ) const {
   assert( current_comp_  && instances_.find(current_comp_) != instances_.end() );
   std::cerr << "<manager>: Error with the " <<  instances_.find(current_comp_)->second
      << " compiler during " << current_step_;
   if( !errormsg.empty() )
      std::cerr << ": " << errormsg;
   std::cerr << std::endl;
}

int
Manager::do_simulation() {
   int res = 0;

   for( auto it = instances_.begin(); it != instances_.end(); ++it ) {
      current_comp_ = it->first;
      res = current_comp_->initialize();
      if( res ) {
         error_message( "initialization failed" );
         return res;
      }
   }

   // For the time being the timestamp is an unsigned long long.
   // If we change the definition we should take care of defining a zero element.
   sim_time_t min = 0;
   for( auto it = instances_.begin(); it != instances_.end(); ++it ) {
      current_comp_ = it->first;
   }
   return 0;
}

int
Manager::do_analysis() {
   int res = 0;

   for( auto it = instances_.begin(); it != instances_.end(); ++it ) {
      current_comp_ = it->first;
      res = current_comp_->analyze();
      if( res ) {
         error_message();
         return res;
      }
   }
   return 0;
}

int
Manager::elaborate( ModuleInstance* mod_inst ) {
   int res = 0;
   std::stack<ModuleSpec*> look_for;

   for( auto it = instances_.begin(); it != instances_.end(); ++it ) {
      current_comp_ = it->first;
      ModuleSpec* tmp = current_comp_->elaborate( mod_inst );
      if( tmp )
         look_for.push( tmp );
   }
   ModuleSpec* avoid_endless = nullptr;
   while( !look_for.empty() ) {
      if( avoid_endless == look_for.top() )
         return 1;
      avoid_endless = look_for.top();
      for( auto it = instances_.begin(); it != instances_.end(); ++it ) {
         current_comp_ = it->first;
         switch( current_comp_->instantiate( *avoid_endless ) ) {
            case Elaborator::FOUND:
               {
                  look_for.pop();
                  res = elaborate( current_comp_->get_instance() );
                  if ( res ) {
                     not_found_ = avoid_endless->name() + "->" + not_found_;
                     return res;
                  }
                  break;
               }
            case Elaborator::NOT_FOUND:
               break;
            case Elaborator::NEED_ANOTHER:
               look_for.push( current_comp_->get_spec() );
               break;
         }
      }
   }

   return 0;
}

int
Manager::do_elaboration() {
   int res = elaborate();

   if( res ) {
      if ( !not_found_.empty() ) {
         error_message( "Not able to find " + not_found_ );
      } else {
         error_message();
      }
      return res;
   }

   // Can we go ahead?
   for( auto it = instances_.begin(); it != instances_.end(); ++it ) {
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
   for( auto it = instances_.begin(); it != instances_.end(); ++it ) {
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

   if( step >= CompilerStep::ANALYSIS ) {
      current_step_ = CompilerStep::ANALYSIS;
      res = do_analysis();
      if( res ) {
         return res;
      }
   }
   if( step >= CompilerStep::ELABORATION ) {
      current_step_ = CompilerStep::ELABORATION;
      res = do_elaboration();
      if( res ) {
         return res;
      }
   }
   if( step >= CompilerStep::SIMULATION ) {
      current_step_ = CompilerStep::SIMULATION;
      res = do_simulation();
      if( res ) {
         return res;
      }
   }

   return 0;
}
