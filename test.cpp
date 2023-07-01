// Client side C/C++ program to demonstrate Socket
// programming
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cmath>
#include <thread>
#include <list>
#include <iostream>
#include <sstream>
#define PORT 8080

using namespace std;

union Sample
{
    uint value;
    char bytes[sizeof(uint)];
};

uint * PRIMES;

int main(int argc, char const *argv[])
{
    cout << ~ (uint64_t) 0 << endl;

    return 0;
}