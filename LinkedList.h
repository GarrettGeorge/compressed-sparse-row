#ifndef LinkedList_h
#define LinkedList_h

#include <iostream>

// Struct representing sparse matrix entry data and pointers referring
// to relative location withing the LinkedList
struct linkedNode {
        int row;
        int col;
        float val;
        linkedNode *next;
        linkedNode *previous;
        
};

class LinkedList {
private:
    int count; // Number of entries in linkedList. Accessed via length()
    int maxRow;
    int maxCol;
    linkedNode *header; // Pointer where header->next is first entry
    linkedNode *footer; // Pointer where footer->next = NULL and last entry
public:
    LinkedList();
    ~LinkedList();
    int length();
    void setDimensions(int, int);
    int getRows();
    int getCols();
    linkedNode* getHeader();
    bool isCompatibleWith(LinkedList&);
    bool isSorted();
    void sort();
    void append(linkedNode*);
    void loopSum(LinkedList&, LinkedList&);
    void recSum(linkedNode*&, linkedNode*&, LinkedList&);
    void writeToFile(std::string);
    void print();
};

// Allocates header in memory and points to footer (must be free'd)
// Initiliazes all members to default values.
LinkedList::LinkedList() {
    header = new linkedNode;
    header->next = footer;
    header->previous = NULL;
    footer = NULL;
    count = 0;
    maxRow = 0;
    maxCol = 0;
}

// If non-empty LinkedList, delete all entries allocated using new.
// Otherwise, header is the only allocated block with new that needs handling.
LinkedList::~LinkedList() {
    linkedNode *current;
    if(count > 0) {
        while(header != NULL) {
            current = header;
            header = header->next;
            delete current;
        }
        footer = NULL;
        delete header;
        header = NULL;
    }
    else {
        delete header;
        header = NULL;    
    }
}

int LinkedList::length() {
   return count;
}

void LinkedList::setDimensions(int row, int col) {
    maxRow = row;
    maxCol = col;
}

int LinkedList::getRows() {
    return maxRow;
}

int LinkedList::getCols() {
    return maxCol;
}

linkedNode* LinkedList::getHeader() {
    return header;
}

/**
 * Verify if LinkedLists are compatible for sparse matrix addition
 * @param - LinkedList to be check against this
 * @return True if rows and columns of this and other are equal
*/ 	
bool LinkedList::isCompatibleWith(LinkedList &other) {
    return (this->maxRow == other.getRows() && this->maxCol == other.getCols());
}

/**
 * Check if LinkedList is currently sorted
 * @return - True if node n is strictly less than or equal to node n+1
*/
bool LinkedList::isSorted() {
    if(count == 0)
        return true;
    linkedNode *current = header;
    linkedNode *next = current->next;
    while(current->next) {
        if(current->row > next->row) {
            return false;
        }
        else if(current->row == next->row || current->col > next->col) {
            return false;
        }
        current = current->next;
        next = current->next;
    }
    std::cout << "true\n";
    return true;
}

/**
 * Sorts LinkedList in ascending order if 'this' is found to unsorted via 
 * bubble sort and value swaps
*/ 
void LinkedList::sort() {
    linkedNode *current = header->next;
    linkedNode *next = current->next;
    for(int i = 0; i < count; i++) {
         while(current->next != NULL){
          if((current->row > next->row) || (current->row == next->row && current->col > next->col)) {
                int row = current->row;
                int col = current->col;
                float val = current->val;
                current->row= next->row;
                current->col = next->col;
                current->val = next->val;
                next->row = row;
                next->col = col;
                next->val = val;
            }
            current = current->next;
            next = next->next;
        }
        current = header->next;
        next = current->next;
    }
    current = NULL;
    next = NULL;    
}

/**
 *  Adds new linkedNode to end of LinkedList
 *  @param - pointer to linkedNode to be added to LinkedList
*/  
void LinkedList::append(linkedNode *e) {
    linkedNode *entry = e;
    if(footer == NULL) {
        header->next = entry;
        footer = entry;
        footer->next = NULL;
        footer->previous = header;
        count++;
    }
    else {
        footer->next = entry;
        entry->previous = footer;
        entry->next = NULL;
        footer = entry;
        count++;
    }
    entry = NULL;
}

