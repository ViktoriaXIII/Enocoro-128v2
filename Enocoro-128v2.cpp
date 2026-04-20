#include <iostream>
#include <vector>
#include <iomanip>
#include <cstdint>
using namespace std;

struct uint128_t {
    uint64_t part[2];
};

template <typename T>
vector<uint8_t> readBytes() {
    constexpr size_t numBytes = sizeof(T);
    vector<uint8_t> bytes(numBytes);
    for (size_t i = 0; i < numBytes; i++) {
        unsigned int tempValue;
        if (!(cin >> hex >> tempValue)) {
            cerr << "Error!!! Invalid input" << endl;
            break;
        }
        bytes[i] = static_cast<uint8_t>(tempValue);
    }
    return bytes;
}

void printBytes(const string& label, const vector<uint8_t>& bytes) {
    cout << label << ": ";
    for (size_t i = 0; i < bytes.size(); i++) cout << hex << uppercase << setw(2) << setfill('0') << (int)bytes[i] << (i < bytes.size() - 1 ? " " : "");
    cout << dec << " (" << bytes.size() << " bytes)" << endl;
}

uint8_t Xtime(uint8_t X) { // Mult by 2 in GF(2^8) with irr.p. f(x) = x^8 + x^4 + x^3 + x + 1 = 0x11b
    if (X & 0x80) return static_cast<uint8_t>((X << 1) ^ 0x1b);
    else return static_cast<uint8_t>(X << 1);
}

vector<uint8_t> Init(const vector<uint8_t>& K, const vector<uint8_t>& IV) {
    vector<uint8_t> constants = { 0x66, 0xe9, 0x4b, 0xd4, 0xef, 0x8a, 0x2c, 0x3b };
    vector<uint8_t> bValues;
    bValues.reserve(32);
    bValues.insert(bValues.end(), K.begin(), K.end());
    bValues.insert(bValues.end(), IV.begin(), IV.end());
    bValues.insert(bValues.end(), constants.begin(), constants.end());
    printBytes("bValues", bValues);
    vector<uint8_t> aValues = { 0x88, 0x4c };
    printBytes("aValues", aValues);
    uint8_t counter = 1;
    vector<uint8_t> S_next;
    for (int i = -96; i < 0; i++) {
        bValues[31] = bValues[31] ^ counter;
        counter = Xtime(counter);
        //S_next = Next(S);
    }
    return S_next;
}

void encryptFile() {
    cout << "~~~For encryption you must enter Key (128 bit) and IV (64 bit) in format:" << endl;
    cout << "~~~ 00 00 00 ... 00 00 00" << endl;
    cout << "Enter the Key: ";
    auto key = readBytes<uint128_t>();
    printBytes("Key", key);
    cout << "Enter the IV: ";
    auto iv = readBytes<uint64_t>();
    printBytes("IV", iv);
    vector<uint8_t> S_0 = Init(key, iv);
}

void decryptFile() {

}

int main()
{
    int choice;
    bool running = true;
    cout << "Welcome!" << endl;
    while (running) {
        cout << "Possible options of the program:" << endl;
        cout << "0. Exit" << endl;
        cout << "1. Encode file with Enocoro-128v2" << endl;
        cout << "2. Decode file with Enocoro-128v2" << endl;
        cout << "Your choice: ";
        if (!(cin >> choice)) {
            cout << "Error!!! Invalid input";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        switch (choice) {
        case 0:
            cout << "You're exiting the program. Thank you for using it!" << endl;
            running = false;
            break;
        case 1:
            encryptFile();
            break;
        case 2: 
            decryptFile();
            break;
        default:
            cout << "There's no such option. Please, try again" << endl;
        }
    }
}