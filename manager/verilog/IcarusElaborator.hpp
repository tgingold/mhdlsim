#ifndef ICARUSELABORATOR_H
#define ICARUSELABORATOR_H

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

#include "module.h"
#include "IcarusHandler.hpp"
#include "netlist.h"
#include "elaborator.h"

class IcarusElaborator : public virtual Elaborator, public virtual IcarusHandler {
   public:
      enum verilog_logic { VERILOGX, VERILOG0, VERILOG1, VERILOGZ };
      enum vhdl_logic { VHDLU, VHDLX, VHDL0, VHDL1, VHDLZ, VHDLW, VHDLL, VHDLH, VHDLDASH };
      enum vhdl_strenght { WEAK, STRONG };

      IcarusElaborator();
      virtual ~IcarusElaborator();

      /**
       * @brief Performs elaboration of the required entity/arch.
       * @param ent entity name.
       * @param arch architecture name.
       * @return unknown module/architecture discovered during elaboration.
       */
      virtual ModuleSpec* elaborate(const std::string& ent, const std::string& arch);

      /**
       * @brief Complete the elaboration.
       * @param module module/architecture needed to continue the elaboration.
       * @return unknown module/architecture discovered during elaboration.
       */
      virtual ModuleSpec* elaborate(ModuleInstance& module);

      /**
       * @brief Creates an instance with a given name.
       * This function is to be called by the Manager. It will also assign nets
       * to ports.
       */
      //ModuleInstance& instantiate(const ModuleSpec& iface);

   protected:
      ///> Modules provided by this simulator instance. They can be instantiated
      ///> as required. The string key is the name of the module, as defined
      ///> in its interface.
      std::map<const std::string, ModuleInterface> modules_;
      
      IcarusElaborator::vhdl_strenght transform(ivl_drive_t type);
      void transform(ivl_variable_type_t type);
      IcarusElaborator::verilog_logic transform(vhdl_logic type);

      Design* des;

      // buf to remember where we got stuck
      //elaborator_work_item_t* cur;

      ///> Instances of modules handled by this simulator instance. The string key
      ///> is the name of an instance, not the name of the module.
      std::map<const std::string, ModuleInstance> instances_;

};

#endif /* ICARUSELABORATOR_H */
