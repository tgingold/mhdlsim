#ifndef ICARUSSIMULATOR_H
#define ICARUSSIMULATOR_H

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

#include "simulator.h"

class IcarusSimulator : public virtual Simulator {
public:
    IcarusSimulator() {};
    virtual ~IcarusSimulator() {};

    /**
     * @brief Initializes the simulator.
     * @return 0 in case of success.
     */
    virtual int initialize() { return 0; };

    /**
     * @brief Notifies the simulator that a net value has changed.
     * @param net is the net which value has just changed.
     */
    virtual void notify(Net* net) {};

    /**
     * @brief Executes the next event from the event queue.
     * @return 0 in case of success.
     */
    virtual int step_event() { return 0; };

    /**
     * @brief Returns the timestamp of the next event in the event queue.
     * @return Timestamp of the next event to be executed.
     */
    virtual sim_time_t next_event() const { return 0; };

    /**
     * @brief Returns the current simulation time.
     */
    virtual sim_time_t current_time() const { return 0; };

    /**
     * @brief Advances the simulation by time. All events in the queue will be
     * executed.
     * @param time is the amount of time for advancement.
     */
    virtual int advance_time(sim_time_t time) { return 0; };

protected:
    // TODO maybe one day an interface to call subprograms? surely not for
    // the first release

    // TODO think about debugging interface:
    // code stepping (how to obtain the reference to the currently executed line)
    // internal variables/processes lookup/modification
    // breakpoints

};

#endif /* ICARUSSIMULATOR_H */
