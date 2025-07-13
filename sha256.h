#pragma once
#include <string>
#include <sstream>
#include <iomanip>
#include <cstring>
#include <vector>
using namespace std;

// Portable SHA256 based on https://stackoverflow.com/a/10632725
// SHA256 Constants
const uint32_t k[64] = {
    0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,
    0x59f111f1,0x923f82a4,0xab1c5ed5,0xd807aa98,0x12835b01,
    0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,
    0xc19bf174,0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,
    0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,0x983e5152,
    0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,
    0x06ca6351,0x14292967,0x27b70a85,0x2e1b2138,0x4d2c6dfc,
    0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
    0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,
    0xd6990624,0xf40e3585,0x106aa070,0x19a4c116,0x1e376c08,
    0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,
    0x682e6ff3,0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,
    0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2
};

inline uint32_t rotr(uint32_t x, uint32_t n) {
    return (x >> n) | (x << (32 - n));
}

inline uint32_t choose(uint32_t e, uint32_t f, uint32_t g) {
    return (e & f) ^ (~e & g);
}

inline uint32_t majority(uint32_t a, uint32_t b, uint32_t c) {
    return (a & b) ^ (a & c) ^ (b & c);
}

inline uint32_t sig0(uint32_t x) {
    return rotr(x, 2) ^ rotr(x, 13) ^ rotr(x, 22);
}

inline uint32_t sig1(uint32_t x) {
    return rotr(x, 6) ^ rotr(x, 11) ^ rotr(x, 25);
}

inline uint32_t theta0(uint32_t x) {
    return rotr(x, 7) ^ rotr(x, 18) ^ (x >> 3);
}

inline uint32_t theta1(uint32_t x) {
    return rotr(x, 17) ^ rotr(x, 19) ^ (x >> 10);
}

string sha256(const string &input) {
    uint32_t h[8] = {
        0x6a09e667,0xbb67ae85,0x3c6ef372,0xa54ff53a,
        0x510e527f,0x9b05688c,0x1f83d9ab,0x5be0cd19
    };

    vector<uint8_t> msg(input.begin(), input.end());
    size_t l = msg.size() * 8;

    msg.push_back(0x80);
    while ((msg.size() * 8) % 512 != 448) msg.push_back(0x00);

    for (int i = 7; i >= 0; --i)
        msg.push_back((l >> (i * 8)) & 0xFF);

    for (size_t i = 0; i < msg.size(); i += 64) {
        uint32_t w[64] = {0};
        for (int j = 0; j < 16; ++j) {
            w[j] = (msg[i + 4 * j] << 24) |
                   (msg[i + 4 * j + 1] << 16) |
                   (msg[i + 4 * j + 2] << 8) |
                   (msg[i + 4 * j + 3]);
        }

        for (int j = 16; j < 64; ++j)
            w[j] = theta1(w[j - 2]) + w[j - 7] + theta0(w[j - 15]) + w[j - 16];

        uint32_t a = h[0], b = h[1], c = h[2], d = h[3];
        uint32_t e = h[4], f = h[5], g = h[6], h_ = h[7];

        for (int j = 0; j < 64; ++j) {
            uint32_t temp1 = h_ + sig1(e) + choose(e, f, g) + k[j] + w[j];
            uint32_t temp2 = sig0(a) + majority(a, b, c);
            h_ = g; g = f; f = e;
            e = d + temp1;
            d = c; c = b; b = a;
            a = temp1 + temp2;
        }

        h[0] += a; h[1] += b; h[2] += c; h[3] += d;
        h[4] += e; h[5] += f; h[6] += g; h[7] += h_;
    }

    stringstream ss;
    for (int i = 0; i < 8; ++i)
        ss << hex << setw(8) << setfill('0') << h[i];

    return ss.str();
}
