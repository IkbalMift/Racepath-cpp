#include <iostream>
#include <queue>
#include <conio.h>
#include <windows.h>

using namespace std;

const int PANJANG_JALAN = 15;


const string SIMBOL_MOBIL = "M ";       
const string SIMBOL_JALAN = "- ";      

queue<string> jalurAtas, jalurTengah, jalurBawah;


int posisiMobil = 1;


HANDLE konsol = GetStdHandle(STD_OUTPUT_HANDLE);

void pindahCursor(int x, int y) {
    COORD posisi = {(SHORT)x, (SHORT)y};
    SetConsoleCursorPosition(konsol, posisi);
}

void ubahWarna(int warna) {
    SetConsoleTextAttribute(konsol, warna);
}

void isiJalur() {
    
    while (!jalurAtas.empty()) jalurAtas.pop();
    while (!jalurTengah.empty()) jalurTengah.pop();
    while (!jalurBawah.empty()) jalurBawah.pop();

    for (int i = 0; i < PANJANG_JALAN; i++) {
        jalurAtas.push(SIMBOL_JALAN);
        jalurTengah.push(SIMBOL_JALAN);
        jalurBawah.push(SIMBOL_JALAN);
    }
}

void tampilkanJalur() {
    pindahCursor(0, 0);

    ubahWarna(15);
    cout << "Kontrol: W = atas, S = bawah\n\n";

    ubahWarna(8);
    cout << "==============================\n";

  
    queue<string> jalur[3] = {jalurAtas, jalurTengah, jalurBawah};

    for (int baris = 0; baris < 3; baris++) {
        ubahWarna(8);
        cout << "|| ";

        for (int kolom = 0; kolom < PANJANG_JALAN; kolom++) {
            string blok = jalur[baris].front();
            jalur[baris].pop();

            
            if (kolom == 2 && baris == posisiMobil) {
                ubahWarna(10);
                cout << SIMBOL_MOBIL;
            } else {
                ubahWarna(7); 
                cout << blok;
            }
        }

        ubahWarna(8);
        cout << " ||\n";
    }

    ubahWarna(8);
    cout << "==============================\n";
    ubahWarna(15);
}


void pindahMobil() {
    if (_kbhit()) { 
        char input = _getch();

      
        if ((input == 'w' || input == 'W') && posisiMobil > 0) {
            posisiMobil--;
        }
     
        else if ((input == 's' || input == 'S') && posisiMobil < 2) {
            posisiMobil++;
        }
    }
}


void jalurBerjalan() {
    jalurAtas.pop(); jalurAtas.push(SIMBOL_JALAN);
    jalurTengah.pop(); jalurTengah.push(SIMBOL_JALAN);
    jalurBawah.pop(); jalurBawah.push(SIMBOL_JALAN);
}


void mainGame() {
    system("CLS");
    isiJalur();

   
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(konsol, &cursorInfo);
    cursorInfo.bVisible = false;
    SetConsoleCursorInfo(konsol, &cursorInfo);

    while (true) {
        tampilkanJalur();
        pindahMobil();
        jalurBerjalan();
        Sleep(150);  
    }

    cursorInfo.bVisible = true;
    SetConsoleCursorInfo(konsol, &cursorInfo);
}
