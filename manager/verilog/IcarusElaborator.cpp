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

#include "IcarusElaborator.hpp"
#include <vector>
#include <list>
#include <cstring>
#include "net.h"
#include "StringHeap.h"
#include "PPackage.h"
#include "elaborate.hh"
#include "PExpr.h"
#include "PWire.h"
#include "netvector.h"
#include "pform.h"
#include "HName.h"
#include "parse_api.h"
#include "Module.h"
#include "PGate.h"
#include "t-dll.h"
#include "vpi_priv.h"

#define VVP_INPUT "myoutput"

extern std::map<perm_string, PPackage*> pform_packages;
extern std::map<perm_string, unsigned> missing_modules;
extern std::list<perm_string> roots;
extern bool debug_elaborate;
extern dll_target dll_target_obj;
const char* basedir = strdup(".");
// map<name_of_the_module, pair< module_instance, father_scope >>
std::map<perm_string, std::pair<const PGModule*,NetScope*>> missing_specs;
std::map<perm_string, Module*> fake_modules;

IcarusElaborator::IcarusElaborator()
   : des_(new Design) {
      // FIXME: temporary hard-coded paths
      f["DLL"] = "/usr/local/lib/ivl/vvp.tgt";
      f["VVP_EXECUTABLE"] = "/usr/local/bin/vvp";
      f["-o"] = VVP_INPUT ;
      vpip_module_path[0] = "/usr/local/lib/ivl";
      ++vpip_module_path_cnt;
   };

IcarusElaborator::~IcarusElaborator() {
   if(des_) {
      delete des_;
      des_ = nullptr;
   }
};

int
IcarusElaborator::emit_code() {
   if(can_continue()) {
      // this is probably not the best place ever to add
      // the set_flag but at least I am sure it will be executed
      // at the end of the elaboration
      assert(des_);
      des_->set_flags(f);
      if (debug_elaborate) {
         cerr << "<Icarus>: elaborate: "
            " Start code generation" << endl;
      }
      return des_->emit(&dll_target_obj);
   }
   return 1;
}

bool
IcarusElaborator::can_continue() {
   if( !missing_modules.empty() || !missing_specs.empty() ) {
      if (debug_elaborate) {
         cerr << "<Icarus>: elaborate: "
            " I can not continue with code generation" << endl;
      }
      return false;
   }
   return true;
}

ModuleSpec*
IcarusElaborator::create_spec( const PGModule* mod, NetScope* scope ) {
   // check the currectness of the assumptions
   assert( mod && scope );
   // instance is father of the module
   assert( pform_modules.find(scope->module_name())->second->get_gate(mod->get_name()) );

   if (debug_elaborate) {
      cerr << "<Icarus>: elaborate:  Create spec for "
         << mod->get_type() << "." << endl;
   }

   StringHeapLex lex_strings;
   ModuleSpec* return_val = new ModuleSpec( std::string( mod->get_type() ),
         std::string( mod->get_name() ) );
   for( int i = 0; i < mod->pin_count(); ++i ) {
      PExpr* name = mod->pin(i);
      assert( name );
      assert( dynamic_cast<PEIdent*>( name ) );
      std::string string_port_name = std::string( dynamic_cast<PEIdent*>(name)->path().back().name.str() );
      assert( !string_port_name.empty() );
      // FIXME: the size, of course, is wrong
      PExpr::width_mode_t mode = PExpr::UNSIZED;
      return_val->make_port( string_port_name, name->test_width( des_, scope, mode ), Port::IN );
      //transform(name->expr_type());
   }
   assert( mod->pin_count() == return_val->size() );
   return return_val;
}

