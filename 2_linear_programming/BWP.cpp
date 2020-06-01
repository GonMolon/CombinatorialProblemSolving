#include <ilcplex/ilocplex.h>
ILOSTLBEGIN

#include <vector>
#include <iostream>
#include "../include/BWPInstance.hh"


IloNumVar L;
IloNumVarArray pos_x;
IloNumVarArray pos_y;
IloBoolVarArray dir;

IloModel create_model(IloEnv env, BWPInstance instance) {
    IloModel model = IloModel(env);

    pos_x = IloNumVarArray(env, instance.N);
    pos_y = IloNumVarArray(env, instance.N);
    dir = IloBoolVarArray(env, instance.N);

    // int aux_bool_var_count = instance.N * (instance.N - 1) / 2;
    // overlap_top = IloBoolVarArray(env, aux_bool_var_count);
    // overlap_bottom = IloBoolVarArray(env, aux_bool_var_count);
    // overlap_left = IloBoolVarArray(env, aux_bool_var_count);
    // overlap_right = IloBoolVarArray(env, aux_bool_var_count);

    vector<IloNumExprArg> width = vector<IloNumExprArg>(instance.N);
    vector<IloNumExprArg> height = vector<IloNumExprArg>(instance.N);

    double precision = double(instance.W - 1) / instance.W - (instance.W - 1) / instance.W;
    cerr << "Precision for floor operation: " << precision << endl;
    for(int i = 0; i < instance.N; ++i) {
        width[i] = dir[i] * instance.boxes[i].first + (1 - dir[i]) * instance.boxes[i].second;
        height[i] = dir[i] * instance.boxes[i].second + (1 - dir[i]) * instance.boxes[i].first;

        pos[i] = IloNumVar(env, 0, instance.W * (instance.L_upper_bound - instance.boxes[i].first + 1) - 1);
        if(instance.boxes[i].first == instance.boxes[i].second || instance.boxes[i].second > instance.W) {
            model.add(dir[i]);
        }
    }

    int index = 0;
    for(int i = 0; i < instance.N; ++i) {
        // model.add(L >= pos[i] / instance.W + height[i]);

        // for(int j = i + 1; j < instance.N; ++j) {

        //     BoolVar top(overlap_top[index]);
        //     BoolVar bottom(overlap_bottom[index]);
        //     BoolVar left(overlap_left[index]);
        //     BoolVar right(overlap_right[index]);
        //     ++index;

        //     rel(*this, pos[j] % instance.W + right * instance.W                 >= pos[i] % instance.W + width[i]);
        //     rel(*this, pos[j] % instance.W + width[j]                           <= pos[i] % instance.W + left * instance.W);
        //     rel(*this, pos[j] / instance.W + bottom * instance.L_upper_bound    >= pos[i] / instance.W + height[i]);
        //     rel(*this, pos[j] / instance.W + height[j]                          <= pos[i] / instance.W + top * instance.L_upper_bound);
        //     rel(*this, top + bottom + left + right <= 3);
        // }
    }

    return model;
}


int main() {
    BWPInstance instance;
    instance.read();

    IloEnv env;
    IloModel model = create_model(env, instance);

    return 0;
    IloCplex cplex(model);
    cplex.solve();
    cout << cplex.getObjValue() << endl;
    env.end();    
}
