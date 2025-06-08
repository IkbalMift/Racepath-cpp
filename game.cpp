#include <iostream>
#include <queue>
#include <conio.h>
#include <windows.h>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <string>

using namespace std;

// --- PENYESUAIAN UNTUK DOUBLE BUFFER ---
const int LEBAR_LAYAR = 110;
const int TINGGI_LAYAR = 20;
CHAR_INFO consoleBuffer[LEBAR_LAYAR * TINGGI_LAYAR];
HANDLE hConsole;

// Konstanta game
const int PANJANG_JALAN = 50;
const string SIMBOL_JALAN = "- ";
const string SIMBOL_KOSONG = "  "; // Pastikan ini spasi biasa, bukan non-breaking space
const int TOTAL_LINTASAN = 6;
const int TOTAL_CABANG = 3;

queue<string> jalur[TOTAL_LINTASAN];

const vector<string> MOBIL = {
    "  . - - ` - .",
    "  '- O - O -'"
};

const string RINTANGAN_ART = " /|||\\ ";
const string RINTANGAN_MARKER = "##";

int posisiMobil = 1;

void gambarKeBuffer(int x, int y, const string& teks, WORD atribut = 7) {
    if (y >= TINGGI_LAYAR || x >= LEBAR_LAYAR) return;
    for (size_t i = 0; i < teks.length(); ++i) {
        if (x + i < LEBAR_LAYAR) {
            consoleBuffer[x + i + LEBAR_LAYAR * y].Char.AsciiChar = teks[i];
            consoleBuffer[x + i + LEBAR_LAYAR * y].Attributes = atribut;
        }
    }
}

void tampilkanBuffer() {
    COORD bufferSize = { LEBAR_LAYAR, TINGGI_LAYAR };
    COORD characterPos = { 0, 0 };
    SMALL_RECT writeArea = { 0, 0, LEBAR_LAYAR - 1, TINGGI_LAYAR - 1 };
    WriteConsoleOutputA(hConsole, consoleBuffer, bufferSize, characterPos, &writeArea);
}


void isiJalur() {
    srand((unsigned int)time(0));
    for (int i = 0; i < TOTAL_LINTASAN; i++) {
        while (!jalur[i].empty()) jalur[i].pop();
        for (int j = 0; j < PANJANG_JALAN; j++) {
            jalur[i].push((i % 2 == 1) ? SIMBOL_JALAN : SIMBOL_KOSONG);
        }
    }
}

void pindahMobil() {
    if (_kbhit()) {
        char input = _getch();
        if ((input == 'w' || input == 'W') && posisiMobil > 0) posisiMobil--;
        else if ((input == 's' || input == 'S') && posisiMobil < TOTAL_CABANG - 1) posisiMobil++;
    }
}

void jalurBerjalan() {
    for (int i = 0; i < TOTAL_LINTASAN; i++) {
        jalur[i].pop();
        jalur[i].push((i % 2 == 1) ? SIMBOL_JALAN : SIMBOL_KOSONG);
    }
}

void tampilkanLayarAkhir() {
    for(int i=0; i < LEBAR_LAYAR * TINGGI_LAYAR; ++i) { consoleBuffer[i] = {' ', 7}; }

    string msg1 = "======================================";
    string msg2 = "|                                    |";
    string msg3 = "|   SELAMAT, SEMUA LEVEL SELESAI!    |";
    string msg4 = "|    Tekan tombol apa saja untuk keluar...   |";

    gambarKeBuffer(LEBAR_LAYAR / 2 - msg1.length() / 2, TINGGI_LAYAR / 2 - 3, msg1, 14);
    gambarKeBuffer(LEBAR_LAYAR / 2 - msg2.length() / 2, TINGGI_LAYAR / 2 - 2, msg2, 14);
    gambarKeBuffer(LEBAR_LAYAR / 2 - msg3.length() / 2, TINGGI_LAYAR / 2 - 1, msg3, 15);
    gambarKeBuffer(LEBAR_LAYAR / 2 - msg2.length() / 2, TINGGI_LAYAR / 2,     msg2, 14);
    gambarKeBuffer(LEBAR_LAYAR / 2 - msg4.length() / 2, TINGGI_LAYAR / 2 + 1, msg4, 14);
    gambarKeBuffer(LEBAR_LAYAR / 2 - msg2.length() / 2, TINGGI_LAYAR / 2 + 2, msg2, 14);
    gambarKeBuffer(LEBAR_LAYAR / 2 - msg1.length() / 2, TINGGI_LAYAR / 2 + 3, msg1, 14);

    tampilkanBuffer();
}