bool
IcarusElaborator::create_instance( ModuleSpec& spec ) {
   if (debug_elaborate) {
      cerr << "<Icarus>: elaborate: "
         << " Create an instance for the spec " << spec.name()
         << endl;
   }
   StringHeapLex lex_strings;
   perm_string cur_name = lex_strings.make( spec.name().c_str() );
   // FIXME: the if is here just for debugging purposes
   // It should be replace by:
   // if( pform_modules.find(cur_name) == pform_modules.end() ) {
   //   return false;
   // Module *module = pform_modules.find(cur_name)->second;
   Module *module;
   if( pform_modules.find(cur_name) == pform_modules.end() ) {
      assert( fake_modules.find(cur_name) != fake_modules.end() );
      module = fake_modules.find(cur_name)->second;
   } else {
      module = pform_modules.find(cur_name)->second;
   }
   // create a new scope to handle the instance
   //NetScope*scope = des_->make_root_scope( lex_strings.make(spec.name()), module->program_block, module->is_interface);
   instance_found_ = new ModuleInstance( &spec );
   // FIXME: we should do some checks here
   /*
      for( auto it = spec.ports().begin(); it != spec.ports().end(); ++it ) {
      char *abc = new char[ (*it)->name().length() + 1 ];
      strncpy( abc, (*it)->name().c_str(), (*it)->name().length() );
      PEString *port_pextring = new PEString( abc );
      tmp->push_back(port_pextring);
      NetNet* tmp = scope->find_signal( lex_strings.make( (*it)->name().c_str() ) );
      if( !tmp ) {
      if (debug_elaborate) {
      cerr << "<Icarus>: elaborate: signal " << (*it)->name().c_str() 
      << " not found" << endl;
      }
      return false;
      }
   // FIXME: this check is wrong but is here just to get the idea
   if( (*it)->width() != tmp->vector_width() ) {
   if (debug_elaborate) {
   cerr << "<Icarus>: elaborate: " << (*it)->name().c_str() 
   << " in " << cur_name.str() << " has wrong width" << endl;
   cerr << (*it)->name().c_str() << " has " << (*it)->width()
   << " whereas " << tmp->vector_width();
   }
   //return false;
   }
   }
   */
   //elaborate_root_scope_t help_call(des_, scope, module);
   //help_call.elaborate_runrun();
   return true;
}

Elaborator::result
IcarusElaborator::instantiate( ModuleSpec& spec ) {
   if (debug_elaborate) {
      cerr << "<Icarus>: elaborate: "
         << " Received the spec " << spec.name()
         << " for instantiation." << endl;
   }
   StringHeapLex lex_strings;
   perm_string cur_name = lex_strings.make( spec.name().c_str() );
   if( !des_ ) {
      // Elaboration has never started
      roots.push_back( lex_strings.make( cur_name ) );
      ModuleSpec* tmp = start_elaboration();
      if( tmp ) {
         spec_to_find_ = tmp;
         return NEED_ANOTHER;
      }
   }
   // FIXME: The following if is here
   // just for testing purposes...TO REMOVE
   if( pform_modules.find(cur_name) == pform_modules.end() ) {
      Module* tmp = mod_from_spec( &spec );
      assert(tmp);
   }
   // If there was a problem in the instance creation, return a NOT_FOUND
   if ( !create_instance( spec ) )
      return NOT_FOUND;
   assert( instance_found_ );
   return FOUND;
}

void
IcarusElaborator::add_vpi_module( const char* name ) {
   if (vpi_module_list == 0) {
      vpi_module_list = strdup(name);
   } else {
      char*tmp = (char*)realloc(vpi_module_list,
            strlen(vpi_module_list)
            + strlen(name)
            + 2);
      strcat(tmp, ",");
      strcat(tmp, name);
      vpi_module_list = tmp;
   }
   f["VPI_MODULE_LIST"] = vpi_module_list;
}

