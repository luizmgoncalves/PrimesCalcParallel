#include <iostream>
#include <cmath>
#include <ctime>
#include <list>
#include <thread>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8080

using namespace std;

uint *PRIMES; // store the primes calculated

uint curr_i;

union Sample {
    uint value;
    char bytes[sizeof(uint)];
};

void calc(unsigned int init, unsigned int end, std::list<unsigned int> **result);

#define STEP 100000

int main(int argc, char *argv[])
{

    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    union Sample numbers[1] = {0};

    char *hello = "Hello from server";

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
    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                             (socklen_t *)&addrlen)) < 0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    valread = read(new_socket, numbers, 1024);
    
    printf("%d\n", numbers[0].value);

    send(new_socket, hello, strlen(hello), 0);
    printf("Hello message sent\n");

    // closing the connected socket
    close(new_socket);
    // closing the listening socket
    shutdown(server_fd, SHUT_RDWR);
    return 0;

    /*
    uint prox;
    clock_t time_i = clock();
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    if (argc != 2)
        return 0;

    uint limit = atoi(argv[1]), last_prime;

    PRIMES = new uint[int((float)limit/(float)(log(limit)-1.3))];

    PRIMES[0] = 2;

    while (true)
    {
        cout << "init cicle\n";
        last_prime = PRIMES[curr_i-1];
        prox = last_prime * last_prime;
        if (prox > limit)
            prox = limit;

        uint dif = prox - last_prime;

        uint amount_workers = dif / STEP + (dif % STEP ? 1 : 0);

        thread workers[amount_workers];

        list<uint> * workers_result[amount_workers];

        uint acc1=last_prime + 1, acc2 = acc1 + STEP;

        if(acc2 > prox) acc2 = prox;

        for(int i=0; i< amount_workers; i++){
            //cout << "New worker: " << i+1 << "/" << amount_workers << "\n";
            workers[i] = std::thread(calc, acc1, acc2, &workers_result[i]);

            acc1=acc2 + 1;
            acc2 = acc1 + STEP;
            if(acc2 > prox) acc2 = prox;
        }

        for(int i=0; i< amount_workers; i++){
            //cout << i+1 << "/" << amount_workers << endl;
            workers[i].join();
        }

        for(int i=0; i< amount_workers; i++){
            list<uint> *primes_computed = workers_result[i];

            for (auto it = primes_computed->begin(); it != primes_computed->end(); it++)
            {
                //cout << *it << endl;
                PRIMES[curr_i++] = *it;
            }
        }

        if (prox == limit)
            break;
    }

    clock_gettime(CLOCK_MONOTONIC, &end);

    double time_taken;
    time_taken = (end.tv_sec - start.tv_sec) * 1e9;
    time_taken = (time_taken + (end.tv_nsec - start.tv_nsec)) * 1e-9;

    cout << PRIMES[curr_i-1] << " "<< curr_i << endl;

    cout << "\n----- " << (float)(clock() - time_i)/(float)CLOCKS_PER_SEC << " ticks and " << time_taken << "  -----\n\n";

    return 0;
    */
}


void calc(unsigned int init, unsigned int end, std::list<unsigned int> **result)
{
    *result = new std::list<unsigned int>;

    if (init % 2 == 0)
        init++; // jump if it's odd

    bool is_prime = true;

    for (unsigned int cur_n = init; cur_n <= end; cur_n += 2)
    {
        is_prime = true;

        for (uint i = 0; i < curr_i && PRIMES[i] <= sqrt(cur_n); i++)
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