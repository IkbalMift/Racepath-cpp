#include <iostream>
#include <queue>
#include <conio.h>
#include <windows.h>

using namespace std;

const int PANJANG_JALAN = 15;

const string SIMBOL_JALAN = "- ";
const string SIMBOL_KOSONG = "  ";
const int TOTAL_LINTASAN = 6;
const int TOTAL_CABANG = 3; // atas, tengah, bawah

queue<string> jalur[TOTAL_LINTASAN];

// Mobil ASCII-art 2 baris
const vector<string> MOBIL = {
    ",---'   \\----.",
    "'--()----()---'"};
// const vector<string> MOBIL = {
//     "  . - - ` - .",
//     "' - O - - O -'"
// };

int posisiMobil = 1; // 0 = atas, 1 = tengah, 2 = bawah

HANDLE konsol = GetStdHandle(STD_OUTPUT_HANDLE);

void pindahCursor(int x, int y)
{
    COORD posisi = {(SHORT)x, (SHORT)y};
    SetConsoleCursorPosition(konsol, posisi);
}

void ubahWarna(int warna)
{
    SetConsoleTextAttribute(konsol, warna);
}

void isiJalur()
{
    for (int i = 0; i < TOTAL_LINTASAN; i++)
    {
        while (!jalur[i].empty())
            jalur[i].pop();
    }

    for (int i = 0; i < PANJANG_JALAN; i++)
    {
        for (int j = 0; j < TOTAL_LINTASAN; j++)
        {
            // Hanya lintasan 0, 2, 4 yang punya garis markah
            string pola = (j % 2 == 1 && i % 2 == 0) ? "- " : "  ";
            jalur[j].push(pola);
        }
    }
}

void tampilkanJalur()
{
    pindahCursor(0, 0);

    ubahWarna(15);
    cout << "Kontrol: W = atas, S = bawah\n\n";

    ubahWarna(8);
    cout << "  ==============================\n";

    for (int baris = 0; baris < TOTAL_LINTASAN; baris++)
    {
        ubahWarna(8);
        cout << " || ";

        for (int kolom = 0; kolom < PANJANG_JALAN; kolom++)
        {
            string blok = jalur[baris].front();
            jalur[baris].pop();

            // Cek posisi mobil (atas dan bawah)
            if (kolom == 2 && baris == posisiMobil * 2)
            {
                ubahWarna(10);
                cout << MOBIL[0];
                kolom += MOBIL[0].length() / 2;
            }
            else if (kolom == 2 && baris == posisiMobil * 2 + 1)
            {
                ubahWarna(10);
                cout << MOBIL[1];
                kolom += MOBIL[1].length() / 2;
            }
            else
            {
                ubahWarna(7);
                cout << blok;
            }

            jalur[baris].push(blok); // Masukkan kembali ke queue
        }

        ubahWarna(8);
        cout << " ||\n";
    }

    ubahWarna(8);
    cout << "  ==============================\n";
    ubahWarna(15);
}

void pindahMobil()
{
    if (_kbhit())
    {
        char input = _getch();

        if ((input == 'w' || input == 'W') && posisiMobil > 0)
        {
            posisiMobil--;
        }
        else if ((input == 's' || input == 'S') && posisiMobil < TOTAL_CABANG - 1)
        {
            posisiMobil++;
        }
    }
}

void jalurBerjalan()
{
    for (int i = 0; i < TOTAL_LINTASAN; i++)
    {
        string depan = jalur[i].front();
        jalur[i].pop();
        jalur[i].push(depan);
    }
}

void mainGame()
{
    system("CLS");

    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(konsol, &cursorInfo);
    cursorInfo.bVisible = false;
    SetConsoleCursorInfo(konsol, &cursorInfo);

    const int totalLevel = 3;
    const DWORD durasi = 2 * 60 * 1000; // 2 menit per level

    for (int level = 1; level <= totalLevel; level++)
    {
        isiJalur();
        posisiMobil = 1;

        DWORD waktuMulai = GetTickCount();
        while (GetTickCount() - waktuMulai < durasi)
        {
            tampilkanJalur();
            ubahWarna(14);
            pindahCursor(0, 20);
            cout << "Level: " << level << " / " << totalLevel;
            pindahCursor(0, 21);
            DWORD waktuTersisa = durasi - (GetTickCount() - waktuMulai);
            cout << " Waktu tersisa: " << waktuTersisa / 1000 << " detik ";
            pindahMobil();
            jalurBerjalan();
            Sleep(150);
        }

        // Tampilkan layar finish level
        system("CLS");
        cout << "\n\n\n";
        cout << "  ======================================\n";
        cout << "  |     Level " << level << " selesai!               |\n";
        if (level < totalLevel)
        {
            cout << "  |  Tekan ENTER untuk ke level berikut |\n";
        }
        else
        {
            cout << "  |      Semua level selesai! 🎉       |\n";
            cout << "  |     Tekan ENTER untuk ke menu       |\n";
        }
        cout << "  ======================================\n";
        cin.ignore();
        cin.get();
    }
}
