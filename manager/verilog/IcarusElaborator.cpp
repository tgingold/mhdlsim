#include "IcarusElaborator.hpp"
#include <vector>
#include <list>
#include "StringHeap.h"
#include "PPackage.h"
#include "elaborate.hh"
#include "PExpr.h"
#include "StringHeap.h"
#include "HName.h"
#include "parse_api.h"

extern std::map<perm_string, PPackage*> pform_packages;
extern std::map<perm_string, unsigned> missing_modules;
extern std::list<perm_string> roots;
extern bool debug_elaborate;

IcarusElaborator::IcarusElaborator() {};
IcarusElaborator::~IcarusElaborator() {};
      
// This is almost a copy/paste of elaborate from Icarus elaborate.cc for the time being
ModuleSpec*
IcarusElaborator::elaborate(const std::string& ent, const std::string& arch) {
   std::vector<struct root_elem> root_elems(roots.size());
   std::vector<struct pack_elem> pack_elems(pform_packages.size());
   bool rc = true;
   unsigned i = 0;
   
   // Elaborate enum sets in $root scope.
   elaborate_rootscope_enumerations(des);

   // Elaborate tasks and functions in $root scope.
   elaborate_rootscope_tasks(des);

   // Elaborate classes in $root scope.
   elaborate_rootscope_classes(des);

   // Elaborate the packages. Package elaboration is simpler
   // because there are fewer sub-scopes involved.
   i = 0;
   for (map<perm_string,PPackage*>::iterator pac = pform_packages.begin()
         ; pac != pform_packages.end() ; ++ pac) {

      assert( pac->first == pac->second->pscope_name() );
      NetScope*scope = des->make_package_scope(pac->first);
      scope->set_line(pac->second);

      elaborator_work_item_t*es = new elaborate_package_t(des, scope, pac->second);
      des->elaboration_work_list.push_back(es);

      pack_elems[i].pack = pac->second;
      pack_elems[i].scope = scope;
      i += 1;
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
         des->errors++;
         continue;
      }

      // Get the module definition for this root instance.
      Module *rmod = (*mod).second;

      // Make the root scope. This makes a NetScope object and
      // pushes it into the list of root scopes in the Design.
      NetScope*scope = des->make_root_scope(*root, rmod->program_block,
            rmod->is_interface);

      // Collect some basic properties of this scope from the
      // Module definition.
      scope->set_line(rmod);
      scope->time_unit(rmod->time_unit);
      scope->time_precision(rmod->time_precision);
      scope->time_from_timescale(rmod->time_from_timescale);
      des->set_precision(rmod->time_precision);


      // Save this scope, along with its definition, in the
      // "root_elems" list for later passes.
      root_elems[i].mod = rmod;
      root_elems[i].scope = scope;
      i += 1;

      // Arrange for these scopes to be elaborated as root
      // scopes. Create an "elaborate_root_scope" object to
      // contain the work item, and append it to the scope
      // elaborations work list.
      elaborator_work_item_t*es = new elaborate_root_scope_t(des, scope, rmod);
      des->elaboration_work_list.push_back(es);
   }

   // Run the work list of scope elaborations until the list is
   // empty. This list is initially populated above where the
   // initial root scopes are primed.
   while ( !des->elaboration_work_list.empty() ) {
      // Push a work item to process the defparams of any scopes
      // that are elaborated during this pass. For the first pass
      // this will be all the root scopes. For subsequent passes
      // it will be any scopes created during the previous pass
      // by a generate construct or instance array.
      des->elaboration_work_list.push_back(new top_defparams(des));

      // Run from the temporary queue. If the temporary queue
      // items create new work queue items, they will show up
      // in the elaboration_work_list and then we get to run
      // through them in the next pass.
      while ( !des->elaboration_work_list.empty() ) {
         elaborator_work_item_t* cur = des->elaboration_work_list.front();
         cur->elaborate_runrun();
         // FIXME here is the point where I should stop and resume if something goes wrong.
         //if( everything_ok ) {
         //   des->elaboration_work_list.pop_front();
         //   delete cur;
         //} else {
         //   // FIXME: create module required and return it.
         //}
      }

      if ( !des->elaboration_work_list.empty() ) {
         des->elaboration_work_list.push_back(new later_defparams(des));
      }
   }

   if (debug_elaborate) {
      cerr << "<toplevel>: elaborate: "
         << "elaboration work list done. Start processing residual defparams." << endl;
   }

   // Look for residual defparams (that point to a non-existent
   // scope) and clean them out.
   des->residual_defparams();

   // Errors already? Probably missing root modules. Just give up
   // now and return nothing.
   if (des->errors > 0)
      abort();

   if (debug_elaborate) {
      cerr << "<toplevel>: elaborate: "
         << "Start calling Package elaborate_sig methods." << endl;
   }

   // With the parameters evaluated down to constants, we have
   // what we need to elaborate signals and memories. This pass
   // creates all the NetNet and NetMemory objects for declared
   // objects.
   for (i = 0; i < pack_elems.size(); i += 1) {
      PPackage*pack = pack_elems[i].pack;
      NetScope*scope= pack_elems[i].scope;

      if (! pack->elaborate_sig(des, scope)) {
         if (debug_elaborate) {
            cerr << "<toplevel>" << ": debug: " << pack->pscope_name()
               << ": elaborate_sig failed!!!" << endl;
         }
         abort();
      }
   }

   if (debug_elaborate) {
      cerr << "<toplevel>: elaborate: "
         << "Start calling $root elaborate_sig methods." << endl;
   }

   des->root_elaborate_sig();

   if (debug_elaborate) {
      cerr << "<toplevel>: elaborate: "
         << "Start calling root module elaborate_sig methods." << endl;
   }

   for (i = 0; i < root_elems.size(); i++) {
      Module *rmod = root_elems[i].mod;
      NetScope *scope = root_elems[i].scope;
      scope->set_num_ports( rmod->port_count() );

      if (debug_elaborate) {
         cerr << "<toplevel>" << ": debug: " << rmod->mod_name()
            << ": port elaboration root "
            << rmod->port_count() << " ports" << endl;
      }

      if (! rmod->elaborate_sig(des, scope)) {
         if (debug_elaborate) {
            cerr << "<toplevel>" << ": debug: " << rmod->mod_name()
               << ": elaborate_sig failed!!!" << endl;
         }
         abort();
      }

      // Some of the generators need to have the ports correctly
      // defined for the root modules. This code does that.
      for (unsigned idx = 0; idx < rmod->port_count(); idx += 1) {
         std::vector<PEIdent*> mport = rmod->get_port(idx);
         unsigned int prt_vector_width = 0;
         PortType::Enum ptype = PortType::PIMPLICIT;
         for (unsigned pin = 0; pin < mport.size(); pin += 1) {
            // This really does more than we need and adds extra
            // stuff to the design that should be cleaned later.
            NetNet *netnet = mport[pin]->elaborate_subport(des, scope);
            if (netnet != 0) {
               // Elaboration may actually fail with
               // erroneous input source
               assert( netnet->pin_count() == 1 );
               prt_vector_width += netnet->vector_width();
               ptype = PortType::merged(netnet->port_type(), ptype);
            }
         }
         if (debug_elaborate) {
            cerr << "<toplevel>" << ": debug: " << rmod->mod_name()
               << ": adding module port "
               << rmod->get_port_name(idx) << endl;
         }
         scope->add_module_port_info(idx, rmod->get_port_name(idx), ptype, prt_vector_width );
      }
   }

   // Now that the structure and parameters are taken care of,
   // run through the pform again and generate the full netlist.

   for (i = 0; i < pack_elems.size(); i += 1) {
      PPackage*pkg = pack_elems[i].pack;
      NetScope*scope = pack_elems[i].scope;
      rc &= pkg->elaborate(des, scope);
   }

   des->root_elaborate();

   for (i = 0; i < root_elems.size(); i++) {
      Module *rmod = root_elems[i].mod;
      NetScope *scope = root_elems[i].scope;
      rc &= rmod->elaborate(des, scope);
   }

   if (rc == false) {
      abort();
   }

   // Now that everything is fully elaborated verify that we do
   // not have an always block with no delay (an infinite loop),
   // or a final block with a delay.
   if (des->check_proc_delay() == false) {
      delete des;
      des = 0;
   }

   if (debug_elaborate) {
      cerr << "<toplevel>" << ": debug: "
         << " finishing with "
         <<  des->find_root_scopes().size() << " root scopes " << endl;
   }

   return NULL;
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

ModuleSpec*
IcarusElaborator::elaborate(ModuleInstance& module) {
   perm_string cur_name = perm_string::literal(module.iface()->name().c_str());

   // FIXME: how to handle the case in which I have 3 modules of the same type.
   // I have to remember where to put them.
   // TODO: handle the case of more instantiation of the same module
   // remove the provided module from the mdule to find
   if(missing_modules.find(perm_string::literal(cur_name)) != missing_modules.end()) {
      missing_modules.erase(missing_modules.find(cur_name));
      instances_.insert( std::pair<const std::string, ModuleInstance>(module.iface()->name(), module) );
   }

   if(!missing_modules.empty()) {
      // TODO: create the ModuleSpec and return it
   }
   // From now on, we can continue to 

   NetScope* found = NULL;
   // find the scope
   for(auto it = des->find_root_scopes().begin(); it != des->find_root_scopes().end(); it++ ) {
      found = (*it)->child( hname_t(cur_name) );
      if(found)
         break;
   }
   if(!found)
      assert(false);

}
