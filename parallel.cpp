#include <iostream>
#include <cmath>
#include <ctime>
#include <list>
#include <thread>

using namespace std;

uint64_t * PRIMES; // store the primes calculated

uint64_t curr_i = 1;

void calc(uint64_t init, uint64_t end, std::list<uint64_t> * * result)
{
    *result = new std::list<uint64_t>;

    if (init % 2 == 0)
        init++; // jump if it's odd

    bool is_prime = true;

    for (uint64_t cur_n = init; cur_n <= end; cur_n += 2)
    {
        is_prime = true;

        for (uint64_t i=0; i<curr_i && PRIMES[i] <= sqrt(cur_n); i++)
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

#define STEP 100000

int main(int argc, char *argv[])
{

    uint64_t prox;
    clock_t time_i = clock();
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    cout << "Digite até qual número devo calcular primos: ";

    uint64_t limit, last_prime;

    cin >> limit;

    PRIMES = new uint64_t[int((float)limit/(float)(log(limit)-1.3))];

    PRIMES[0] = 2;

    while (true)
    {
        cout << "init cicle\n";
        last_prime = PRIMES[curr_i-1];
        prox = last_prime * last_prime;
        if (prox > limit)
            prox = limit;

        uint64_t dif = prox - last_prime;

        uint64_t amount_workers = dif / STEP + (dif % STEP ? 1 : 0);

        thread workers[amount_workers];

        list<uint64_t> * workers_result[amount_workers];

        uint64_t acc1=last_prime + 1, acc2 = acc1 + STEP;

        if(acc2 > prox) acc2 = prox;

        for(uint64_t i=0; i< amount_workers; i++){
            //cout << "New worker: " << i+1 << "/" << amount_workers << "\n";
            workers[i] = std::thread(calc, acc1, acc2, &workers_result[i]);

            acc1=acc2 + 1;
            acc2 = acc1 + STEP;
            if(acc2 > prox) acc2 = prox;
        }

        for(uint64_t i=0; i< amount_workers; i++){
            //cout << i+1 << "/" << amount_workers << endl;
            workers[i].join();
        }

        for(uint64_t i=0; i< amount_workers; i++){
            list<uint64_t> *primes_computed = workers_result[i];

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
}
