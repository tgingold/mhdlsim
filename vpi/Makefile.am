# Copyright CERN 2016 Michele Castellana <michele.castellana@cern.ch>
#
# This source code is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This source code is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this code.  If not, see <http://www.gnu.org/licenses/>.

#---------------------------------------------------------------------------

noinst_LTLIBRARIES = libivpi.la

libivpi_la_CPPFLAGS = \
	-I$(top_srcdir)/vpi \
	-I$(top_srcdir)

libivpi_la_SOURCES = \
	fastlz.c \
	fstapi.c \
	lxt2_write.c \
	lxt_write.c \
	lz4.c \
	mt19937int.c \
	sdf_parse.c \
	sdf_lexor.c \
	stringheap.c \
	sys_clog2.c \
	sys_convert.c \
	sys_countdrivers.c \
	sys_darray.c \
	sys_deposit.c \
	sys_display.c \
	sys_fileio.c \
	sys_finish.c \
	sys_fst.c \
	sys_icarus.c \
	sys_lxt2.c \
	sys_lxt.c \
	sys_plusargs.c \
	sys_priv.c \
	sys_queue.c \
	sys_random.c \
	sys_random_mti.c \
	sys_readmem.c \
	sys_readmem_lex.c \
	sys_scanf.c \
	sys_sdf.c \
	sys_table.c \
	table_mod_parse.c \
	system.sft \
	sys_time.c \
	sys_vcd.c \
	sys_vcdoff.c \
	table_mod.c \
	table_mod_lexor.c \
	v2005_math.c \
	v2009_array.c \
	v2009_enum.c \
	v2009_string.c \
	v2009_table.c \
	va_math.c \
	vams_simparam.c \
	vcd_priv2.cc \
	vcd_priv.c \
	vhdl_table.c \
	vhdl_textio.c \
	vpi_debug.c

sdf_parse.c: sdf_lexor.c $(srcdir)/sdf_parse.y
	$(YACC) --verbose -t -p sdf -d -o $@ $(srcdir)/sdf_parse.y

table_mod_parse.c: $(srcdir)/table_mod_parse.y
	$(YACC) --verbose -t -p tblmod -d -o $@ $<

sys_readmem_lex.c: $(srcdir)/sys_readmem_lex.lex
	$(LEX) -t $< > $@

sdf_lexor.c: $(srcdir)/sdf_lexor.lex
	$(LEX) -t $< > $@

table_mod_lexor.c: $(srcdir)/table_mod_lexor.lex 
	$(LEX) -t $< > $@

