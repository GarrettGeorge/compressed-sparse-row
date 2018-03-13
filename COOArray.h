//
//  NodeArray.h
//  sparse
//
//  Created by Admin on 9/5/16.
//  Copyright © 2016 Admin. All rights reserved.
//

#ifndef NodeArray_h
#define NodeArray_h

#include <iostream>
#include <fstream>
#include <cmath>
#include <cstring>
#include <assert.h>
struct cooNode {
    float val;
    int row;
    int col;
};

class COOArray {
private:
    cooNode * matrix;
    int count;
    int capacity;
    int matrixRow;
    int matrixCol;
    int countComparisons;
public:
    COOArray();
    COOArray(int);
    ~COOArray();
    const void operator=(const COOArray& rhs);
    cooNode at(int) const;
    void print();
    void append(cooNode);
    int length() const;
    void setLength(int);
    cooNode* getMatrix();
    void setDimensions(int, int);
    int getRows() const;
    int getCols() const;
    bool isSorted();
    void sort(int, int);
    int partition(int, int);
    void bubbleSort();
    void writeToBigO(std::string);
    int seqFind(int, int);
    int binFind(int, int);
    bool isCompatibleWith(COOArray&);
    bool isCompatibleForMultWith(COOArray&);
    void writeToFile(std::string str);
    void recSum(COOArray&, int, int, COOArray&);
    void recSumInner(COOArray&, COOArray&, int&, int); 
};

COOArray::COOArray() {
    count = 0;
    capacity = 1;
    countComparisons = 0;
    matrixRow = 0;
    matrixCol = 0;
    cooNode *newAlloc = new cooNode[1];
    matrix = newAlloc;
}

COOArray::COOArray(int size) {
    count = 0;
    capacity = size;
    countComparisons = 0;
    matrixRow = 0;
    matrixCol = 0;
    cooNode *newAlloc = new cooNode[size];
    matrix = newAlloc;
}

COOArray::~COOArray() {
    //std::cout << "Deleting matrix.\n"; 
    //std::cout << "ROW " << (matrix[0]).row << ". COL " << (matrix[0]).col << "\n";
    //std::cout << "Setting to null,\n";
    delete [] matrix;
    matrix = NULL;
}

const void COOArray::operator=(const COOArray& rhs) {
    //std::cout << "= operator\n";
    count = rhs.count;
    capacity = rhs.capacity;
    matrixRow = rhs.matrixRow;
    matrixCol = rhs.matrixCol;
    delete [] matrix;
    matrix = new cooNode[capacity];
    //assert(matrix != NULL);
    for(int i = 0; i < count; i++) {
       matrix[i].row  = rhs.matrix[i].row;
       matrix[i].col = rhs.matrix[i].col;
       matrix[i].val = rhs.matrix[i].val;
    }    
    return;
}

cooNode COOArray::at(int index) const {
    if(index >= count || index < 0) {
        cooNode temp;
        temp.row = matrix[0].row;
        temp.col = -1;
        temp.val = -1.0;
        return temp;
    }
    return matrix[index];
}

void COOArray::print() {
    for(int i = 0; i < count; i++) {
        std::cout << "ROW " << (matrix[i]).row << ". COL " << (matrix[i]).col << " Val: " << matrix[i].val << "\n";
    }
    std::cout << "Count: " << count << " Cap: " << capacity << "\n";
    std::cout << "Max Row: " << matrixRow << " Max Col: " << matrixCol << "\n";
}

void COOArray::append(cooNode node) {
    // Check if room exists in the current matrix
    if(count < capacity) {
        if(node.row == matrix[count-1].row && node.col == matrix[count-1].col) {
            matrix[count-1] = node;
        }
        else {
            matrix[count] = node;
            count++;
        }
        //std::cout << "ROW " << (matrix[count - 1]).row << ". COL " << (matrix[count - 1]).col << "\n";
    }
    // Matrix is currently full -> realloc and copy 
    else {
        int newSize = round(count * 1.5);
        cooNode* newMatrix = new cooNode[newSize];

        std::memcpy(newMatrix, matrix, count * sizeof(cooNode));

        capacity = newSize;
        //std::cout << "Count: " << count << ". Capacity: " << capacity << "\n";
        delete [] matrix;
        matrix = newMatrix;
        // Add node to newly allocated memory space
        if(node.row == matrix[count-1].row && node.col == matrix[count-1].col) {
            matrix[count-1] = node;
        }
        else {
            matrix[count] = node;
            count++; 
        }
        //std::cout << "ROW " << (matrix[count - 1]).row << ". COL " << (matrix[count - 1]).col << " Val " << (matrix[count - 1]).val << "\n";
    } 
}

