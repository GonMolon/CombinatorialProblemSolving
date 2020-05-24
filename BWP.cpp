#include <gecode/int.hh>
#include <gecode/minimodel.hh>
#include <gecode/search.hh>

#include <list>
#include <vector>
#include <iostream>

using namespace Gecode;
using namespace std;

struct BWPDescription {

    int W;
    int N;
    int L_bound;
    vector<pair<int, int>> boxes;

    BWPDescription() {
        W = 0;
        N = 0;
        L_bound = 0;
    }

    void read() {
        cin >> W;
        cin >> N;
        cout << W << " " << W << endl;
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
    IntVar L;
    IntVarArray pos_x;
    IntVarArray pos_y;

public:
    
    BWP(BWPDescription description) 
      : L(*this, 0, description.L_bound), 
        pos_x(*this, description.boxes.size(), 0, description.W), 
        pos_y(*this, description.boxes.size(), 0, description.L_bound) {

    }

    virtual Space* copy() {
        return new BWP(*this);
    }

    void print() const {
        cout << L << endl;
        for(int i = 0; i < pos_x.size(); ++i) {
            cout << pos_x[i] << " " << pos_y[i] << endl;
        }
    }
};


int main() {
    BWPDescription description;
    description.read();

    BWP* model = new BWP(description);
    BAB<BWP> engine(model);
    delete model;

    BWP* best_solution = NULL;
    while(BWP* solution = engine.next()) {
        if(best_solution != NULL) {
            delete best_solution;
        }
        best_solution = solution;
        // cout << "Solution found" << endl;
        // solution->print(); 
    }
    best_solution->print();
    delete best_solution;
    return 0;
}