ModuleSpec*
IcarusElaborator::start_elaboration() {
   assert( !roots.empty() );

   // Elaborate enum sets in $root scope.
   elaborate_rootscope_enumerations(des_);

   // Elaborate tasks and functions in $root scope.
   elaborate_rootscope_tasks(des_);

   // Elaborate classes in $root scope.
   elaborate_rootscope_classes(des_);

   // Elaborate the packages. Package elaboration is simpler
   // because there are fewer sub-scopes involved.
   unsigned i = 0;
   for (map<perm_string,PPackage*>::iterator pac = pform_packages.begin()
         ; pac != pform_packages.end() ; ++ pac) {

      //assert(*pac->second, pac->first == pac->second->pscope_name());
      NetScope*scope = des_->make_package_scope(pac->first);
      scope->set_line(pac->second);

      elaborator_work_item_t*es = new elaborate_package_t(des_, scope, pac->second);
      des_->elaboration_work_list.push_back(es);

      pack_elems.push_back( { pac->second, scope } );
      ++i;
   }

   // Scan the root modules by name, and elaborate their scopes.
   i = 0;
   for (list<perm_string>::const_iterator root = roots.begin()
         ; root != roots.end() ; ++ root ) {

      // Look for the root module in the list.
      map<perm_string,Module*>::const_iterator mod = pform_modules.find(*root);
      if (mod == pform_modules.end()) {
         cerr << "error: Unable to find the root module \""
            << (*root) << "\" in the Verilog source." << endl;
         cerr << "     : Perhaps ``-s " << (*root)
            << "'' is incorrect?" << endl;
         des_->errors++;
         continue;
      }

      // Get the module definition for this root instance.
      Module *rmod = (*mod).second;

      // Make the root scope. This makes a NetScope object and
      // pushes it into the list of root scopes in the Design.
      NetScope*scope = des_->make_root_scope(*root, rmod->program_block,
            rmod->is_interface);

      // Collect some basic properties of this scope from the
      // Module definition.
      scope->set_line(rmod);
      scope->time_unit(rmod->time_unit);
      scope->time_precision(rmod->time_precision);
      scope->time_from_timescale(rmod->time_from_timescale);
      des_->set_precision(rmod->time_precision);


      // Save this scope, along with its definition, in the
      // "root_elems" list for later passes.
      struct root_elem tmp = { rmod, scope };
      root_elems.push_back( tmp );

      // Arrange for these scopes to be elaborated as root
      // scopes. Create an "elaborate_root_scope" object to
      // contain the work item, and append it to the scope
      // elaborations work list.
      elaborator_work_item_t*es = new elaborate_root_scope_t(des_, scope, rmod);
      des_->elaboration_work_list.push_back(es);
   }

   // Run the work list of scope elaborations until the list is
   // empty. This list is initially populated above where the
   // initial root scopes are primed.
   while (! des_->elaboration_work_list.empty()) {
      // Push a work item to process the defparams of any scopes
      // that are elaborated during this pass. For the first pass
      // this will be all the root scopes. For subsequent passes
      // it will be any scopes created during the previous pass
      // by a generate construct or instance array.
      des_->elaboration_work_list.push_back(new top_defparams(des_));

      // Transfer the queue to a temporary queue.
      list<elaborator_work_item_t*> cur_queue;
      while (! des_->elaboration_work_list.empty()) {
         cur_queue.push_back(des_->elaboration_work_list.front());
         des_->elaboration_work_list.pop_front();
      }

      // Run from the temporary queue. If the temporary queue
      // items create new work queue items, they will show up
      // in the elaboration_work_list and then we get to run
      // through them in the next pass.
      while (! cur_queue.empty()) {
         elaborator_work_item_t*tmp = cur_queue.front();
         cur_queue.pop_front();
         tmp->elaborate_runrun();
         delete tmp;
      }

      if (! des_->elaboration_work_list.empty()) {
         des_->elaboration_work_list.push_back(new later_defparams(des_));
      }
   }

   if (debug_elaborate) {
      cerr << "<Icarus>: elaborate: "
         << "elaboration work list done. Start processing residual defparams." << endl;
   }

   // Look for residual defparams (that point to a non-existent
   // scope) and clean them out.
   des_->residual_defparams();

   // Errors already? Probably missing root modules. Just give up
   // now and return nothing.
   if (des_->errors > 0) {
      return nullptr;
   }

   if( !missing_specs.empty() ) {
      auto it = missing_specs.begin();
      ModuleSpec* abc = create_spec( (*it).second.first, (*it).second.second );
      assert( abc );
      return abc;
   }

   return continue_elaboration(nullptr);
}

