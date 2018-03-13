//
//  FileManager.h
//  sparse
//
//  Created by Admin on 9/5/16.
//  Copyright © 2016 Admin. All rights reserved.
//
#ifndef FileManager_h
#define FileManager_h

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "COOArray.h"
#include "LinkedList.h"

class FileManager {
private:
    std::string comment;
    std::fstream fileRead;
    std::string fileName;
public:
    FileManager();
    FileManager(std::string fileToRead);
    ~FileManager();
    void openFile(std::string fileToRead);
    void getContentAsMatrix(COOArray&);
    void getContentAsMatrix(LinkedList&);
    bool isValidEntry(std::string);
    void parseRawString(std::string, COOArray&);
    void parseRawString(std::string, LinkedList&);
};

FileManager::FileManager() {
}

FileManager::FileManager(std::string fileToRead) {
    comment = "";
    fileRead.open(fileToRead);
    if(!fileRead.is_open()) {
        std::cout << "File " << fileToRead << " failed to open. Try again with `.open` before `.getContentAsMatrix`.\n";
    }
    else {
        fileName = fileToRead;
    }
    fileRead.close();
}

FileManager::~FileManager() {
    //std::cout << "FileManager " << fileName  << " Destructor.\n";
    //fileRead.close();
}

void FileManager::openFile(std::string fileToOpen) {
    fileRead.open(fileToOpen);
    if(!fileRead.is_open()) {
        std::cout << "File " << fileToOpen << " failed to open.\n";
    }
    else {
        fileName = fileToOpen;
    }
    fileRead.close();
}

void FileManager::getContentAsMatrix(COOArray &coo) {
    fileRead.open(fileName);
    if(fileRead.is_open()) {
        while(true) {
            // Check if line should be processed as a comment
            if(fileRead.peek() == '#') {
                std::getline(fileRead, comment);
                comment = "";
            }
            // Lines assumed to be not comments but may contain bad entries
            else {
                std::string testEntry;
                std::getline(fileRead, testEntry);
                // Parse string in COO entry with error checks
                parseRawString(testEntry, coo);
            }
            if(fileRead.eof()) {
                break;
            }
        }
    }
    else {
        std::cout << "File " << fileName << " failed to open.\n";
    }
    fileRead.close();
}

void FileManager::getContentAsMatrix(LinkedList &list) {
    fileRead.open(fileName);
    if(fileRead.is_open()) {
        while(true) {
            if(fileRead.peek() == '#') {
                std::getline(fileRead, comment);
                comment="";
            }
            else {
                std::string testEntry;
                std::getline(fileRead, testEntry);
                parseRawString(testEntry, list);
            }
            if(fileRead.eof()) {
                break;
            }
        }
    }
    else {
        std::cout << "File " << fileName << " failed to open.\n";
    }    
    fileRead.close();
}


void FileManager::parseRawString(std::string entry, COOArray& coo) {
    std::stringstream ss;
    ss << entry;
    cooNode *node = new cooNode;
    int index = 0;
    bool hasZero = false;
    while(true){
        float temp;
        ss >> temp;
        if((ss.rdstate() & std::stringstream::failbit) != 0) {
            //std::cout << "Fail entry\n";
            delete node;
            break;
        }
        else {
            switch(index){
                case 0:
                    //std::cout << "row " << temp << "\n";
                    node->row = (int)temp;
                    if(node->row == 0) {
                        delete node;
                        node = NULL;
                        return;
                    }
                    index++;
                    break;
                case 1:
                    //std::cout << "col " << temp <<  "\n";
                    node->col = (int)temp;
                    if(node->col == 0) {
                        delete node;
                        node = NULL;
                        return;
                    }
                    index++;
                    break;
                case 2:
                    //std::cout << "val " << temp << "\n";
                    node->val = temp;
                    index++;
                    if(node->row > coo.getRows())
                        coo.setDimensions(node->row,node->col);
                    else if(node->row == coo.getRows() && node->col > coo.getCols())
                        coo.setDimensions(node->row, node->col);
                    if(temp == 0.0) {
                        delete node;
                        node = NULL;   
                        return;
                    }
                    break;
                default:
                     break;
            }
            if(ss.eof() || (index > 2)){
                //std::cout << "appending" << "\n";
                coo.append(*node);
                node = NULL;
                break;
                return;
            }
        }   
    }
    node = NULL;
}

void FileManager::parseRawString(std::string entry, LinkedList &list) {
    std::stringstream ss;
    ss << entry;
    linkedNode *node = new linkedNode;
    int index = 0;
    bool hasZero = false;
    while(true){
        float temp;
        ss >> temp;
        if((ss.rdstate() & std::stringstream::failbit) != 0) {
            delete node;
            node = NULL;
            break;
        }
        else {
            switch(index){
                case 0:
                    //std::cout << "row " << temp << "\n";
                    node->row = (int)temp;
                    if(node->row == 0) {
                        delete node;
                        node = NULL;
                        return;
                    }
                    index++;
                    break;
                case 1:
                    //std::cout << "col " << temp <<  "\n";
                    node->col = (int)temp;
                    if(node->col == 0) {
                        delete node;
                        node = NULL;
                        return;
                    }
                    index++;
                    break;
                case 2:
                    //std::cout << "val " << temp << "\n";
                    node->val = temp;
                    index++;
                    if(node->row > list.getRows())
                        list.setDimensions(node->row,node->col);
                    else if(node->row == list.getRows() && node->col > list.getCols())
                        list.setDimensions(node->row, node->col);
                    if(temp == 0.0) {
                        delete node;
                        node = NULL;   
                        return;
                    }
                    break;
                default:
                     break;
            }
            if(ss.eof() || (index > 2)){
                //std::cout << "appending" << "\n";
                list.append(node);
                node = NULL;
                break;
                return;
            }
        }   
    }
}

#endif
