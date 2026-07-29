#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "buses.h"

Bus buses[MAX_BUSES];
Schedule schedules[MAX_SCHEDULES];

int bus_count = 0;
int schedule_count = 0;

/* ================= BASIC UTILS ================= */

void clear_console() {
    system("cls");
}

void press_any_key() {
    printf("\nPress Enter...");
    getchar();
}

void remove_newline(char *s) {
    int n = strlen(s);
    if (n > 0 && s[n - 1] == '\n')
        s[n - 1] = '\0';
}
  
/* ================= LOAD & SAVE BUSES ================= */

void load_buses() {
    bus_count = 0;

    FILE *f = fopen("Buses.csv", "r");
    if (!f) return;

    char line[200];
    fgets(line, sizeof(line), f);

    while (fscanf(f, "%49[^,],%d,%49[^,],%d,%f\n",
                  buses[bus_count].name,
                  &buses[bus_count].bus_no,
                  buses[bus_count].type,
                  &buses[bus_count].max_seats,
                  &buses[bus_count].base_price) == 5) {

        bus_count++;
        if (bus_count >= MAX_BUSES) break;
    }

    fclose(f);
}

void save_buses() {
    FILE *f = fopen("Buses.csv", "w");
    if (!f) return;

    fprintf(f, "BusName,BusNo,BusType,MaxSeats,BasePrice\n");

    for (int i = 0; i < bus_count; i++) {
        fprintf(f, "%s,%d,%s,%d,%.2f\n",
                buses[i].name,
                buses[i].bus_no,
                buses[i].type,
                buses[i].max_seats,
                buses[i].base_price);
    }

    fclose(f);
}

/* ============== LOAD & SAVE DAILY SCHEDULE ============== */
/* HEADER FORMAT:
BusNo,Source,Destination,Departure,Arrival,AvailableSeats,Price,BusName,BusType
*/

void load_daily_schedules() {
    schedule_count = 0;

    FILE *f = fopen("Buses_schedule.csv", "r");
    if (!f) return;

    char line[256];
    fgets(line, sizeof(line), f); // skip header

    while (fscanf(f, "%d,%49[^,],%49[^,],%49[^,],%49[^,],%d,%f,%49[^,],%49[^\n]\n",
                  &schedules[schedule_count].busNo,
                  schedules[schedule_count].source,
                  schedules[schedule_count].destination,
                  schedules[schedule_count].departure,
                  schedules[schedule_count].arrival,
                  &schedules[schedule_count].availableSeats,
                  &schedules[schedule_count].price,
                  schedules[schedule_count].busName,
                  schedules[schedule_count].busType) == 9) {

        schedule_count++;
        if (schedule_count >= MAX_SCHEDULES) break;
    }

    fclose(f);
}

void save_daily_schedules() {
    FILE *f = fopen("Buses_schedule.csv", "w");
    if (!f) return;

    fprintf(f, "BusNo,Source,Destination,Departure,Arrival,AvailableSeats,Price,BusName,BusType\n");

    for (int i = 0; i < schedule_count; i++) {
        fprintf(f, "%d,%s,%s,%s,%s,%d,%.2f,%s,%s\n",
                schedules[i].busNo,
                schedules[i].source,
                schedules[i].destination,
                schedules[i].departure,
                schedules[i].arrival,
                schedules[i].availableSeats,
                schedules[i].price,
                schedules[i].busName,
                schedules[i].busType);
    }

    fclose(f);
}

/* ================= ADD BUS ================= */

void addBus() {
    clear_console();
    load_buses();

    if (bus_count >= MAX_BUSES) {
        printf("Bus limit reached.\n");
        return;
    }

    Bus b;

    printf("Enter Bus Name: "); fgets(b.name, 50, stdin); remove_newline(b.name);
    printf("Enter Bus No: "); scanf("%d", &b.bus_no); getchar();
    printf("Enter Type: "); fgets(b.type, 50, stdin); remove_newline(b.type);
    printf("Enter Max Seats: "); scanf("%d", &b.max_seats);
    printf("Enter Base Price: "); scanf("%f", &b.base_price);

    buses[bus_count++] = b;
    save_buses();

    printf("Bus added.\n");
    press_any_key();
}

/* ================= VIEW BUSES (TABLE) ================= */

void viewAllBuses() {
    clear_console();
    load_buses();

    if (bus_count == 0) {
        printf("No buses found.\n");
        return;
    }

    printf("\n+----------------------------------------------------------------------------------------+\n");
    printf("| %-3s | %-25s | %-10s | %-18s | %-5s | %-8s |\n",
           "No", "Bus Name", "Bus Number", "Type", "Seats", "Price");
    printf("+----------------------------------------------------------------------------------------+\n");

    for (int i = 0; i < bus_count; i++) {
        printf("| %-3d | %-25s | %-10d | %-18s | %-5d | %-8.2f |\n",
               i + 1,
               buses[i].name,
               buses[i].bus_no,
               buses[i].type,
               buses[i].max_seats,
               buses[i].base_price);
    }

    printf("+-----------------------------------------------------------------------------------------+\n");
    press_any_key();
}

