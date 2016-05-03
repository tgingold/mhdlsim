#ifndef GHDLSIMULATOR_H
#define GHDLSIMULATOR_H

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

#include "simulator.h"

class GhdlSimulator : public virtual Simulator {
   public:
      GhdlSimulator();
      virtual ~GhdlSimulator();

      virtual int initialize();

      virtual void notify(Net* );

      virtual outcome step_event();

      virtual void end_simulation();

      virtual bool other_event();

      virtual sim_time_t next_event() const;

      virtual sim_time_t current_time() const;

      virtual int advance_time(sim_time_t );

};

#endif /* GHDLSIMULATOR_H */
