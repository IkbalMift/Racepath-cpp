#include <iostream>
#include "game.h"

using namespace std;

void menu() {
    while (true) {
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
            mainGame();
            break;
        case 2:
            cout << "Options belum tersedia...\n";
            system("pause");
            break;
        case 3:
            cout << "Scores belum tersedia...\n";
            system("pause");
            break;
        case 0:
            cout << "Keluar dari program\n";
            return;
        default:
            cout << "Opsi tidak valid\n";
            system("pause");
            break;
        }
    }
}

int main() {
    menu();
    return 0;
}