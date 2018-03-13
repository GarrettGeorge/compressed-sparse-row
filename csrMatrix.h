#include <iostream>
#include <vector>
#include <algorithm>
#include "COOArray.h"

static std::vector<bool> unionize(std::vector<bool> lhs, std::vector<bool> rhs, int &past, int &current) {
    past = current;
    for(int i = 0; i < lhs.size(); i++) {
        if(lhs.at(i) ^ rhs.at(i)) {
            lhs.at(i) = true;
            current++;
        }
    }
    return lhs;
}

class csrMatrix {
private:
    float * a_array;
    int * ja_array;
    int * ia_array;
    int nnz;
    int rows;
    int cols;
    int k;
public:
    csrMatrix(const COOArray&);
    csrMatrix(int, int, int);
    ~csrMatrix();
    COOArray loopSum(csrMatrix&);
    void mult(csrMatrix&, csrMatrix&);
    std::vector<bool> multiplyWithVector(std::vector<bool>);
    void reachabilityWithMult(std::vector<bool>&);
    float aArray(int);
    void setAArrayAt(int, float);
    int jaArray(int);
    void setJAArrayAt(int, int);
    int iaArray(int);
    void setIAArrayAt(int, int);
    int iaDiff(int);
    void setNNZ(int);
    int getNNZ();
    int getRows();
    int getCols(); 
    void writeToFile(std::string);
    void print();

    struct SparseAccumulator {
        // Dense vector of calculated values
        std::vector<float> w;
        // Dense vector indicating if current entry is nonzero
        std::vector<bool> b;
        // Unordered list of column indices 
        std::vector<int> ls;
        
        /**
          *  Reset dense vectors to default
          * @param - Rows of A
        */    
        void setSPA(int n) {
            w = std::vector<float>(n,0.0);
            b = std::vector<bool>(n,false);
            ls = std::vector<int>(0,0);
        };
        
        /**
         * Push localized calculatd value
         * @param - Calculated value of specific Ai,j * Bj,i
         * @param - Column index in current row
        */   
        void scatterSPA(float val, int pos) {
            printf("Size of b: %d, pos: %d\n", b.size(), pos);
            if(b.at(pos) == false) {
                w.at(pos) = val;
                b.at(pos) = true;
                ls.push_back(pos);
            }
            else {
                w.at(pos) = w.at(pos) + val;
            }
        };

        /**
         * Load sparse accumulator data into CSR matrix C
         * @param - csr matrix C to be updated
         * @param - row index corresponding to current row of C
        */  
        void gatherSPA(csrMatrix &csr, int row_index) {
            sortLS(0,ls.size()-1);
            int nnz_curr = csr.getNNZ();
            int nnz_rowi = 0;
            if(!ls.empty()) {
                int col_ref = 0;
                while(col_ref < ls.size()) {
                    csr.setJAArrayAt(nnz_curr + nnz_rowi, ls.at(col_ref)+1);
                    csr.setAArrayAt(nnz_curr + nnz_rowi, w.at(ls.at(col_ref)));
                    nnz_rowi++;
                    col_ref++;
                }
            }
            csr.setNNZ(nnz_curr + nnz_rowi);
            //std::cout << "gatherSPA row_index: " << row_index << "\n";
            csr.setIAArrayAt(row_index, nnz_curr);
            csr.setIAArrayAt(row_index + 1, csr.getNNZ());
        };

        void sortLS(int l, int r) {
            if(l >= r)
            return;
     
            int part = partition(l, r);

            sortLS(l, part - 1);
            sortLS(part + 1, r);
        };

        int partition(int l, int r) {
            int pivot = ls.at(r);
            int index = l - 1;
            for(int i = l; i < r; i++) {
                if(ls.at(i) <= pivot) {
                    index++;
                    int temp = ls.at(index);
                    ls.at(index) = ls.at(i);
                    ls.at(i) = temp;
                }
            }
            int temp = ls.at(index+1);
            ls.at(index+1) = ls.at(r);
            ls.at(r) = temp;
            return index+1;
        };        
    };
};

