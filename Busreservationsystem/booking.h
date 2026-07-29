#ifndef MODULE_BOOKING_H
#define MODULE_BOOKING_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- Constants ---
#define BOOKING_FILENAME "booking.csv"
#define CANCEL_FILENAME  "cancel_booking.csv"
#define MAX_LINE         256
#define MAX_LINE_1       256
#define MAX_SEAT_LEN     10
#define KEY_UP          72
#define KEY_DOWN        80
#define KEY_ENTER       13
#define KEY_ESC         27

// --- Booking Structure ---
typedef struct {
    char name[50];
    int age;
    char gender[10];
    int seat_number;
    char phone_number[15];
    char bus_name[50];
    int bus_number;
    char bus_type[50];
    char arrival_time[10];
    char destination_time[10];
    char paymentmode[30];
    char password[30];

} Booking;

void generate_booking_password(char *password, int length);
void get_current_datetime(char *buffer, size_t size);


// --- Function Prototypes ---
void generate_booking_password(char *password, int length);
// Ensure CSV files exist and contain headers.
void display_csv_table();

// Display all bookings in formatted table.
void view_bookings();

// Cancel booking by seat number.
void cancel_booking();

// Write booking record to CSV.
void book_ticket(const char *name, int age, const char *gender,
            int seat_num, const char *phone_number,
            const char *bus_name, int bus_number,const char *bus_type,
            const char *arrival_time, const char *destination_time,
            const char *paymentmode,const char *password);

// Display all cancelled bookings.
void view_cancellations();

// Add booking by taking user input.
void add_booking();

#endif // BOOKING_MODULE_H
