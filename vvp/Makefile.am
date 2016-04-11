# Copyright (c) 2016 CERN
# @author Michele Castellana <michele.castellana@cern.ch>
#
# This source code is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This source code is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this code.  If not, see <http://www.gnu.org/licenses/>.
#
#---------------------------------------------------------------------------

ACLOCAL_AMFLAGS = -I m4

bin_PROGRAMS = draw_tt.exe

draw_tt_exe_SOURCES = \
	draw_tt.c

noinst_LTLIBRARIES = libvvp.la

libvvp_la_CPPFLAGS = \
	-I$(top_srcdir)/vvp \
	-I$(top_srcdir)

libvvp_la_SOURCES = \
	arith.cc \
	array.cc \
	array_common.cc \
	bufif.cc \
	class_type.cc \
	codes.cc \
	compile.cc \
	concat.cc \
	delay.cc \
	dff.cc \
	enum_type.cc \
	event.cc \
	extend.cc \
	file_line.cc \
	island_tran.cc \
	lexor.cc \
	libvpi.c \
	logic.cc \
	npmos.cc \
	parse.cc \
	parse_misc.cc \
	part.cc \
	permaheap.cc \
	reduce.cc \
	resolv.cc \
	schedule.cc \
	sfunc.cc \
	statistics.cc \
	stop.cc \
	substitute.cc \
	symbols.cc \
	tables.cc \
	udp.cc \
	ufunc.cc \
	vpi_callback.cc \
	vpi_cobject.cc \
	vpi_const.cc \
	vpi_darray.cc \
	vpi_event.cc \
	vpi_iter.cc \
	vpi_mcd.cc \
	vpi_modules.cc \
	vpip_bin.cc \
	vpip_format.cc \
	vpip_hex.cc \
	vpip_oct.cc \
	vpi_priv.cc \
	vpip_to_dec.cc \
	vpi_real.cc \
	vpi_scope.cc \
	vpi_signal.cc \
	vpi_string.cc \
	vpi_tasks.cc \
	vpi_time.cc \
	vpi_vthr_vector.cc \
	vthread.cc \
	vvp_cobject.cc \
	vvp_darray.cc \
	vvp_island.cc \
	vvp_net.cc \
	vvp_net_sig.cc \
	vvp_object.cc \
	vvp_vpi.cc \
	words.cc

tables.cc: draw_tt.exe
	./draw_tt.exe > tables.cc

parse.h: parse.cc
	mv parse.cc.h $@ 2>/dev/null || mv parse.hh $@

lexor.cc: parse.h $(srcdir)/lexor.lex
	$(LEX) -s -olexor.cc $(srcdir)/lexor.lex

parse.cc: $(srcdir)/parse.y
	$(YACC) --verbose -t -d -o $@ $<
