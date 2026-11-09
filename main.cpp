#include <iostream>
#include "DoublyLinkedList.h"
#include "BrowserHistory.h"

int main() {
    // just a test to see if ts works
    auto head = new DoublyLinkedList();
    std::cout << head->getUrl() << std::endl;

    return 0;
}