/* ================= EDIT OR DELETE BUS ================= */

void editOrDeleteBus() {
    clear_console();
    load_buses();

    if (bus_count == 0) {
        printf("No buses.\n");
        return;
    }

    viewAllBuses();

    int ch;
    printf("\nEnter bus number to modify: ");
    scanf("%d", &ch);
    getchar();

    if (ch < 1 || ch > bus_count) {
        printf("Invalid.\n");
        return;
    }

    int i = ch - 1;

    printf("\n1. Edit\n2. Delete\nChoose: ");
    int op; scanf("%d", &op); getchar();

    if (op == 1) {
        printf("New Name: "); fgets(buses[i].name, 50, stdin); remove_newline(buses[i].name);
        printf("New Bus No: "); scanf("%d", &buses[i].bus_no); getchar();
        printf("New Type: "); fgets(buses[i].type, 50, stdin); remove_newline(buses[i].type);
        printf("New Seats: "); scanf("%d", &buses[i].max_seats);
        printf("New Price: "); scanf("%f", &buses[i].base_price);

        save_buses();
        printf("Updated.\n");
    }
    else if (op == 2) {
        for (int j = i; j < bus_count - 1; j++)
            buses[j] = buses[j + 1];
        bus_count--;

        save_buses();
        printf("Deleted.\n");
    }
}

/* ================= VIEW SCHEDULES (TABLE) ================= */

void viewAllSchedules() {
    clear_console();
    load_daily_schedules();

    if (schedule_count == 0) {
        printf("No schedules.\n");
        return;
    }

    printf("\n+---------------------------------------------------------------------------------------------------------------------------------------+\n");
    printf("| %-3s | %-5s | %-15s | %-15s | %-10s | %-10s | %-5s | %-8s | %-20s | %-15s |\n",
           "No","BusNo","Source","Dest","Depart","Arrive","Seat","Price","BusName","BusType");
    printf("+---------------------------------------------------------------------------------------------------------------------------------------+\n");

    for (int i = 0; i < schedule_count; i++) {
        printf("| %-3d | %-5d | %-15s | %-15s | %-10s | %-10s | %-5d | %-8.2f | %-20s | %-15s |\n",
               i+1,
               schedules[i].busNo,
               schedules[i].source,
               schedules[i].destination,
               schedules[i].departure,
               schedules[i].arrival,
               schedules[i].availableSeats,
               schedules[i].price,
               schedules[i].busName,
               schedules[i].busType);
    }

    printf("+---------------------------------------------------------------------------------------------------------------------------------------+\n");
    press_any_key();
}

/* ================= ADD SCHEDULE ================= */

void addDailySchedule() {
    clear_console();
    load_buses();
    load_daily_schedules();

    viewAllBuses();

    int ch;
    printf("Choose bus (1-%d): ", bus_count);
    scanf("%d", &ch); getchar();

    if (ch < 1 || ch > bus_count) return;

    Bus b = buses[ch - 1];

    Schedule s;
    s.busNo = b.bus_no;
    strcpy(s.busName, b.name);
    strcpy(s.busType, b.type);

    getchar();
    printf("Source: "); fgets(s.source, 50, stdin); remove_newline(s.source);
    printf("Destination: "); fgets(s.destination, 50, stdin); remove_newline(s.destination);
    printf("Departure: "); fgets(s.departure, 50, stdin); remove_newline(s.departure);
    printf("Arrival: "); fgets(s.arrival, 50, stdin); remove_newline(s.arrival);

    printf("Seats: "); scanf("%d", &s.availableSeats);
    printf("Price: "); scanf("%f", &s.price);

    schedules[schedule_count++] = s;
    save_daily_schedules();

    printf("Schedule added.\n");
    press_any_key();
}

/* ================= UPDATE SCHEDULE ================= */

void updateDailySchedule() {
    clear_console();
    load_daily_schedules();

    viewAllSchedules();

    int n;
    printf("Enter schedule number: ");
    scanf("%d", &n); getchar();

    if (n < 1 || n > schedule_count) return;

    int i = n - 1;

    printf("Source: "); fgets(schedules[i].source, 50, stdin); remove_newline(schedules[i].source);
    printf("Destination: "); fgets(schedules[i].destination, 50, stdin); remove_newline(schedules[i].destination);
    printf("Departure: "); fgets(schedules[i].departure, 50, stdin); remove_newline(schedules[i].departure);
    printf("Arrival: "); fgets(schedules[i].arrival, 50, stdin); remove_newline(schedules[i].arrival);

    printf("Seats: "); scanf("%d", &schedules[i].availableSeats);
    printf("Price: "); scanf("%f", &schedules[i].price);

    save_daily_schedules();
    printf("Updated.\n");
}
