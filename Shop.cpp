#include "Shop.h"

// Constructor
Shop::Shop(int num_barbers, int num_chairs) : max_waiting_cust_(num_chairs), cust_drops_(0) {
    pthread_mutex_init(&lock_, NULL);
    pthread_cond_init(&cond_customers_waiting_, NULL);

    for (int i = 0; i < num_barbers; i++) {
        barbers_.emplace_back(i);
    }
}

// Default Constructor
Shop::Shop() : Shop(kDefaultBarbers, kDefaultNumChairs) {}

// Int 2 String converter function
string Shop::int2string(int i) {
    stringstream out;
    out << i;
    return out.str();
}

// Print function
void Shop::print(int person, string message) {
    cout << ((person > 0) ? "customer[" : "barber[") << abs(person) << "]: " << message << endl;
}

// Get unserved customer function
int Shop::get_cust_drops() const {
    return cust_drops_;
}

// Visit Shop function to enter customer into waiting or service chair or refuse service
int Shop::visitShop(int customer_id) {
    pthread_mutex_lock(&lock_); // Enter Critical Section

    // Check if all waiting chairs are full if they are available
    if (waiting_chairs_.size() > 0) {
        if ((waiting_chairs_.size() == max_waiting_cust_)) {
            print(customer_id, "leaves the shop because no chairs are available.");
            ++cust_drops_;
            pthread_mutex_unlock(&lock_); // Exit Critical Section
            return -1;
        }
    }

    // Check if a barber is free, then serve customer immediately
    if (waiting_chairs_.empty()) {
        for (auto &barber : barbers_) {
            if (!barber.in_service_ && barber.customer_in_chair_ == 0) {
                print(customer_id, "moves to the service chair[" + int2string(barber.barber_id_) + "]. # waiting seats available = " + int2string(max_waiting_cust_ - waiting_chairs_.size()));
                barber.customer_in_chair_ = customer_id;
                barber.in_service_ = true;
                pthread_cond_signal(&barber.cond_barber_sleeping_);
                pthread_mutex_unlock(&lock_); // Exit Critical Section
                return barber.barber_id_;
            }
        }
    }

    // If a barber was not free for immediate service and waiting chairs size is 0, customer leaves
    if ((waiting_chairs_.size() == max_waiting_cust_)) {
        print(customer_id, "leaves the shop because no chairs are available.");
        ++cust_drops_;
        pthread_mutex_unlock(&lock_); // Exit Critical Section
        return -1;
    }

    // If no barber is available, customer waits until a barber becomes available
    waiting_chairs_.push(customer_id);
    print(customer_id, "takes a waiting chair. # waiting seats available = " + int2string(max_waiting_cust_ - waiting_chairs_.size()));
    while (true) {
        pthread_cond_wait(&cond_customers_waiting_, &lock_); // Wait Condition
        // Find available barber
        for (auto &barber : barbers_) {
            if (!barber.in_service_ && !waiting_chairs_.empty() && barber.customer_in_chair_ == 0) {
                int currCustomer = waiting_chairs_.front();
                if (currCustomer != customer_id) {
                    break;
                }
                waiting_chairs_.pop(); // Remove customer from waiting queue
                // Assign customer to barber
                print(currCustomer, "moves to the service chair[" + int2string(barber.barber_id_) + "]. # waiting seats available = " + int2string(max_waiting_cust_ - waiting_chairs_.size()));

                barber.customer_in_chair_ = currCustomer;
                barber.in_service_ = true;
                pthread_cond_signal(&barber.cond_barber_sleeping_); // Signal to barber in case sleeping
                pthread_mutex_unlock(&lock_); // Exit Critical Section
                return barber.barber_id_;
            }
        }
    }
    pthread_mutex_unlock(&lock_); // Exit Critical Section
    return -1;
}

// Leave Shop function to leave the shop following a haircut
void Shop::leaveShop(int customer_id, int barber_id) {
    pthread_mutex_lock(&lock_); // Enter Critical Section
    
    //Get barber cutting hair
    Barber &barber = barbers_[barber_id];
    // Wait for haircut to finish
    print(customer_id, "wait for barber[" + int2string(barber_id) + "] to be done with the hair-cut");
    while (barber.in_service_) {
        pthread_cond_wait(&barber.cond_customer_served_, &lock_);
    }

    // Pay the barber
    barber.money_paid_ = true;
    pthread_cond_signal(&barber.cond_barber_paid_); // Signal barber was paid
    print(customer_id, "says good-bye to barber[" + int2string(barber_id) + "]");
    pthread_mutex_unlock(&lock_); // Exit Critical Section
}

// Hello Customer function: barber will sleep if there are no customers, otherwise begins haircut
void Shop::helloCustomer(int barber_id) {
    pthread_mutex_lock(&lock_); // Enter Critical Section
    Barber &barber = barbers_[barber_id];

    // Barber sleeps if no customers and waiting chairs are empty
    if (waiting_chairs_.empty() && barber.customer_in_chair_ == 0) {
        print(-barber.barber_id_, "sleeps because of no customers.");
        pthread_cond_wait(&barber.cond_barber_sleeping_, &lock_); // Wait until a customer wakes me up.
    }
    
    // check if the customer, sit down.
    if (barber.customer_in_chair_ == 0) {
        pthread_cond_wait(&barber.cond_barber_sleeping_, &lock_); // Wait until a customer wakes me up.
    }
    print(-barber.barber_id_, "starts a hair-cut service for customer[" + int2string(barber.customer_in_chair_) + "]");
    pthread_mutex_unlock(&lock_); // Exit Critical Section
}

// Bye Customer function: completes the haircut for a customer and notifies next customer in chair
void Shop::byeCustomer(int barber_id) {
    pthread_mutex_lock(&lock_); // Enter Critical Section
    Barber &barber = barbers_[barber_id];

    // Notify that haircut is complete
    print(-barber.barber_id_, "says he's done with a hair-cut service for customer[" + int2string(barber.customer_in_chair_) + "]");
    barber.money_paid_ = false;
    barber.in_service_ = false;
    pthread_cond_broadcast(&barber.cond_customer_served_); // Notify that the haircut is complete

    // Wait for payment
    while (!barber.money_paid_) {
        pthread_cond_wait(&barber.cond_barber_paid_, &lock_); // Wait for payment
    }

    // Reset barber for the next customer
    barber.customer_in_chair_ = 0;

    // Notify waiting customers
    print(barber.barber_id_, "calls in another customer");
    pthread_cond_broadcast(&cond_customers_waiting_); // Notify all waiting customers
    pthread_mutex_unlock(&lock_); // Exit Critical Section
}