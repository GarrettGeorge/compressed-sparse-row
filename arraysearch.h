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
