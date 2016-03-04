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

#include <cstdlib>
#include <iostream>
#include "compiler.h"
#include "manager.h"
#include "IcarusCompiler.hpp"
#include "ArgumentParser.hpp"
#include <boost/scoped_ptr.hpp>

int main( int argc, char*argv[] )
{
   //Compiler* vhdl = new GHDLCompiler();
   boost::scoped_ptr<Compiler> verilog( new IcarusCompiler() );
   ArgumentParser ap;
   switch( ap.vectorifyArguments( argc, argv ) ) {
      case ArgumentParser::CONTINUE_OK:
         break;
      case ArgumentParser::EXIT_OK:
         // Pass flags to compilers and exit.
         // I expect compilers to print something and do no computation at all
         verilog->processParams( ap.getVerilogParams() );
         //vhdl.processParam(ap.getVHDLParams());
         return EXIT_SUCCESS;
         break;
      case ArgumentParser::ERROR:
         // An error message has been already printed, just exit.
         return EXIT_FAILURE;
      default:
         std::cerr << "Something bad happened, you should never see this message." << std::endl;
         return EXIT_FAILURE;
   }

   Manager dumbledore;

   //if( ap.getVHDLFiles().size() ) {
      //vhdl.add_files( ap.getVHDLFiles() );
      //vhdl.processParam( ap.getVHDLParams() );
      //dumbledore.add_instance( Compiler::Type::VHDL, vhdl );
   //}

   if( ap.getVerilogFiles().size() ) {
      verilog->add_files( ap.getVerilogFiles() );
      verilog->processParams( ap.getVerilogParams() );
      dumbledore.add_instance( Compiler::Type::VERILOG, verilog.get() );
   }

   int result = dumbledore.run();

   // cleanup
   //delete vhdl;

   if( result ) {
      return EXIT_FAILURE;
   } else {
      return EXIT_SUCCESS;
   }
}
