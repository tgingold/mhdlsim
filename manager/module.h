#ifndef MODULE_H
#define MODULE_H

/*
 * Copyright (c) 2016 CERN
 * @author Maciej Suminski <maciej.suminski@cern.ch>
 * @author Michele Castellana <michele.castellana@cern.ch>
 *
 * This source code is free software; you can redistribute it
 * and/or modify it in source code form under the terms of the GNU
 * General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */

#include <string>
#include <deque>
#include <map>
#include <cassert>

class Net;

class Port {
   public:
      enum direction_t { IN, OUT, INOUT };
      Port(const std::string& name, direction_t dir, int width) :
         name_(name),
         dir_(dir),
         width_(width) {};
      virtual ~Port() {};

      const std::string& name() const { return name_; }
      direction_t direction() const { return dir_; }
      int width() const { return width_; }

   private:
      const std::string name_;
      const direction_t dir_;
      const int width_;

};

class ModuleInterface {
   public:
      ModuleInterface(const std::string& name, const std::deque<Port>& ports) :
         name_(name),
         ports_(ports) {};
      virtual ~ModuleInterface() {};

      const std::string& name() const { return name_; }
      const std::deque<Port>& ports() const { return ports_; }
      const Port& port(int idx) const { assert(idx < ports_.size()); return ports_[idx]; }
      // TODO get_default_generic/param(const std::string& ?)const

      // TODO accessors for variables? constants? internal signals?

   private:
      ///> Name of the component/module.
      const std::string name_;

      ///> Interconnecting ports.
      const std::deque<Port> ports_;

};

class ModuleSpec : public ModuleInterface {
   public:
      ModuleSpec(const ModuleInterface& iface) :
         ModuleInterface(iface) {};
      virtual ~ModuleSpec() {};

      // TODO get_generic/param
      // TODO set_generic/param

   private:
};

class ModuleInstance {
   public:
      ModuleInstance(const std::string& name, const ModuleSpec* iface) :
         name_(name),
         iface_(iface) {};
      virtual ~ModuleInstance() {};

      const std::string& name() const { return name_; }
      const ModuleInterface* iface() const { return iface_; }

      // TODO pick either of the following, the first one may be better
      // as it is possible to assign port using named expressions, like
      // test_module(.clk(clk), .data(data))
      //
      //const Net& net(const Port& port);
      //void connect(const Port&, Net& net);
      //
      //const Net& net(int idx);
      //connect(int idx, Net& net);

      // TODO get_generic/param

   private:
      ///> Name of the instance.
      const std::string name_;

      ///> Associated component/module interface.
      const ModuleSpec* iface_;

      // TODO this could be also map<int, Net>
      // TODO if we keep such map, then we have to define comparison functions
      // for Port class
      std::map<const Port, Net*> connections_;

      // TODO const std::map<???> generics_; // aka parameters

};

#endif /* MODULE_H */