Module*
IcarusElaborator::mod_from_spec( ModuleSpec* inst ) {
   StringHeapLex lex_strings;
   const perm_string provided = lex_strings.make( inst->name().c_str() );
   Module* ret_val = nullptr;
   if( fake_modules.find(provided) == fake_modules.end() ) {
      fake_modules[provided] = new FakeModule( nullptr, provided );
      ret_val = fake_modules[provided];
      for( auto it = inst->ports().begin(); it != inst->ports().end(); ++it ) {
         // fill the port list of the module
         perm_string cur_name = lex_strings.make( (*it)->name().c_str() );
         ret_val->ports.push_back( pform_module_port_reference(
                  lex_strings.make((*it)->name()),
                  "filename", 0
                  ));
         PWire *tmp;
         switch( (*it)->direction() ) {
            case Port::IN:
               tmp = new PWire( cur_name, NetNet::WIRE, NetNet::PINPUT, IVL_VT_NO_TYPE );
               break;
            case Port::OUT:
               tmp = new PWire( cur_name, NetNet::WIRE, NetNet::POUTPUT, IVL_VT_NO_TYPE );
               break;
            case Port::INOUT:
               tmp = new PWire( cur_name, NetNet::WIRE, NetNet::PINOUT, IVL_VT_NO_TYPE );
               break;
            case Port::UNKNOWN:
               tmp = new PWire( cur_name, NetNet::WIRE, NetNet::NOT_A_PORT, IVL_VT_NO_TYPE );
               break;
            default:
               break;
         }
         assert(tmp);
         ret_val->wires[ cur_name ] = tmp;
         //tmp->set_wire_type(NetNet::INTEGER);
         //tmp->set_data_type();
         tmp->set_range_scalar(SR_BOTH);
         tmp->set_signed(true);
      }
   } else {
      ret_val = fake_modules[provided];
   }
   assert(ret_val);
   assert( inst->ports().size() == ret_val->ports.size() );
   assert( ret_val->mod_name() == provided );
   return ret_val;
}

void
IcarusElaborator::create_and_substitute_pgmodule( ModuleInstance* inst, NetScope* scope ) {
   StringHeapLex lex_strings;
   const perm_string module_name = lex_strings.make( inst->iface()->name().c_str() );
   const perm_string instance_name = lex_strings.make( inst->iface()->instance_name().c_str() );
   assert( fake_modules.find(module_name) != fake_modules.end() );
   assert( fake_modules.find(module_name)->second );
   PGModule *instance = new PGModule( fake_modules.find(module_name)->second, instance_name, true );
   assert(pform_modules.find(scope->module_name()) != pform_modules.end());
   Module *father = pform_modules[scope->module_name()];
   unsigned num = father->get_gates().size();
   static_cast<FakeModule*>(father)->remove_gate(instance_name);
   father->add_gate(instance);
   assert(father->get_gates().size() == num);
   // This will elaborate the fake module as well.
   instance->elaborate_scope( des_, scope );
}

