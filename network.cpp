/**
 *  @author Twinkle Jaswal and George Jacob
 *  Simulated Toplogy 
 *  Program reads in a topology, initalizes a neuron, and computes the
 *  Voltage and the Raster Plot
 *
 *  Output: Voltage for each neuron, Raster Plot for each neuron, data for voltage map, and
 *  aggregated voltage
 */

#include<iostream>
#include<iomanip>
#include<sstream>
#include<fstream>
#include<stdio.h>
#include<stdlib.h>
#include<cmath>
#include<math.h>
#include<vector>
#include<algorithm>
#include<string.h>
#include<time.h>
#include<cstdlib>
#include "Neuron.h"
#include "RK4.h"
using namespace std;

//Parameters
#define max_steps 200000
#define transient 200000//2
#define stepSize 0.05


int main(int argc, char** argv){
    
    float coupling = 0.12;       //fixed strength between neurons
    
    stringstream ss;
    ss << fixed << setprecision(3) << coupling;
    string couplingStr = ss.str();
    
    //initalizing data files
    FILE *fp_raster;
    string outputName = "./Outputs/raster_" + couplingStr +".txt";
    fp_raster = fopen(outputName.c_str(), "w");
    
    FILE *fp_voltMap;
    string outputName2 = "./Outputs/voltMap_" + couplingStr +".txt";
    fp_voltMap = fopen(outputName2.c_str(), "w");
    
    FILE *fp_aggVolt;
    string outputName3 = "./Outputs/aggVolt_" + couplingStr + ".txt";
    fp_aggVolt = fopen(outputName3.c_str(), "w");
    
    ofstream out("./Outputs/voltage_"+ couplingStr +".txt");
    
    ifstream inp("./Inputs/topology.txt");
    
    //Size of input functional connectivity matrix
    int size;
    inp >> size;
    vector<vector<int> > graph(size, vector<int> (size, 0)) ;
    
    //reading in topology
    if (!inp) {
        cout << "Error, file couldn't be opened" << endl;
        return 1;
    }
    for(int row = 0; row < size; row++)
    {
        for(int column = 0; column < size; column++){
            inp >> graph[row][column];
            if ( !inp ) {
                cout << "Error reading file for element " << row << "," << column << endl;
                return 1;
            }
        }
    }
    
    //array of neuron objects
    Neuron nrn[size];
    RK4 RangeKutta(coupling, size, graph);
    
    //Generating and assigning random gSR values
    srand(1);       //seed
    double gsr;
    //gSR range [0.2-0.5]
    double upperBound = 0.5;
    double lowerBound = 0.2;
    double diff = upperBound - lowerBound;
    
    for(int i = 0; i < size; i++){
        gsr = (((float) rand() / RAND_MAX) * diff) + lowerBound;
        nrn[i].gSR = gsr;
    }
    
    //setting a fixed temperature for each neuron
    double neuronTemp = 37.0;
    for(int i = 0; i < size; i++){
        nrn[i].set_TempParameters(neuronTemp);
    }
    
    //neuron states using Runge-Kutta method for a transient period
    for(int n = 0; n < transient; n++){
        for(int i = 0; i < size; i++){
            RangeKutta.RK4_solver(nrn[i], stepSize, 0.0, nrn, i);
        }
    }
    
    //arrays for previous values and locking status for rach neuron
    double prev[size];  //stores previous values
    double lock[size];  //indicates whether neuron is locked or not
    
    //initializes prev and lock arrays
    for(int k = 0; k < size; k++){
        prev[k] = -100;
        lock[k] = 0;
    }
    
    //simulation loop
    for(int time = 0; time < max_steps; time++){
        //update neuron state
        for(int i =0; i< size; i++)
            RangeKutta.RK4_solver(nrn[i], stepSize, 0.0, nrn, i);
        
        if(time % 1 == 0){
            
            double totalVoltage = 0.0;
            //append voltage values
            for(int j = 0; j < size; j++){
                totalVoltage += nrn[j].X[0];
            }
            
            double aggVolt = totalVoltage/size;
            fprintf(fp_aggVolt, "%lf\t%lf\n", time*stepSize, aggVolt);
            
            //output raster data
            for(int i = 0; i < size; i++){
                //output voltage for color maps
                fprintf(fp_voltMap, "%lf\t%d\t%lf\n", time*stepSize, i+1, nrn[i].X[0]);
                
                if(nrn[i].X[0] > -20 && nrn[i].X[0] > prev[i] && lock[i] == 0 ){
                    fprintf(fp_raster, "%lf\t%d\n", time*stepSize, (i+1));
                    lock[i]=1;
                }
                
                prev[i] = nrn[i].X[0];
                if (nrn[i].X[0] < -20){
                    lock[i] = 0;
                }
            }
            
        }
        
    }
    
    //closing files
    fclose(fp_voltMap);
    fclose(fp_raster);
    fclose(fp_aggVolt);
    
    cout << "Simulated Network Program Finished" << endl;
    
    return 0;
    
} //end main
