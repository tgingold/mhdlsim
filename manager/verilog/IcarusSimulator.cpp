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
#include "IcarusSimulator.hpp"
#include "net.h"
#include "schedule.h"
#include "compile.h"
#include "vvp_object.h"
#include "vpi_priv.h"
#include "vpi_mcd.h"
#include "vvp_vpi.h"
#include "parse_misc.h"
#include "vpi_callback.h"
#include "vvp_cleanup.h"

using std::cerr;

#define VVP_INPUT "myoutput"
extern vector<const char *> file_names;
bool have_ivl_version;
// TODO: to assign
int vpip_delay_selection;
int vvp_return_value;
extern bool sim_started;
extern struct event_time_s* sched_list;
extern struct event_s* schedule_init_list;

IcarusSimulator::IcarusSimulator() {};
IcarusSimulator::~IcarusSimulator() {};

int
IcarusSimulator::initialize() {
   vpip_mcd_init( NULL );
   vpi_mcd_printf(1, "Compiling VVP ...\n");
   vvp_vpi_init();
   //vpi_set_vlog_info( 0, 0 );
   compile_init();
   for( unsigned i = 0; i < module_cnt; ++i ){
      vpip_load_module( module_tab[i] );
   }
   int ret = compile_design(VVP_INPUT);
   vvp_destroy_lexor();
   print_vpi_call_errors();
   if( ret )
      return ret;
   compile_cleanup();
   schedule_simulate();
   if( compile_errors ) {
      vpi_mcd_printf(1, "%s: Program not runnable, %u errors.\n",
            VVP_INPUT, compile_errors);
      return compile_errors;
   }
   // Execute initialization events.
   while (schedule_init_list) {
      struct event_s*cur = schedule_init_list->next;
      if (cur->next == cur) {
         schedule_init_list = 0;
      } else {
         schedule_init_list->next = cur->next;
      }
      cur->run_run();
      delete cur;
   }
   // Execute start of simulation callbacks
   vpiStartOfSim();
   sim_started = true;
   return 0;
};

bool
IcarusSimulator::other_event() {
   if( !sim_started )
      return schedule_init_list == nullptr;
   return schedule_finished();
}

void
IcarusSimulator::notify( Net* net ) {
   assert( sim_started );
};

Simulator::outcome
IcarusSimulator::step_event() {
   assert( sim_started );
   if( !schedule_finished() ) {
      return Simulator::OK;
   }
   // If the manager calls this function when the simulator does not
   // have events to execute it means that something went terribly wrong
   // and therefore is better to return an error
   return Simulator::ERROR;
};

void
IcarusSimulator::end_simulation() {
   assert( sim_started );
   //vvp_object::cleanup();
   //load_module_delete();
};

sim_time_t
IcarusSimulator::next_event() const {
   if( !sim_started )
      return IcarusSimulator::minSimValue();
   return schedule_simtime() + sched_list->delay;
};

sim_time_t
IcarusSimulator::current_time() const {
   assert( sim_started );
   return static_cast<sim_time_t>(schedule_simtime());
};

int
IcarusSimulator::advance_time( sim_time_t time_new ) {
   assert( sim_started );
   assert( time_new <= std::numeric_limits<vvp_time64_t>::max() );
   // TODO: complete here
   assert(schedule_time == time_new);
   return 0;
};
