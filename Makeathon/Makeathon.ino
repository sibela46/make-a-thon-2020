#include <limits.h> 
#include <stdio.h>
#include "newvector.h"
#include "newqueue.h"

#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

#define H 6
#define W 3

int pixels[H][W];

// 2-dimensional array of row pin numbers:
const int row[W] = { 0, 1, 2 };

// 2-dimensional array of column pin numbers:
const int col[H] = { 3 ,4, 5, 6, 7, 8 };

for (int thisPin = 0; thisPin < 3; thisPin++) {
  // initialize the output pins:
  pinMode(col[thisPin], OUTPUT);
  pinMode(row[thisPin], OUTPUT);
  digitalWrite(col[thisPin], LOW);
  digitalWrite(row[thisPin], LOW);
  // take the col pins (i.e. the cathodes) high to ensure that the LEDS are off:
}

// initialize the pixel matrix:
for (int x = 0; x < 3; x++) {
  for (int y = 0; y < 3; y++) {
    pixels[x][y] = LOW;
  }
}

#define V 18 //define number of vertices in graph
#define X 3 //x width
#define Y 6 //y height

#define L0 1 //Pin light 0 TODO: change pin numbers 
#define L1 2 //Pin light 1
#define L2 3 //and so on
#define L3 4
#define L4 4
#define L5 4
#define L6 4
#define L7 4
#define L8 4
#define L9 4
#define L10 4
#define L11 4

BluetoothSerial SerialBT;

int flag = 0;
int num = 2 ;

IntVector adj[V]; //12

void add_edge(IntVector adj[], int src, int dest) 
{ 
  adj[src].vecPushOn(dest); 
  adj[dest].vecPushOn(src); 
} 

void setup() {

    add_edge(adj, 0, 1);
    add_edge(adj, 0, 3);
    add_edge(adj, 1, 2);
    add_edge(adj, 1, 4);
    add_edge(adj, 2, 5);
    add_edge(adj, 3, 4);
    add_edge(adj, 3, 6);
    add_edge(adj, 4, 7);
    add_edge(adj, 6, 9);
    add_edge(adj, 7, 8);
    add_edge(adj, 7, 10);
    add_edge(adj, 8, 11);
    add_edge(adj, 9, 12);
    add_edge(adj, 10, 13);
    add_edge(adj, 11, 14);
    add_edge(adj, 12, 13);
    add_edge(adj, 12, 15);
    add_edge(adj, 13, 16);
    add_edge(adj, 14, 17);
    add_edge(adj, 15, 16);
    add_edge(adj, 16, 17);

    int leds[V] = {L0, L1, L2, L3, L4, L5, L6, L7, L8, L9, L10, L11};


    //TODO - define bluetooth stuff

    Serial.begin(115200);
    SerialBT.begin("ESP32test"); //Bluetooth device name
    delay(2000);
    Serial.println("***RESET***");
}

void loop() {
  if (SerialBT.available() > 0) {
    char in = SerialBT.read();
    int val = in - 'a';
    Serial.println(val);
    printPath(adj, 0, val, V);
  }

  refreshScreen(); 
}

void LightUpSolution(int shortestPath[V], int leds[V]){
  for(int i = 0; i < V; i++){
    digitalWrite(leds[i], shortestPath[i]);
  }
}
  
// A utility function to find the vertex with minimum distance value, from 
// the set of vertices not yet included in shortest path tree 
int minDistance(int dist[], bool shortestPath[]) 
{ 
    // Initialize min value 
    int min = INT_MAX, min_index; 
  
    for (int v = 0; v < v; v++) 
        if (shortestPath[v] == false && dist[v] <= min) 
            min = dist[v], min_index = v; 
  
    return min_index; 
} 
  
// A utility function to print the constructed distance array 
int printSolution(int dist[]) 
{ 
    printf("vertex \t\t Distance from Source\n"); 
    for (int i = 0; i < V; i++) 
        printf("%d \t\t %d\n", i, dist[i]); 
} 

bool BFS(IntVector adj[], int src, int dest, int v, int pred[], int dist[]){
  // a queue to maintain queue of vertices whose 
  // adjacency list is to be scanned as per normal 
  // DFS algorithm 
  IntQueue queue;

  // boolean array visited[] which stores the 
  // information whether ith vertex is reached 
  // at least once in the Breadth first search 
  bool visited[v];

  // initially all vertices are unvisited 
  // so v[i] for all i is false 
  // and as no path is yet constructed 
  // dist[i] for all i set to infinity 
  for (int i = 0; i < v; i++) { 
      visited[i] = false; 
      dist[i] = INT_MAX; 
      pred[i] = -1; 
  } 

  // now source is first to be visited and 
  // distance from source to itself should be 0 
  visited[src] = true; 
  dist[src] = 0; 
  queue.pushOn(src);

  // standard BFS algorithm 
  while (!queue.queueEmpty()) { 
    int u = queue.popOff(); // TODO is this right?
    for (int i = 0; i < adj[u].getVectorSize(); i++) { 
      if (visited[adj[u].vectorGet(i)] == false){
        visited[adj[u].vectorGet(i)] = true; 
        dist[adj[u].vectorGet(i)] = dist[u] + 1; 
        pred[adj[u].vectorGet(i)] = u;
        queue.pushOn(adj[u].vectorGet(i)); //is this right?
  
        // We stop BFS when we find 
        // destination.
        if (adj[u].vectorGet(i) == dest) return true; 
      } 
    } 
  } 
  return false; 
} 

void printPath(IntVector adj[], int s, int dest, int v){
  int pred[v];
  int dist[v]; 
  BFS(adj, s, dest, v, pred, dist);
  IntVector path;
  int crawl = dest; 
  path.vecPushOn(crawl); 
  while (pred[crawl] != -1) { 
    path.vecPushOn(pred[crawl]); 
    crawl = pred[crawl]; 
  }

  IntVector test;
  test.vecPushOn(1);

  for (int i = path.getVectorSize() - 1; i >= 0; i--){
    Serial.print(path.vectorGet(i));
    Serial.print(", ");
  }
}

void refreshScreen(){
  for(int i = 0; i < H; i++){ //go through vertical LED'S
    digitalWrite(row[i], LOW);//toggle value
    for(int j = 0; j < W; j++){
      digitalWrite(col[j], pixels[i][j]);
    }
    delay(1);
    digitalWrite(col[i], HIGH);
  }
}
