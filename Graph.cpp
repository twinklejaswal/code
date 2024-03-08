// Graph implementation using adjacency matrix 
// By Twinkle Jaswal
// 4-17-2023
#include "Graph.h"
#include "DisjointSet.h"
#include <iostream>
#include <string>
#include <fstream>
#include <queue>
using namespace std; 

//read graph 
bool Graph::readGraph(string fileName){

    clear(); 

    string toVertex; 
    string fromVertex;
    string vertexName;
    int toIndex ; 
    int fromIndex; 
    int weight; 
    
    ifstream infile(fileName); 

    if(!infile.is_open()){
        return false;

    } else{
        infile >> numVertices; 

        for(int i = 0; i < numVertices; i++){
            infile >> vertexName; 
            vertexNames.push_back(vertexName); 
            myMatrix.push_back(vector<int>(numVertices, -1));  
        }
        infile >> numEdges; 

        for(int i = 1; i <= numEdges; i++){
            infile >> fromVertex >> toVertex >> weight; 

            toIndex = findVertexIndex(toVertex); 
            fromIndex = findVertexIndex(fromVertex); 
            
            myMatrix[fromIndex][toIndex] = weight;  
            
        }

        infile.close(); 
        return true;
    }

}

//print graph 
void Graph::printGraph(){
    cout<<numVertices<<"\n"; 
    
    for(int i = 0; i < numVertices; i++){
        cout<<vertexNames[i]<<"\n"; 
    }
    cout<<numEdges<<"\n"; 

    for(int from = 0; from < myMatrix.size(); from++){
        for(int to = 0; to < myMatrix[from].size(); to++){
            if(myMatrix[from][to]!=-1){
            cout<<vertexNames[from] << " " << vertexNames[to] 
            << " " <<myMatrix[from][to] << "\n";
            } 
        }
    }
}

//compute topological sort 
void Graph::computeTopologicalSort(){
    
    queue<int> myVertexQueue; 
    vector<int> inDegree(numVertices, 0); 
    vector<int> sortedVertices;  

    //Count in-degrees 
    for(int i =0; i < myMatrix.size(); i++){
        for(int j= 0; j < myMatrix.size(); j++){
            if(myMatrix[i][j] != -1)
            inDegree[j]++; 
        }
    }

    //Push all 0 degree vertices in queue 
    for(int i =0; i < numVertices; i++){
        if(inDegree[i]==0)
            myVertexQueue.push(i); 
    }

    //sort 
    while(!myVertexQueue.empty()){
        int sorted = myVertexQueue.front(); 
        myVertexQueue.pop(); 
        sortedVertices.push_back(sorted); 

        for(int i =0; i < myMatrix.size(); i++){
            if(myMatrix[sorted][i] != -1){
                inDegree[i]--; 
                if(inDegree[i]==0)
                    myVertexQueue.push(i); 
            }
            
        }
    }

    //print 
    if(sortedVertices.size()!=numVertices){
        cout<<"This graph cannot be topologically sorted.\n"; 
    }else{
        cout<<"Topological Sort:\n";
        for(int i = 0; i < numVertices-1; i++){
            cout<<vertexNames[sortedVertices[i]] << " --> ";
        }
        cout<<vertexNames[sortedVertices[numVertices-1]] <<"\n"; 
    }
}

