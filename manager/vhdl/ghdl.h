extern "C" void mhdlsim_vhdl_init (void);

/* Add option OPT[0 .. LEN - 1].  Return != 0 in case of error.  */
extern "C" int mhdlsim_vhdl_process_param (const char *opt, int len);

/* Call once before analysis and after processing all params.  */
extern "C" void mhdlsim_vhdl_analyze_init();

/* Analyze FILE[0 .. LEN - 1].  Return != 0 in case of error.  */
extern "C" int mhdlsim_vhdl_analyze_file (const char *file, int len);

/* Return True if the top unit (set by "-e UNIT" option) is known by vhdl.  */
extern "C" int mhdlsim_vhdl_known_top_unit (void);

extern "C" void mhdlsim_vhdl_elaborate();

extern "C" void mhdlsim_vhdl_run();
