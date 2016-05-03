#ifndef GHDLELABORATOR_H
#define GHDLELABORATOR_H

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

#include "module.h"
#include "GhdlHandler.hpp"
#include "elaborator.h"

class GhdlElaborator : public virtual Elaborator {
   public:
      enum verilog_logic { VERILOGX, VERILOG0, VERILOG1, VERILOGZ };
      enum vhdl_logic { VHDLU, VHDLX, VHDL0, VHDL1, VHDLZ, VHDLW, VHDLL, VHDLH, VHDLDASH };
      enum vhdl_strenght { WEAK, STRONG };

      GhdlElaborator();
      virtual ~GhdlElaborator();

      virtual ModuleSpec* elaborate(ModuleInstance* module);

      bool can_continue();

      int emit_code();

      result instantiate(ModuleSpec& iface);

   protected:
      ///> Modules provided by this simulator instance. They can be instantiated
      ///> as required. The string key is the name of the module, as defined
      ///> in its interface.
      std::map<const std::string, ModuleInterface> modules_;

      GhdlElaborator::verilog_logic transform(vhdl_logic type);

      // buf to remember where we got stuck
      //elaborator_work_item_t* cur;

      ///> Instances of modules handled by this simulator instance. The string key
      ///> is the name of an instance, not the name of the module.
      std::map<const std::string, ModuleInstance> instances_;

};

#endif /* GHDLELABORATOR_H */
