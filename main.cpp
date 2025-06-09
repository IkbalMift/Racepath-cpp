#include <iostream>
#include "game.h"
#include <fstream>
#include <vector>
#include <algorithm>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

using namespace std;

void tampilkanTopScore() {
    std::ifstream in("scores.json");
    json data;
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
    // Ambil vector pair<score, user>
    std::vector<std::pair<int, std::string>> daftar;
    for (const auto& entry : data) {
        if (entry.contains("user") && entry.contains("score")) {
            daftar.emplace_back(entry["score"].get<int>(), entry["user"].get<std::string>());
        }
    }
    // Urutkan dari skor terbesar
    std::sort(daftar.begin(), daftar.end(), [](const auto& a, const auto& b) {
        return a.first > b.first;
    });
    std::cout << "========== TOP 10 SCORE ==========" << std::endl;
    std::cout << "| No |      Nama      |   Score   |" << std::endl;
    std::cout << "----------------------------------" << std::endl;
    int n = std::min(10, (int)daftar.size());
    for (int i = 0; i < n; ++i) {
        std::cout << "| " << (i+1) << "  | ";
        std::cout.width(13); std::cout << std::left << daftar[i].second << " | ";
        std::cout.width(8); std::cout << std::right << daftar[i].first << " |" << std::endl;
    }
    if (n == 0) std::cout << "|         Belum ada skor         |" << std::endl;
    std::cout << "----------------------------------" << std::endl;
    system("pause");
}

void menu() {
    int difficulty = 1; 
    float spawnMultiplier = 1.0f;
    float scoreMultiplier = 1.0f;
    string user;

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
            system("CLS");
            cout << "Masukkan nama Anda: "; cin >> user;
            cin.ignore(); // Clear newline character from input buffer
            while (true) {
                cout << "Pilih Difficulty:" << endl;
                cout << "1. Normal" << endl;
                cout << "2. Hard" << endl;
                cout << "3. Extreme" << endl;
                cout << "Pilihan (1-3): "; cin >> difficulty;
                if (difficulty == 1) {
                    spawnMultiplier = 1.0f; // Normal
                    scoreMultiplier = 1.0f;
                    break;
                } else if (difficulty == 2) {
                    spawnMultiplier = 0.7f; // Hard
                    scoreMultiplier = 1.5f;
                    break;
                } else if (difficulty == 3) {
                    spawnMultiplier = 0.5f; // Extreme
                    scoreMultiplier = 2.0f;
                    break;
                } else {
                    cout << "Pilihan tidak valid, coba lagi!\n";
                    system("CLS");
                }
            }
            cout << "Difficulty di-set ke ";
            if (difficulty == 1) cout << "Normal\n";
            else if (difficulty == 2) cout << "Hard\n";
            else cout << "Extreme\n";

            cout << "Memulai permainan... tekan tombol apapun untuk melanjutkan.\n";
            system("pause");

            mainGame(difficulty, spawnMultiplier, scoreMultiplier, user);
            break;
        case 2: {
            system("CLS");
            cout << "========== OPTIONS ==========" << endl;
            cout << "Pilih Difficulty:" << endl;
            cout << "1. Normal" << endl;
            cout << "2. Hard" << endl;
            cout << "3. Extreme" << endl;
            cout << "Pilihan (1-3): ";
            cin >> difficulty;
            if (difficulty == 1) {
                spawnMultiplier = 1.0f;
            } else if (difficulty == 2) {
                spawnMultiplier = 0.7f;
            } else if (difficulty == 3) {
                spawnMultiplier = 0.5f; 
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
            system("CLS");
            tampilkanTopScore();
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