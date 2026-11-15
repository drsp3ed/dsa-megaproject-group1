//
// Created by salah0v on 09.11.2026.
// Written by leylaaghayeva on 10.11.2026.
//

#ifndef DSA_MINIPROJECT_BROWSERHISTORY_H
#define DSA_MINIPROJECT_BROWSERHISTORY_H

#include "DoublyLinkedList.h"
#include <string>
#include <stack>

class BrowserHistory {
private:
    struct DeletedPage {
        std::string url;
        std::string title;
        int originalPosition;
        bool wasCurrent;
    };

    DoublyLinkedList* head;
    DoublyLinkedList* current;
    DoublyLinkedList* tail;

    int size;
    int capacity;

    std::stack<DeletedPage> deletedPages;

public:
    BrowserHistory(std::string homepage, std::string title = "");

    void visit(std::string url, std::string title = "");
    std::string back(int steps);
    std::string forward(int steps);
    std::string current_page();
    std::string current_title();
    void display_history();

    void delete_history();
    void delete_url(const std::string& url);
    void restore();

    ~BrowserHistory();
};

#endif // DSA_MINIPROJECT_BROWSERHISTORY_H