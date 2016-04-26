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

bool
Manager::set_variable( sim_time_t& min ) {
   bool other_events = false;
   std::vector<Compiler*> tie;
   min = Compiler::maxSimValue();
   for( auto it = instances_.begin(); it != instances_.end(); ++it ) {
      other_events |= it->first->other_event();
      if( min > it->first->next_event() ) {
         current_comp_ = it->first;
         min = it->first->next_event();
         tie.erase(tie.begin(), tie.end());
      }
      else if( min == it->first->next_event() ) {
         tie.push_back(it->first);
      }
   }
   if( other_events && !tie.empty() ) {
      // TODO: Break the tie...for the moment it can be left like that.
   }
   return other_events;
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
   current_comp_ = nullptr;

   // For the time being the timestamp is an unsigned long long.
   // If we change the definition we should take care of defining a zero element.
   sim_time_t min;
   Simulator::outcome result = Simulator::OK;
   while( set_variable(min) ) {
      result = current_comp_->step_event();
      switch( result ) {
         case Simulator::OK:
            // Do nothing
            break;
         case Simulator::ERROR:
            error_message( "At time " + min );
            return 1;
            break;
         case Simulator::CHANGED:
            // TODO: advice the other simulators
            break;
         default:
            error_message( "Function step_event() returned a wrong value" );
            return 1;
            break;
      }
   }
   return 0;
}

void
Manager::end_simulation() {
   for( auto it = instances_.begin(); it != instances_.end(); ++it ) {
      current_comp_ = it->first;
      current_comp_->end_simulation();
   }
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

void
Manager::add_not_found(const std::string& module_name) {
   assert( !module_name.empty() );
   if( not_found_.empty() )
      not_found_ = module_name;
   else
      not_found_ = module_name + "->" + not_found_;
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
      if( avoid_endless == look_for.top() ) {
         add_not_found(avoid_endless->name());
         return 1;
      }
      avoid_endless = look_for.top();
      for( auto it = instances_.begin(); it != instances_.end(); ++it ) {
         current_comp_ = it->first;
         switch( current_comp_->instantiate( *avoid_endless ) ) {
            case Elaborator::FOUND:
               {
                  look_for.pop();
                  // Here I should save the something for fast retrieve
                  res = elaborate( current_comp_->get_instance() );
                  if ( res ) {
                     add_not_found(avoid_endless->name());
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
   assert(not_found_.empty());

   // Can we go ahead?
   for( auto it = instances_.begin(); it != instances_.end(); ++it ) {
      current_comp_ = it->first;
      if ( !current_comp_->can_continue() )
         break;
   }
   // At least one compiler had a problem.
   if( !current_comp_->can_continue() ) {
      error_message("can_continue() returned false");
      return 1;
   }
   // Emit code
   for( auto it = instances_.begin(); it != instances_.end(); ++it ) {
      current_comp_ = it->first;
      res = current_comp_->emit_code();
      if( res ) {
         error_message("In emit_code()");
         return res;
      }
   }
   return 0;
}

int
Manager::run( CompilerStep step ) {
   assert( instances_.size() );
   int res = 0;

   if( step >= CompilerStep::ANALYSIS ) {
      current_step_ = CompilerStep::ANALYSIS;
      res = do_analysis();
      current_comp_ = nullptr;
      if( res ) {
         return res;
      }
   }
   if( step >= CompilerStep::ELABORATION ) {
      current_step_ = CompilerStep::ELABORATION;
      res = do_elaboration();
      current_comp_ = nullptr;
      if( res ) {
         return res;
      }
   }
   if( step >= CompilerStep::SIMULATION ) {
      current_step_ = CompilerStep::SIMULATION;
      res = do_simulation();
      current_comp_ = nullptr;
      end_simulation();
      current_comp_ = nullptr;
      if( res ) {
         return res;
      }
   }
   return 0;
}
