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
    int L_upper_bound;
    int L_lower_bound;
    vector<pair<int, int>> boxes;

    BWPInstance() {
        W = 0;
        N = 0;
        L_upper_bound = 0;
        L_lower_bound = 0;
    }

    void read() {
        cin >> W;
        cin >> N;
        cout << W << " " << N << endl;
        int count;
        int y;
        int x;
        int total_area = 0;
        while(cin >> count) {
            cin >> x;
            cin >> y;
            cout << count << " " << x << " " << y << endl;
            for(int j = 0; j < count; ++j) {
                boxes.push_back({x, y});
                total_area += x * y;
                L_lower_bound = max(L_lower_bound, x);
                L_upper_bound += y;
            }
        }
        L_lower_bound = max(L_lower_bound, (total_area + W - 1) / W);
    }
};


class BWP : public Space {

public:
    BWPInstance instance;

    IntVar L;
    IntVarArray pos_x;
    IntVarArray pos_y;
    IntVarArray dir;
    BoolVarArray overlap_top;
    BoolVarArray overlap_bottom;
    BoolVarArray overlap_left;
    BoolVarArray overlap_right;
    
    BWP(BWPInstance instance) :
        L(*this, instance.L_lower_bound, instance.L_upper_bound),
        pos_x(*this, instance.N), 
        pos_y(*this, instance.N),
        dir(*this, instance.N, 0, 1),
        overlap_top(*this, instance.N * (instance.N - 1) / 2, 0, 1),
        overlap_bottom(*this, instance.N * (instance.N - 1) / 2, 0, 1),
        overlap_left(*this, instance.N * (instance.N - 1) / 2, 0, 1),
        overlap_right(*this, instance.N * (instance.N - 1) / 2, 0, 1) {

        this->instance = instance;

        vector<LinIntExpr> width(instance.N);
        vector<LinIntExpr> height(instance.N);
        IntVarArgs all_variables(instance.N * 3);

        cerr << "L: " << L << endl;

        for(int i = 0; i < instance.N; ++i) {
            width[i] = dir[i] * instance.boxes[i].first + (1 - dir[i]) * instance.boxes[i].second;
            height[i] = dir[i] * instance.boxes[i].second + (1 - dir[i]) * instance.boxes[i].first;

            pos_x[i] = IntVar(*this, 0, instance.W - instance.boxes[i].first);
            pos_y[i] = IntVar(*this, 0, instance.L_upper_bound - instance.boxes[i].first);

            rel(*this, pos_x[i] <= instance.W - width[i]);

            all_variables[i] = pos_x[i];
            all_variables[instance.N + i] = pos_y[i];
            all_variables[instance.N*2 + i] = dir[i];
        }

        int index = 0;
        for(int i = 0; i < instance.N; ++i) {
            rel(*this, L >= pos_y[i] + height[i]);

            for(int j = i + 1; j < instance.N; ++j) {

                BoolVar top(overlap_top[index]);
                BoolVar bottom(overlap_bottom[index]);
                BoolVar left(overlap_left[index]);
                BoolVar right(overlap_right[index]);
                ++index;

                rel(*this, pos_x[j] + right * instance.W                >= pos_x[i] + width[i]);
                rel(*this, pos_x[j] + width[j]                          <= pos_x[i] + left * instance.W);
                rel(*this, pos_y[j] + bottom * instance.L_upper_bound   >= pos_y[i] + height[i]);
                rel(*this, pos_y[j] + height[j]                         <= pos_y[i] + top * instance.L_upper_bound);
                rel(*this, top + bottom + left + right <= 3);
            }
        }

        branch(*this, all_variables, INT_VAR_RND(1), INT_VAL_RND(1));
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
        print(cout);
    }

    void print(ostream& stream) const {
        stream << L.min() << endl;
        for(int i = 0; i < instance.N; ++i) {
            stream << pos_x[i] << " " << pos_y[i] << " " << 
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
        cerr << "Solution found: " << solution->L.min() << endl;
        solution->print(cerr);
    }
    best_solution->print();
    delete best_solution;
    return 0;
}
