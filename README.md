# Synchronizaton The Sleeping Barbers Problem

In this programming assignment we will extend the sleeping-barber problem to a multiple sleeping
barbers problem where many customers visit a barbershop and receive a haircut service from any one
of the available barbers in the shop.

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

### cpp files

- shop.h/cpp: Sleeping Barbers Problem header and implementation
- driver.cpp Driver file

### Text files for testing
- 1barber_1chair_10customer_1000stime.txt
- 3barber_1chair_10customer_1000stime.txt

### Testing
- Compile with “g++ driver.cpp Shop.cpp –o sleepingBarbers –lpthread”
- Run your program with the following two scenarios:
./sleepingBarbers 1 1 10 1000
./sleepingBarbers 3 1 10 1000




