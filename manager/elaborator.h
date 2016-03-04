#ifndef ELABORATOR_H
#define ELABORATOR_H

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

#include "module.h"
#include "handler.h"
#include <map>

class Elaborator : public virtual FileParamHandler {
public:
    Elaborator() {};
    virtual ~Elaborator() {};

    /**
     * @brief Complete the elaboration.
     * @param module module/architecture needed to continue the elaboration.
     * @return unknown module/architecture discovered during elaboration.
     * NULL in any other case.
     */
    virtual ModuleSpec* elaborate(ModuleInstance* module = NULL) = 0;

    /**
     * @brief True if there are no internal problems. False otherwise.
     * Waiting for an Instance to complete the elaboration has to return
     * false
     */
    virtual bool can_continue() = 0;

    /**
     * @brief Emit code for the simulation.
     * If something went wrong in the elaboration phase, do nothing.
     */
    virtual int emit_code() = 0;

    /**
     * @brief Creates an instance with the given spec.
     * @param iface interface of the the instance to create.
     * @return the created instance.
     */
    virtual ModuleInstance* instantiate(const ModuleSpec& iface) = 0;

protected:
    ///> Modules provided by this simulator instance. They can be instantiated
    ///> as required. The string key is the name of the module, as defined
    ///> in its interface.
    std::map<const std::string, ModuleInterface> modules_;

    ///> Instances of modules handled by this simulator instance. The string key
    ///> is the name of an instance, not the name of the module.
    std::map<const std::string, ModuleInstance> instances_;

};

#endif /* ELABORATOR_H */
