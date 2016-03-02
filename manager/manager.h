/*
 * Copyright (c) 2016 CERN
 * @author Maciej Suminski <maciej.suminski@cern.ch>
 * @author Michele Castellana <michele.castellana@cern.ch>
 *
 * This source code is free software; you can redistribute it
 * and/or modify it in source code form under the terms of the GNU
 * General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */

#ifndef MANAGER_H
#define MANAGER_H

#include "compiler.h"
#include <list>

class Simulator;
class Net;
class ModuleInstance;

/**
 * @brief Class responsible for mixed-language elaboration and simulation.
 * Creates simulator instances as needed and coordinates them.
 */
class Manager {
public:
    Manager();
    virtual ~Manager();

    /**
     * @brief Adds a new simulator instance to be managed.
     * @param sim is the instance to be added.
     * @param type is instance type.
     */
    void add_instance(Compiler::Type type, Compiler* sim);

    /**
     * @brief Starts the simulation.
     */
    int run();

    // TODO interface to inspect signals, variables, control the sim flow?

private:
    ///> Instances of simulators to manage.
    std::map<Compiler::Type, Compiler*> instances_;

    ///> Keep list of modules that have to be notified
    ///> when a particular signal changes.
    std::map<Net*, std::list<ModuleInstance*> > sensitivity_;

    ///> Currently active Compiler (i.e. executing code)
    Compiler* current_comp_;
};

#endif /* MANAGER_H */