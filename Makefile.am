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

SUBDIRS = ivlpp vpi vvp manager tgt-vvp

noinst_LTLIBRARIES = libicarus.la

libicarus_la_CPPFLAGS = \
	-I$(top_srcdir) \
	-I$(top_srcdir)/libmisc

libicarus_la_SOURCES = \
	parse_misc.cc \
	async.cc \
	design_dump.cc \
	discipline.cc \
	dup_expr.cc \
	elaborate.cc \
	elab_expr.cc \
	elaborate_analog.cc \
	elab_lval.cc \
	elab_net.cc \
	elab_scope.cc \
	elab_sig.cc \
	elab_sig_analog.cc \
	elab_type.cc \
	libmisc/LineInfo.cc  \
	libmisc/StringHeap.cc \
	acc_user.h \
	link_const.cc \
	net_assign.cc \
	net_nex_output.cc \
	pform_string_type.cc \
	target.cc \
	HName.cc \
	load_module.cc \
	netclass.cc \
	netparray.cc \
	parse_misc.cc \
	pform_struct_type.cc \
	PUdp.cc \
	pform_types.cc \
	t-dll-analog.cc \
	AStatement.cc \
	netdarray.cc \
	net_proc.cc \
	PWire.cc \
	t-dll-api.cc \
	verinum.cc \
	netqueue.cc \
	PClass.cc \
	PFunction.cc \
	t-dll.cc \
	net_design.cc \
	PGate.cc \
	t-dll-expr.cc \
	verireal.cc \
	Attrib.cc \
	netenum.cc \
	netscalar.cc \
	PDelays.cc \
	PGenerate.cc \
	t-dll-proc.cc \
	cprop.cc \
	net_event.cc \
	net_scope.cc \
	PEvent.cc \
	net_expr.cc \
	netstruct.cc \
	Statement.cc \
	version.c \
	net_func.cc \
	PExpr.cc \
	PModport.cc \
	emit.cc \
	net_func_eval.cc \
	net_tran.cc \
	eval_attrib.cc \
	net_link.cc \
	nettypes.cc \
	pform_analog.cc \
	PPackage.cc \
	eval.cc \
	netlist.cc \
	pform.cc \
	eval_tree.cc \
	net_udp.cc \
	pform_class_type.cc \
	symbol_search.cc \
	netvector.cc \
	pform_disciplines.cc \
	PScope.cc \
	sync.cc \
	dosify.c \
	expr_synth.cc \
	Module.cc \
	netmisc.cc \
	pform_dump.cc \
	nodangle.cc \
	PSpec.cc \
	synth2.cc \
	functor.cc \
	net_modulo.cc \
	pform_package.cc \
	synth.cc \
	net_analog.cc \
	net_nex_input.cc \
	pad_to_width.cc \
	pform_pclass.cc \
	PTask.cc \
	lexor.cc \
	lexor_keyword.cc \
	parse.h \
	parse.cc \
	sys_funcs.cc

parse.cc: $(srcdir)/parse.y
	$(YACC) --verbose -t -p VL -d -o $@ $<

parse.h: parse.cc
	mv parse.cc.h $@ 2>/dev/null || mv parse.hh $@

lexor.cc: $(srcdir)/lexor.lex parse.h
	$(LEX) -s -t $< > $@

lexor_keyword.cc: $(srcdir)/lexor_keyword.gperf parse.h
	gperf -o -i 7 -C -k 1-4,6,9,$$ -H keyword_hash -N check_identifier -t $(srcdir)/lexor_keyword.gperf > lexor_keyword.cc || (rm -f lexor_keyword.cc ; false)

bin_PROGRAMS = mixedmanager

mixedmanager_CPPFLAGS = \
   -I$(top_srcdir)/manager \
   -I$(top_srcdir)/libmisc \
   -I$(top_srcdir)/manager/verilog \
   -I$(top_srcdir)/manager/vhdl

mixedmanager_SOURCES = \
	managermain.cc \
	manager/ArgumentParser.cpp

mixedmanager_LDADD = \
	$(top_builddir)/vvp/libvvp.la \
	$(top_builddir)/manager/libinterface.la \
	$(top_builddir)/libicarus.la \
	$(top_builddir)/tgt-vvp/libvvptarget.la \
	-lboost_program_options \
	-ldl
