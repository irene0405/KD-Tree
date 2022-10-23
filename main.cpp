#include <iostream>
#include <fstream>
#include <chrono>
#include <cmath>
#include <algorithm>

using namespace std;

#define DATASET 100000
#define DIMENSION 2

struct kd_node {
    int value[DIMENSION];
    struct kd_node *left, *right;
};


void swap(struct kd_node *x, struct kd_node *y) {
    double tmp[DIMENSION];
    memcpy(tmp, x->value, sizeof(tmp));
    memcpy(x->value, y->value, sizeof(tmp));
    memcpy(y->value, tmp, sizeof(tmp));
}

struct kd_node *
find_mid(struct kd_node *start, struct kd_node *end, int divide) {
    if (end <= start) {
        return NULL;
    } else if (end == start + 1) {
        return start;
    } else {
        struct kd_node *p, *store, *mid = start + (end - start) / 2;
        double pivot;
        while (1) {
            pivot = mid->value[divide];
            swap(mid, end - 1);
            for (store = p = start; p < end; p++) {
                if (p->value[divide] < pivot) {
                    if (p != store) {
                        swap(p, store);
                    }
                    store++;
                }
            }
            swap(store, end - 1);

            if (store == mid) {
                return mid;
            } else if (store > mid) {
                end = store;
            } else {
                start = store;
            }
        }
    }
}

struct kd_node *
make_tree(struct kd_node *data, int data_num, int divide) {
    struct kd_node *n;

    if ((n = find_mid(data, data + data_num, divide))) {
        divide = (divide + 1) % DIMENSION;
        n->left = make_tree(data, n - data, divide);
        n->right = make_tree(n + 1, data + data_num - (n + 1), divide);
    }
    return n;
}

long long int getDistance(struct kd_node *x, struct kd_node *y) {
    long long int distance, distanceSquared = 0;
    int dimension = DIMENSION;
    while (dimension--) {
        distance = x->value[dimension] - y->value[dimension];
        distanceSquared += distance * distance;
    }
    return distanceSquared;
}

double best_distance_squared;

void nearest(struct kd_node *root, struct kd_node *node, int i, struct kd_node **found, double input_distance) {
    long long int distance_squared, dx, dx_squared;
    int dimension = DIMENSION;
    if (!root) return;
    distance_squared = getDistance(root, node);
    dx = root->value[i] - node->value[i];
    dx_squared = dx * dx;

    if (!*found || distance_squared <= input_distance) {
        best_distance_squared = distance_squared;
        *found = root;
    }

    if (!best_distance_squared) return;

    if (++i >= dimension) i = 0;

    nearest(dx > 0 ? root->left : root->right, node, i, found, best_distance_squared);
    if (dx_squared >= best_distance_squared) return;
    nearest(dx > 0 ? root->right : root->left, node, i, found, best_distance_squared);
}

int main() {
    auto start = std::chrono::steady_clock::now();

    struct kd_node input_node, *root, *found;
    double input_distance;
    struct kd_node *data = new kd_node[DATASET];

    int data_num = 0;
//    ifstream ifs("kd-test-input.txt", ios::in);
    ifstream ifs("kd-test-big-input.txt", ios::in);
    if (!ifs.is_open()) {
        cout << "Failed to open file.\n";
    } else {
        int x;
        int y;
        while (ifs >> x >> y) {
            data[data_num].value[0] = x;
            data[data_num].value[1] = y;
            data_num++;
        }
        ifs.close();
    }

    root = make_tree(data, data_num, 0);

    cout << "Please input x: ";
    cin >> input_node.value[0];
    cout << "Please input y: ";
    cin >> input_node.value[1];
    cout << "Please input d: ";
    cin >> input_distance;

    found = 0;
    nearest(root, &input_node, 0, &found, input_distance);

    ofstream ofs;
//    ofs.open("kd-test-output.txt");
    ofs.open("kd-test-big-output.txt");
    if (!ofs.is_open()) {
        cout << "Failed to open file.\n";
    } else {
        ofs << "t: ";
        ofs << "(";
        ofs << fixed << setprecision(0) << input_node.value[0];
        ofs << ",";
        ofs << fixed << setprecision(0) << input_node.value[1];
        ofs << ")";
        ofs << "\t t: ";
        ofs << fixed << setprecision(0) << input_distance << endl;
    }
    if (best_distance_squared <= input_distance * input_distance) {
        ofs << "Output: (";
        ofs << fixed << setprecision(0) << found->value[0];
        ofs << ",";
        ofs << fixed << setprecision(0) << found->value[1];
        ofs << ")";
        ofs << endl;
        ofs.close();
    } else {
        ofs << "There is no point in a circle with a radius of " << input_distance << " drawn with ("
            << input_node.value[0] << "," << input_node.value[1] << ") as the center." << endl;
    }

    auto end = chrono::steady_clock::now();
    cout << "Time taken: " << chrono::duration<double>(end - start).count() << " s" << endl;
}
