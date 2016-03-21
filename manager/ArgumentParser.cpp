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
#include <string>
#include <cstring>
#include <boost/program_options.hpp>
#include "ArgumentParser.hpp"
#include <vector>
#include "config.h"

#define MAX_INPUT_FILE_NUM -1
bool mixed_lang_enabled;

const std::set<std::string> ArgumentParser::vhdlexts_ = { std::string(".vhdl"), std::string(".vhd") };
const std::set<std::string> ArgumentParser::verilogexts_ = { std::string(".v") };

ArgumentParser::ArgumentParser(bool complainAndExitOnError)
   : complainAndExitOnError_(complainAndExitOnError),
   analyze_(false),
   version_(false),
   verbose_(false) {
      mixed_lang_enabled = false;
   }

ArgumentParser::~ArgumentParser() {}

ArgumentParser::ParsingStatus
ArgumentParser::vectorifyArguments( int argc, char **argv ) {
   std::vector<std::string> tmp_file_vec;
   namespace po = boost::program_options;
   po::options_description desc("Options");
   desc.add_options()
      ("analyze,a",  po::value<bool>(&analyze_)->implicit_value(true)->default_value(false)->zero_tokens(), "Analyze only" )
      ("elaborate,e", po::value<std::vector<std::string>>(&elaborate_)->multitoken(),                       "Analyze and elaborate only" )
      ("simulate,s", po::value<std::vector<std::string>>(&simulate_)->multitoken(),                         "Simulate" )
      ("version,V",  po::value<bool>(&version_)->implicit_value(true)->default_value(false)->zero_tokens(), "Print version number and exit." )
#ifdef NDEBUG
      ("verbose,v",  po::value<bool>(&verbose_)->implicit_value(true)->default_value(false)->zero_tokens(), "Verbose output" )
#else
      ("verbose,v",  po::value<bool>(&verbose_)->implicit_value(true)->default_value(true)->zero_tokens(),  "Verbose output" )
#endif
      ;
   po::options_description hidden("hidden");
   hidden.add_options()
      ("files",       po::value<std::vector<std::string>>(&tmp_file_vec),                                   "Files to compile" )
      ;

   po::options_description all_options;
   all_options.add(desc).add(hidden);
   po::positional_options_description positional;
   positional.add( "files", MAX_INPUT_FILE_NUM );
   po::variables_map vm;
   try {
      po::store( po::command_line_parser(argc, argv).options(all_options).positional(positional).run(), vm );
      po::notify( vm );
   } catch (po::error& e) {
      std::cerr << "Error in argument parsing: " << e.what() << std::endl;
      std::cerr << desc << std::endl;
      return ERROR;
   }

   // This check is the first one because we do not need to wait for the translation
   if( version_ ) {
      std::cout << "Running the version " << PACKAGE_VERSION << " of " << PACKAGE_NAME << std::endl;
      return EXIT_OK;
   }

   translate_parameters();

   if( (!simulate_.empty()) + analyze_ + (!elaborate_.empty()) != 1 ) {
      std::cerr << "Extactly one (no more no less) parameter between analyze, elaborate and simulate must be used" << std::endl;
      return ERROR;
   }

   if( !checkFiles(tmp_file_vec) )
      return ERROR;

   if( !(getVHDLFiles().size() || getVerilogFiles().size()) ) {
      std::cerr << "No input files " << std::endl;
      return ERROR;
   }

   // This check must be after the checkFiles call
   if( getVHDLFiles().size() && getVerilogFiles().size() )
      mixed_lang_enabled = true;

   assert( (!simulate_.empty()) + analyze_ + (!elaborate_.empty()) == 1 );
   assert( getVHDLFiles().size() || getVerilogFiles().size() );

   return CONTINUE_OK;
}

