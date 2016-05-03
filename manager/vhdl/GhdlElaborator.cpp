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

#include "GhdlElaborator.hpp"
#include "ghdl.h"
#include <iostream>
#include <vector>
#include <list>

GhdlElaborator::GhdlElaborator() {};
GhdlElaborator::~GhdlElaborator() {};

int
GhdlElaborator::emit_code() {
   return 0;
}

bool
GhdlElaborator::can_continue() {
   return true;
}

Elaborator::result
GhdlElaborator::instantiate(ModuleSpec& iface) {
  std::cout << "GHDL: elaborate instantiate " << iface.name() << std::endl;
  return NOT_FOUND;
}

ModuleSpec*
GhdlElaborator::elaborate(ModuleInstance*inst) {
  if (inst == nullptr) {
    //  Case for the root instance.  Check if the root is known by vhdl
    if (!mhdlsim_vhdl_known_top_unit())
      return nullptr;
    mhdlsim_vhdl_elaborate();
    return nullptr;
  }
  else {
    std::cout << "GHDL: elaborate " << inst->iface()->name() << std::endl;
    return nullptr;
  }
}

GhdlElaborator::verilog_logic
GhdlElaborator::transform(vhdl_logic type) {
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
