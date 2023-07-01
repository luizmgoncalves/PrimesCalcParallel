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
#define PORT 8080

using namespace std;

#define PROTOCOL_FLAG ~(uint64_t)0

uint64_t *PRIMES;

uint64_t curr_i;

void calc(uint64_t init, uint64_t end, std::list<uint64_t> **result);

int main(int argc, char const *argv[])
{
    int status, valread, client;
    struct sockaddr_in serv_addr;

    if ((client = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary
    // form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
    {
        printf(
            "\nInvalid address/ Address not supported \n");
        return -1;
    }

    while ((status = connect(client, (struct sockaddr *)&serv_addr,
                             sizeof(serv_addr))) < 0)
    {
        printf("\nConnection Failed \n");
        sleep(1);
    }
    printf("Client: Connected\n");

    // Primes calculating

    uint64_t limit;

    read(client, &limit, sizeof(limit));

    cout << "Setted limit to " << limit << endl;

    PRIMES = new uint64_t[int((float)limit / (float)(log(limit) - 1.3))];
    PRIMES[0] = 2;
    curr_i = 1;

    uint64_t *buffer;

    while (true)
    {
        buffer = new uint64_t[2];
        read(client, buffer, 2 * sizeof(uint64_t));

        cout << "Received: " << buffer[0] << " " << buffer[1] << endl;

        sleep(1);

        if (buffer[0] == 0)
        {
            uint64_t amount_workers = buffer[1];

            delete buffer;

            buffer = new uint64_t[amount_workers * 2];
            read(client, buffer, sizeof(uint64_t) * 2 * amount_workers);

            thread workers[amount_workers];

            list<uint64_t> *workers_result[amount_workers];

            for (uint64_t i = 0; i < amount_workers; i++)
            {
                cout << "New worker: " << buffer[2 * i] << " -> " << buffer[2 * i + 1] << endl;
                workers[i] = std::thread(calc, buffer[2 * i], buffer[2 * i + 1], &workers_result[i]);
            }

            uint64_t response_buffer_size=0;

            for (int i = 0; i < amount_workers; i++)
            {
                workers[i].join();
                response_buffer_size += workers_result[i]->size();
            }

            delete buffer;
            buffer = new uint64_t[2]{0, response_buffer_size};
            send(client, buffer, 2 * sizeof(int64_t), 0);

            delete buffer;
            buffer = new uint64_t[response_buffer_size];
            uint64_t buffer_i=0;

            for (int i = 0; i < amount_workers; i++)
            {
                list<uint64_t> *primes_computed = workers_result[i];

                for (auto it = primes_computed->begin(); it != primes_computed->end(); it++)
                {
                     cout << "Achei o primo: "<< *it << endl;
                    buffer[buffer_i++] = *it;
                }
            }

            send(client, buffer, response_buffer_size*sizeof(int64_t), 0);
        }
        if (buffer[0] == 1){
            
        }
    }
    close(client);
    return 0;
}

void calc(uint64_t init, uint64_t end, std::list<uint64_t> **result)
{
    *result = new std::list<uint64_t>;

    if (init % 2 == 0)
        init++; // jump if it's odd

    bool is_prime = true;

    for (uint64_t cur_n = init; cur_n <= end; cur_n += 2)
    {
        is_prime = true;

        for (uint64_t i = 0; i < curr_i && PRIMES[i] <= sqrt(cur_n); i++)
        {
            if (cur_n % PRIMES[i] == 0)
            {
                is_prime = false;
                break;
            }
        }

        if (is_prime)
        {
            (*result)->push_back(cur_n);
        }
    }
}