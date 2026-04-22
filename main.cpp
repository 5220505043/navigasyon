//
//  main.cpp
//  5220505043navigasyon sistemi
//
//  Created by mavlyuda almazova on 22.04.2026.
//

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>

/**
 * 1. VERI KATMANI (Data Layer)
 * OOP Prensipleri: Encapsulation
 */
struct Point {
    double x;
    double y;
    int label; // +1 veya -1 (Engel sınıfları)
};

/**
 * 2. DOSYA YONETIM KATMANI (File Management Layer)
 * OOP Prensipleri: Separation of Concerns (Sorumlulukların Ayrılması)
 */
class DataManager {
public:
    static std::vector<Point> readFromTxt(const std::string& filename) {
        std::vector<Point> points;
        std::ifstream file(filename);
        
        if (!file.is_open()) {
            throw std::runtime_error("Dosya acilamadi! Lutfen 'veriler' dosyasinin konumunu kontrol edin.");
        }

        std::string line;
        while (std::getline(file, line)) {
            if (line.empty()) continue;
            std::stringstream ss(line);
            double x, y;
            int label;
            if (ss >> x >> y >> label) {
                points.push_back({x, y, label});
            }
        }
        file.close();
        return points;
    }
};

/**
 * 3. ALGORITMA VE MATEMATIKSEL MODEL KATMANI
 * "Maksimum Guvenlik Koridoru" burada hesaplanır.
 */
class SVMSolver {
private:
    double w1, w2, b;        // Agirliklar ve Bias (Denklem parametreleri)
    double learningRate;      // Ogrenme hizi
    double lambdaParam;       // Regularizasyon (Koridorun genisligini belirler)

public:
    SVMSolver(double lr = 0.0001, double lambda = 0.01)
        : w1(0.0), w2(0.0), b(0.0), learningRate(lr), lambdaParam(lambda) {}

    // Algoritmanin kalbi: Stochastic Gradient Descent
    void train(const std::vector<Point>& data, int epochs) {
        std::cout << "Egitim baslatiliyor..." << std::endl;
        
        for (int i = 1; i <= epochs; ++i) {
            for (const auto& p : data) {
                // Hinge Loss kosulu: y * (w.x - b) >= 1
                bool condition = p.label * (w1 * p.x + w2 * p.y - b) >= 1;

                if (condition) {
                    // Sadece regularizasyon uygula (Agirliklari kucult ki margin buyusun)
                    w1 -= learningRate * (2 * lambdaParam * w1);
                    w2 -= learningRate * (2 * lambdaParam * w2);
                } else {
                    // Hata var: Agirliklari veriye gore guncelle
                    w1 -= learningRate * (2 * lambdaParam * w1 - p.x * p.label);
                    w2 -= learningRate * (2 * lambdaParam * w2 - p.y * p.label);
                    b -= learningRate * p.label;
                }
            }
            // Her 2000 epoch'ta bir ilerlemeyi goster
            if (i % 2000 == 0) std::cout << "Epoch " << i << " tamamlandi..." << std::endl;
        }
    }

    void displayResults() const {
        std::cout << "\n--- OPTIMUM AYRISTIRICI SINIR SONUCLARI ---" << std::endl;
        std::cout << "Dogru Denklemi: " << std::fixed << std::setprecision(4)
                  << w1 << "x + " << w2 << "y + (" << -b << ") = 0" << std::endl;
        std::cout << "Bu dogru, engeller arasindaki MAKSIMUM mesafeyi (Guvenlik Koridoru) saglar." << std::endl;
    }
};

/**
 * 4. KONTROL KATMANI (Application Controller)
 */
int main() {
    // main içindeki satırı bununla değiştir:
    const std::string fileName = "/Users/mavlyudaalmazova/Downloads/5220505043navigasyon sistemi/5220505043navigasyon sistemi/veriler.txt";
    try {
        // 1. Veriyi Yukle
        std::vector<Point> data = DataManager::readFromTxt(fileName);
        std::cout << "Basariyla " << data.size() << " adet koordinat yuklendi." << std::endl;

        // 2. Modeli Olustur ve egit
        // Parametreler: (Ogrenme Hizi, Lambda, Epoch Sayisi)
        SVMSolver solver(0.0001, 0.01);
        solver.train(data, 10000);

        // 3. Sonuclari Yazdir
        solver.displayResults();

    } catch (const std::exception& e) {
        std::cerr << "\nKritik Hata: " << e.what() << std::endl;
        std::cerr << "I pucu: 'veriler' dosyasinin exe ile ayni klasorde oldugundan emin olun." << std::endl;
        return 1;
    }

    return 0;
}
