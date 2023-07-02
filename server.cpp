#include <iostream>
#include <cmath>
#include <ctime>
#include <list>
#include <vector>
#include <thread>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fstream>

#define PORT 8080
#define PACKAGE_SIZE 1000

using namespace std;

uint64_t *PRIMES; // store the primes calculated

uint64_t curr_i;

#define STEP 100000

#define PROTOCOL_FLAG ~(uint64_t)0

#define CLIENTS_AMOUNT 3

void send_by_packages(int socket, uint64_t *buffer, uint64_t package_len, uint64_t total_len);

void recv_by_packages(int socket, uint64_t *buffer, uint64_t package_len, uint64_t total_len);

int main(int argc, char *argv[])
{

    int server_fd, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET,
                   SO_REUSEADDR | SO_REUSEPORT, &opt,
                   sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, CLIENTS_AMOUNT) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    vector<int> clients;

    int wainting_clients = 1;

    while (wainting_clients)
    {
        int new_socket;
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                                 (socklen_t *)&addrlen)) < 0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        else
        {
            clients.push_back(new_socket);
            cout << "Server: Connected\n";
            if (clients.size() == CLIENTS_AMOUNT)
            {
                cout << "Server: All connected\n";
                wainting_clients = 0;
            }
        }
    }

    // Primes Generator...

    uint64_t prox;
    clock_t time_i = clock();
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    if (argc != 2)
        return 0;

    uint64_t limit = atoi(argv[1]), last_prime;

    for (uint64_t i = 0; i < clients.size(); i++)
    {
        cout << "sending " << limit << " to " << i << "\n";
        send(clients[i], &limit, sizeof(limit), 0); // send to clients the limit
    }

    cout << "done..." << endl;

    PRIMES = new uint64_t[int((float)limit / (float)(log(limit) - 1.3))]{2, 3, 5, 7, 11};

    curr_i = 5;

    uint64_t *buffer;

    while (true)
    {
        last_prime = PRIMES[curr_i - 1];
        prox = last_prime * last_prime;
        if (prox > limit)
            prox = limit;

        uint64_t dif = prox - last_prime;

        uint64_t amount_workers = dif / STEP + (dif % STEP ? 1 : 0);

        uint64_t amount_workers_per_client[CLIENTS_AMOUNT];
        amount_workers_per_client[0] = amount_workers / CLIENTS_AMOUNT + amount_workers % CLIENTS_AMOUNT;

        for (uint64_t i = 1; i < CLIENTS_AMOUNT; i++)
        {
            amount_workers_per_client[i] = amount_workers / CLIENTS_AMOUNT;
        }

        cout << "Calculating primes between " << last_prime + 1 << " and " << prox << ", needed " << amount_workers << " workers" << endl;

        uint64_t acc1 = last_prime + 1, acc2 = acc1 + STEP - 1;

        if (acc2 > prox)
            acc2 = prox;

        for (uint i = 0; i < clients.size(); i++)
        {
            cout << "Sending " << amount_workers_per_client[i] << " workers to " << i << "th client" << endl;

            buffer = new uint64_t[2]{0, amount_workers_per_client[i]};
            send(clients[i], buffer, 2 * sizeof(uint64_t), 0);
            delete buffer;

            uint64_t buffer_size = amount_workers_per_client[i] * 2;
            buffer = new uint64_t[buffer_size];
            for (uint64_t j = 0; j < amount_workers_per_client[i]; j++)
            {
                // cout << "acc1: "<< acc1 <<" acc2: " << acc2 << endl;
                buffer[j * 2] = acc1;
                buffer[j * 2 + 1] = acc2;

                acc1 = acc2 + 1;
                acc2 = acc1 + STEP - 1;
                if (acc2 > prox)
                    acc2 = prox;
            }

            send_by_packages(clients[i], buffer, PACKAGE_SIZE, buffer_size);
            delete buffer;
        }

        uint64_t last_curr_i = curr_i;

        for (uint i = 0; i < clients.size(); i++)
        {
            buffer = new uint64_t[2];

            read(clients[i], buffer, 2 * sizeof(uint64_t));

            uint64_t amount_primes = buffer[1];
            delete buffer;
            buffer = new uint64_t[amount_primes];

            cout << i << "th client founded " << amount_primes << " primes" << endl;

            recv_by_packages(clients[i], buffer, PACKAGE_SIZE, amount_primes);

            for (uint64_t j = 0; j < amount_primes; j++)
            {
                if (!buffer[j])
                {
                    cout << "zero in " << j << " position or " << j * sizeof(uint64_t) << " byte" << endl;
                    break;
                }
                PRIMES[curr_i++] = buffer[j];
            }
            delete buffer;
        }

        for (uint i = 0; i < clients.size(); i++)
        {
            uint64_t amount_primes = curr_i - last_curr_i;

            buffer = new uint64_t[2]{1, amount_primes};
            send(clients[i], buffer, 2 * sizeof(uint64_t), 0);
            delete buffer;

            cout << "Sending " << amount_primes << " new primes to" << i << " th client" << endl;
            send_by_packages(clients[i], &PRIMES[last_curr_i], PACKAGE_SIZE, amount_primes);
        }

        if (prox == limit)
            break;
    }

    for (uint i = 0; i < clients.size(); i++)
    {
        buffer = new uint64_t[2]{3};
        send(clients[i], buffer, 2 * sizeof(uint64_t), 0);
    }

    for (int client : clients)
    {
        close(client);
    }

    clock_gettime(CLOCK_MONOTONIC, &end);

    double time_taken;
    time_taken = (end.tv_sec - start.tv_sec) * 1e9;
    time_taken = (time_taken + (end.tv_nsec - start.tv_nsec)) * 1e-9;

    cout << PRIMES[curr_i - 1] << " " << curr_i << endl;

    cout << "\n----- " << (float)(clock() - time_i) / (float)CLOCKS_PER_SEC << " ticks and " << time_taken << "  -----\n\n";

    cout << "Writing primes on primes.txt file" << endl;

    ofstream PrimesFile("primes.txt");

    for (uint64_t i = 0; i < curr_i; i++)
    {
        cout << PRIMES[i] << endl;
        PrimesFile << PRIMES[i] << endl;
    }
    
    PrimesFile.close();

    shutdown(server_fd, SHUT_RDWR);

    return 0;
}

void send_by_packages(int socket, uint64_t *buffer, uint64_t package_len, uint64_t total_len)
{
    uint64_t acc = 0;
    uint64_t cur_l;
    char ok;
    for (uint64_t i = 0; acc < total_len; i++)
    {
        cur_l = (total_len - acc) >= package_len ? package_len * sizeof(uint64_t) : (total_len - acc) * sizeof(uint64_t);
        write(socket, &buffer[i * package_len], cur_l);

        read(socket, &ok, 1);
        acc += package_len;
    }
}

void recv_by_packages(int socket, uint64_t *buffer, uint64_t package_len, uint64_t total_len)
{
    uint64_t acc = 0;
    uint64_t cur_l;
    char ok;
    for (uint64_t i = 0; acc < total_len; i++)
    {
        cur_l = (total_len - acc) >= package_len ? package_len * sizeof(uint64_t) : (total_len - acc) * sizeof(uint64_t);
        read(socket, &buffer[i * package_len], cur_l);
        write(socket, "k", 1);
        acc += package_len;
    }
}
