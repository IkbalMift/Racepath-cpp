#include <iostream>

using namespace std;

void menu() {
    while(true) {
        system("CLS");
        cout << "========================================" << endl;
        cout << "| No |           MENU UTAMA            |" << endl; 
        cout << "========================================" << endl;
        cout << "| 1  | Play                            |" << endl;
        cout << "| 2  | Options                         |" << endl;
        cout << "| 3  | Scores                          |" << endl;
        cout << "| 0  | Exit                            |" << endl;
        cout << "========================================" << endl << endl;
        cout << "Pilih opsi (0-3): ";
        int pilihan;
        cin >> pilihan;
        switch (pilihan) {
        case 1:
            cout << "Play game";
            break;
        case 2:
            cout << "Options";
            break;
        case 3:
            cout << "Scores";
            break;
        case 0:
            cout << "Keluar dari program" << endl;
            return;
        default:
            break;
        }
    }
}

int main() {
    menu();
    return 0;
}