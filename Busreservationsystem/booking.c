#include "booking.h"
#include <unistd.h>
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#define MAX_LINE_1 256

static void clear_console() {
    system("cls");
}

void generate_booking_password(char *password, int length) {
    const char charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    for (int i = 0; i < length; i++) {
        password[i] = charset[rand() % (sizeof(charset) - 1)];
    }
    password[length] = '\0';
}


void display_csv_table(const char *filename, const char *title) {
    clear_console();
    printf("--- %s ---\n", title);

    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("\n❌ File '%s' not found.\n", filename);
        return;
    }

    char line[MAX_LINE_1];
    int row_count = 0;

    printf("+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+\n");
    printf("| %-3s | %-15s | %-4s | %-6s | %-4s | %-10s | %-14s | %-6s | %-14s | %-7s | %-14s | %-19s | %-12s | %-10s |\n",
           "No", "NAME", "AGE", "GENDER", "SEAT", "PHONE", "BUS_NAME", "BUS_NO", "BUS_TYPE",
           "ARRIVAL", "DEST", "BOOK_DT", "PAYMENT_MODE", "Price");
    printf("+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+\n");


    fgets(line, MAX_LINE_1, file); // skip header

    while (fgets(line, MAX_LINE_1, file)) {
        row_count++;
        char *token;

        printf("| %-3d ", row_count);

        token = strtok(line, ","); printf("| %-15s ", token ? token : "");
        token = strtok(NULL, ","); printf("| %-4s ", token ? token : "");
        token = strtok(NULL, ","); printf("| %-6s ", token ? token : "");
        token = strtok(NULL, ","); printf("| %-4s ", token ? token : "");
        token = strtok(NULL, ","); printf("| %-10s ", token ? token : "");
        token = strtok(NULL, ","); printf("| %-14s ", token ? token : "");
        token = strtok(NULL, ","); printf("| %-6s ", token ? token : "");
        token = strtok(NULL, ","); printf("| %-14s ", token ? token : "");
        token = strtok(NULL, ","); printf("| %-7s ", token ? token : "");
        token = strtok(NULL, ","); printf("| %-14s ", token ? token : "");
        token = strtok(NULL, ","); printf("| %-12s ", token ? token : "");
        token = strtok(NULL, ","); printf("| %-12s ", token ? token : "");
        token = strtok(NULL, ",");
        token = strtok(NULL, "\n"); printf("| %-10s |\n", token ? token : "");
    }
    /*
    fgets reads one line at a time and stores it in the line variable.
    strtok is a function that reads a line and separates words with "," when we write NULL means it starts from when it ends. If we write "line" a second time, it again start from the beginning.
    strtok doesn't copy and return a word, it only returns the address of that word.
    Token? Token: "" It helps prevent the program from crashing when certain words are missing in a line.
    This approach also helps avoid program crashes. */

    printf("+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+\n");

    if (row_count == 0)
        printf("| No records found.                                                                                                                                         |\n"
            ,"+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+\n");

    fclose(file);
}


void view_bookings() {
    display_csv_table(BOOKING_FILENAME, "🚌 All Current Bookings 🚌");
}

void view_cancellations() {
    display_csv_table(CANCEL_FILENAME, "🚫 All Cancelled Bookings 🚫");
}

void book_ticket(const char *name, int age, const char *gender,
            int seat_num, const char *phone_number,
            const char *bus_name, int bus_number,const char *bus_type,
            const char *arrival_time, const char *destination_time,
            const char *paymentmode,const char *password){

    if (strlen(name) == 0 || age <= 0 || seat_num <= 0 || bus_number <= 0) {
        printf("❌ Invalid input.\n");
        return;
    }

    FILE *file = fopen(BOOKING_FILENAME, "r");
    if (file) {
        char line[256], s_name[50], s_gender[10], s_phone[15],s_password[20],s_payment[20],s_bus_type[20];
        char s_busname[50], s_arr[20], s_dest[20], s_dt[30];
        int s_age,s_seat, s_busnum;
        while (fgets(line, sizeof(line), file)) {
            sscanf(line, "%49[^,],%d,%9[^,],%d,%14[^,],%49[^,],%d,%49[^,],%19[^,],%19[^,],%29[^,],%19[^,],%19[^\n]",
                s_name, &s_age, s_gender, &s_seat,
                s_phone, s_busname, s_busnum, s_bus_type,
                s_arr, s_dest, s_dt,
                s_payment, s_password);

            if (s_seat==seat_num && s_busnum == bus_number) {
                printf("⚠️ Seat %d on Bus %d already booked by %s.\n",
                       seat_num, bus_number, s_name);
                fclose(file);
                return;
            }
        }
        fclose(file);
    }


    char datetime[30];
   time_t now = time(NULL);
struct tm *t = localtime(&now);
strftime(datetime, sizeof(datetime), "%Y-%m-%d %H:%M:%S", t);

    file = fopen(BOOKING_FILENAME, "a");
    if (!file) {
        printf("❌ Cannot open '%s'\n", BOOKING_FILENAME);
        return;
    }

    fprintf(file,"%s,%d,%s,%d,%s,%d,%s,%s,%s,%s,%s,%s,%s\n",
        name,
        age,
        gender,
        seat_num,
        phone_number,
        bus_name,
        bus_number,
        bus_type,
        arrival_time,
        destination_time,
        datetime,
        paymentmode,
        password);

    fclose(file);

    printf("✅ Booking Confirmed for %s (Seat %d, Bus %s-%d)\n",
           name, seat_num, bus_name, bus_number);
}

