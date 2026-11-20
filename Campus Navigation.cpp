#include <iostream>
#include <fstream>
#include <string>
using namespace std;

const int MAX = 30;
const int INF = 1000000;

string names[MAX];
int dist[MAX][MAX];
int n = 0;

int findBuilding(string name) {
    for (int i = 0; i < n; i++)
        if (names[i] == name) return i;
    return -1;
}

int addBuilding(string name) {
    int id = findBuilding(name);
    if (id != -1) return id;
    names[n] = name;
    n++;
    return n - 1;
}

bool loadMap(string filename) {
    ifstream file(filename);
    if (!file) {
        cout << "Cannot open " << filename << endl;
        return false;
    }
    string a, b;
    int w;
    while (file >> a) {
        if (a[0] == '#') {
            getline(file, a);
            continue;
        }
        file >> b >> w;
        if (n >= MAX - 1) break;
        int x = addBuilding(a);
        int y = addBuilding(b);
        dist[x][y] = w;
        dist[y][x] = w;
    }
    return n > 0;
}

void showBuildings() {
    cout << "\nBuildings:\n";
    for (int i = 0; i < n; i++)
        cout << "  " << i + 1 << ". " << names[i] << endl;
}

int askBuilding(string question) {
    int choice;
    cout << question;
    cin >> choice;
    if (choice < 1 || choice > n) return -1;
    return choice - 1;
}

void printPath(int parent[], int to) {
    if (parent[to] == -1) {
        cout << names[to];
        return;
    }
    printPath(parent, parent[to]);
    cout << " -> " << names[to];
}


void shortestPath(int start, int end) {
    int d[MAX], parent[MAX];
    bool done[MAX];
    for (int i = 0; i < n; i++) {
        d[i] = INF;
        parent[i] = -1;
        done[i] = false;
    }
    d[start] = 0;

    for (int count = 0; count < n; count++) {

        int u = -1;
        for (int i = 0; i < n; i++)
            if (!done[i] && (u == -1 || d[i] < d[u])) u = i;
        if (u == -1 || d[u] == INF) break;
        done[u] = true;

        for (int v = 0; v < n; v++) {
            if (dist[u][v] > 0 && d[u] + dist[u][v] < d[v]) {
                d[v] = d[u] + dist[u][v];
                parent[v] = u;
            }
        }
    }

    if (d[end] == INF) {
        cout << "No route found.\n";
        return;
    }
    cout << "\nShortest route: ";
    printPath(parent, end);
    cout << "\nDistance: " << d[end] << " m\n";
}


void fewestStops(int start, int end) {
    int parent[MAX], queue[MAX];
    bool visited[MAX];
    int head = 0, tail = 0;
    for (int i = 0; i < n; i++) {
        parent[i] = -1;
        visited[i] = false;
    }

    visited[start] = true;
    queue[tail++] = start;
    while (head < tail) {
        int u = queue[head++];
        for (int v = 0; v < n; v++) {
            if (dist[u][v] > 0 && !visited[v]) {
                visited[v] = true;
                parent[v] = u;
                queue[tail++] = v;
            }
        }
    }

    if (!visited[end]) {
        cout << "No route found.\n";
        return;
    }


    int stops = 1, total = 0;
    for (int v = end; parent[v] != -1; v = parent[v]) {
        total += dist[parent[v]][v];
        stops++;
    }
    cout << "\nRoute with fewest stops: ";
    printPath(parent, end);
    cout << "\nBuildings visited: " << stops << ", Distance: " << total << " m\n";
}


int group[MAX];

int findGroup(int x) {
    while (group[x] != x) x = group[x];
    return x;
}

void minimumCabling() {

    int eu[MAX * MAX], ev[MAX * MAX], ew[MAX * MAX];
    int m = 0;
    for (int i = 0; i < n; i++)
        for (int j = i + 1; j < n; j++)
            if (dist[i][j] > 0) {
                eu[m] = i;
                ev[m] = j;
                ew[m] = dist[i][j];
                m++;
            }


    for (int i = 0; i < m - 1; i++)
        for (int j = 0; j < m - 1 - i; j++)
            if (ew[j] > ew[j + 1]) {
                int t;
                t = eu[j]; eu[j] = eu[j + 1]; eu[j + 1] = t;
                t = ev[j]; ev[j] = ev[j + 1]; ev[j + 1] = t;
                t = ew[j]; ew[j] = ew[j + 1]; ew[j + 1] = t;
            }

    for (int i = 0; i < n; i++) group[i] = i;

    cout << "\nCables to lay:\n";
    int total = 0, used = 0;
    for (int i = 0; i < m; i++) {
        int a = findGroup(eu[i]);
        int b = findGroup(ev[i]);
        if (a != b) {
            group[a] = b;
            cout << "  " << names[eu[i]] << " --- " << names[ev[i]]
                 << " (" << ew[i] << " m)\n";
            total += ew[i];
            used++;
        }
    }
    cout << "Total cable length: " << total << " m\n";
    if (used < n - 1) cout << "(Campus is not fully connected.)\n";
}



int main() {
    if (!loadMap("campus_map.txt")) return 1;
    cout << "=== Campus Navigation System ===\n";
    cout << "Loaded " << n << " buildings.\n";

    int choice;
    do {
        cout << "\n1. Show all buildings\n";
        cout << "2. Shortest path (Dijkstra)\n";
        cout << "3. Fewest stops (BFS)\n";
        cout << "4. Minimum cabling (Kruskal)\n";
        cout << "0. Exit\n";
        cout << "Choice: ";
        cin >> choice;

        if (choice == 1) {
            showBuildings();
        } else if (choice == 2 || choice == 3) {
            showBuildings();
            int s = askBuilding("From (number): ");
            int t = askBuilding("To (number): ");
            if (s == -1 || t == -1) cout << "Invalid number.\n";
            else if (choice == 2) shortestPath(s, t);
            else fewestStops(s, t);
        } else if (choice == 4) {
            minimumCabling();
        }
    } while (choice != 0);

    cout << "Goodbye!\n";
    return 0;
}
