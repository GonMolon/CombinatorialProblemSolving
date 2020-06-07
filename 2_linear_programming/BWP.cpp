#include <ilcplex/ilocplex.h>
ILOSTLBEGIN

#include <vector>
#include <iostream>
#include <list>
#include "../include/BWPInstance.hh"


#define width(i) (dir[i] * instance.boxes[i].first + (1 - dir[i]) * instance.boxes[i].second)
#define height(i) (dir[i] * instance.boxes[i].second + (1 - dir[i]) * instance.boxes[i].first)

IloIntVar L;
IloIntVarArray pos_x;
IloIntVarArray pos_y;
IloBoolVarArray dir;
list<IloOr> overlapping_constraints;

IloModel create_model(IloEnv env, BWPInstance instance) {
    IloModel model = IloModel(env);

    L = IloIntVar(env, instance.L_lower_bound, instance.L_upper_bound);
    pos_x = IloIntVarArray(env, instance.N);
    pos_y = IloIntVarArray(env, instance.N);
    dir = IloBoolVarArray(env, instance.N);

    for(int i = 0; i < instance.N; ++i) {
        pos_x[i] = IloIntVar(env, 0, instance.W - instance.boxes[i].first);
        pos_y[i] = IloIntVar(env, 0, instance.L_upper_bound - instance.boxes[i].first);
        pos_x[i].setName(("pos_x[" + to_string(i) + "]").c_str());
        pos_y[i].setName(("pos_y[" + to_string(i) + "]").c_str());
        dir[i].setName(("dir[" + to_string(i) + "]").c_str());

        model.add(pos_x[i] <= instance.W - width(i));
        if(instance.boxes[i].first == instance.boxes[i].second || instance.boxes[i].second > instance.W) {
            model.add(dir[i]);
        }
    }

    for(int i = 0; i < instance.N; ++i) {
        model.add(L >= pos_y[i] + height(i));

        for(int j = i + 1; j < instance.N; ++j) {
            IloOr constraint = 
                (pos_x[j] >= pos_x[i] + width(i)) || 
                (pos_x[j] + width(j) <= pos_x[i]) || 
                (pos_y[j] >= pos_y[i] + height(i)) || 
                (pos_y[j] + height(j) <= pos_y[i]);
            model.add(
                constraint
            );
            overlapping_constraints.push_back(constraint);
        }
    }

    model.add(IloMinimize(env, L));

    return model;
}


void print_solution(IloCplex cplex, BWPInstance instance) {
    cout << cplex.getObjValue() << endl;
    for(int i = 0; i < instance.N; ++i) {
        IloInt x = cplex.getValue(pos_x[i]);
        IloInt y = cplex.getValue(pos_y[i]);
        IloBool d = cplex.getValue(dir[i]);
        cout << x << " " << y << " " 
        << x + d * instance.boxes[i].first + (1 - d) * instance.boxes[i].second - 1 << " " 
        << y + d * instance.boxes[i].second + (1 - d) * instance.boxes[i].first - 1 << endl;
    }
    for(IloOr constraint : overlapping_constraints) {
        if(not cplex.getValue(constraint)) {
            cerr << "Constraint:" << endl;
            cerr << constraint << endl;
            cerr << "is not satisfied in the found solution!!!!!!" << endl;
        }
    }
}


int main() {
    BWPInstance instance;
    instance.read();

    IloEnv env;
    IloModel model = create_model(env, instance);

    IloCplex cplex(model);
    cplex.setOut(env.getNullStream());
    bool solution_found = cplex.solve();
    if(!solution_found) {
        cerr << "No solution found" << endl;
        exit(1);
    }
    cerr << cplex.getCplexStatus() << endl;
    print_solution(cplex, instance);
    env.end();    
}
