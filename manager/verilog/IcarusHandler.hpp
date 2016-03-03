#ifndef ICARUSHANDLER_H
#define ICARUSHANDLER_H

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

#include "handler.h"
#include <set>
#include <vector>
#include <string>


/**
 * @brief A class to manage the parameters and the file list.
 */
class IcarusHandler : public virtual FileParamHandler {
public:
    IcarusHandler();
    virtual ~IcarusHandler();

    /**
     * @brief Add file(s).
     */
    virtual void add_files(std::vector<std::string>&);
    virtual void add_file(const std::string&);
    
    /**
     * @brief Add param(s).
     */
    virtual void processParams(std::vector<std::string>&);

protected:
    std::set<std::string> files_;
};

#endif /* ICARUSHANDLER_H */
