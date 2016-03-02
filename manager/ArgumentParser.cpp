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
#include <algorithm>
#include <iterator>
#include <utility>


#define MAX_INPUT_FILE_NUM -1
bool mixed_lang_enabled;

ArgumentParser::ArgumentParser(bool complainAndExitOnError)
   : complainAndExitOnError_(complainAndExitOnError),
   analyze_(false),
   version_(false),
   verbose_(false) {
      mixed_lang_enabled = false;
   }

ArgumentParser::~ArgumentParser() {}

ParsingStatus
ArgumentParser::vectorifyArguments( int argc, char **argv ) {
   std::vector<std::string> tmp_file_vec;
   namespace po = boost::program_options;
   po::options_description desc("Options");
   desc.add_options()
      ("analyze,a",  po::value<bool>(&analyze_)->implicit_value(true)->default_value(false)->zero_tokens(), "Analyze only" )
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
      return ParsingStatus::ERROR;
   }

   if( (simulate_.size() > 0) == analyze_ ) {
      std::cerr << "Extactly one (no more no less) parameter between analyze and simulate must be used" << std::endl;
      return ParsingStatus::ERROR;
   }

   if( tmp_file_vec.empty() ) {
      std::cerr << "No input files " << tmp_file_vec.size() << std::endl;
      return ParsingStatus::ERROR;
   }

   translate_parameters();

   assert( (simulate_.size() > 0) ^ analyze_ );

   if( !checkFiles(tmp_file_vec) )
      return ParsingStatus::ERROR;

   assert( getVHDLFiles().size() || getVerilogFiles().size() );

   // This check must be after the checkFiles call
   if( getVHDLFiles().size() && getVerilogFiles().size() )
      mixed_lang_enabled = true;

   if(version_) {
      std::cout << "print my_version here" << std::endl;
      return ParsingStatus::EXIT_OK;
   }

   return ParsingStatus::CONTINUE_OK;
}

bool
isVHDLExtension(const std::string& input) {
   static const std::set<std::string> vhdlexts = { std::string(".vhdl"), std::string(".vhd") };
   for(auto vhdl = vhdlexts.begin(); vhdl != vhdlexts.end(); vhdl++) {
      if( input.compare( input.length() - vhdl->length(), vhdl->length(), *vhdl ) == 0 ) {
         return true;
      }
   }
   return false;
}

bool
isVerilogExtension(const std::string& input) {
   static const std::string verilogext(".v");
   if( input.compare( input.length() - verilogext.length(), verilogext.length(), verilogext ) == 0 ) {
      return true;
   }
   return false;
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
   }
   if( verbose_ ) {
      verilogParams_.emplace_back("-v");
   }

   if( simulate_.size() > 1 ) {
      // If I have received more params, check from the second one on.
      for (unsigned i = 1; i < simulate_.size(); i++) {
         if( isVHDLExtension(simulate_[i]) ) {
            verilogFiles_.push_back( simulate_[i] );
            simulate_.pop_back();
            continue;
         }
         if( isVerilogExtension(simulate_[i]) ) {
            verilogFiles_.push_back( simulate_[i] );
            simulate_.pop_back();
            continue;
         }
      }
      VHDLParams_.emplace_back("-e");
      verilogParams_.emplace_back("-tvvp");
   }
}

bool
ArgumentParser::checkFiles( std::vector<std::string>& files ) {
   for( auto it = std::find_if(files.begin(), files.end(), isVHDLExtension);
         it != files.end(); 
         it = std::find_if(files.begin(), files.end(), isVHDLExtension) ) {
      VHDLFiles_.push_back( std::move(*it) );
      it = files.erase(it);
   }

   for( auto it = std::find_if(files.begin(), files.end(), isVerilogExtension);
         it != files.end(); 
         it = std::find_if(it, files.end(), isVerilogExtension) ) {
      verilogFiles_.push_back( std::move(*it) );
      files.erase(it);
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

