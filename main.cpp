#include <iostream>
#include "game.h"

using namespace std;

void menu() {
    int difficulty = 1; 
    float spawnMultiplier = 1.0f;
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
            mainGame(difficulty, spawnMultiplier);
            break;
        case 2: {
            system("CLS");
            cout << "========== OPTIONS ==========" << endl;
            cout << "Pilih Difficulty:" << endl;
            cout << "1. Normal" << endl;
            cout << "2. Hard" << endl;
            cout << "3. Extreme" << endl;
            cout << "Pilihan (1-3): ";
            int diffInput;
            cin >> diffInput;
            if (diffInput == 1) {
                difficulty = 1;
                spawnMultiplier = 1.0f;
            } else if (diffInput == 2) {
                difficulty = 2;
                spawnMultiplier = 0.7f; // lebih sering rintangan
            } else if (diffInput == 3) {
                difficulty = 3;
                spawnMultiplier = 0.5f; // sangat sering rintangan
            } else {
                cout << "Pilihan tidak valid, tetap Normal.\n";
                difficulty = 1;
                spawnMultiplier = 1.0f;
            }
            cout << "Difficulty di-set ke ";
            if (difficulty == 1) cout << "Normal\n";
            else if (difficulty == 2) cout << "Hard\n";
            else cout << "Extreme\n";
            system("pause");
            break;
        }
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