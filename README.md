# Synchronizaton The Sleeping Barbers Problem
This program is the solution to the classic Sleeping Barber problem extended to included multiple sleeping barbers. 

A barbershop consists of a waiting room with n waiting chairs and a barber room with one barber chair.
If there are no customers to be served, the barber goes to sleep. If a customer enters the barbershop
and all the waiting chairs are occupied, then the customer leaves the shop. If the barber is busy but
waiting chairs are available, then the customer sits in one of the free chairs. If the barber is asleep, the
customer wakes up the barber and gets served.

A barbershop consists of a waiting room with n waiting chairs and a barber room with m barber chairs. If
there are no customers to be served, all the barbers go to sleep. If a customer enters the barbershop
and all chairs (including both, waiting and barber chairs) are occupied, then the customer leaves the
shop.
If all the barbers are busy but chairs are available, then the customer sits in one of the
free waiting chairs. If the barbers are asleep, the customer wakes up one of the barbers.

### Synchronization Mechanisms
Mutex (`pthread_mutex_lock` / `pthread_mutex_unlock`)
- Ensures only one thread accesses critical sections at a time.

Condition Variables (`pthread_cond_wait` / `pthread_cond_signal`)
- Used to signal barbers and customers when an event occurs.
- Barbers sleep when no customers are available and wake up when one arrives.
- Customers wait for an available barber if all are busy.

### cpp files

- `shop.h/cpp`: Sleeping Barbers Problem header and implementation
- `driver.cpp` Driver file

### Testing
- Compile with “g++ driver.cpp Shop.cpp –o sleepingBarbers –lpthread”
./sleepingBarbers 1 1 10 1000
./sleepingBarbers 3 1 10 1000




