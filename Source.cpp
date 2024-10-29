#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <cmath>
#include <bitset>

using namespace std;

// ��������� ��� �������� ������ ��������
struct Symbol {
    double low;
    double high;
    double range;
};

// ������� ��� ������ ������ �������� �� �����
void readFrequencies(const string& filename, map<char, int>& frequencies) {
    ifstream input(filename);
    char c;
    while (input.get(c)) {
        frequencies[c]++;
    }
    input.close();
}

// ������� ��� ���������� ������ ��������
void calculateFrequencies(const map<char, int>& frequencies, map<char, Symbol>& symbols, int total) {
    double low = 0.0;
    for (const auto& pair : frequencies) {
        char c = pair.first;
        int count = pair.second;
        double range = static_cast<double>(count) / total;
        symbols[c] = { low, low + range, range };
        low += range;
    }
}

// ������� ��� ��������������� �����������
void encode(const string& inputFilename, const string& outputFilename, const map<char, Symbol>& symbols) {
    ifstream input(inputFilename);
    ofstream output(outputFilename, ios::binary);

    double low = 0.0;
    double high = 1.0;
    double range = 1.0;

    char c;
    while (input.get(c)) {
        double newLow = low + range * symbols.at(c).low;
        double newHigh = low + range * symbols.at(c).high;
        low = newLow;
        high = newHigh;
        range = high - low;
    }
    input.close();

    // ���������� ��������� ����������� � ����
    double code = (low + high) / 2;
    output.write(reinterpret_cast<const char*>(&code), sizeof(code));
    output.close();
}

// ������� ��� ��������������� �������������
void decode(const string& inputFilename, const string& outputFilename, const map<char, Symbol>& symbols, int total) {
    ifstream input(inputFilename, ios::binary);
    ofstream output(outputFilename);

    double code;
    input.read(reinterpret_cast<char*>(&code), sizeof(code));

    double low = 0.0;
    double high = 1.0;
    double range = 1.0;

    for (int i = 0; i < total; ++i) {
        double value = (code - low) / range;
        for (const auto& pair : symbols) {
            char c = pair.first;
            const Symbol& symbol = pair.second;
            if (value >= symbol.low && value < symbol.high) {
                output.put(c);
                low = low + range * symbol.low;
                high = low + range * symbol.range;
                range = high - low;
                break;
            }
        }
    }

    input.close();
    output.close();
}

int main() {
    setlocale(LC_ALL, "Rus");

    map<char, int> frequencies;
    map<char, Symbol> symbols;

    // ������ ������ �������� �� �����
    readFrequencies("input.txt", frequencies);

    // ���������� ������ ���������� ��������
    int total = 0;
    for (const auto& pair : frequencies) {
        total += pair.second;
    }

    // ���������� ������ ��������
    calculateFrequencies(frequencies, symbols, total);

    // �����������
    encode("input.txt", "encoded.bin", symbols);

    // �������������
    decode("encoded.bin", "decoded.txt", symbols, total);

    cout << "�������������� ����������� � ������������� ���������." << endl;

    return 0;
}