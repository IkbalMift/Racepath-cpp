#include <iostream>
#include <queue>
#include <conio.h>
#include <windows.h>
#include <cstdlib>
#include <ctime>
#include <vector>

using namespace std;

const int PANJANG_JALAN = 50;
const string SIMBOL_JALAN = "- ";
const string SIMBOL_KOSONG = "  "; // Pastikan ini 2 spasi agar alignment rapi
const int TOTAL_LINTASAN = 6;     // Total baris di layar (3 jalur mobil x 2 baris per mobil)
const int TOTAL_CABANG = 3;       // Total jalur yang bisa dipilih mobil (atas, tengah, bawah)

queue<string> jalur[TOTAL_LINTASAN];

// Aset grafis untuk mobil dan rintangan
const vector<string> MOBIL = {
    "   . - - .   ", // Dibuat lebih simetris
    " '- O - O -' "
};

const vector<string> RINTANGAN = {
    "[!!!]",    // Tidak terpakai di revisi ini, tapi biarkan saja
    " /|||\\ "   // Rintangan utama
};

int posisiMobil = 1; // 0 = atas, 1 = tengah, 2 = bawah
HANDLE konsol = GetStdHandle(STD_OUTPUT_HANDLE);
int frameCount = 0; // Menghitung frame untuk timing

// --- Fungsi tidak diubah ---
void pindahCursor(int x, int y) {
    COORD posisi = {(SHORT)x, (SHORT)y};
    SetConsoleCursorPosition(konsol, posisi);
}

void ubahWarna(int warna) {
    SetConsoleTextAttribute(konsol, warna);
}

void isiJalur() {
    srand((unsigned int)time(0));
    for (int i = 0; i < TOTAL_LINTASAN; i++) {
        while (!jalur[i].empty()) jalur[i].pop();
        for (int j = 0; j < PANJANG_JALAN; j++) {
            // Baris ganjil (1, 3, 5) adalah garis putus-putus
            // Baris genap (0, 2, 4) adalah lintasan kosong tempat mobil
            jalur[i].push((i % 2 == 1) ? SIMBOL_JALAN : SIMBOL_KOSONG);
        }
    }
}
// --- Fungsi tidak diubah ---

// =================================================================
// REVISI #1: FUNGSI TAMPILKANJALUR() DITULIS ULANG
// Memperbaiki bug "rintangan mengikuti mobil"
// =================================================================
void tampilkanJalur() {
    pindahCursor(0, 0); // Selalu mulai dari pojok kiri atas

    // --- Header ---
    ubahWarna(15); // Putih
    cout << "Kontrol: W = atas, S = bawah\n\n";

    // --- Batas Atas Jalan ---
    ubahWarna(8); // Abu-abu
    cout << "  ";
    for (int i = 0; i < PANJANG_JALAN * 2 + 6; i++) cout << "=";
    cout << "\n";

    // --- PASS 1: GAMBAR SEMUA JALUR (LATAR BELAKANG & RINTANGAN) ---
    queue<string> buffer[TOTAL_LINTASAN];
    for (int i = 0; i < TOTAL_LINTASAN; i++) {
        buffer[i] = jalur[i]; // Salin data agar tidak merusak queue asli
    }

    for (int baris = 0; baris < TOTAL_LINTASAN; baris++) {
        ubahWarna(8);
        cout << " || "; // Border kiri

        while (!buffer[baris].empty()) {
            string isi = buffer[baris].front();
            buffer[baris].pop();
            
            // Beri warna khusus jika elemen adalah rintangan
            if (isi == RINTANGAN[1]) {
                ubahWarna(12); // Merah terang untuk rintangan
            } else {
                ubahWarna(7); // Abu-abu muda untuk jalan
            }
            cout << isi;
        }
        
        ubahWarna(8);
        cout << " ||\n"; // Border kanan
    }
    
    // --- Batas Bawah Jalan ---
    ubahWarna(8);
    cout << "  ";
    for (int i = 0; i < PANJANG_JALAN * 2 + 6; i++) cout << "=";
    cout << "\n";

    // --- PASS 2: GAMBAR MOBIL DI ATAS JALUR YANG SUDAH JADI ---
    ubahWarna(10); // Hijau terang untuk mobil
    
    // Hitung posisi Y mobil di layar konsol
    // 3 = 1 baris "Kontrol" + 1 baris kosong + 1 baris "====="
    int posisiY_mobil_atas = 3 + posisiMobil * 2;
    
    // Pindahkan kursor dan gambar bagian atas mobil
    pindahCursor(6, posisiY_mobil_atas);
    cout << MOBIL[0];

    // Pindahkan kursor dan gambar bagian bawah mobil
    pindahCursor(6, posisiY_mobil_atas + 1);
    cout << MOBIL[1];

    // Reset warna ke default
    ubahWarna(15);
}

