/**
 * Sort coordinate array using quicksort
*/ 
void COOArray::sort(int l, int r) {
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
        if(matrix[i].row < pivot.row || (matrix[i].row == pivot.row && matrix[i].col < pivot.col)) {
            index++;
            cooNode temp = matrix[index];
            matrix[index] = matrix[i];
            matrix[i] = temp;
        }
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
            }
            else if(matrix[j].row == matrix[j+1].row && matrix[j].col > matrix[j+1].col) {
                cooNode temp = matrix[j];
                matrix[j] = matrix[j+1];
                matrix[j+1] = temp;
            }
        }
    }
    //print();
}
