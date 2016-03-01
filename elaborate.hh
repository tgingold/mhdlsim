# include "Module.h"

extern bool debug_scopes;

struct pack_elem {
      PPackage*pack;
      NetScope*scope;
};

struct root_elem {
      Module *mod;
      NetScope *scope;
};

class elaborate_package_t : public elaborator_work_item_t {
    public:
      elaborate_package_t(Design*d, NetScope*scope, PPackage*p)
      : elaborator_work_item_t(d), scope_(scope), package_(p)
      { }

      ~elaborate_package_t() { }

      virtual void elaborate_runrun()
      {
	    if (! package_->elaborate_scope(des, scope_))
		  des->errors += 1;
      }

    private:
      NetScope*scope_;
      PPackage*package_;
};

class elaborate_root_scope_t : public elaborator_work_item_t {
    public:
      elaborate_root_scope_t(Design*des__, NetScope*scope, Module*rmod)
      : elaborator_work_item_t(des__), scope_(scope), rmod_(rmod)
      { }

      ~elaborate_root_scope_t() { }

      virtual void elaborate_runrun()
      {
	    Module::replace_t root_repl;
	    for (list<Module::named_expr_t>::iterator cur = Module::user_defparms.begin()
		       ; cur != Module::user_defparms.end() ; ++ cur ) {

		  pform_name_t tmp_name = cur->first;
		  if (peek_head_name(tmp_name) != scope_->basename())
			continue;

		  tmp_name.pop_front();
		  if (tmp_name.size() != 1)
			continue;

		  root_repl[peek_head_name(tmp_name)] = cur->second;
	    }

	    if (! rmod_->elaborate_scope(des, scope_, root_repl))
		  des->errors += 1;
      }

    private:
      NetScope*scope_;
      Module*rmod_;
};

class top_defparams : public elaborator_work_item_t {
    public:
      explicit top_defparams(Design*des__)
      : elaborator_work_item_t(des__)
      { }

      ~top_defparams() { }

      virtual void elaborate_runrun()
      {
	    if (debug_scopes) {
		  cerr << "debug: top_defparams::elaborate_runrun()" << endl;
	    }
	      // This method recurses through the scopes, looking for
	      // defparam assignments to apply to the parameters in the
	      // various scopes. This needs to be done after all the scopes
	      // and basic parameters are taken care of because the defparam
	      // can assign to a parameter declared *after* it.
	    des->run_defparams();

	      // At this point, all parameter overrides are done. Scan the
	      // scopes and evaluate the parameters all the way down to
	      // constants.
	    des->evaluate_parameters();

	    if (debug_scopes) {
		  cerr << "debug: top_defparams::elaborate_runrun() done" << endl;
	    }
      }
};

class later_defparams : public elaborator_work_item_t {
    public:
      explicit later_defparams(Design*des__)
      : elaborator_work_item_t(des__)
      { }

      ~later_defparams() { }

      virtual void elaborate_runrun()
      {
	    if (debug_scopes) {
		  cerr << "debug: later_defparams::elaborate_runrun()" << endl;
	    }

	    list<NetScope*>tmp_list;
	    for (set<NetScope*>::iterator cur = des->defparams_later.begin()
		       ; cur != des->defparams_later.end() ; ++ cur )
		  tmp_list.push_back(*cur);

	    des->defparams_later.clear();

	    while (! tmp_list.empty()) {
		  NetScope*cur = tmp_list.front();
		  tmp_list.pop_front();
		  cur->run_defparams_later(des);
	    }

	      // The overridden parameters will be evaluated later in
	      // a top_defparams work item.

	    if (debug_scopes) {
		  cerr << "debuf: later_defparams::elaborate_runrun() done" << endl;
	    }
      }
};
