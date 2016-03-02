#ifndef ICARUSIMPL_H
#define ICARUSIMPL_H

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

#include "IcarusElaborator.hpp"
#include "IcarusSimulator.hpp"
#include "IcarusAnalyzer.hpp"
#include "compiler_interface.h"

/**
 * @brief Class that represents a single compiler instance. Icarus and GHDL
 * have to provide such interface.
 */
class IcarusCompiler : public IcarusAnalyzer, public IcarusElaborator, public IcarusSimulator, public Compiler {
public:
    IcarusCompiler();
    virtual ~IcarusCompiler();

};

#endif /* ICARUSIMPL_H */
