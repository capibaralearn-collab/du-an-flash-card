#include "structures.h"
#include <iostream>
#include <limits>
#include <iomanip>

using namespace std;

int main() {
    FlashcardManager mgr;
    if (mgr.loadFromFile()) {
        cout << "Da tai " << mgr.totalCards() << " flashcard tu file.\n";
    }
    else {
        cout << "Chua co du lieu cu, bat dau tu dien trong.\n";
    }

    bool running = true;
    while (running) {
        showMenu();
        int choice;
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (choice) {
        case 1: menuAddCard(mgr); break;
        case 2: menuEditCard(mgr); break;
        case 3: menuDeleteCard(mgr); break;
        case 4: menuStudySession(mgr); break;
        case 5: menuSearch(mgr); break;
        case 6: menuListAll(mgr); break;
        case 7: menuStats(mgr); break;
        case 8: menuUndo(mgr); break;
        case 9:
            mgr.saveToFile();
            cout << "Da luu tien do.\n";
            break;
        case 0:
            mgr.saveToFile();
            cout << "Da luu va thoat. Hen gap lai!\n";
            running = false;
            break;
        default:
            cout << "Lua chon khong hop le.\n";
        }
    }

    return 0;
}