//compute shortest path 
void Graph::computeShortestPaths(string node){
    cout<<"Shortest paths from " << node<<":\n"; 

    vector<vertexInfo> vertexArr(numVertices); 
    priority_queue<myQueueInfo> myQueue; 

    int nodeIndex = findVertexIndex(node); 

    int totalPathCost; 
    
    //add edges out of node 
    for(int to = 0; to < myMatrix.size(); to++){
        if(myMatrix[nodeIndex][to] != -1){
            myQueue.push(myQueueInfo(nodeIndex, to, myMatrix[nodeIndex][to])); 
        }
    }
    //add node to vertexArr
    vertexArr[nodeIndex] = (vertexInfo(-1, -1, true)); 

    //compute path 
    while(!myQueue.empty()){
        myQueueInfo currNode = myQueue.top(); 
        myQueue.pop();  
        if(!vertexArr[currNode.to].pathFound){
            vertexArr[currNode.to] = (vertexInfo(currNode.from, currNode.cost, true));
            for(int i = 0; i < myMatrix.size(); i++){
                if(myMatrix[currNode.to][i] != -1 && vertexArr[i].pathFound != true){
                    totalPathCost = myMatrix[currNode.to][i]+currNode.cost; 
                    myQueue.push(myQueueInfo(currNode.to, i, totalPathCost)); 
                }
            }
        }

    } 

    //print 
    for(int i = 0; i < numVertices; i++){
        if(vertexArr[i].prevVertex == nodeIndex)
            cout<< node << " --> " <<vertexNames[i] <<" || Weight: "<<vertexArr[i].cost<<"\n"; 
        else if(!vertexArr[i].pathFound)
            cout<<"No path from "<<node <<" to " << vertexNames[i]<< " found.\n"; 
        else if(vertexArr[i].prevVertex != nodeIndex && vertexArr[i].cost != -1)
            printShortestPath(vertexArr, i, nodeIndex);
    }
}

//compute minimum spanning tree
//Precondition: graph is connected 
void Graph::computeMinimumSpanningTree(){
    cout <<"Minimum Spanning Tree:\n"; 

    int totalCost=0; 
    DisjointSet mySet(numVertices); 
    vector<myQueueInfo> mySpanTreeEdges;
    priority_queue<myQueueInfo> mySortedEdgeList; 
    bool treeComplete=false; 

    //traverse through matrix and sort all the edges 
    for(int from = 0; from <myMatrix.size(); from++){
        for(int to =0; to < myMatrix[from].size(); to++){
            if(myMatrix[from][to]!=-1)
                mySortedEdgeList.push(myQueueInfo(from, to, myMatrix[from][to]));
        }
    }

    //find minimal spanning tree 
    while(!treeComplete){
        myQueueInfo currEdge = mySortedEdgeList.top(); 

        if(mySet.find(currEdge.from) != mySet.find(currEdge.to)){
           if(mySet.doUnion(currEdge.from, currEdge.to)==1){
            treeComplete = true; 
           }
           totalCost = totalCost+currEdge.cost; 
           mySpanTreeEdges.push_back(myQueueInfo(currEdge.from, currEdge.to, currEdge.cost)); 
        }
        mySortedEdgeList.pop(); 
    }
    
    //print 
    for(int i =0; i < mySpanTreeEdges.size(); i++){
        cout << vertexNames[mySpanTreeEdges[i].from] <<" -- "<<vertexNames[mySpanTreeEdges[i].to]<< " || Weight: " << mySpanTreeEdges[i].cost<<"\n";
    } cout <<"Total Cost: " << totalCost<<"\n"; 
    
}

//helper method to find int vertex from string 
int Graph::findVertexIndex(std::string vertexName){
    bool found = false; 
    int vertexIndex = 0;
    int index = 0; 

    while(!found){
        if(vertexNames[index]== vertexName){
            vertexIndex = index;
            found = true; 
        }
        index++; 
    }
    return vertexIndex; 
}

//helper function to clear all vectors after calling readGraph 
void Graph::clear(){
    myMatrix.clear(); 
    vertexNames.clear();  
}

//helper method to print path from vertex to node 
void Graph:: printShortestPath(vector<vertexInfo> vertexPath, int val, int node) const {

    vector<int>sorted; 
    sorted.push_back(val);
    int index =0; 
    int origVal = val; 

    while(vertexPath[val].prevVertex != node){
        index = vertexPath[val].prevVertex; 
        val = index; 
        sorted.push_back(val); 
    }
    sorted.push_back(node); 

    for(int i = sorted.size()-1; i > 0; i--){
       cout<< vertexNames[sorted[i]] <<" --> "; 
    }
    cout << vertexNames[sorted[0]]<<" || Weight: "<<vertexPath[origVal].cost<<"\n"; 
    sorted.clear(); 
}


