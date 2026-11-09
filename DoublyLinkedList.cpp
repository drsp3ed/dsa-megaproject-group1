//
// Created by salah0v on 09.11.2026.
//
#include "DoublyLinkedList.h"
#include <stdexcept>

DoublyLinkedList::DoublyLinkedList() : url("https://www.youtube.com/watch?v=E4WlUXrJgy4"), next(nullptr), prev(nullptr) {}
DoublyLinkedList::DoublyLinkedList(const std::string &url, DoublyLinkedList *next, DoublyLinkedList *prev) : url(url), next(next), prev(prev) {}

std::string DoublyLinkedList::getUrl() const { return url; }
std::string DoublyLinkedList::getNextUrl() const
{
    if (next == nullptr) throw std::out_of_range("THIS IS THE LAST ELEMENT OF THE HISTORY");
    return next->getUrl();
}
std::string DoublyLinkedList::getPrevUrl() const
{
    if (prev == nullptr) throw std::out_of_range("THIS IS THE FIRST ELEMENT OF THE HISTORY");
    return prev->getUrl();
}

DoublyLinkedList* DoublyLinkedList::getNext() const { return next; }
DoublyLinkedList* DoublyLinkedList::getPrev() const { return prev; }

void DoublyLinkedList::setUrl(const std::string &url) { this->url = url; }
void DoublyLinkedList::setNext(DoublyLinkedList *next) { this->next = next; }
void DoublyLinkedList::setPrev(DoublyLinkedList *prev) { this->prev = prev; }

