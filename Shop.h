#ifndef SHOP_H_
#define SHOP_H_

#include <pthread.h>
#include <iostream>
#include <queue>
#include <sstream>
#include <string>
#include <unistd.h>
using namespace std;

#define kDefaultNumChairs 3 // default number of chairs
#define kDefaultBarbers 1   // default number of barbers

// Barber object
class Barber {
public:
   Barber(int id) : barber_id_(id), customer_in_chair_(0), in_service_(false), money_paid_(false) {
    pthread_cond_init(&cond_customer_served_, NULL);
    pthread_cond_init(&cond_barber_sleeping_, NULL);
    pthread_cond_init(&cond_barber_paid_, NULL);
   }

   pthread_cond_t cond_customer_served_;  // Condition for serving customer
   pthread_cond_t cond_barber_sleeping_;  // Condition for barber sleeping
   pthread_cond_t cond_barber_paid_;      // Condition for paying barber

   int barber_id_;              // Int for barber id
   int customer_in_chair_;      // Int for customer id
   bool in_service_;            // Bool for if barber is in service
   bool money_paid_;            // Bool for if barber was paid
};

// Shop
class Shop {
public:
   Shop(int num_barbers, int num_chairs); // Constructors
   Shop();                                // Constructors

   int visitShop(int customer_id);     // Visit Shop for customer_id, returns barber_id or -1
   void leaveShop(int customer_id, int barber_id);    // Leave Shop for customer_id
   void helloCustomer(int barber_id);  // Hello Customer for barber_id to start service
   void byeCustomer(int barber_id);    // Bye Customer func for barber_id to complete service
   int get_cust_drops() const;         // Getter for customers that were not served

private:
   const int max_waiting_cust_; // the max number of threads that can wait
   vector<Barber> barbers_;     // Vector of barber objects
   queue<int> waiting_chairs_;  // Queue for all customers in waiting chairs
   int cust_drops_;             // Number of customers that were not served

   // Mutexes and condition variables to coordinate threads
   // mutex_ is used in conjuction with all conditional variables
   pthread_mutex_t lock_;                  // Mutex lock for critical sections
   pthread_cond_t cond_customers_waiting_; // Condition for customers waiting
   
   string int2string(int i);               // int to string converter
   void print(int person, string message); // print function
};
#endif