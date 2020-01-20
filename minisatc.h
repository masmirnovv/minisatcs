#include "minisat/core/Solver.h"
#include <vector>
#include <algorithm>

class WrappedMinisatSolver: public Minisat::Solver {
    int m_new_clause_max_var = 0;

    void add_vars() {
        int tgt_nvar = m_new_clause_max_var;
        while (nVars() < tgt_nvar) {
            newVar();
        }
        m_new_clause_max_var = 0;
    }

    Minisat::Lit make_lit(int lit) {
        assert(lit != 0);
        int lv = std::abs(lit);
        m_new_clause_max_var = std::max(m_new_clause_max_var, lv);
        return Minisat::mkLit(lv - 1, lit < 0);
    }

public:
    void new_clause_prepare() {
        m_new_clause_max_var = 0;
        add_tmp.clear();
    }

    void new_clause_add_lit(int lit) {
        add_tmp.push(make_lit(lit));
    }

    void new_clause_commit() {
        add_vars();
        addClause_(add_tmp);
        add_tmp.clear();
    }

    void new_clause_commit_leq(int bound, int dst) {
        auto dstl = make_lit(dst);
        add_vars();
        addLeqAssign_(add_tmp, bound, dstl);
        add_tmp.clear();
    }

    void new_clause_commit_geq(int bound, int dst) {
        auto dstl = make_lit(dst);
        add_vars();
        addGeqAssign_(add_tmp, bound, dstl);
        add_tmp.clear();
    }

    std::vector<int> get_model() const {
        std::vector<int> ret;
        for (int i = 0; i < model.size(); ++i) {
            if (model[i] == l_True) {
                ret.push_back(i + 1);
            } else if (model[i] == l_False) {
                ret.push_back(-i - 1);
            }
        }
        return ret;
    }
};

