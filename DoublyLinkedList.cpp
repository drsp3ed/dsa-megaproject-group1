//
// Created by salah0v on 09.11.2026.
//
#include "DoublyLinkedList.h"
#include <stdexcept>

//ONE THING that's worth noting - it's actually a class for a Node, and it's just called linkedlist for convenience
DoublyLinkedList::DoublyLinkedList() : url("https://www.youtube.com/watch?v=E4WlUXrJgy4"), title("not a rickroll :D"), next(nullptr), prev(nullptr) {}
DoublyLinkedList::DoublyLinkedList(const std::string &url, const std::string &title, DoublyLinkedList *next, DoublyLinkedList *prev) : url(url), title(title), next(next), prev(prev) {}

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

std::string DoublyLinkedList::getTitle() const { return title; }
std::string DoublyLinkedList::getNextTitle() const
{
    if (next == nullptr) throw std::out_of_range("THIS IS THE LAST ELEMENT OF THE HISTORY");
    return next->getTitle();
}
std::string DoublyLinkedList::getPrevTitle() const
{
    if (prev == nullptr) throw std::out_of_range("THIS IS THE FIRST ELEMENT OF THE HISTORY");
    return prev->getTitle();
}

DoublyLinkedList* DoublyLinkedList::getNext() const { return next; }
DoublyLinkedList* DoublyLinkedList::getPrev() const { return prev; }

void DoublyLinkedList::setUrl(const std::string &url) { this->url = url; }
void DoublyLinkedList::setTitle(const std::string &title) { this->title = title; }
void DoublyLinkedList::setNext(DoublyLinkedList *next) { this->next = next; }
void DoublyLinkedList::setPrev(DoublyLinkedList *prev) { this->prev = prev; }