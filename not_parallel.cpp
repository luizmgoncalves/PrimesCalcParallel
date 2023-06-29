#include <iostream>
#include <cmath>
#include <ctime>
#include <vector>

using namespace std;

uint * PRIMES; // store the primes calculated

unsigned int curr_pos = 1;

int main(int argc, char *argv[])
{

    uint prox;

    if (argc != 2)
        return 0;

    unsigned int limit = atoi(argv[1]);

    cout << int((float)limit/(float)(log(limit)-1.3)) << endl;

    PRIMES = new uint[int((float)limit/(float)(log(limit)-1.3))];

    PRIMES[0] = 2;

    while (true)
    {
        prox = PRIMES[curr_pos-1] * PRIMES[curr_pos-1];
        if (prox > limit)
            prox = limit;

        bool is_prime = true;

        uint init = PRIMES[curr_pos-1]+1;
        uint end = curr_pos;

        if(init % 2 == 0) init++;

        for (unsigned int cur_n = init; cur_n <= prox; cur_n += 2)
        {
            is_prime = true;

            for (uint i=0; i < end && PRIMES[i] <= sqrt(cur_n); i++)
            {
                if (cur_n % PRIMES[i] == 0)
                {
                    is_prime = false;
                    break;
                }
            }

            if (is_prime)
            {
                PRIMES[curr_pos++] = cur_n;
            }
        }

        if (prox == limit)
            break;
    }

    cout << PRIMES[curr_pos-1] << " "<<curr_pos<<endl;

    cout << "\n-----   -----\n\n";

    return 0;
}