/**
 * Matrix addition of this LinkedList and some LinkedList B using loops
 * @param bList - LinkedList to be added with this LinkedList
 * @param finalList - LinkedList to contain the results of A+B
*/
void LinkedList::loopSum(LinkedList &bList, LinkedList &finalList) {
    finalList.setDimensions(this->maxRow, this->maxCol);
    linkedNode *current_a = NULL;
    linkedNode *current_b = NULL;
    if(count > 0)
        current_a = header->next;
    if(bList.length() > 0) 
         current_b = (bList.getHeader())->next;
    printf("a count: %d, b count: %d\n", count, bList.length());
    while(current_a && current_b) {
        if(current_a->row == current_b->row && current_a->col == current_b->col) {
            float val = current_a->val + current_b->val;
            if(val != 0.0) {
                linkedNode *temp = new linkedNode;
                temp->row = current_a->row;
                temp->col = current_b->col;
                temp->val = val;
                finalList.append(temp);
            }
            current_a = current_a->next;
            current_b = current_b->next;
        }
        else if(current_a->row < current_b->row || (current_a->row == current_b->row && current_a->col < current_b->col)) {
            linkedNode *entry = new linkedNode;
            *entry = *current_a;
            finalList.append(entry);
            current_a = current_a->next;
        }
        else if(current_b->row < current_a->row || (current_b->row == current_a->row && current_b->col < current_a->col)) {
            linkedNode *entry = new linkedNode;
            *entry = *current_b;
            finalList.append(entry);
            current_b = current_b->next;
        }
    }
    while(current_a) {
        linkedNode *entry = new linkedNode;
        *entry = *current_a;
        finalList.append(entry);
        current_a = current_a->next;
    }
    while(current_b) {
        linkedNode *entry = new linkedNode;
        *entry = *current_b;
        finalList.append(entry);
        current_b = current_b->next;
    }
}

/**
 * Matrix addition of this LinkedList and some LinkedList B using recursion
 * @param bList - LinkedList to be added with this LinkedList
 * @param finalList - LinkedList to contain the results of A+B
*/
void LinkedList::recSum(linkedNode *&a, linkedNode *&b, LinkedList &finalList) {
     std::cout << "rec sum\n";
     linkedNode *current_a = a;
     linkedNode *current_b = b;
    
     if(current_a && current_b) {
         float val = current_a->val + current_b->val;
         if(current_a->row == current_b->row && current_a->col == current_b->col) { 
             if(val != 0.0) {
                 linkedNode *temp = new linkedNode;
                 temp->val = val;
                 temp->row = current_a->row;
                 temp->col = current_a->col;
                 finalList.append(temp);
                 current_a = current_a->next;
                 current_b = current_b->next;
                 recSum(current_a, current_b, finalList);
             }
         }
         else if(current_a->row < current_b->row || (current_a->row == current_b->row && current_a->col < current_b->col)) {
            linkedNode *entry = new linkedNode;
            *entry = *current_a;
            finalList.append(entry);
            current_a = current_a->next;
            recSum(current_a, current_b, finalList);
         }
         else if(current_b->row < current_a->row || (current_b->row == current_a->row && current_b->col < current_a->col)) {
            linkedNode *entry = new linkedNode;
            *entry = *current_b;
            finalList.append(entry);
            current_b = current_b->next;
            recSum(current_a, current_b, finalList);
         }
     }
     else if(current_a) {
         linkedNode *temp = new linkedNode;  
         temp->val = current_a->val; 
         temp->row = current_a->row;
         temp->col = current_a->col;
         finalList.append(temp);
         current_a = current_a->next;
         recSum(current_a, current_b, finalList);
     }
     else if(current_b) {
         linkedNode *temp = new linkedNode;
         temp->val = current_b->val; 
         temp->row = current_b->row;
         temp->col = current_b->col;
         finalList.append(temp);
         current_b = current_b->next;
         recSum(current_a, current_b, finalList);
     }
}

/**
 * Writes contents of this LinkedList to specified file name
 * @param - String of file name to be written into
*/ 
void LinkedList::writeToFile(std::string out) {
    FILE *outputWriter;
    outputWriter = fopen(out.c_str(), "w");
    if(outputWriter) {
        fprintf(outputWriter, "%s", "#Matrix C=A+B\n");
        //fprintf(outputWriter, "%s", "#### SPARSE MATRIX ####\n");
        linkedNode *current = header->next;
        for(int i = 0; i < count; i++) {
            std::cout << "writing\n";
            fprintf(outputWriter, "%d %d %.2f\n", current->row, current->col, current->val);
            current = current->next;
        }
        //printf("Dimensions: %dx%d\n", maxRow, maxCol);
        if(count == 0) {
            fprintf(outputWriter, "%d %d 0.00\n", maxRow, maxCol);
            return;
        }
        if((footer->row != maxRow) || (footer->col != maxCol)) {
            fprintf(outputWriter, "%d %d 0.00\n", maxRow, maxCol);
        }
    }
    else {
        std::cout << "Failed to open <" << out << ">for file write\n";
    }
}

void LinkedList::print() {
    linkedNode *current = header->next;
    while(current != NULL) {
        printf("Row: %d | Col: %d | Val: %.2f\n", current->row, current->col, current->val);
        current = current->next;   
    }
}
#endif
