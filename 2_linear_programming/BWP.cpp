#include <ilcplex/ilocplex.h>
ILOSTLBEGIN

#include <vector>
#include <iostream>
#include "../include/BWPInstance.hh"


#define width(i) (dir[i] * instance.boxes[i].first + (1 - dir[i]) * instance.boxes[i].second)
#define height(i) (dir[i] * instance.boxes[i].second + (1 - dir[i]) * instance.boxes[i].first)

IloNumVar L;
IloNumVarArray pos_x;
IloNumVarArray pos_y;
IloBoolVarArray dir;

IloModel create_model(IloEnv env, BWPInstance instance) {
    IloModel model = IloModel(env);

    L = IloNumVar(env, instance.L_lower_bound, instance.L_upper_bound);
    pos_x = IloNumVarArray(env, instance.N);
    pos_y = IloNumVarArray(env, instance.N);
    dir = IloBoolVarArray(env, instance.N);

    for(int i = 0; i < instance.N; ++i) {
        pos_x[i] = IloNumVar(env, 0, instance.W - instance.boxes[i].first);
        pos_y[i] = IloNumVar(env, 0, instance.L_upper_bound - instance.boxes[i].first);
        model.add(pos_x[i] <= instance.W - width(i));
        if(instance.boxes[i].first == instance.boxes[i].second || instance.boxes[i].second > instance.W) {
            model.add(dir[i]);
        }
    }

    for(int i = 0; i < instance.N; ++i) {
        model.add(L >= pos_y[i] + height(i));

        for(int j = i + 1; j < instance.N; ++j) {
            model.add(
                pos_x[j] >= pos_x[i] + width(i) || 
                pos_x[j] + width(j) <= pos_x[i] || 
                pos_y[j] >= pos_y[i] + height(i) || 
                pos_y[j] + height(j) <= pos_y[i]
            );
        }
    }

    model.add(IloMinimize(env, L));

    return model;
}


void print_solution(IloCplex cplex, BWPInstance instance) {
    cout << cplex.getObjValue() << endl;
    for(int i = 0; i < instance.N; ++i) {
        int x = cplex.getValue(pos_x[i]);
        int y = cplex.getValue(pos_y[i]);
        bool d = cplex.getValue(dir[i]);
        cout << x << " " << y << " " 
        << x + d * instance.boxes[i].first + (1 - d) * instance.boxes[i].second - 1 << " " 
        << y + d * instance.boxes[i].second + (1 - d) * instance.boxes[i].first - 1 << endl;
    }
    for(int i = 0; i < instance.N; ++i) {
        for(int j = 0; j < instance.N; ++j) {
            if(i != j) {
                cerr << i << ", " << j << endl;
                cerr << cplex.getValue(pos_x[j] >= pos_x[i] + width(i)|| pos_x[j] + width(j) <= pos_x[i] || pos_y[j] >= pos_y[i] + height(i) || pos_y[j] + height(j) <= pos_y[i]) << endl;
            }
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
    cplex.solve();
    print_solution(cplex, instance);
    env.end();    
}
