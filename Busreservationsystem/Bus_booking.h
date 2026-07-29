#ifndef BUS_BOOKING_H
#define BUS_BOOKING_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>
#include "booking.h"

#define MAX_BUSES 100
#define MAX_SEATS 60
#define MAX_PASSENGERS 200

struct Bus {
    char busNo[20];
    char source[50];
    char destination[50];
    char departure[20];
    char arrival[20];
    int available_seats;
    float price;
    char bus_name[50];
    char bus_type[50];
    int seat_map[MAX_SEATS];
};

struct Passenger {
    char name[50];
    int age;
    char busNo[20];
    int seat_no;
    char journey_date[15];
    int window_pref;
    int seater_pref;
    char gender[50];
    char phone[15];
    char booking_password[10];
    int payment_success;
};

/************* Utility *************/
void toLower(char *str);
void trim(char *str);
void get_current_datetime(char *buffer, size_t buffer_size);
int process_payment();

/************* Core Functions *************/
int assignSeat(struct Bus *bus, int window_pref, int seater_pref);
void print_receipt(struct Passenger p, struct Bus bus, float fare, const char* paymentType);

/************** Main booking operation **************/
int busbookingmenu();

#endif
