#include <iostream>
#include "COOArray.h"

struct GraphNode {
     int value;
     GraphNode *next = NULL;
     GraphNode *previous = NULL;
};

struct GraphList {
    GraphNode *header = NULL;
    GraphNode *footer = NULL;
    
    int size = 0; 
    
    void append(int val) {
        GraphNode *temp = new GraphNode;
        temp->value = val;
        if(!header) {
            header = temp;
            footer = temp;
        }
        else {
            footer->next = temp;
            footer = temp;
        }
        size++;
    }
};

struct Stack {
   int count = 0;
   GraphNode *header = NULL;
   GraphNode *footer = NULL;
   void push(int val) {
       GraphNode *temp = new GraphNode;
        temp->value = val;
        if(footer == NULL) {
            std::cout << "Pushing onto empty\n";
            header = temp;
            footer = temp;
            std::cout << "setting footer pointers\n";
            footer->next = NULL;
            footer->previous = NULL;
        }
        else {
            std::cout << "Pushing onto non empty\n";
            footer->next = temp;
            temp->previous = footer;
            temp->next = NULL;
            footer = temp;
        }
        count++;
        temp = NULL;
   } 

   void pop() {
       GraphNode *temp = footer;
       if(count == 1) {
           delete temp;
           header = NULL;
           footer = NULL;
           count--;
           return;
       }
       footer = footer->previous;
       footer->next = NULL;
       delete temp;
       count--;
   }

   int first() {
       if(count > 0)
           return footer->value;
       return 0;
   }

   bool empty() {
       std::cout << "count: " << count << "\n"; 
       return count == 0;
   }
};

class Graph {
private:
    GraphList *graphNodes;
    int k = 0;
    int size = 0;
    int source;
    std::vector<int> vertices;
public:
    Graph(COOArray &coo, int source) {
        size = coo.getRows();        
        this->source = source;

        GraphList *newAlloc = new GraphList[coo.getRows()];
        graphNodes = newAlloc;
 
        for(int i = 0; i < size; i++) {
             graphNodes[i].header = NULL;
             graphNodes[i].footer = NULL;
        }
        for(int i = 0; i < coo.length(); i++) {
            graphNodes[coo.at(i).row-1].append(coo.at(i).col);
        } 
    }
    void print() {
        for(int i = 0; i < size; i++) {
            GraphNode *current = graphNodes[i].header;
            while(current) {
                std::cout << "GraphNode value: " << current->value << "\n";
                current = current->next;
            }
        }
    }
    std::vector<bool> dfs(std::vector<bool> s0, int vertex) {
       std::vector<bool> s = s0;

       int v = vertex-1; 
       //std::cout << "Setting s to true at = " << vertex << "\n";
       s.at(v) = true;
       GraphNode *current = graphNodes[v].header;
       while(current) {
           if(!s.at(current->value - 1) || (current->value == source && vertices.size() > 0)) {
               //printf("current->value: %d | source: %d\n", current->value, source);
               vertices.push_back(current->value);
               s = dfs(s, current->value);           
           }
           current = current->next;
       }
       return s; 
    }

    std::vector<bool> dfs_iterative(std::vector<bool> s0, int vertex) {
        std::vector<bool> s = s0;
        Stack stack;
        std::cout << "size of s: " << s.size() << "\n"; 
        stack.push(vertex);
        std::cout << "finished pushing source\n";
        while(!stack.empty()) {
            std::cout << "Stack non empty\n";
            vertex = stack.first();
            stack.pop();
            
            if(!s.at(vertex-1)) {
                s.at(vertex-1) = true;
            }
            GraphNode *current = graphNodes[vertex-1].header;
            while(current) {
                std::cout << "current value: " << current->value << "\n";
                if(!s.at(current->value - 1))
                    stack.push(current->value);
                current = current->next; 
            }
        }
        return s;
    }

    std::vector<int> getVertices() {
        return vertices;
    }
};
