//
// Created by salah0v on 09.11.2026.
//

#ifndef DSA_MINIPROJECT_DOUBLYLINKEDLIST_H
#define DSA_MINIPROJECT_DOUBLYLINKEDLIST_H

#pragma once
#include <string>

class DoublyLinkedList {
private:
    std::string url;
    DoublyLinkedList* next;
    DoublyLinkedList* prev;

public:
    // default constructor method
    DoublyLinkedList();

    // constructor method for inputs (not default)
    DoublyLinkedList(const std::string &url, DoublyLinkedList* next, DoublyLinkedList* prev);

    std::string getUrl() const;
    std::string getNextUrl() const;
    std::string getPrevUrl() const;
    DoublyLinkedList* getNext() const;
    DoublyLinkedList* getPrev() const;

    void setUrl(const std::string &url);
    void setNext(DoublyLinkedList* next);
    void setPrev(DoublyLinkedList* prev);

};


#endif //DSA_MINIPROJECT_DOUBLYLINKEDLIST_H