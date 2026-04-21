#include <iostream>
#include <vector>
#include <iomanip>
#include <cstdint>
#include <array>
#include <algorithm>
using namespace std;

struct uint128_t {
    uint64_t part[2];
};

struct State {
    uint8_t a[2];
    uint8_t b[32];
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

const uint8_t S_8[256] = {
    99, 82, 26, 223, 138, 246, 174, 85, 137, 231, 208, 45, 189, 1, 36, 120,
    27, 217, 227, 84, 200, 164, 236, 126, 171, 0, 156, 46, 145, 103, 55, 83,
    78, 107, 108, 17, 178, 192, 130, 253, 57, 69, 254, 155, 52, 215, 167, 8,
    184, 154, 51, 198, 76, 29, 105, 161, 110, 62, 197, 10, 87, 244, 241, 131,
    245, 71, 31, 122, 165, 41, 60, 66, 214, 115, 141, 240, 142, 24, 170, 193,
    32, 191, 230, 147, 81, 14, 247, 152, 221, 186, 106, 5, 72, 35, 109, 212,
    30, 96, 117, 67, 151, 42, 49, 219, 132, 25, 175, 188, 204, 243, 232, 70,
    136, 172, 139, 228, 123, 213, 88, 54, 2, 177, 7, 114, 225, 220, 95, 47,
    93, 229, 209, 12, 38, 153, 181, 111, 224, 74, 59, 222, 162, 104, 146, 23,
    202, 238, 169, 182, 3, 94, 211, 37, 251, 157, 97, 89, 6, 144, 116, 44,
    39, 149, 160, 185, 124, 237, 4, 210, 80, 226, 73, 119, 203, 58, 15, 158,
    112, 22, 92, 239, 33, 179, 159, 13, 166, 201, 34, 148, 250, 75, 216, 101,
    133, 61, 150, 40, 20, 91, 102, 234, 127, 206, 249, 64, 19, 173, 195, 176,
    242, 194, 56, 128, 207, 113, 11, 135, 77, 53, 86, 233, 100, 190, 28, 187,
    183, 48, 196, 43, 255, 98, 65, 168, 21, 140, 18, 199, 121, 143, 90, 252,
    205, 9, 79, 125, 248, 134, 218, 16, 50, 118, 180, 163, 63, 68, 129, 235
};


vector<uint8_t> L(uint8_t U_0, uint8_t U_1) {
    vector<uint8_t> V(2);
    V[0] = U_0 ^ U_1;
    V[1] = U_0 ^ Xtime(U_1);
    return V;
}

vector<uint8_t> Rho(const vector<uint8_t>& A, const vector<uint8_t>& B) {
    uint8_t u_0 = A[0] ^ S_8[B[2]];
    uint8_t u_1 = A[1] ^ S_8[B[7]];
    vector<uint8_t> V = L(u_0, u_1);
    vector<uint8_t> A_next(2);
    A_next[0] = V[0] ^ S_8[B[16]];
    A_next[1] = V[1] ^ S_8[B[29]];
    return A_next;
}

vector<uint8_t> Lambda(const vector<uint8_t>& A, const vector<uint8_t>& B) {
    vector<uint8_t> B_next(32);
    for (int j = 1; j < 32; j++) B_next[j] = B[j - 1];
    B_next[0] = B[31] ^ A[0];
    B_next[3] = B[2] ^ B[6];
    B_next[8] = B[7] ^ B[15];
    B_next[17] = B[16] ^ B[28];
    return B_next;
}

State Next(const State& S) {
    vector<uint8_t> A = { S.a[0], S.a[1] };
    vector<uint8_t> B(S.b, S.b + 32);
    vector<uint8_t> aValues_next = Rho(A, B);
    vector<uint8_t> bValues_next = Lambda(A, B);
    State S_next{};
    copy(aValues_next.begin(), aValues_next.begin() + 2, S_next.a);
    copy(bValues_next.begin(), bValues_next.begin() + 32, S_next.b);
    return S_next;
}

State Init(const vector<uint8_t>& K, const vector<uint8_t>& IV) {
    vector<uint8_t> constants = { 0x66, 0xe9, 0x4b, 0xd4, 0xef, 0x8a, 0x2c, 0x3b };
    vector<uint8_t> bValues;
    bValues.reserve(32);
    bValues.insert(bValues.end(), K.begin(), K.end());
    bValues.insert(bValues.end(), IV.begin(), IV.end());
    bValues.insert(bValues.end(), constants.begin(), constants.end());
    vector<uint8_t> aValues = { 0x88, 0x4c };
    State S{};
    copy(aValues.begin(), aValues.begin() + 2, S.a);
    copy(bValues.begin(), bValues.begin() + 32, S.b);
    uint8_t counter = 1;
    State S_next = {};
    for (int i = -96; i < 0; i++) {
        bValues[31] = bValues[31] ^ counter;
        counter = Xtime(counter);
        S_next = Next(S);
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
    State S_0 = Init(key, iv);
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