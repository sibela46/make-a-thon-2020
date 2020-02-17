#include <limits.h> 
#include <stdio.h>
#include <Wire.h>
#include "newvector.h"
#include "newqueue.h"

#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

#define W 3
#define H 6

// 2-dimensional array of row pin numbers: TODO: CHANGE THESE
const int row[W] = { 2, 16, 17 };

// 2-dimensional array of column pin numbers:
const int col[H] = { 18, 19, 21 ,25, 35, 5 };

int pixels[H][W];

void ClearPixels(){
  for (int x = 0; x < H; x++) {
    for (int y = 0; y < W; y++) {
      pixels[x][y] = LOW;
    }
  }
}

#define V 18 //define number of vertices in graph
#define X 3 //x width
#define Y 3 //y height

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

int source = 16; // start at bottom middle of factory
int latestDestination = 16;

IntVector adj[V]; //18

void add_edge(IntVector adj[], int src, int dest) 
{ 
  adj[src].vecPushOn(dest); 
  adj[dest].vecPushOn(src); 
} 

void setup() {
    for (int thisPin = 0; thisPin < 3; thisPin++) {
      // initialize the output pins:
      pinMode(col[thisPin], OUTPUT);
      pinMode(row[thisPin], OUTPUT);
      digitalWrite(col[thisPin], LOW);
      digitalWrite(row[thisPin], LOW);
      // take the col pins (i.e. the cathodes) high to ensure that the LEDS are off:
    }

    // initialize the pixel matrix:
    ClearPixels();
    
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
//      int leds[V] = {L0, L1, L2, L3, L4, L5, L6, L7, L8};


    //TODO - define bluetooth stuff

    Serial.begin(115200);
    SerialBT.begin("ESP32test"); //Bluetooth device name
    delay(2000);
    Serial.println("***RESET***");
    
    pixels[source/W][source%W] = HIGH; // light starting source

    Wire.begin();
}

void loop() {
  if (SerialBT.available() > 0) {
    char in = SerialBT.read();
    if(in == 'z'){
      Serial.println("Setting Source");
      source = latestDestination;
      ClearPixels();
      pixels[source/W][source%W] = HIGH;
    } else {
      int val = in - 'a';
      Serial.println(val);
      printPath(adj, source, val, V);
      latestDestination = val;
    }
  }

  sendPixels(); 
}

void LightUpSolution(int shortestPath[V], int leds[V]){
//  for(int i = 0; i < V; i++){
//    digitalWrite(leds[i], shortestPath[i]);
//  }

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
  ClearPixels();
  for (int i = path.getVectorSize() - 1; i >= 0; i--){
    int pin = path.vectorGet(i);
    // TODO, make sure dis ain't fucked (inc that stuff you dropped)
    pixels[pin/W][pin%W] = HIGH;
    Serial.print(pin);
    Serial.print(", ");
  }
  printPixels();
}

void printPixels(){
  Serial.println("-+-+-");
  for(int i = 0; i < H; i++){
    for(int j = 0; j < W; j++){
      Serial.print(pixels[i][j]);
      Serial.print("|");  
    }
    Serial.println("");
  }
}

void refreshScreen(){
  for(int i = H-1; i >= 0; i--){ //go through vertical LED'S
    digitalWrite(col[i], HIGH);//toggle value
    for(int j = W-1; j >= 0; j--){
      digitalWrite(row[j], !pixels[i][j]);
    }
    delay(100);
    digitalWrite(col[i], LOW);
  }
  printPixels();
//  Serial.println("Done.");
}

void sendPixels(){
  Wire.beginTransmission(4);
  Wire.write("b");

  for(int i = 0; i < H; i++){
    for(int j = 0; j< W; j++){
      if(pixels[i][j] == HIGH){
        Wire.write("1");     
      } else {
        Wire.write("0"); 
      }
    }
  }
  
  Wire.endTransmission();
  delay(5000);
}
