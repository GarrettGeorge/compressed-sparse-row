#include <iostream>
#include <fstream>
#include "ArgumentManager.h"
#include "FileManager.h"
#include "csrMatrix.h"
#include "COOArray.h"

int main(int argc, char * argv[]) {
    if(argc != 2) {
        std::cout << "Usage: " << argv[0] << "sparse operation=<add|multiply>;storage=<array|linkedlist>;sort=<nosort|nsquared|nlogn>;A=<file>;B=<file>;result=<file>\n";
    }
    else {
        // Parse command line argument string
        ArgumentManager aManager(argc, argv);
        // Use the manager to get the first input matrix
        std::string inputA = aManager.get("A");
        FileManager fManagerA(inputA);
        
        std::string inputB = aManager.get("B");
        FileManager fManagerB(inputB);

        std::string sort = aManager.get("sort");
    
        COOArray matrixA;
        fManagerA.getContentAsMatrix(matrixA);
        COOArray matrixB;
        fManagerB.getContentAsMatrix(matrixB);
   
        if(!matrixA.isSorted()) {
            if(sort == "nsquared")
                matrixA.bubbleSort();
            else if(sort == "nlogn")
                matrixA.sort(0, matrixA.length()-1);
            else
                matrixA.sort(0, matrixA.length()-1);
        }
        if(!matrixB.isSorted()) {
            if(sort == "nsquared")
                matrixB.bubbleSort();
            else if(sort == "nlogn")
                matrixB.sort(0, matrixB.length()-1);
            else
                matrixB.sort(0, matrixB.length()-1);
        }
        std::ofstream bigO; 
        bigO.open("bigO.txt", fstream::app);
        bigO << "algorithm #entries(n) countComparisons bigOcountComparisons \n";
        bigO.close();
        matrixA.writeToBigO(sort);
        matrixB.writeToBigO(sort);
        if(matrixA.isCompatibleForMultWith(matrixB)) {
            csrMatrix csr_a(matrixA);
            csrMatrix csr_b(matrixB);
            csr_a.print();
            csr_b.print();
            int m = csr_a.getRows(), n = csr_b.getCols(), est_nnz = 1.5 * (csr_a.getNNZ() +  csr_b.getNNZ());
            csrMatrix csr_c(m,n,est_nnz);
            csr_a.mult(csr_b, csr_c);
            csr_c.print();
            csr_c.writeToFile(aManager.get("result"));
        }
        else {
             std::string out = aManager.get("result");
             std::ofstream output;
             output.open(out);
             output.close(); 
        }
    }
    return 0;
}
