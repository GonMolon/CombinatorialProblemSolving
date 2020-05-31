#include <vector>
#include <iostream>

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
                L_lower_bound = max(L_lower_bound, y <= W ? x : y);
                L_upper_bound += y <= W ? x : y;
            }
        }
        L_lower_bound = max(L_lower_bound, (total_area + W - 1) / W);
    }
};
