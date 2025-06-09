#include <iostream>
#include <vector>
#include <string>
#include <conio.h>
#include <windows.h>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <algorithm>
#include <nlohmann/json.hpp> // Library JSON modern C++ (https://github.com/nlohmann/json)
#include <mmsystem.h> // Tambahkan untuk PlaySound
#pragma comment(lib, "winmm.lib")
bool soundEnabled = true;
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

    // Tambahkan fungsi untuk mengubah elemen pada index tertentu
    void setAt(int idx, const string& val) {
        if (idx >= 0 && idx < top) {
            isi[idx] = val;
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
const string KOIN_MARKER = "$$";
const string KOIN_ART = "  $  ";
const int KOIN_SCORE = 300;
const int KOIN_SHOW_FRAMES = 15; // Tampilkan "+300" selama 15 frame (~0.3-0.5 detik)

int posisiMobil = 1;

// Tambah variabel global untuk nyawa dan invulnerable
int nyawa = 3;
bool invulnerable = false;
DWORD invulnerableStart = 0;
const DWORD INVULNERABLE_DURATION = 2000; // 2 detik

// Tambah variabel global untuk efek koin
int koinShowCounter = 0;

using json = nlohmann::json;

// Fungsi untuk menyimpan skor ke file JSON
void simpanScore(const string& user, int score) {
    json data;
    ifstream in("scores.json");
    if (in.is_open()) {
        try {
            in >> data;
        } catch (...) {
            data = json::array();
        }
        in.close();
    } else {
        data = json::array();
    }
    data.push_back({ {"user", user}, {"score", score} });
    ofstream out("scores.json");
    out << data.dump(4);
    out.close();
}

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
    // Cek apakah posisi mobil bertabrakan dengan rintangan atau koin
    int lintasanMobil = posisiMobil * 2 + 1;
    vector<string> contents = jalur[lintasanMobil].getContents();
    // Cek collision rintangan (2 cell terdepan)
    if (contents.size() > 1 && !invulnerable) {
        string depan = contents[0] + contents[1];
        if (depan.find(RINTANGAN_MARKER) != string::npos) {
            // Sound effect rintangan
            if (soundEnabled) {
                PlaySound(TEXT("hit.wav"), NULL, SND_FILENAME | SND_ASYNC);
            }
            nyawa--;
            invulnerable = true;
            invulnerableStart = GetTickCount();
        }
    }
    // Cek collision koin (perbesar: 4 cell terdepan)
    if (contents.size() > 0) {
        int maxCek = min(4, (int)contents.size());
        string depanKoin = "";
        for (int i = 0; i < maxCek; ++i) {
            depanKoin += contents[i];
        }
        size_t posKoin = depanKoin.find(KOIN_MARKER);
        if (posKoin != string::npos) {
            koinShowCounter = KOIN_SHOW_FRAMES;
            // Sound effect koin
            if (soundEnabled) {
                PlaySound(TEXT("coin.wav"), NULL, SND_FILENAME | SND_ASYNC);
            }
            // Tentukan cell mana yang kena koin, lalu hapus koin dari cell tersebut
            int cellIdx = 0, charCount = 0;
            for (; cellIdx < maxCek; ++cellIdx) {
                if (posKoin < charCount + contents[cellIdx].size()) break;
                charCount += contents[cellIdx].size();
            }
            if (cellIdx < maxCek) {
                jalur[lintasanMobil].setAt(cellIdx, (lintasanMobil % 2 == 1) ? SIMBOL_JALAN : SIMBOL_KOSONG);
            }
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

void tampilkanEndScreen(int score) {
    for(int i = 0; i < LEBAR_LAYAR * TINGGI_LAYAR; ++i) { consoleBuffer[i] = {' ', 7}; }
    string msg = "SELAMAT!";
    string msg2 = "Kamu telah menyelesaikan permainan!";
    string msg3 = "Skor: " + to_string(score);
    gambarKeBuffer(LEBAR_LAYAR / 2 - (int)(msg.length()/2), TINGGI_LAYAR / 2, msg, 12);
    gambarKeBuffer(LEBAR_LAYAR / 2 - (int)(msg2.length()/2), TINGGI_LAYAR / 2 + 1, msg2, 12);
    gambarKeBuffer(LEBAR_LAYAR / 2 - (int)(msg3.length()/2), TINGGI_LAYAR / 2 + 2, msg3, 12);
    tampilkanBuffer();
    Sleep(3000);
}

// Tambah parameter untuk difficulty dan spawnMultiplier
void mainGame(int difficulty, float spawnMultiplier, float scoreMultiplier, string user) {
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
    int score = 0;

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
        int koinSpawnCounter = 0;
        invulnerable = false;
        invulnerableStart = 0;
        koinShowCounter = 0;
        int pendingKoinScore = 0;

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

            // Gambar koin di lintasan
            for (int baris = 0; baris < TOTAL_LINTASAN; baris++) {
                // Mengambil isi dari CustomQueue untuk digambar
                vector<string> contents = jalur[baris].getContents();
                string lineContent = "";
                for(const auto& s : contents) {
                    lineContent += s;
                }
                
                string tempLine = lineContent;
                size_t koinPos = tempLine.find(KOIN_MARKER);
                while (koinPos != string::npos) {
                    gambarKeBuffer(4 + koinPos, 4 + baris, KOIN_ART, 14);
                    koinPos = tempLine.find(KOIN_MARKER, koinPos + KOIN_MARKER.length());
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

            // Tambahkan skor setiap frame
            score += static_cast<int>(scoreMultiplier * level);

            // Tambahkan skor koin jika baru saja diambil
            if (koinShowCounter == KOIN_SHOW_FRAMES - 1) {
                score += KOIN_SCORE;
            }

            // Tampilkan skor di layar
            gambarKeBuffer(50, 1, "| Score: " + to_string(score), 11);
            if (koinShowCounter > 0) {
                gambarKeBuffer(65, 1, "+300", 14);
                koinShowCounter--;
            }

            tampilkanBuffer();

            pindahMobil();
            jalurBerjalan();
            cekTabrakan();
            updateInvulnerable();

            spawnCounter++;
            koinSpawnCounter++;

            // Spawn rintangan
            if (spawnCounter >= spawnRate) {
                spawnCounter = 0;
                int mode = rand() % 10; // 0-9, 0-2: double spawn (30%), 3-9: single spawn (70%)
                if (mode < 3) {
                    // Double spawn: pilih 2 lane berbeda (selain lajurAman)
                    int lajurAman = rand() % TOTAL_CABANG;
                    vector<int> lajurTidakAman;
                    for (int i = 0; i < TOTAL_CABANG; i++) {
                        if (i != lajurAman) lajurTidakAman.push_back(i);
                    }
                    // Pilih dua lane berbeda
                    if (lajurTidakAman.size() >= 2) {
                        int idx1 = rand() % lajurTidakAman.size();
                        int idx2;
                        do {
                            idx2 = rand() % lajurTidakAman.size();
                        } while (idx2 == idx1);
                        int lintasan1 = lajurTidakAman[idx1] * 2 + 1;
                        int lintasan2 = lajurTidakAman[idx2] * 2 + 1;
                        jalur[lintasan1].updateBack(RINTANGAN_MARKER);
                        jalur[lintasan2].updateBack(RINTANGAN_MARKER);
                    } else {
                        // fallback ke single spawn jika hanya satu lajurTidakAman
                        int lintasan = lajurTidakAman[0] * 2 + 1;
                        jalur[lintasan].updateBack(RINTANGAN_MARKER);
                    }
                } else {
                    // Single spawn seperti biasa
                    int lajurAman = rand() % TOTAL_CABANG;
                    vector<int> lajurTidakAman;
                    for (int i = 0; i < TOTAL_CABANG; i++) {
                        if (i != lajurAman) lajurTidakAman.push_back(i);
                    }
                    int targetLajur = lajurTidakAman[rand() % lajurTidakAman.size()];
                    int lintasan = targetLajur * 2 + 1;
                    jalur[lintasan].updateBack(RINTANGAN_MARKER);
                }
            }

            // Spawn koin: setiap 45-70 frame (acak), tidak tergantung difficulty
            static int nextKoinFrame = 0;
            if (nextKoinFrame == 0) {
                nextKoinFrame = 45 + rand() % 26; // 45-70 frame
            }
            if (koinSpawnCounter >= nextKoinFrame) {
                koinSpawnCounter = 0;
                nextKoinFrame = 45 + rand() % 26;
                // Pilih lintasan random (hanya di lajur jalan, bukan kosong)
                int lajurKoin = rand() % TOTAL_CABANG;
                int lintasanKoin = lajurKoin * 2 + 1;
                // Pastikan tidak menimpa rintangan/koin di 2 posisi belakang (seluruh cell, bukan hanya substring)
                vector<string> isi = jalur[lintasanKoin].getContents();
                bool aman = true;
                int cekN = 3; // cek 3 cell terakhir
                if (isi.size() >= cekN) {
                    for (int i = 1; i <= cekN; ++i) {
                        const string& cell = isi[isi.size() - i];
                        if (cell.find(RINTANGAN_MARKER) != string::npos || cell.find(KOIN_MARKER) != string::npos) {
                            aman = false;
                            break;
                        }
                    }
                } else {
                    aman = false;
                }
                if (aman) {
                    jalur[lintasanKoin].updateBack(KOIN_MARKER);
                }
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

        } else if (level == totalLevel) {
            tampilkanEndScreen(score);
            while (_kbhit()) _getch();
            _getch();
        } 
    }

    // Simpan skor ke file JSON setelah game selesai
    simpanScore(user, score);

    cursorInfo.bVisible = true;
    SetConsoleCursorInfo(hConsole, &cursorInfo);
    SetConsoleTextAttribute(hConsole, 7); 
}

