#include <gecode/int.hh>
#include <gecode/minimodel.hh>
#include <gecode/search.hh>

#include <list>
#include <vector>
#include <iostream>

using namespace Gecode;
using namespace std;

struct BWPInstance {

    int W;
    int N;
    int L_bound;
    vector<pair<int, int>> boxes;

    BWPInstance() {
        W = 0;
        N = 0;
        L_bound = 0;
    }

    void read() {
        cin >> W;
        cin >> N;
        cout << W << " " << N << endl;
        int count;
        int y;
        int x;
        while(cin >> count) {
            cin >> x;
            cin >> y;
            cout << count << " " << x << " " << y << endl;
            for(int j = 0; j < count; ++j) {
                boxes.push_back({x, y});
                L_bound += y;
            }
        }
    }
};


class BWP : public Space {

protected:

    BWPInstance instance;

    IntVar L;
    IntVarArray pos_x;
    IntVarArray pos_y;
    IntVarArray dir;
    BoolVarArray overlap_top;
    BoolVarArray overlap_bottom;
    BoolVarArray overlap_left;
    BoolVarArray overlap_right;

public:
    
    BWP(BWPInstance instance) :
        L(*this, 1, instance.L_bound + 10000),
        pos_x(*this, instance.N), 
        pos_y(*this, instance.N),
        dir(*this, instance.N, 0, 1),
        overlap_top(*this, instance.N * (instance.N - 1) / 2, 0, 1),
        overlap_bottom(*this, instance.N * (instance.N - 1) / 2, 0, 1),
        overlap_left(*this, instance.N * (instance.N - 1) / 2, 0, 1),
        overlap_right(*this, instance.N * (instance.N - 1) / 2, 0, 1) {

        this->instance = instance;

        for(int i = 0; i < instance.N; ++i) {
            pos_x[i] = IntVar(*this, 0, instance.W - instance.boxes[i].second);
            pos_y[i] = IntVar(*this, 0, instance.L_bound - instance.boxes[i].second);
        }

        int index = 0;
        for(int i = 0; i < instance.N; ++i) {
            auto i_width = dir[i] * instance.boxes[i].first + (1 - dir[i]) * instance.boxes[i].second;
            auto i_height = dir[i] * instance.boxes[i].second + (1 - dir[i]) * instance.boxes[i].first;
            rel(*this, L >= pos_y[i] + i_height);

            for(int j = i + 1; j < instance.N; ++j) {
                auto j_width = dir[j] * instance.boxes[j].first + (1 - dir[j]) * instance.boxes[j].second;
                auto j_height = dir[j] * instance.boxes[j].second + (1 - dir[j]) * instance.boxes[j].first;

                BoolVar top(overlap_top[index]);
                BoolVar bottom(overlap_bottom[index]);
                BoolVar left(overlap_left[index]);
                BoolVar right(overlap_right[index]);
                ++index;

                rel(*this, pos_x[j] + right * instance.W        >= pos_x[i] + i_width);
                rel(*this, pos_x[j] + j_width                   <= pos_x[i] + left * instance.W);
                rel(*this, pos_y[j] + bottom * instance.L_bound >= pos_y[i] + i_height);
                rel(*this, pos_y[j] + j_height                  <= pos_y[i] + top * instance.L_bound);
                rel(*this, top + bottom + left + right <= 3);
            }
        }
        IntVarArgs variables(instance.N * 3);
        for(int i = 0; i < instance.N; ++i) {
            variables[i] = pos_x[i];
            variables[instance.N + i] = pos_y[i];
            variables[instance.N*2 + i] = dir[i];
        }
        branch(*this, variables, INT_VAR_DEGREE_MAX(), INT_VAL_MAX());
    }

    virtual void constrain(const Space& _prev) {
        const BWP& prev = static_cast<const BWP&>(_prev);
        rel(*this, L < prev.L.min());
    }

    BWP(BWP& other) : Space(other) {
        instance = other.instance;
        L.update(*this, other.L);
        pos_x.update(*this, other.pos_x);
        pos_y.update(*this, other.pos_y);
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
        cout << L.min() << endl;
        for(int i = 0; i < instance.N; ++i) {
            cout << pos_x[i] << " " << pos_y[i] << " " << 
            pos_x[i].val() + dir[i].val() * instance.boxes[i].first + (1 - dir[i].val()) * instance.boxes[i].second - 1 << " " <<
            pos_y[i].val() + dir[i].val() * instance.boxes[i].second + (1 - dir[i].val()) * instance.boxes[i].first - 1 << endl;
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
        // cout << "Solution found:" << endl;
        // solution->print();
    }
    best_solution->print();
    delete best_solution;
    return 0;
}
