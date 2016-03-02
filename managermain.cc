#include <cstdlib>
#include <iostream>
#include "compiler.h"
#include "manager.h"
#include "IcarusCompiler.hpp"
#include "ArgumentParser.hpp"
#include <boost/scoped_ptr.hpp>

bool debug_elaborate;
bool warn_ob_select = false;
bool verbose_flag = false;
unsigned integer_width = 32;
unsigned width_cap = 65536;
char* depfile_name = NULL;
char depfile_mode = 'a';
bool warn_implicit  = false;
bool warn_implicit_dimensions = false;
bool warn_timescale = false;
bool warn_portbinding = false;
bool warn_inf_loop = false;
bool warn_sens_entire_vec = false;
bool warn_sens_entire_arr = false;
bool warn_anachronisms = false;
bool warn_floating_nets = false;
char*ivlpp_string = 0;
list<perm_string> roots;
list<const char*> library_suff;
int def_ts_prec = 0;
int def_ts_units = 0;
/* Dependency file output mode */
char dep_mode = 'a';
/* Path to vhdlpp */
char *vhdlpp_path = 0;
/* vhdlpp work directory */
char *vhdlpp_work = 0;

char**vhdlpp_libdir = 0;
unsigned vhdlpp_libdir_cnt = 0;


char**include_dir = 0;
unsigned include_cnt = 0;

int relative_include = 0;

int line_direct_flag = 0;

extern unsigned error_count;
FILE *depend_file = NULL;

/*
 * For some generations we allow a system function to be called
 * as a task and only print a warning message. The default for
 * this is that it is a run time error.
 */
ivl_sfunc_as_task_t def_sfunc_as_task = IVL_SFUNC_AS_TASK_ERROR;

/*
 * Optimization control flags.
 */
unsigned opt_const_func = 0;

/*
 * Are we doing synthesis?
 */
bool synthesis = false;

/*
 * Miscellaneous flags.
 */
bool disable_virtual_pins = false;
unsigned long array_size_limit = 16777216;  // Minimum required by IEEE-1364?
unsigned recursive_mod_limit = 10;
bool disable_concatz_generation = false;

map<perm_string,unsigned> missing_modules;
map<perm_string,bool> library_file_map;

/*
 * Debug message class flags.
 */
bool debug_scopes = false;
bool debug_eval_tree = false;
bool debug_emit = false;
bool debug_synth2 = false;
bool debug_optimizer = false;

/*
 * Keep a heap of identifier strings that I encounter. This is a more
 * efficient way to allocate those strings.
 */
StringHeapLex lex_strings;
StringHeapLex filename_strings;
StringHeapLex bits_strings;

/*
 * These are the language support control flags. These support which
 * language features (the generation) to support. The generation_flag
 * is a major mode, and the gn_* flags control specific sub-features.
 */
generation_t generation_flag = GN_VER2009;
bool gn_icarus_misc_flag = true;
bool gn_cadence_types_flag = true;
bool gn_specify_blocks_flag = true;
bool gn_assertions_flag = true;
bool gn_io_range_error_flag = true;
bool gn_strict_ca_eval_flag = false;
bool gn_strict_expr_width_flag = false;
bool gn_verilog_ams_flag = false;

int main( int argc, char*argv[] )
{
   //Compiler* vhdl = new GHDLCompiler();
   boost::scoped_ptr<Compiler> verilog(new IcarusCompiler());
   ArgumentParser ap;
   switch( ap.vectorifyArguments( argc, argv ) ) {
      case CONTINUE_OK:
         break;
      case EXIT_OK:
         // Pass flags to compilers and exit.
         // I expect compilers to print something and do no computation at all
         verilog->processParams( ap.getVerilogParams() );
         //vhdl.processParam(ap.getVHDLParams());
         return EXIT_SUCCESS;
         break;
      case ERROR:
         // An error message has been already printed, just exit.
         return EXIT_FAILURE;
      default:
         std::cerr << "Something bad happened, you should never see this message." << std::endl;
         return EXIT_FAILURE;
   }

   Manager dumbledore;

   if( ap.getVHDLFiles().size() ) {
      //vhdl.add_files( ap.getVHDLFiles() );
      //vhdl.processParam( ap.getVHDLParams() );
      //dumbledore.add_instance( Compiler::Type::VHDL, vhdl );
   }

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