void cancel_booking() {
    clear_console();
    printf("--- Cancel Booking ---\n");
    
    FILE *file = fopen(BOOKING_FILENAME, "r");
    if (!file) {
        printf("No bookings available.\n");
        getchar();
        return;
    }

    char header[256];
    fgets(header, sizeof(header), file);  // keep header

    char lines[500][256];
    int count = 0;

    while (fgets(lines[count], 256, file)) {
        count++;
    }
    fclose(file);

    if (count == 0) {
        printf("No bookings.\n");
        getchar();
        return;
    }

    view_bookings();
    int choice;
    scanf("%d", &choice);
    getchar();

    if (choice < 1 || choice > count) {
        printf("Invalid.\n");
        getchar();
        return;
    }

    FILE *cancel = fopen(CANCEL_FILENAME, "a");
    fprintf(cancel, "%s", lines[choice - 1]);
    fclose(cancel);

    FILE *temp = fopen("temp.csv", "w");
    fprintf(temp, "%s", header);

    for (int i = 0; i < count; i++)
        if (i != choice - 1)
            fprintf(temp, "%s", lines[i]);

    fclose(temp);

    remove(BOOKING_FILENAME);
    rename("temp.csv", BOOKING_FILENAME);

    printf("Booking cancelled.\n");
    getchar();
}


void add_booking() {
    clear_console();
    printf("=== 🚌 ADD NEW BOOKING ===\n\n");

    char name[50], gender[10], phone[20];
    char bus[50], bus_type[20];
    char arr[20], dest[20];
    int age, seat, bus_no;

    printf("Enter Name: ");fgets(name, sizeof(name), stdin);name[strcspn(name, "\n")] = 0;
    printf("Enter Age: "); scanf("%d", &age);getchar();
    printf("Enter Gender: "); fgets(gender, sizeof(gender), stdin);gender[strcspn(gender, "\n")] = 0;
    printf("Enter Seat No: ");scanf("%d", &seat);getchar();
    printf("Enter Phone: ");fgets(phone, sizeof(phone), stdin);phone[strcspn(phone, "\n")] = 0;
    printf("Enter Bus Name: ");fgets(bus, sizeof(bus), stdin);bus[strcspn(bus, "\n")] = 0;
    printf("Enter Bus No: "); scanf("%d", &bus_no);getchar();
    printf("Enter Bus Type: ");fgets(bus_type, sizeof(bus_type), stdin);bus_type[strcspn(bus_type, "\n")] = 0;
    printf("Enter Arrival: ");fgets(arr, sizeof(arr), stdin);arr[strcspn(arr, "\n")] = 0;
    printf("Enter Destination: ");fgets(dest, sizeof(dest), stdin);dest[strcspn(dest, "\n")] = 0;

    printf("\nConfirm booking? (Y/N): ");
    char conf = toupper(getch());
    if (conf != 'Y') {
        printf("\n❌ Cancelled.\nPress Enter...");
        getchar();
        return;
    }

    char auto_password[20];
    generate_booking_password(auto_password, 6);  

    char payment_mode[20];
    strcpy(payment_mode, "admin");

    book_ticket(
        name, age, gender,
        seat, phone,
        bus, bus_no, bus_type,
        arr, dest,
        payment_mode, auto_password
    );

    printf("\nPress Enter...");
    getchar();
}