int COOArray::length() const {
    return count;
}

void COOArray::setLength(int len) {
    count = len;
}

cooNode* COOArray::getMatrix() {
    return matrix;
}

int COOArray::getRows() const {
    return matrixRow;
}

int COOArray::getCols() const {
    return matrixCol;
}

void COOArray::setDimensions(int row, int col) {
    //printf("Setting dimensions to %d by %d\n", row, col);
    matrixRow = row;
    matrixCol = col;
}

bool COOArray::isCompatibleWith(COOArray &coo) {
    return (this->matrixRow == coo.getRows()) && (this->matrixCol == coo.getCols());
}

bool COOArray::isCompatibleForMultWith(COOArray &coo) {
    return this->matrixCol == coo.getRows();
}

bool COOArray::isSorted() {
    for(int i = 0; i < count-1; i++){
        if(matrix[i].row > matrix[i+1].row || (matrix[i].row == matrix[i+1].row && matrix[i].col > matrix[i+1].col)) {
            return false;
        } 
    }
    return true;
}

/**
 * Sort coordinate array using quicksort
*/ 
void COOArray::sort(int l, int r) {
    countComparisons++;
    if(l >= r)
        return;
    
    int part = partition(l, r);

    sort(l, part - 1);
    sort(part + 1, r);
}

int COOArray::partition(int l, int r) {
    cooNode pivot = matrix[r];
    int index = l - 1;
    for(int i = l; i < r; i++) {
        if(matrix[i].row < pivot.row) {
            index++;
            cooNode temp = matrix[index];
            matrix[index] = matrix[i];
            matrix[i] = temp;
            countComparisons++;
        }
        else if((matrix[i].row == pivot.row && matrix[i].col < pivot.col)) {
            index++;
            cooNode temp = matrix[index];
            matrix[index] = matrix[i];
            matrix[i] = temp;
            countComparisons += 4;
        }
        else
            countComparisons += 4;
    }
    cooNode temp = matrix[index+1];
    matrix[index+1] = matrix[r];
    matrix[r] = temp;
    return index+1;
}

void COOArray::bubbleSort() {
    for(int i = 1; i < count; i++) {
        for(int j = 0; j < count - 1; j++) {
            //printf("This row: %d. Next Row: %d\nThis col: %d. Next Col: %d\n", matrix[j].row, matrix[j+1].row, matrix[j].col, matrix[j+1].col);
            if(matrix[j].row > matrix[j+1].row) {
                cooNode temp = matrix[j];
                matrix[j] = matrix[j+1];
                matrix[j+1] = temp;
                countComparisons++;
            }
            else if(matrix[j].row == matrix[j+1].row && matrix[j].col > matrix[j+1].col) {
                cooNode temp = matrix[j];
                matrix[j] = matrix[j+1];
                matrix[j+1] = temp;
                countComparisons += 4;
            }
            else
                countComparisons += 4;
        }
    }
    //print();
}

void COOArray::writeToBigO(std::string algorithm) {
    std::fstream bigO;
    bigO.open("bigO.txt", fstream::app|fstream::out);
    if(bigO.is_open()) {
        int bigOCount = 4 * pow(count, 2);
        bigO << algorithm << "\t\t" << count << "\t\t" << countComparisons << "\t\t" << bigOCount << " \n";
    }
    else
       std::cout << "File bigO.txt failed to open\n";
    std::cout << "Count Comparisons: " << countComparisons << " for n: " << count << "\n";
    bigO.close();
}

int COOArray::seqFind(int row, int col) {
    for(int i = 0; i < count; i++){
        if(matrix[i].row == row && matrix[i].col == col)
            return i;
    }
    return -1;
}
 
