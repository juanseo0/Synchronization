#include <iostream>
#include <sys/time.h>
#include <unistd.h>
#include "Shop.h"
using namespace std;

void *barber(void *);
void *customer(void *);

// ThreadParam class
// This class is used as a way to pass more
// than one argument to a thread. 
class ThreadParam
{
public:
    ThreadParam(Shop* shop, int id, int service_time) :
        shop(shop), id(id), service_time(service_time) {};
    Shop *shop;
    int id;
    int service_time;
};

int main(int argc, char *argv[])
{
   // Read arguments from command line
   if (argc != 5)
   {
       cout << "Usage: num_barbers num_chairs num_customers service_time" << endl;
      return -1;
   }

   int num_barbers = atoi(argv[1]);
   int num_chairs = atoi(argv[2]);
   int num_customers = atoi(argv[3]);
   int service_time = atoi(argv[4]);

   // Validate values
   if (!(num_barbers > 0 || num_chairs >= 0 || num_customers > 0 || service_time > 0))
   {
      cout << "Values must be greater than 0" << endl;
      return -1;
   }

   //Multiple barbers, one shop, many customers
   pthread_t barber_threads[num_barbers];
   pthread_t customer_threads[num_customers];
   Shop shop(num_barbers, num_chairs);

   // Create barber threads
   for (int i = 0; i < num_barbers; i++)
   {
      ThreadParam *param = new ThreadParam(&shop, i, service_time);
      pthread_create(&barber_threads[i], NULL, barber, param);
   }

   // Create customer threads
   for (int i = 0; i < num_customers; i++)
   {
      usleep(rand() % 1000);
      ThreadParam *param = new ThreadParam(&shop, i + 1, 0);
      pthread_create(&customer_threads[i], NULL, customer, param);
   }

   // Wait for customers to finish
   for (int i = 0; i < num_customers; i++)
   {
      pthread_join(customer_threads[i], NULL);
   }

   // Cancel barber threads
   for (int i = 0; i < num_barbers; i++)
   {
      pthread_cancel(barber_threads[i]);
   }

   cout << "# customers who didn't receive a service = " << shop.get_cust_drops() << endl;
   return 0;
}

void *barber(void *arg)
{
   ThreadParam *param = (ThreadParam *)arg;
   Shop &shop = *param->shop;
   int barber_id = param->id;
   int service_time = param->service_time;
   delete param;

   while (true)
   {
      shop.helloCustomer(barber_id);
      usleep(service_time);
      shop.byeCustomer(barber_id);
   }
   return nullptr;
}

void *customer(void *arg)
{
   ThreadParam *param = (ThreadParam *)arg;
   Shop &shop = *param->shop;
   int id = param->id;
   
   delete param;
   int barber_id = shop.visitShop(id);

   if (barber_id != -1)
   {
      shop.leaveShop(id, barber_id);
   }
   return nullptr;
}