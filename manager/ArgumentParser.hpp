#ifndef ARGUMENT_PARSER_H
#define ARGUMENT_PARSER_H

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

#include <cstdlib>
#include <iostream>
#include <string>
#include <cstring>
#include <vector>

/*
 * CONTINUE_OK -> execution can continue
 * ERROR       -> execution must finish with EXIT_FAILURE
 * EXIT_OK     -> execution must finish with EXIT_SUCCESS
 */
enum ParsingStatus { CONTINUE_OK, ERROR, EXIT_OK };

class ArgumentParser {
   public:

      ArgumentParser( bool complainAndExitOnError = true );
      virtual ~ArgumentParser();

      ParsingStatus vectorifyArguments( int argc, char **argv );

      std::vector<std::string>& getVerilogFiles();
      std::vector<std::string>& getVHDLFiles();

      std::vector<std::string>& getVerilogParams();
      std::vector<std::string>& getVHDLParams();

   private:
      std::vector<std::string> VHDLFiles_;
      std::vector<std::string> verilogFiles_;
      
      std::vector<std::string> VHDLParams_;
      std::vector<std::string> verilogParams_;

      void translate_parameters();
      bool checkFiles( std::vector<std::string>& files );

      bool complainAndExitOnError_;     
      bool analyze_;
      std::vector<std::string> simulate_;
      bool version_;
      bool verbose_;
};

#endif
