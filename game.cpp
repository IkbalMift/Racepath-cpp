#include <iostream>
#include <vector>
#include <string>
#include <conio.h>
#include <windows.h>
#include <cstdlib>
#include <ctime>

using namespace std;

class CustomQueue {
private:
    int top;
    string isi[50]; 
    const int ukuran = 50;
public:
    CustomQueue() {
        top = 0;
    }
    void clear() {
        top = 0; 
    }

    bool isEmpty() {
        return (top == 0);
    }
    bool isFull() {
        return (top >= ukuran);
    }

    void enqueue(string data) {
        if (!isFull()) {
            isi[top] = data;
            top++;
        }
    }


    void dequeue() {
        if (!isEmpty()) {
            for (int i = 0; i < top - 1; i++) {
                isi[i] = isi[i + 1];
            }
            top--;
        }
    }
    vector<string> getContents() {
        vector<string> contents;
        for(int i = 0; i < top; i++) {
            contents.push_back(isi[i]);
        }
        return contents;
    }
    void updateBack(string data) {
        if (!isEmpty()) {
            isi[top - 1] = data;
        }
    }
};

class Graph {
private:
    int numVertices;
    vector<int>* adjLists; // Adjacency List

public:
    // Constructor untuk membuat graph
    Graph(int vertices) {
        numVertices = vertices;
        adjLists = new vector<int>[vertices];
    }

    // Menambahkan edge  antara dua node
    void addEdge(int src, int dest) {
        adjLists[src].push_back(dest);
        adjLists[dest].push_back(src);
    }

    bool isConnected(int src, int dest) {
        if (src < 0 || src >= numVertices || dest < 0 || dest >= numVertices) {
            return false; // Posisi di luar jangkauan
        }
        for (int neighbor : adjLists[src]) {
            if (neighbor == dest) {
                return true;
            }
        }
        return false;
    }
    
    Graph() {
        delete[] adjLists;
    }
};


const int LEBAR_LAYAR = 110;
const int TINGGI_LAYAR = 20;
CHAR_INFO consoleBuffer[LEBAR_LAYAR * TINGGI_LAYAR];
HANDLE hConsole;

const int PANJANG_JALAN = 50;
const string SIMBOL_JALAN = "- ";
const string SIMBOL_KOSONG = "  ";
const int TOTAL_LINTASAN = 6;
const int TOTAL_CABANG = 3;

CustomQueue jalur[TOTAL_LINTASAN]; 
Graph jalanGraph(TOTAL_CABANG);

const vector<string> MOBIL = { "  . - - ` - .", "  '- O - O -'" };
const string RINTANGAN_ART = " /|||\\ ";
const string RINTANGAN_MARKER = "##";

int posisiMobil = 1;

// Tambah variabel global untuk nyawa dan invulnerable
int nyawa = 3;
bool invulnerable = false;
DWORD invulnerableStart = 0;
const DWORD INVULNERABLE_DURATION = 2000; // 2 detik

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
        jalur[i].clear();
        for (int j = 0; j < PANJANG_JALAN; j++) {
            jalur[i].enqueue((i % 2 == 1) ? SIMBOL_JALAN : SIMBOL_KOSONG);
        }
    }
}

void cekTabrakan() {
    // Cek apakah posisi mobil bertabrakan dengan rintangan
    int lintasanMobil = posisiMobil * 2 + 1;
    vector<string> contents = jalur[lintasanMobil].getContents();
    // Mobil berada di kolom 6, artinya index ke-1 (karena 4 spasi di awal)
    // Rintangan muncul di belakang, jadi cek 2 kolom pertama
    // Cek apakah ada RINTANGAN_MARKER di posisi depan mobil
    if (contents.size() > 1 && !invulnerable) {
        string depan = contents[0] + contents[1];
        if (depan.find(RINTANGAN_MARKER) != string::npos) {
            nyawa--;
            invulnerable = true;
            invulnerableStart = GetTickCount();
        }
    }
}

void updateInvulnerable() {
    if (invulnerable && (GetTickCount() - invulnerableStart >= INVULNERABLE_DURATION)) {
        invulnerable = false;
    }
}

void pindahMobil() {
    if (_kbhit()) {
        char input = _getch();
        if (input == 'w' || input == 'W') {
            if (jalanGraph.isConnected(posisiMobil, posisiMobil - 1)) {
                posisiMobil--;
            }
        } else if (input == 's' || input == 'S') {
            // Cek ke graph apakah ada jalur dari posisi sekarang ke bawah
            if (jalanGraph.isConnected(posisiMobil, posisiMobil + 1)) {
                posisiMobil++;
            }
        }
    }
}

void jalurBerjalan() {
    for (int i = 0; i < TOTAL_LINTASAN; i++) {
        jalur[i].dequeue();
        jalur[i].enqueue((i % 2 == 1) ? SIMBOL_JALAN : SIMBOL_KOSONG);
    }
}

