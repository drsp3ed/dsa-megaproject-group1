#include <iostream>
#include <string>
#include <stdexcept>
#include "DoublyLinkedList.h"
#include "BrowserHistory.h"


int main() {
    // just a test to see if ts works
    auto head = new DoublyLinkedList();
    std::cout << head->getUrl() << " " << head->getTitle() << std::endl;
}