void mainGame() {
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hConsole, &cursorInfo);
    cursorInfo.bVisible = false;
    SetConsoleCursorInfo(hConsole, &cursorInfo);

    const int totalLevel = 3;
    const DWORD durasi = 20 * 1000;

    for (int level = 1; level <= totalLevel; level++) {
        int kecepatan;
        int spawnRate;
        if (level == 1) {
            kecepatan = 50;
            spawnRate = 40;
        } else if (level == 2) {
            kecepatan = 40;
            spawnRate = 30;
        } else {
            kecepatan = 35;
            spawnRate = 20;
        }

        isiJalur();
        posisiMobil = 1;
        DWORD waktuMulai = GetTickCount();
        int spawnCounter = 0;

        while (GetTickCount() - waktuMulai < durasi) {
            for(int i=0; i < LEBAR_LAYAR * TINGGI_LAYAR; ++i) {
                consoleBuffer[i] = {' ', 7};
            }

            gambarKeBuffer(0, 0, "Kontrol: W = atas, S = bawah", 15);
            gambarKeBuffer(0, 1, "Level: " + to_string(level) + " / " + to_string(totalLevel), 14);
            DWORD waktuTersisa = durasi - (GetTickCount() - waktuMulai);
            gambarKeBuffer(20, 1, "| Waktu tersisa: " + to_string(waktuTersisa / 1000) + " detik", 14);

            string bingkai(PANJANG_JALAN * 2 + 6, '=');
            gambarKeBuffer(2, 3, bingkai, 8);
            gambarKeBuffer(2, 4 + TOTAL_LINTASAN, bingkai, 8);
            for(int i=0; i<TOTAL_LINTASAN; ++i) {
                gambarKeBuffer(1, 4+i, "||", 8);
                gambarKeBuffer(4 + PANJANG_JALAN * 2, 4+i, "||", 8);
            }

            queue<string> bufferJalan[TOTAL_LINTASAN];
            for (int i = 0; i < TOTAL_LINTASAN; i++) bufferJalan[i] = jalur[i];

            for (int baris = 0; baris < TOTAL_LINTASAN; baris++) {
                string lineContent = "";
                while(!bufferJalan[baris].empty()){
                    lineContent += bufferJalan[baris].front();
                    bufferJalan[baris].pop();
                }
                
                string tempLine = lineContent;
                size_t startPos = 0;
                
                gambarKeBuffer(4, 4 + baris, tempLine, 7);
                
                while((startPos = tempLine.find(RINTANGAN_MARKER, startPos)) != string::npos) {
                    gambarKeBuffer(4 + startPos, 4 + baris, RINTANGAN_ART, 12);
                    startPos += RINTANGAN_MARKER.length();
                }
            }

            int posisiY_mobil = 4 + posisiMobil * 2;
            gambarKeBuffer(6, posisiY_mobil, MOBIL[0], 10);
            gambarKeBuffer(6, posisiY_mobil + 1, MOBIL[1], 10);

            tampilkanBuffer();
            
            pindahMobil();
            jalurBerjalan();
            
            spawnCounter++;
            if (spawnCounter >= spawnRate) {
                spawnCounter = 0;
                
                int lajurAman = rand() % TOTAL_CABANG;
                vector<int> lajurTidakAman;
                for (int i = 0; i < TOTAL_CABANG; i++) {
                    if (i != lajurAman) {
                        lajurTidakAman.push_back(i);
                    }
                }
                int targetLajur = lajurTidakAman[rand() % lajurTidakAman.size()];
                int lintasan = targetLajur * 2 + 1;
                jalur[lintasan].back() = RINTANGAN_MARKER;
            }

            Sleep(kecepatan);
        }

        if (level < totalLevel) {
            for(int i=0; i < LEBAR_LAYAR * TINGGI_LAYAR; ++i) { consoleBuffer[i] = {' ', 7}; }
            string msg = "Level " + to_string(level) + " Selesai!";
            string msg2 = "Bersiap untuk level berikutnya...";
            gambarKeBuffer(LEBAR_LAYAR / 2 - msg.length() / 2, TINGGI_LAYAR / 2, msg, 14);
            gambarKeBuffer(LEBAR_LAYAR / 2 - msg2.length() / 2, TINGGI_LAYAR / 2 + 1, msg2, 14);
            tampilkanBuffer();
            Sleep(2500);
        } else {
            tampilkanLayarAkhir();
            while (_kbhit()) _getch(); // Bersihkan sisa input
            _getch(); 
        }
    }

    cursorInfo.bVisible = true;
    SetConsoleCursorInfo(hConsole, &cursorInfo);
    SetConsoleTextAttribute(hConsole, 7); 
}