csrMatrix::csrMatrix(const COOArray &coo) {
    // Instantiate memory for all 3 array sizes
    int len = coo.length();
    nnz = len;
    float *aAlloc = new float[len];
    a_array = aAlloc;
    int *jaAlloc = new int[len];
    ja_array = jaAlloc;
    this->rows = coo.getRows();
    int cols = coo.getCols();
    this->cols = coo.getCols();
    int *iaAlloc = new int[this->rows+1];
    ia_array = iaAlloc;
    
    // Convert coordinate to compressed sparse row
    int current_nnz = 0;
    int current_row = 1;
    int row_ptr = 0;
    ia_array[0] = 0;
    for(int i = 1; i < (coo.at(0).row); i++) {
        ia_array[i] = 0;
    }
    if(coo.at(0).row == -1) {
        for(int i = 0; i < coo.length(); i++) {
            ia_array[i] = 0;    
        }
    }
    else {
        for(int i = 0; i < coo.length(); i++) {
            cooNode node = coo.at(i);
            a_array[i] = node.val;
            ja_array[i] = node.col;
            if(current_row  == node.row) {
                row_ptr++;
                ia_array[current_row] = row_ptr;
            }
            // Next row
            else {
                if((current_row + 1) == node.row) {
                    current_row++;
                    row_ptr++;
                    ia_array[current_row] = row_ptr;
                }
            else {
                current_row++;
                for(current_row; current_row < node.row; current_row++) {
                    ia_array[current_row] = ia_array[current_row - 1];
                }
                i--;
            }
        }
    }
    }
    int last_coo_row = (coo.at(coo.length() - 1)).row;
    if(last_coo_row < this->rows){
        for(int i = last_coo_row; i < this->rows; i++) {
            ia_array[i+1] = ia_array[i];
        }
    }
}

csrMatrix::csrMatrix(int m, int n, int est_nnz) {
    nnz = 0;
    float *aAlloc = new float[est_nnz];
    a_array = aAlloc;
    int *jaAlloc = new int[est_nnz];
    ja_array = jaAlloc;
    for(int i = 0; i < est_nnz; i++) {
        a_array[i] = 0;
        ja_array[i] = 0;
    }
    rows = m;
    cols = n;
    int *iaAlloc = new int[m+1];
    ia_array = iaAlloc;
    std::cout << "est _nnz  " << est_nnz << "\n";
}

csrMatrix::~csrMatrix() {
    delete  a_array;
    a_array = NULL;
    delete  ja_array;
    ja_array = NULL;
    delete  ia_array;
    ia_array = NULL;
}

	void csrMatrix::setNNZ(int n) {
	    nnz = n;
	}

	int csrMatrix::getNNZ() {
	    return nnz;
} 

int csrMatrix::getRows() {
    return rows;
}

int csrMatrix::getCols() {
    return cols;
} 
// Taken from SciPy 's generic binary operator algorithm for canonical csr form
// https://github.com/scipy/scipy/blob/master/scipy/sparse/sparsetools/csr.h
COOArray csrMatrix::loopSum(csrMatrix &csr_b) {
    int size = (this->getNNZ() >= csr_b.getNNZ()) ? this->nnz : csr_b.getNNZ();
    COOArray final_matrix(size);
    final_matrix.setDimensions(rows-1, cols);
    if(size == 0)
        return final_matrix;
    
    for(int ia_index = 0; ia_index < (rows - 1); ia_index++) {
        int pos_a = iaArray(ia_index);
        int end_a = iaArray(ia_index + 1);
        int pos_b = csr_b.iaArray(ia_index);
        int end_b = csr_b.iaArray(ia_index + 1);
        //std::cout << "ia_index: " << ia_index << "ia_next_a" << ia_next_a << "\n";
        //std::cout << "pos_b: " << pos_b << " end_b: " << end_b << "\n";
        while((pos_a < end_a) && (pos_b < end_b)) {
            int ja_entry_a = this->jaArray(pos_a);
            int ja_entry_b = csr_b.jaArray(pos_b);
            // Check indicates a match, i.e. addition needs to be 
            // performed with a nonzero entry from both A and B
            if(ja_entry_a == ja_entry_b) {
                float val = this->aArray(pos_a) + csr_b.aArray(pos_b);
                if(val != 0.0) {
                    cooNode temp;
                    temp.row = ia_index + 1;
                    temp.col = ja_entry_a + 1;
                    temp.val = val;
                    final_matrix.append(temp);
                    //std::cout << temp.val << "\n";             
                }
                pos_a++;
                pos_b++;
            }
            // Check indicates no match and ja_entry_a does not 
            // have the potential to match
            else if(ja_entry_a < ja_entry_b) {
                cooNode temp;
                temp.row = ia_index + 1;
                temp.col =  ja_entry_a + 1;
                temp.val = this->aArray(pos_a);
                final_matrix.append(temp);
                //std::cout << temp.val << "\n";
                pos_a++;
            } 
            // ja_entry_b does not have the potential
            // to match with current column of A
            // ja_entry b < ja_entry_a
            else {
                cooNode temp;
                temp.row = ia_index + 1;
                temp.col = ja_entry_b + 1;
                temp.val = csr_b.aArray(pos_b);
                final_matrix.append(temp);
                //std::cout << temp.val << "\n";
                pos_b++;
            }
        }
        // left over column indices when nnz of A and B
        // are different
        while(pos_a < end_a) {
            cooNode temp;
            temp.row = ia_index + 1;
            temp.col = this->jaArray(pos_a) + 1;
            temp.val = this->aArray(pos_a);
            final_matrix.append(temp);
            //std::cout << temp.val << "\n";
            pos_a++;
        } 
        while(pos_b < end_b) {
            cooNode temp;
            temp.row = ia_index + 1;
            temp.col = csr_b.jaArray(pos_b) + 1;
            temp.val = csr_b.aArray(pos_b);
            final_matrix.append(temp);
            //std::cout << temp.val << "\n";
            pos_b++;
        }
    }
    std::cout << "\n";
    return final_matrix;
}
/**
 * Multiply two matrices in CSR form
 * @param - matrix B to be multiplied
 * @param - matrix C to receive calculated values
*/ 
void csrMatrix::mult(csrMatrix &b, csrMatrix &c) {
    int m = c.getRows(), n = c.getCols();
    
    SparseAccumulator spa;
    // Initialize sparse accumulator to default
    spa.setSPA(n);
    c.setIAArrayAt(0,0);
    
    // Loop through rows of A
    for(int i = 0; i < m; i++) {
        // Loop through number of non zero's in row i
        for(int k = this->iaArray(i); k < this->iaArray(i+1); k++) {
            // Using column indices of A, iterate through row entries in B
            for(int j = b.iaArray((this->jaArray(k))-1); j < b.iaArray(this->jaArray(k)); j++) {
                float val = this->aArray(k) * b.aArray(j);
                std::cout << "j: " << j << " | j end: " << b.iaArray(this->jaArray(k)) << " | nnz: " << b.getNNZ() << " | pos: " << b.jaArray(j) << "\n";
                spa.scatterSPA(val,b.jaArray(j)-1);
            }
        }
        spa.gatherSPA(c,i);
        spa.setSPA(n);
    }
}

