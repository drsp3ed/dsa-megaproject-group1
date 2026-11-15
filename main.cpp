//
// Created by Rasul Gurbanli on 15.11.2026.
// CLI Driver and User Interface for BrowserHistory
//

#include <iostream>
#include <string>
#include <limits>
#include "BrowserHistory.h"

// function to display available options to the user
void showMenu() {

    std::cout << "\n========================================\n";
    std::cout << "       BROWSER HISTORY MANAGER          \n";
    std::cout << "========================================\n";
    std::cout << " 1. Visit new page\n";
    std::cout << " 2. Go back\n";
    std::cout << " 3. Go forward\n";
    std::cout << " 4. View current page\n";
    std::cout << " 5. Display full history\n";
    std::cout << " 6. Delete a specific URL\n";
    std::cout << " 7. Restore last deleted page (Undo)\n";
    std::cout << " 8. Clear all history\n";
    std::cout << " 9. Export history to JSON file\n";
    std::cout << " 10. Import history from JSON file\n";
    std::cout << " 0. Exit\n";
    std::cout << "========================================\n";
    std::cout << "Enter your choice: ";
}

// helper function to clear any bad input from cin
void clearInput() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

int main() {
    std::cout << "===============================================\n";
    std::cout << " Welcome to Browser History Management System! \n";
    std::cout << "===============================================\n";

    // ask for the initial homepage or use a default one
    std::cout << "Enter homepage URL (press Enter for https://google.com): ";
    std::string homepage;
    std::getline(std::cin, homepage);
    if (homepage.empty()) {
        homepage = "https://google.com";
    }

    std::cout << "Enter homepage title (press Enter for 'Google'): ";
    std::string homeTitle;
    std::getline(std::cin, homeTitle);
    if (homeTitle.empty()) {
        homeTitle = "Google";
    }

    // create the browser history object with starting page
    BrowserHistory browser(homepage, homeTitle);
    std::cout << "Browser started with: " << homepage << " (" << homeTitle << ")\n";

    int choice = -1;

    // main loop for the CLI menu
    while (true) {
        showMenu();

        if (!(std::cin >> choice)) {
            std::cout << "\nInvalid input! Please enter a valid number.\n";
            clearInput();
            continue;
        }

        // clear the leftover newline after reading number
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (choice == 0) {
            std::cout << "\nExiting the program. Goodbye!\n";
            break;
        }

        switch (choice) {
            // Case 1: Visit new URL
            case 1: {
                std::string url;
                std::string title;

                std::cout << "Enter URL (e.g., https://youtube.com): ";
                std::getline(std::cin, url);

                if (url.empty()) {
                    std::cout << "URL cannot be empty.\n";
                    break;
                }

                std::cout << "Enter page title (optional): ";
                std::getline(std::cin, title);

                browser.visit(url, title);
                std::cout << "Successfully visited: " << url << "\n";
                break;
            }

            // Case 2: Go back
            case 2: {
                int steps = 1;
                std::cout << "How many steps to go back? ";
                if (!(std::cin >> steps)) {
                    std::cout << "Invalid number of steps.\n";
                    clearInput();
                    break;
                }
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                std::string currentUrl = browser.back(steps);
                std::cout << "Moved back. Current page is now: " << currentUrl << "\n";
                break;
            }

            // Case 3: Go forward
            case 3: {
                int steps = 1;
                std::cout << "How many steps to go forward? ";
                if (!(std::cin >> steps)) {
                    std::cout << "Invalid number of steps.\n";
                    clearInput();
                    break;
                }
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                std::string currentUrl = browser.forward(steps);
                std::cout << "Moved forward. Current page is now: " << currentUrl << "\n";
                break;
            }

            // Case 4: View current active page
            case 4: {
                std::string url = browser.current_page();
                std::string title = browser.current_title();

                if (url.empty()) {
                    std::cout << "History is currently empty. No active page.\n";
                } else {
                    std::cout << "\n--- Current Active Page ---\n";
                    std::cout << "Title: " << (title.empty() ? "(No Title)" : title) << "\n";
                    std::cout << "URL:   " << url << "\n";
                }
                break;
            }

            // Case 5: Display full history chain
            case 5: {
                std::cout << "\n--- Browsing History Chain ---\n";
                browser.display_history();
                std::cout << "------------------------------\n";
                break;
            }

            // Case 6: Delete specific URL
            case 6: {
                std::string urlToDelete;
                std::cout << "Enter the exact URL you want to delete: ";
                std::getline(std::cin, urlToDelete);

                if (urlToDelete.empty()) {
                    std::cout << "URL cannot be empty.\n";
                    break;
                }

                browser.delete_url(urlToDelete);
                std::cout << "URL deleted (if it was present in history) and pushed to restore stack.\n";
                break;
            }

            // Case 7: Restore last deleted page from stack
            case 7: {
                std::cout << "Attempting to restore last deleted page...\n";
                browser.restore();
                break;
            }

            // Case 8: Delete all history
            case 8: {
                char confirm = 'n';
                std::cout << "Are you sure you want to clear all history? (y/n): ";
                std::cin >> confirm;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                if (confirm == 'y' || confirm == 'Y') {
                    browser.delete_history();
                    std::cout << "All browsing history has been cleared.\n";
                } else {
                    std::cout << "Clear operation cancelled.\n";
                }
                break;
            }

            // Case 9: Export history to JSON
            case 9: {
                std::string filename;
                std::cout << "Enter export filename (default: history.json): ";
                std::getline(std::cin, filename);

                if (filename.empty()) {
                    filename = "history.json";
                }

                browser.export_history(filename);
                break;
            }

            // Case 10: Import history from JSON
            case 10: {
                std::string filename;
                std::cout << "Enter import filename (default: history.json): ";
                std::getline(std::cin, filename);

                if (filename.empty()) {
                    filename = "history.json";
                }

                browser.import_history(filename);
                break;
            }

            default: {
                std::cout << "Invalid choice! Please select an option between 0 and 10.\n";
                break;
            }
        }
    }

    return 0;
}