bool
ArgumentParser::isExtension(const std::string& input, const std::set<std::string>& exts ) {
   for( auto ext = exts.begin(); ext != exts.end(); ext++ ) {
      try {
         if( !strcasecmp( input.substr( input.length() - ext->length() ).c_str(), ext->c_str() ) ) {
            return true;
         }
      } catch ( ... ) {
         break;
      }
   }
   return false;
}

CompilerStep
ArgumentParser::getCompType() {
   if( !simulate_.empty() )
      return CompilerStep::SIMULATION;
   if( !elaborate_.empty() )
      return CompilerStep::ELABORATION;
   return CompilerStep::ANALYSIS;
}

void
ArgumentParser::translate_parameters() {
   if( version_ ) {
      verilogParams_.emplace_back("-V");
      VHDLParams_.emplace_back("--version");
      // who cares about the other parameters?
      return;
   }
   if( analyze_ ) {
      VHDLParams_.emplace_back("-a");
      verilogParams_.emplace_back("-E");
   }
   if( verbose_ ) {
      verilogParams_.emplace_back("-v");
      VHDLParams_.emplace_back("-v");
   }

   if( !elaborate_.empty() ) {
      checkErrors( elaborate_ );
      std::string tmp;
      for( auto it = elaborate_.begin(); it != elaborate_.end(); it++ ) {
         tmp += *it;
      }
      assert( !tmp.empty() );
      VHDLParams_.emplace_back("-e " + tmp);
      verilogParams_.emplace_back("-s " + tmp);
   }

   if( !simulate_.empty() ) {
      checkErrors( simulate_ );
      std::string tmp;
      for( auto it = elaborate_.begin(); it != elaborate_.end(); it++ ) {
         tmp += *it;
      }
      assert( !tmp.empty() );
      VHDLParams_.emplace_back("-e " + tmp);
      verilogParams_.emplace_back("-s " + tmp);
      verilogParams_.emplace_back("-tvvp");
   }
}

void
ArgumentParser::checkErrors( std::vector<std::string>& toCheck ) {
   if( toCheck.size() <= 1 )
      return;
   for (unsigned i = 1; i < toCheck.size(); i++) {
      if( isExtension( toCheck[i], ArgumentParser::vhdlexts_ ) ) {
         verilogFiles_.push_back( toCheck[i] );
         toCheck.pop_back();
         continue;
      }
      if( isExtension( toCheck[i], ArgumentParser::verilogexts_ ) ) {
         verilogFiles_.push_back( toCheck[i] );
         toCheck.pop_back();
         continue;
      }
   }
   assert( toCheck.size() <= 2 );
}

bool
ArgumentParser::checkFiles( std::vector<std::string>& files ) {
   for( auto it = files.begin(); it != files.end(); ) {
      if( isExtension( *it, ArgumentParser::vhdlexts_ ) ) {
         VHDLFiles_.push_back( std::move(*it) );
         it = files.erase(it);
      } else
         it++;
   }

   for( auto it = files.begin(); it != files.end(); ) {
      if( isExtension( *it, ArgumentParser::verilogexts_ ) ) {
         verilogFiles_.push_back( std::move(*it) );
         it = files.erase(it);
      } else
         it++;
   }

   if( files.size() > 0 ) {
      std::cerr << "There is at least one parameter which is not a Verilog or VHDL file." << std::endl
         << "Please, make sure that the extensions are correct:" << std::endl
         << ".v for Verilog files and either .vhdl or .vhd for VHDL files." << std::endl;
      return false;
   }

   assert( files.size() == 0 );
   return true;
}

std::vector<std::string>&
ArgumentParser::getVerilogParams() {
   return verilogParams_;
}

std::vector<std::string>&
ArgumentParser::getVerilogFiles() {
   return verilogFiles_;
}

std::vector<std::string>&
ArgumentParser::getVHDLParams() {
   return VHDLParams_;
}

std::vector<std::string>&
ArgumentParser::getVHDLFiles() {
   return VHDLFiles_;
}

