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
    std::string title;
    DoublyLinkedList* next;
    DoublyLinkedList* prev;

public:
    // default constructor method
    DoublyLinkedList();

    // constructor method for inputs (not default)
    DoublyLinkedList(const std::string &url, const std::string &title, DoublyLinkedList* next, DoublyLinkedList* prev);

    std::string getUrl() const;
    std::string getNextUrl() const;
    std::string getPrevUrl() const;
    std::string getTitle() const;
    std::string getNextTitle() const;
    std::string getPrevTitle() const;
    DoublyLinkedList* getNext() const;
    DoublyLinkedList* getPrev() const;

    void setUrl(const std::string &url);
    void setTitle(const std::string &title);
    void setNext(DoublyLinkedList* next);
    void setPrev(DoublyLinkedList* prev);

};


#endif //DSA_MINIPROJECT_DOUBLYLINKEDLIST_H