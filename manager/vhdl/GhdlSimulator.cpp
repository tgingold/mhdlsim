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

#include <iostream>
#include "GhdlSimulator.hpp"
#include "ghdl.h"

GhdlSimulator::GhdlSimulator() {};
GhdlSimulator::~GhdlSimulator() {};

int
GhdlSimulator::initialize() {
  std::cout << "GhdlSimulator::initialize\n";
  mhdlsim_vhdl_run();
   return 0;
};

bool
GhdlSimulator::other_event() {
  std::cout << "GhdlSimulator::other_event\n";
  return false;
}

void
GhdlSimulator::notify( Net*) {
  std::cout << "GhdlSimulator::notify\n";
};

Simulator::outcome
GhdlSimulator::step_event() {
  std::cout << "GhdlSimulator::step_event\n";
  return Simulator::OK;
};

void
GhdlSimulator::end_simulation() {
  std::cout << "GhdlSimulator::end_simulation\n";
};

sim_time_t
GhdlSimulator::next_event() const {
  std::cout << "GhdlSimulator::next_event\n";
  return 0;
};

sim_time_t
GhdlSimulator::current_time() const {
  std::cout << "GhdlSimulator::current_time\n";
  return 0;
};

int
GhdlSimulator::advance_time( sim_time_t time_new ) {
  std::cout << "GhdlSimulator::advance_time\n";
   return 0;
};
