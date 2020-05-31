#include <gecode/int.hh>
#include <gecode/minimodel.hh>
#include <gecode/search.hh>

#include <vector>
#include <iostream>

#include "../include/BWPInstance.hh"

using namespace Gecode;
using namespace std;


class BWP : public Space {

public:
    BWPInstance instance;

    IntVar L;
    IntVarArray pos;
    IntVarArray dir;
    BoolVarArray overlap_top;
    BoolVarArray overlap_bottom;
    BoolVarArray overlap_left;
    BoolVarArray overlap_right;
    
    BWP(BWPInstance instance) :
        L(*this, instance.L_lower_bound, instance.L_upper_bound),
        pos(*this, instance.N), 
        dir(*this, instance.N, 0, 1),
        overlap_top(*this, instance.N * (instance.N - 1) / 2, 0, 1),
        overlap_bottom(*this, instance.N * (instance.N - 1) / 2, 0, 1),
        overlap_left(*this, instance.N * (instance.N - 1) / 2, 0, 1),
        overlap_right(*this, instance.N * (instance.N - 1) / 2, 0, 1) {

        this->instance = instance;

        vector<LinIntExpr> width(instance.N);
        vector<LinIntExpr> height(instance.N);
        IntVarArgs all_variables(instance.N * 2);

        cerr << "L: " << L << endl;

        for(int i = 0; i < instance.N; ++i) {
            width[i] = dir[i] * instance.boxes[i].first + (1 - dir[i]) * instance.boxes[i].second;
            height[i] = dir[i] * instance.boxes[i].second + (1 - dir[i]) * instance.boxes[i].first;

            pos[i] = IntVar(*this, 0, instance.W * (instance.L_upper_bound - instance.boxes[i].first + 1) - 1);

            rel(*this, pos[i] % instance.W <= instance.W - width[i]);
            if(instance.boxes[i].first == instance.boxes[i].second || instance.boxes[i].second > instance.W) {
                rel(*this, dir[i] == 1);
            }

            all_variables[i] = pos[i];
            all_variables[instance.N + i] = dir[i];
        }

        int index = 0;
        for(int i = 0; i < instance.N; ++i) {
            rel(*this, L >= pos[i] / instance.W + height[i]);

            for(int j = i + 1; j < instance.N; ++j) {

                BoolVar top(overlap_top[index]);
                BoolVar bottom(overlap_bottom[index]);
                BoolVar left(overlap_left[index]);
                BoolVar right(overlap_right[index]);
                ++index;

                rel(*this, pos[j] % instance.W + right * instance.W                 >= pos[i] % instance.W + width[i]);
                rel(*this, pos[j] % instance.W + width[j]                           <= pos[i] % instance.W + left * instance.W);
                rel(*this, pos[j] / instance.W + bottom * instance.L_upper_bound    >= pos[i] / instance.W + height[i]);
                rel(*this, pos[j] / instance.W + height[j]                          <= pos[i] / instance.W + top * instance.L_upper_bound);
                rel(*this, top + bottom + left + right <= 3);
            }
        }


        auto weight_func = [](const Space& _home, IntVar var, int i) {
            const BWP& home = static_cast<const BWP&>(_home);
            if(i < home.instance.N) {
                return home.instance.boxes[i].first * home.instance.boxes[i].second;
            } else {
                return 0;
            }
        };
        branch(*this, all_variables, INT_VAR_MERIT_MAX(weight_func), INT_VAL_MIN());
    }

    virtual void constrain(const Space& _prev) {
        const BWP& prev = static_cast<const BWP&>(_prev);
        rel(*this, L < prev.L.min());
    }

    BWP(BWP& other) : Space(other) {
        instance = other.instance;
        L.update(*this, other.L);
        pos.update(*this, other.pos);
        dir.update(*this, other.dir);
        overlap_top.update(*this, other.overlap_top);
        overlap_bottom.update(*this, other.overlap_bottom);
        overlap_left.update(*this, other.overlap_left);
        overlap_right.update(*this, other.overlap_right);
    }

    virtual Space* copy() {
        return new BWP(*this);
    }

    void print() const {
        print(cout);
    }

    void print(ostream& stream) const {
        stream << L.min() << endl;
        for(int i = 0; i < instance.N; ++i) {
            stream << pos[i].val() % instance.W << " " << pos[i].val() / instance.W << " " << 
            pos[i].val() % instance.W + dir[i].val() * instance.boxes[i].first + (1 - dir[i].val()) * instance.boxes[i].second - 1 << " " <<
            pos[i].val() / instance.W + dir[i].val() * instance.boxes[i].second + (1 - dir[i].val()) * instance.boxes[i].first - 1 << endl;
        }
    }
};


int main() {
    BWPInstance instance;
    instance.read();

    BWP* model = new BWP(instance);
    BAB<BWP> engine(model);
    delete model;

    BWP* best_solution = NULL;
    while(BWP* solution = engine.next()) {
        if(best_solution != NULL) {
            delete best_solution;
        }
        best_solution = solution;
        cerr << "Solution found: " << solution->L.min() << endl;
        solution->print(cerr);
    }
    best_solution->print();
    delete best_solution;
    return 0;
}