int COOArray::binFind(int row, int col) {
    int lower = 0;
    int upper = count;
    while(lower <= upper) {
        int pos = lower/2 + upper/2;
        if(matrix[pos].row == row && matrix[pos].col == col)
            return pos;
        else if(matrix[pos].row > row || (matrix[pos].row == row && matrix[pos].col > col))
            upper = pos - 1;
        else
            lower = pos + 1;
    }
}

void COOArray::writeToFile(std::string output) {
    FILE *outputWriter;
    outputWriter = fopen(output.c_str(), "w");
    if(outputWriter != NULL) {
        fprintf(outputWriter,"%s","#Matrix C=A+B\n");
        for(int i = 0; i < count; i++) {
            fprintf(outputWriter, "%i %i %.2f\n", matrix[i].row, matrix[i].col, matrix[i].val);  
        }
        if((matrix[count - 1].row != matrixRow) || (matrix[count-1].col != matrixCol)) {
             fprintf(outputWriter, "%i %i 0.00\n", matrixRow, matrixCol);
        }
    }
    else {
        std::cout << "Failed to open " << output << " for file write.\n";
    }
}

void COOArray::recSum(COOArray &coo_b, int index_a, int index_b, COOArray &finalMatrix) {
    //printf("1: %d, 2: %d\n",this->count, coo_b.length());
    finalMatrix.setDimensions(matrixRow, matrixCol);
    if(count == 0)
        return;
    if(index_a < this->count && index_b < coo_b.length()) {
        cooNode current_a = matrix[index_a];
        cooNode current_b = coo_b.at(index_b);
        int large_row = (current_a.row < current_b.row) ? current_b.row : current_a.row; 
        if(current_a.row == current_b.row && current_a.col == current_b.col) {
            if(current_a.val + current_b.val != 0.0) {
                cooNode temp;
                temp.row = current_a.row;
                temp.col = current_a.col;
                temp.val = current_a.val + current_b.val;
                //printf("Row: %d, Col: %d, Val: %.2f\n", temp.row, temp.col, temp.val);
                index_a++;
                index_b++;
                finalMatrix.append(temp);
                recSum(coo_b, index_a, index_b, finalMatrix);
            }
        }
        else if(current_a.col < current_b.col && current_a.row == current_b.row) {
            finalMatrix.append(matrix[index_a]);
            //printf("Row: %d, Col: %d, Val: %.2f\n", matrix[index_a].row, matrix[index_a].col, matrix[index_a].val);
            index_a++;
            recSum(coo_b, index_a, index_b, finalMatrix);
        }
        else if(current_b.col < current_a.col && current_a.row == current_b.row) {
            finalMatrix.append(coo_b.at(index_b));
            //printf("Row: %d, Col: %d, Val: %.2f\n", coo_b.at(index_b).row, coo_b.at(index_b).col, coo_b.at(index_b).val);
            index_b++;
            recSum(coo_b, index_a, index_b, finalMatrix);
        }
        else if(current_a.row < large_row) {
            recSumInner(*this, finalMatrix, index_a, large_row);
            recSum(coo_b, index_a, index_b, finalMatrix);
        }
        else if(current_b.row < large_row) {
            recSumInner(coo_b, finalMatrix, index_b, large_row);
            recSum(coo_b, index_a, index_b, finalMatrix);
        }
    }
    else if(index_a < this->count) {
        finalMatrix.append(matrix[index_a]);
        //printf("Row: %d, Col: %d, Val: %.2f\n", matrix[index_a].row, matrix[index_a].col, matrix[index_a].val);
        index_a++;
        recSum(coo_b, index_a, index_b, finalMatrix);
    }
    else if(index_b < coo_b.length()) {
        finalMatrix.append(coo_b.at(index_b));
        //printf("Row: %d, Col: %d, Val: %.2f\n", coo_b.at(index_b).row, coo_b.at(index_b).col, coo_b.at(index_b).val);
        index_b++;
        recSum(coo_b, index_a, index_b, finalMatrix);
    }
    else {
        //std::cout << "end\n";
        return;
    }
}

void COOArray::recSumInner(COOArray &coo_in, COOArray &coo_out, int &index, int end_row) {
    if((coo_in.at(index).row >= end_row) || (coo_in.at(index).row == -1)) {
        return;
    }
    else {
        coo_out.append(coo_in.at(index));
        index++;
        recSumInner(coo_in, coo_out, index, end_row);
    }
}


#endif
