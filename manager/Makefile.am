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
#
#---------------------------------------------------------------------------

noinst_LTLIBRARIES = libinterface.la

libinterface_la_CPPFLAGS = \
	-I$(top_builddir)/vvp \
	-I$(top_srcdir)/vvp \
	-I$(top_srcdir) \
	-I$(top_srcdir)/ivlpp \
	-I$(top_srcdir)/driver \
	-I$(top_srcdir)/libmisc

libinterface_la_SOURCES = \
   verilog/IcarusCompiler.cpp \
   verilog/IcarusSimulator.cpp \
   verilog/IcarusElaborator.cpp \
   verilog/IcarusHandler.cpp \
   verilog/IcarusAnalyzer.cpp \
   manager.cpp

libinterface_la_LIBADD = \
	$(top_builddir)/ivlpp/libivlpp.la \
	$(top_builddir)/vvp/libvvp.la \
	$(top_builddir)/vpi/libivpi.la \
	$(top_builddir)/vvp/libvvp.la
