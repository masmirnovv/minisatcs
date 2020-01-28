#include "minisat/core/Solver.h"

#include <signal.h>

#include <algorithm>
#include <cstring>
#include <vector>

class WrappedMinisatSolver : public Minisat::Solver {
    class ScopedSolverAssign {
        WrappedMinisatSolver** dst;

    public:
        explicit ScopedSolverAssign(WrappedMinisatSolver** dst,
                                    WrappedMinisatSolver* src) {
            *dst = src;
        }
        ~ScopedSolverAssign() { *dst = nullptr; }
    };
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

    void new_clause_add_lit(int lit) { add_tmp.push(make_lit(lit)); }

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

    void set_var_preference(int x, int p) {
        setVarPreference(std::abs(x) - 1, p);
    }

    void set_var_name(int x, const char* name) {
        var_names[std::abs(x) - 1] = name;
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

    bool solve_with_signal(bool setup) {
        static WrappedMinisatSolver* g_solver = nullptr;
        static auto on_sig = [](int) {
            if (g_solver) {
                g_solver->interrupt();
            }
        };
        clearInterrupt();
        struct sigaction old_action;
        ScopedSolverAssign g_solver_assign{&g_solver, this};
        if (setup) {
            struct sigaction act;
            memset(&act, 0, sizeof(act));
            act.sa_handler = on_sig;
            if (sigaction(SIGINT, &act, &old_action)) {
                char msg[1024];
                snprintf(msg, sizeof(msg), "failed to setup signal handler: %s",
                         strerror(errno));
                throw std::runtime_error{msg};
            }
        }
        budgetOff();
        assumptions.clear();
        auto ret = solve_();
        if (sigaction(SIGINT, &old_action, nullptr)) {
            char msg[1024];
            snprintf(msg, sizeof(msg), "failed to restore signal handler: %s",
                     strerror(errno));
            throw std::runtime_error{msg};
        }

        if (ret.is_not_undef()) {
            return ret == l_True;
        }
        throw std::runtime_error("computation interrupted");
    }
};

