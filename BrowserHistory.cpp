//
// Created by salah0v on 09.11.2026.
// Written by leylaaghayeva on 10.11.2026.
//

#include "BrowserHistory.h"
#include "DoublyLinkedList.h"
#include "nlohmann/json.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

using json = nlohmann::json;

BrowserHistory::BrowserHistory(std::string homepage, std::string title) {
    head = new DoublyLinkedList(homepage, title, nullptr, nullptr);
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

// Return the title of the current page
std::string BrowserHistory::current_title() {
    if (current == nullptr) {
        return "";
    }
    return current->getTitle();
}

// Display all pages and mark the current page
void BrowserHistory::display_history() {
    DoublyLinkedList* temp = head;
    while (temp != nullptr) {
        if (temp == current) {
            std::cout << "  [<] ";
        } else {
            std::cout << "      ";
        }
        if (!temp->getTitle().empty()) {
            std::cout << temp->getTitle() << " - ";
        }
        std::cout << temp->getUrl() << std::endl;
        temp = temp->getNext();
    }
}

// Visit a new page
void BrowserHistory::visit(std::string url, std::string title) {
    // Handle an empty history
    if (current == nullptr) {
        head = new DoublyLinkedList(url, title, nullptr, nullptr);
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
    DoublyLinkedList* newNode = new DoublyLinkedList(url, title, nullptr, current);
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
            int originalPosition = 0;
            DoublyLinkedList* counter = head;
            while (counter != temp) {
                counter = counter->getNext();
                originalPosition++;
            }
            bool wasCurrent = (temp == current);
            deletedPages.push({
                temp->getUrl(),
                temp->getTitle(),
                originalPosition,
                wasCurrent
            });
            DoublyLinkedList* previous = temp->getPrev();
            DoublyLinkedList* next = temp->getNext();
            if (previous != nullptr) {
                previous->setNext(next);
            } else {
                head = next;
            }
            if (next != nullptr) {
                next->setPrev(previous);
            } else {
                tail = previous;
            }
            if (wasCurrent) {
                current = (previous != nullptr) ? previous : next;
            }
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
    DoublyLinkedList* newNode =
        new DoublyLinkedList(lastDeleted.url, lastDeleted.title, nullptr, nullptr);
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
    if (lastDeleted.wasCurrent) {
        current = newNode;
    }
    size++;
    if (size > capacity) {
        DoublyLinkedList* oldHead = head;
        head = head->getNext();
        head->setPrev(nullptr);
        if (current == oldHead) {
            current = head;
        }
        delete oldHead;
        size--;
    }
    deletedPages.pop();
    std::cout << "Restored: " << lastDeleted.url << std::endl;
}

// Export the full history to a JSON file so it can be imported later
// all of our knowledge of these JSON libraries was from public sources... so it's probably not used in the best way
// but we tried our best!
bool BrowserHistory::export_history(const std::string& filename) const {
    json root;
    root["version"] = 1;
    root["capacity"] = capacity;

    json entries = json::array();
    int currentIndex = -1;
    int count = 0;

    for (DoublyLinkedList* temp = head; temp != nullptr; temp = temp->getNext()) {
        json entry;
        entry["url"] = temp->getUrl();
        entry["title"] = temp->getTitle();
        entries.push_back(entry);

        if (temp == current) {
            currentIndex = count;
        }
        count++;
    }

    root["size"] = count;
    root["current_index"] = currentIndex;
    root["entries"] = entries;

    std::ofstream out(filename);
    if (!out.is_open()) { // is_open() because just if (!out) produced a bug where export reports success and does nothing
        std::cout << "Could not open file for writing: " << filename << std::endl;
        return false;
    }

    try {
        out << root.dump(4) << std::endl;
    } catch (const json::exception& e) {
        std::cout << "Failed to serialize history: " << e.what() << std::endl;
        return false;
    }

    out.close();
    if (!out) {
        std::cout << "Failed to write file: " << filename << std::endl;
        return false;
    }

    std::cout << "Exported " << count << " page(s) to " << filename << std::endl;
    return true;
}

// Import a history written by export_history(), replacing whatever is loaded now
// again, probably not the best usage of this library, but we did our best
bool BrowserHistory::import_history(const std::string& filename) {
    std::ifstream in(filename);
    if (!in.is_open()) { // is_open is same reason as before
        std::cout << "Could not open file for reading: " << filename << std::endl;
        return false;
    }

    json root;
    try {
        in >> root;
    } catch (const json::parse_error& e) {
        std::cout << "Not a valid JSON file: " << e.what() << std::endl;
        return false;
    } catch (const json::exception& e) {
        std::cout << "Could not read history: " << e.what() << std::endl;
        return false;
    }
    if (!root.is_object()) {
        std::cout << "Unexpected format: the top level must be a JSON object." << std::endl;
        return false;
    }
    if (root.contains("version") && root["version"].is_number_integer() &&
        root["version"].get<int>() != 1) {
        std::cout << "Unsupported file version: " << root["version"] << std::endl;
        return false;
    }
    if (!root.contains("entries") || !root["entries"].is_array()) {
        std::cout << "Unexpected format: missing an \"entries\" array." << std::endl;
        return false;
    }

    std::vector<std::pair<std::string, std::string>> pages;
    int skipped = 0;
    for (const auto& entry : root["entries"]) {
        if (!entry.is_object() || !entry.contains("url") || !entry["url"].is_string()) {
            skipped++;
            continue;
        }
        std::string url = entry["url"].get<std::string>();
        std::string title;
        if (entry.contains("title") && entry["title"].is_string()) {
            title = entry["title"].get<std::string>();
        }
        pages.push_back({url, title});
    }
    if (skipped > 0) {
        std::cout << "Skipped " << skipped << " entry/entries without a valid \"url\"." << std::endl;
    }

    int newCapacity = capacity;
    if (root.contains("capacity") && root["capacity"].is_number_integer()) {
        int fileCapacity = root["capacity"].get<int>();
        if (fileCapacity > 0) {
            newCapacity = fileCapacity;
        }
    }

    int newCurrentIndex = -1;
    if (root.contains("current_index") && root["current_index"].is_number_integer()) {
        newCurrentIndex = root["current_index"].get<int>();
    }

    if (static_cast<int>(pages.size()) > newCapacity) {
        int excess = static_cast<int>(pages.size()) - newCapacity;
        pages.erase(pages.begin(), pages.begin() + excess);
        newCurrentIndex -= excess;   // may go out of range; handled below
    }

    delete_history();
    capacity = newCapacity;

    for (const auto& page : pages) {
        DoublyLinkedList* node = new DoublyLinkedList(page.first, page.second, nullptr, tail);
        if (tail == nullptr) {
            head = node;
        } else {
            tail->setNext(node);
        }
        tail = node;
        size++;
    }
    if (newCurrentIndex >= 0 && newCurrentIndex < size) {
        current = head;
        for (int i = 0; i < newCurrentIndex; i++) {
            current = current->getNext();
        }
    } else {
        current = tail;
    }

    std::cout << "Imported " << size << " page(s) from " << filename << std::endl;
    return true;
}