ModuleSpec*
IcarusElaborator::continue_elaboration( ModuleInstance* inst ) {
   StringHeapLex lex_strings;
   bool rc = true;
   if( inst ) {
      const perm_string provided = lex_strings.make( inst->iface()->name().c_str() );
      assert( missing_specs.find( provided ) != missing_specs.end() );
      missing_specs.erase( missing_specs.find( provided ) );
      if( !missing_specs.empty() ) {
         auto it = missing_specs.begin();
         return create_spec( (*it).second.first, (*it).second.second );
      }
      auto it = missing_specs.find( provided );
      assert( it == missing_specs.end() );
   }

   if (inst && debug_elaborate) {
      cerr << "<Icarus>: elaborate: "
         " Continue the elaboration of the instance " << inst->iface()->instance_name() << endl;
   }
   if (debug_elaborate) {
      cerr << "<Icarus>: elaborate: "
         << "Start calling Package elaborate_sig methods." << endl;
   }

   // With the parameters evaluated down to constants, we have
   // what we need to elaborate signals and memories. This pass
   // creates all the NetNet and NetMemory objects for declared
   // objects.
   for (unsigned i = 0; i < pack_elems.size(); ++i) {
      PPackage*pack = pack_elems[i].pack;
      NetScope*scope= pack_elems[i].scope;

      if (! pack->elaborate_sig(des_, scope)) {
         if (debug_elaborate) {
            cerr << "<Icarus>" << ": debug: " << pack->pscope_name()
               << ": elaborate_sig failed!!!" << endl;
         }
         return nullptr;
      }
   }

   if (debug_elaborate) {
      cerr << "<Icarus>: elaborate: "
         << "Start calling $root elaborate_sig methods." << endl;
   }

   des_->root_elaborate_sig();

   if (debug_elaborate) {
      cerr << "<Icarus>: elaborate: "
         << "Check wheter there are missing modules." << endl;
   }

   if (debug_elaborate) {
      cerr << "<Icarus>: elaborate: "
         << "Start calling root module elaborate_sig methods." << endl;
   }

   for (unsigned i = 0; i < root_elems.size(); ++i) {
      Module *rmod = root_elems[i].mod;
      NetScope *scope = root_elems[i].scope;
      scope->set_num_ports( rmod->port_count() );

      if (debug_elaborate) {
         cerr << "<Icarus>" << ": debug: " << rmod->mod_name()
            << ": port elaboration root "
            << rmod->port_count() << " ports" << endl;
      }

      if (! rmod->elaborate_sig(des_, scope)) {
         if (debug_elaborate) {
            cerr << "<Icarus>" << ": debug: " << rmod->mod_name()
               << ": elaborate_sig failed!!!" << endl;
         }
         return nullptr;
      }

      // Some of the generators need to have the ports correctly
      // defined for the root modules. This code does that.
      for (unsigned idx = 0; idx < rmod->port_count(); ++idx) {
         vector<PEIdent*> mport = rmod->get_port(idx);
         unsigned int prt_vector_width = 0;
         PortType::Enum ptype = PortType::PIMPLICIT;
         for (unsigned pin = 0; pin < mport.size(); ++pin) {
            // This really does more than we need and adds extra
            // stuff to the design that should be cleaned later.
            NetNet *netnet = mport[pin]->elaborate_subport(des_, scope);
            if (netnet != 0) {
               // Elaboration may actually fail with
               // erroneous input source
               //assert(*mport[pin], netnet->pin_count()==1);
               prt_vector_width += netnet->vector_width();
               ptype = PortType::merged(netnet->port_type(), ptype);
            }
         }
         if (debug_elaborate) {
            cerr << "<Icarus>" << ": debug: " << rmod->mod_name()
               << ": adding module port "
               << rmod->get_port_name(idx) << endl;
         }
         scope->add_module_port_info(idx, rmod->get_port_name(idx), ptype, prt_vector_width );
      }
   }

   // Now that the structure and parameters are taken care of,
   // run through the pform again and generate the full netlist.

   for (unsigned i = 0; i < pack_elems.size(); ++i) {
      PPackage*pkg = pack_elems[i].pack;
      NetScope*scope = pack_elems[i].scope;
      rc &= pkg->elaborate(des_, scope);
   }

   des_->root_elaborate();

   for (unsigned i = 0; i < root_elems.size(); ++i) {
      Module *rmod = root_elems[i].mod;
      NetScope *scope = root_elems[i].scope;
      rc &= rmod->elaborate(des_, scope);
   }

   if (rc == false) {
      return nullptr;
   }

   // Now that everything is fully elaborated verify that we do
   // not have an always block with no delay (an infinite loop),
   // or a final block with a delay.
   if (des_->check_proc_delay() == false) {
      return nullptr;
   }

   if (debug_elaborate) {
      cerr << "<Icarus>" << ": debug: "
         << " finishing with "
         <<  des_->find_root_scopes().size() << " root scopes " << endl;
   }

   if (debug_elaborate) {
      cerr << "<Icarus>: elaborate: "
         " End of the scope elaboration of the new instance " << endl;
   }

   if (debug_elaborate) {
      cerr << "<Icarus>: elaborate: "
         " End of the elaboration of the new instance " << endl;
   }

   return nullptr;
}

