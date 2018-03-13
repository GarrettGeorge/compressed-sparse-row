#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "ArgumentManager.h"
#include "FileManager.h"
#include "csrMatrix.h"
#include "COOArray.h"
#include "Graph.h"
/*
static void writeVectorToFile(std::vector<int> bigS, std::string outputFile) {
    std::ofstream bigSOutput;
    bigSOutput.open(outputFile);
    for(int i = 0; i < bigS.size(); i++) {
        bigSOutput << bigS.at(i) << "\n";
    }
}*/

static void writeVectorToFile(std::vector<bool> &bigS, std::string outputFile){
    std::ofstream bigSOutput;
    bigSOutput.open(outputFile);
    for(int i = 0; i < bigS.size(); i++) {
        if(bigS.at(i))
            bigSOutput << i + 1 << "\n";
    }   
}

static void createHashTable(std::string output, std::string hash[]) {
    
    std::cout << "creating hash table\n";
    ifstream fileRead;
    fileRead.open(output);
    if(fileRead.is_open()) {
        while(true) {
            std::string line;
            std::getline(fileRead, line);
            
            std::stringstream ss;
            ss << line;
            
            int index;
            ss >> index;
            
            std::string label;
            label = ss.str();
            std::cout << "setting hash at index: " << index << "\n";
            hash[index-1] = label;

            if(fileRead.eof()) {
                break;
            }
        }
    }
    else {
        std::cout << "error reading file\n";
    }
}

int main(int argc, char * argv[]) {
    if(argc != 2) {
        std::cout << "Usage: " << argv[0] << "reachability E=<file>;labels=<file>;result=<file>;source=<integer>;dfs=no|recursive|iterative; k=<integer>\n";
    }
    else {
        // Parse command line argument string
        ArgumentManager aManager(argc, argv);
        // Use the manager to get input matrix E
        std::string inputE = aManager.get("E");
        FileManager manager(inputE);
 
        COOArray matrixE;
        manager.getContentAsMatrix(matrixE);
        
        if(!matrixE.isSorted())
            matrixE.sort(0,matrixE.length() - 1);
        
        std::string result = aManager.get("result"); 
        std::string dfs = aManager.get("dfs");
        std::string source = aManager.get("source");
        if(source == "") {
            std::cout << "Invalid source vertex\n";
            return 1;
        }
    
        std::vector<bool> bigS(matrixE.getRows(), false);   

        if(dfs == "no" || dfs == ""){
            csrMatrix csr_e(matrixE);
            //csr_e.print();
            bigS.at(std::stoi(source)-1) = true;
            csr_e.reachabilityWithMult(bigS);
            /*for(int i = 0; i < bigS.size(); i++) {
                if(bigS.at(i))
                    std::cout << i+1 << "\n";
            }*/
        }
        else if(dfs == "recursive") {
            //delete matrixE;
            Graph graph_e(matrixE, std::stoi(source));
            //graph_e.print();
            bigS = graph_e.dfs(bigS, std::stoi(source));
            /*for(int i = 0; i < bigS.size(); i++) {
                if(bigS.at(i))
                    std::cout << i+1 << "\n";
            }*/
        }
        else {
            // Phase 2
            Graph graph_e(matrixE,0);
            bigS = graph_e.dfs_iterative(bigS,std::stoi(source));
        }
        if(aManager.get("labels") != "") {
            std::string hashTable [10000];
            createHashTable(aManager.get("labels"), hashTable);
            std::cout << "created hashtable\n";
            std::ofstream bigSOutput;
            bigSOutput.open(result);
            
            for(int i = 0; i < bigS.size(); i++) {
                if(bigS.at(i))
                    bigSOutput  << hashTable[i] << "\n";
            } 
        }
        else {
            writeVectorToFile( bigS, result);
        }
    }
}
