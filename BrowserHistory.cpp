//
// Created by salah0v on 09.11.2026.
// Written by leylaaghayeva on 10.11.2026.
//

#include "BrowserHistory.h"
#include "DoublyLinkedList.h"
#include <iostream>

BrowserHistory::BrowserHistory(std::string homepage) {
    head = new DoublyLinkedList(homepage, nullptr, nullptr);
    current = head;
    tail = head;
    size = 1;
    capacity = 50;
}

// Move backward by the requested number of steps
std::string BrowserHistory::back(int steps) {
    if (current == nullptr) {
        return "";
    }
    if (steps <= 0) {
        return current->getUrl();
    }
    while (steps > 0 && current->getPrev() != nullptr) {
        current = current->getPrev();
        steps--;
    }
    return current->getUrl();
}

// Move forward by the requested number of steps
std::string BrowserHistory::forward(int steps) {
    if (current == nullptr) {
        return "";
    }
    if (steps <= 0) {
        return current->getUrl();
    }
    while (steps > 0 && current->getNext() != nullptr) {
        current = current->getNext();
        steps--;
    }
    return current->getUrl();
}

// Return the URL of the current page
std::string BrowserHistory::current_page() {
    if (current == nullptr) {
        return "";
    }
    return current->getUrl();
}

// Display all pages and mark the current page
void BrowserHistory::display_history() {
    DoublyLinkedList* temp = head;
    while (temp != nullptr) {
        if (temp == current) {
            std::cout << "[<]";
        } else {
            std::cout << "  ";
        }
        std::cout << temp->getUrl() << std::endl;
        temp = temp->getNext();
    }
}

// Visit a new page
void BrowserHistory::visit(std::string url) {
    // Handle an empty history
    if (current == nullptr) {
        head = new DoublyLinkedList(url, nullptr, nullptr);
        current = head;
        tail = head;
        size = 1;
        return;
    }
    // Remove all forward history
    if (current->getNext() != nullptr) {
        DoublyLinkedList* temp = current->getNext();
        while (temp != nullptr) {
            DoublyLinkedList* nextNode = temp->getNext();
            delete temp;
            temp = nextNode;
            size--;
        }
        current->setNext(nullptr);
        tail = current;
    }

    // Create and append the new page
    DoublyLinkedList* newNode = new DoublyLinkedList(url, nullptr, current);
    current->setNext(newNode);
    current = newNode;
    tail = newNode;
    size++;

    // Prune the oldest page if capacity is exceeded
    if (size > capacity) {
        DoublyLinkedList* oldHead = head;
        head = head->getNext();
        head->setPrev(nullptr);
        delete oldHead;
        size--;
    }
}

// Delete all browsing history
void BrowserHistory::delete_history() {
    DoublyLinkedList* temp = head;
    while (temp != nullptr) {
        DoublyLinkedList* nextNode = temp->getNext();
        delete temp;
        temp = nextNode;
    }
    head = nullptr;
    current = nullptr;
    tail = nullptr;
    size = 0;
    // Clear the deleted-pages stack as well
    while (!deletedPages.empty()) {
        deletedPages.pop();
    }
}

// Destructor
BrowserHistory::~BrowserHistory() {
    delete_history();
}

// Delete a specific URL and save it for restoration
void BrowserHistory::delete_url(const std::string& url) {
    DoublyLinkedList* temp = head;
    while (temp != nullptr) {
        if (temp->getUrl() == url) {
            // Calculate the original position
            int originalPosition = 0;
            DoublyLinkedList* counter = head;
            while (counter != temp) {
                counter = counter->getNext();
                originalPosition++;
            }
            // Save whether this page is currently active
            bool wasCurrent = (temp == current);
            // Save deleted page information in the stack
            deletedPages.push({
                temp->getUrl(),
                originalPosition,
                wasCurrent
            });
            // Save neighboring nodes
            DoublyLinkedList* previous = temp->getPrev();
            DoublyLinkedList* next = temp->getNext();
            // Connect the previous node to the next node
            if (previous != nullptr) {
                previous->setNext(next);
            } else {
                head = next;
            }
            // Connect the next node to the previous node
            if (next != nullptr) {
                next->setPrev(previous);
            } else {
                tail = previous;
            }
            // Update current page if necessary
            if (wasCurrent) {
                current = (previous != nullptr) ? previous : next;
            }
            // Delete the node
            delete temp;
            size--;
            return;
        }
        temp = temp->getNext();
    }
}

// Restore the most recently deleted page
void BrowserHistory::restore() {
    if (deletedPages.empty()) {
        std::cout << "No deleted pages to restore." << std::endl;
        return;
    }
    DeletedPage lastDeleted = deletedPages.top();
    // Create a new node for the restored page
    DoublyLinkedList* newNode =
        new DoublyLinkedList(lastDeleted.url, nullptr, nullptr);
    // Case 1: History is empty or original position is 0
    if (head == nullptr || lastDeleted.originalPosition <= 0) {
        newNode->setNext(head);
        if (head != nullptr) {
            head->setPrev(newNode);
        }
        head = newNode;
        if (tail == nullptr) {
            tail = newNode;
        }
    }
    // Case 2: Insert at the original position
    else {
        DoublyLinkedList* temp = head;
        for (int i = 0;
             i < lastDeleted.originalPosition - 1 &&
             temp->getNext() != nullptr;
             i++) {

            temp = temp->getNext();
        }
        DoublyLinkedList* nextNode = temp->getNext();
        newNode->setPrev(temp);
        newNode->setNext(nextNode);
        temp->setNext(newNode);
        if (nextNode != nullptr) {
            nextNode->setPrev(newNode);
        } else {
            tail = newNode;
        }
    }
    // Update current page if necessary
    if (lastDeleted.wasCurrent) {
        current = newNode;
    }
    size++;
    // Remove the oldest page if capacity is exceeded
    if (size > capacity) {
        DoublyLinkedList* oldHead = head;
        head = head->getNext();
        head->setPrev(nullptr);
        // Prevent current from pointing to deleted memory
        if (current == oldHead) {
            current = head;
        }
        delete oldHead;
        size--;
    }
    // Remove the restored page from the stack
    deletedPages.pop();
    std::cout << "Restored: " << lastDeleted.url << std::endl;
}