ModuleSpec*
IcarusElaborator::elaborate( ModuleInstance* inst ) {
   if( !inst ) {
      if( !roots.empty() ) {
         if (debug_elaborate) {
            cerr << "<Icarus>: elaborate: "
               " Start elaboration." << endl;
         }
         add_vpi_module("system");
         ModuleSpec* tmp = start_elaboration();
         if( !tmp ) {
            return continue_elaboration( nullptr );
         }
         return tmp;
      } else {
         if (debug_elaborate) {
            cerr << "<Icarus>: elaborate: "
               " Not a root module." << endl;
         }
         return nullptr;
      }
   }
   if (debug_elaborate) {
      cerr << "<Icarus>: elaborate: "
         " Provided an instance to complete elaboration." << endl;
   }
   StringHeapLex lex_strings;
   const perm_string provided = lex_strings.make( inst->iface()->name().c_str() );
   if( missing_specs.find( provided ) == missing_specs.end() ) {
      if (debug_elaborate) {
         cerr << "<Icarus>: elaborate: "
            " The instance provided was not required." << endl;
      }
      return nullptr;
   }
   assert ( fake_modules.find(provided) != fake_modules.end() );
   assert ( fake_modules.find(provided)->second );
   // scope of the father
   NetScope *net = nullptr;
   for( auto it = missing_specs.begin(); it != missing_specs.end(); ++it ) {
      if( it->first == provided ) {
         net = it->second.second;
         break;
      }
   }
   assert(net);
   mod_from_spec( inst->iface() );
   create_and_substitute_pgmodule( inst, net );
   return continue_elaboration( inst );
}

IcarusElaborator::verilog_logic
IcarusElaborator::transform(vhdl_logic type) {
   switch(type) {
      case vhdl_logic::VHDLU:
      case vhdl_logic::VHDLX:
      case vhdl_logic::VHDLDASH:
      case vhdl_logic::VHDLW:
         return verilog_logic::VERILOGX;
      case vhdl_logic::VHDL0:
         return verilog_logic::VERILOG0;
      case vhdl_logic::VHDL1:
         return verilog_logic::VERILOG1;
      case vhdl_logic::VHDLZ:
         return verilog_logic::VERILOGZ;
      case vhdl_logic::VHDLL:
         return verilog_logic::VERILOG0;
      case vhdl_logic::VHDLH:
         return verilog_logic::VERILOG1;
      default:
         // error unrecognized
         abort();
   }
}

IcarusElaborator::vhdl_strenght
IcarusElaborator::transform(ivl_drive_t type) {
   switch(type) {
      case IVL_DR_HiZ:
         // This is a special case.
         // This is a Z in std_logic
         return vhdl_strenght::STRONG;
         break;
      case IVL_DR_SUPPLY:
      case IVL_DR_PULL:
      case IVL_DR_STRONG:
         return vhdl_strenght::STRONG;
         break;
      case IVL_DR_SMALL:
      case IVL_DR_MEDIUM:
      case IVL_DR_WEAK:
      case IVL_DR_LARGE:
         return vhdl_strenght::WEAK;
         break;
      default:
         // error unrecognized
         abort();
   }
}

void
IcarusElaborator::transform(ivl_variable_type_t type) {
   switch(type) {
      case IVL_VT_REAL:
         break;
      case IVL_VT_CLASS:
      case IVL_VT_VOID:
         // error untranslatable
      default:
         // error unrecognized
         abort();
   }
}