// Tambah parameter untuk difficulty dan spawnMultiplier
void mainGame(int difficulty = 1, float spawnMultiplier = 1.0f) {
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hConsole, &cursorInfo);
    cursorInfo.bVisible = false;
    SetConsoleCursorInfo(hConsole, &cursorInfo);

    jalanGraph.addEdge(0, 1); // Jalur antara lajur 0 dan 1
    jalanGraph.addEdge(1, 2); // Jalur antara lajur 1 dan 2

    nyawa = 3; // Reset nyawa di awal game

    const int totalLevel = 3;
    const DWORD durasi = 20 * 1000;

    for (int level = 1; level <= totalLevel; level++) {
        int kecepatan, spawnRate;
        if (level == 1) { kecepatan = 40; spawnRate = 30; } 
        else if (level == 2) { kecepatan = 30; spawnRate = 20; } 
        else { kecepatan = 20; spawnRate = 15; }

        // Modifikasi spawnRate sesuai difficulty
        spawnRate = static_cast<int>(spawnRate * spawnMultiplier);

        isiJalur();
        posisiMobil = 1;
        DWORD waktuMulai = GetTickCount();
        int spawnCounter = 0;
        invulnerable = false;
        invulnerableStart = 0;

        while (GetTickCount() - waktuMulai < durasi && nyawa > 0) {
            for(int i=0; i < LEBAR_LAYAR * TINGGI_LAYAR; ++i) { consoleBuffer[i] = {' ', 7}; }

            gambarKeBuffer(0, 0, "Kontrol: W = atas, S = bawah", 15);
            gambarKeBuffer(0, 1, "Level: " + to_string(level) + " / " + to_string(totalLevel), 14);
            DWORD waktuTersisa = durasi - (GetTickCount() - waktuMulai);
            gambarKeBuffer(20, 1, "| Waktu tersisa: " + to_string(waktuTersisa / 1000) + " detik", 14);
            // Hapus baris ini (duplikat nyawa)
            // gambarKeBuffer(60, 1, "| Nyawa: " + string(nyawa, 3), 12);

            string nyawaStr = "Nyawa: ";
            for (int i = 0; i < nyawa; ++i) nyawaStr += "\3 "; // ASCII heart
            gambarKeBuffer(80, 1, nyawaStr, 12);

            string bingkai(PANJANG_JALAN * 2 + 6, '=');
            gambarKeBuffer(2, 3, bingkai, 8);
            gambarKeBuffer(2, 4 + TOTAL_LINTASAN, bingkai, 8);
            for(int i=0; i<TOTAL_LINTASAN; ++i) {
                gambarKeBuffer(1, 4+i, "||", 8);
                gambarKeBuffer(4 + PANJANG_JALAN * 2, 4+i, "||", 8);
            }

            for (int baris = 0; baris < TOTAL_LINTASAN; baris++) {
                // Mengambil isi dari CustomQueue untuk digambar
                vector<string> contents = jalur[baris].getContents();
                string lineContent = "";
                for(const auto& s : contents) {
                    lineContent += s;
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
            // Efek blinking jika invulnerable
            bool tampilkanMobil = true;
            if (invulnerable) {
                DWORD blinkTime = (GetTickCount() - invulnerableStart) / 150;
                tampilkanMobil = (blinkTime % 2 == 0);
            }
            if (tampilkanMobil) {
                gambarKeBuffer(6, posisiY_mobil, MOBIL[0], invulnerable ? 14 : 10);
                gambarKeBuffer(6, posisiY_mobil + 1, MOBIL[1], invulnerable ? 14 : 10);
            }

            tampilkanBuffer();

            pindahMobil();
            jalurBerjalan();
            cekTabrakan();
            updateInvulnerable();

            spawnCounter++;
            if (spawnCounter >= spawnRate) {
                spawnCounter = 0;
                int lajurAman = rand() % TOTAL_CABANG;
                vector<int> lajurTidakAman;
                for (int i = 0; i < TOTAL_CABANG; i++) { if (i != lajurAman) { lajurTidakAman.push_back(i); } }
                int targetLajur = lajurTidakAman[rand() % lajurTidakAman.size()];
                int lintasan = targetLajur * 2 + 1;
                jalur[lintasan].updateBack(RINTANGAN_MARKER);
            }

            Sleep(kecepatan);
        }

        if (nyawa <= 0) {
            for(int i=0; i < LEBAR_LAYAR * TINGGI_LAYAR; ++i) { consoleBuffer[i] = {' ', 7}; }
            string msg = "GAME OVER!";
            string msg2 = "Kamu kehabisan nyawa!";
            gambarKeBuffer(LEBAR_LAYAR / 2 - msg.length() / 2, TINGGI_LAYAR / 2, msg, 12);
            gambarKeBuffer(LEBAR_LAYAR / 2 - msg2.length() / 2, TINGGI_LAYAR / 2 + 1, msg2, 12);
            tampilkanBuffer();
            Sleep(3000);
            break;
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
            while (_kbhit()) _getch();
            _getch();
        }
    }

    cursorInfo.bVisible = true;
    SetConsoleCursorInfo(hConsole, &cursorInfo);
    SetConsoleTextAttribute(hConsole, 7); 
}