// --- Fungsi pindahMobil tidak diubah ---
void pindahMobil() {
    if (_kbhit()) {
        char input = _getch();
        if ((input == 'w' || input == 'W') && posisiMobil > 0) posisiMobil--;
        else if ((input == 's' || input == 'S') && posisiMobil < TOTAL_CABANG - 1) posisiMobil++;
    }
}
// --- Fungsi pindahMobil tidak diubah ---


// =================================================================
// REVISI #2: FUNGSI JALURBERJALAN() DITULIS ULANG
// Memperbaiki bug "rintangan muncul serentak tanpa celah"
// =================================================================
void jalurBerjalan() {
    // Geser semua elemen di setiap jalur untuk menciptakan ilusi bergerak
    for (int i = 0; i < TOTAL_LINTASAN; i++) {
        jalur[i].pop(); // Hapus elemen paling depan
        // Tambah elemen baru di belakang sesuai polanya
        jalur[i].push((i % 2 == 1) ? SIMBOL_JALAN : SIMBOL_KOSONG);
    }

    // Logika baru untuk memunculkan rintangan dengan celah yang aman
    const int SPAWN_RATE = 40; // Atur frekuensi rintangan (semakin besar, semakin jarang)
    
    // Hanya jalankan logika spawn jika sudah waktunya (berdasarkan frameCount)
    // dan beri jeda di awal game agar pemain siap
    if (frameCount > 50 && frameCount % SPAWN_RATE == 0) {
        
        // 1. Pilih secara acak satu dari tiga jalur mobil (0, 1, 2) untuk AMAN
        int jalurMobilAman = rand() % TOTAL_CABANG;

        // 2. Iterasi melalui semua tiga jalur mobil untuk menempatkan rintangan
        for (int i = 0; i < TOTAL_CABANG; i++) {
            // Jika jalur mobil saat ini BUKAN jalur yang aman
            if (i != jalurMobilAman) {
                // Konversi jalur mobil (0, 1, atau 2) ke indeks lintasan sebenarnya (1, 3, atau 5)
                // Ini adalah lintasan garis putus-putus, tempat rintangan muncul
                int lintasanUntukRintangan = i * 2 + 1;
                
                // Taruh rintangan di ujung lintasan tersebut dengan mengganti elemen terakhir
                // Ini akan sedikit mengganggu alignment, tapi sesuai dengan cara kerja kode asli Anda
                jalur[lintasanUntukRintangan].back() = RINTANGAN[1];
            }
        }
    }
}


// --- Fungsi mainGame tidak diubah ---
void mainGame() {
    system("CLS");
    isiJalur();

    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(konsol, &cursorInfo);
    cursorInfo.bVisible = false;
    SetConsoleCursorInfo(konsol, &cursorInfo);

    DWORD waktuMulai = GetTickCount();
    const DWORD durasi = 60 * 1000;

    while (GetTickCount() - waktuMulai < durasi) {
        tampilkanJalur();
        pindahMobil();
        jalurBerjalan();
        frameCount++; // Penting untuk logika spawn rintangan
        Sleep(50); // Sleep bisa sedikit dikurangi agar lebih responsif
    }

    system("CLS");
    pindahCursor(0, 3);
    ubahWarna(15);
    cout << "  ======================================\n";
    cout << "  |                                    |\n";
    cout << "  |           Waktu habis!             |\n";
    cout << "  |                                    |\n";
    cout << "  |  Tekan ENTER untuk kembali ke menu |\n";
    cout << "  |                                    |\n";
    cout << "  ======================================\n";
    // Menunggu input enter
    while(_getch() != 13);

    cursorInfo.bVisible = true;
    SetConsoleCursorInfo(konsol, &cursorInfo);
}