std::vector<bool> csrMatrix::multiplyWithVector(std::vector<bool> s) {
    std::vector<bool> y = s;

    for(int i = 0; i < this->rows; i++) {
        int entry = false;
        
        for(int k = this->ia_array[i]; k < this->ia_array[i+1]; k++){
            int a_array_bool = (this->a_array[k] == 1);
            //std::cout << "ja_array: " << this->ja_array[k]-1 << "\n";
            //entry = entry || (a_array_bool && s.at(this->ja_array[k]-1));
            y.at(this->ja_array[k]-1) = true;
        }
        //std::cout << "entry at: " << i << " is " << entry << "\n";
        //y.at(i) = entry;
    }
    return y;
}	

void csrMatrix::reachabilityWithMult(std::vector<bool> &s) {
    int current_k = 0, past_k = -1;
    do {
        s = unionize(s, this->multiplyWithVector(s), past_k, current_k);
        printf("current_k = %d, past_k = %d\n", current_k, past_k);
    }while(current_k != past_k);
}

float csrMatrix::aArray(int a_index) {
    return a_array[a_index];
}

void csrMatrix::setAArrayAt(int a_index, float val) {
    a_array[a_index] = val;
}

int csrMatrix::iaArray(int ia_index) {
    return ia_array[ia_index];
}

void csrMatrix::setIAArrayAt(int ia_index, int val) {
    ia_array[ia_index] = val;
}

int csrMatrix::jaArray(int ja_index) {
    //if(ja_index >= nnz)
        
    return ja_array[ja_index];
}

void csrMatrix::setJAArrayAt(int ja_index, int val) {
    ja_array[ja_index] = val;
}

int csrMatrix::iaDiff(int ia_index) {
    return ia_array[ia_index] - ia_array[ia_index];
}

void csrMatrix::print() {
    std::cout << "a_array: [";
    for(int i = 0; i < nnz; i++) {
        std::cout << a_array[i] << ", ";
    }
    std::cout << "\nja_array: [";
    for(int i = 0; i < nnz; i++) {
        std::cout << ja_array[i] << ", ";
    }
    std::cout << "\nia_array: [";
    for(int i = 0; i <= rows; i++) {
        std::cout << ia_array[i] << ", ";
    }
    std::cout << "\n";
    printf("Rows: %d\n", rows);
}

void csrMatrix::writeToFile(std::string output) {
    FILE *outputWriter;
    outputWriter = fopen(output.c_str(), "w");
    if(outputWriter != NULL) {
        std::cout << "writing to file\n";
        for(int i = 0; i < rows; i++) {
            //std::cout  << "i+1: " << i+1 << "\n";
            //std::cout << "ia_array: " << ia_array[i] << " to " << ia_array[i+1] << "\n";
            for(int j = ia_array[i]; j < ia_array[i+1]; j++) {
                fprintf(outputWriter,"%d %d %.2f\n",i+1,ja_array[j],a_array[j]);
            } 
        }
        if(ia_array[rows] == ia_array[rows-1] && ja_array[nnz-1] != cols)
            fprintf(outputWriter, "%d %d 0.00\n",rows, cols);
        if(nnz == 0)
            fprintf(outputWriter, "%d %d 0.00\n", rows, cols);
    }
    else {
        std::cout << "fopen failed\n";
    }
}
