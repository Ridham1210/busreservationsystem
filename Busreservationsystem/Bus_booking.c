#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>
#include <conio.h>
#include "bus_booking.h"
#include "booking.h"

/****************** Utility Functions ******************/

void toLower(char *str) {
    for (int i = 0; str[i]; i++)
        str[i] = tolower(str[i]);
}

void trim(char *str) {
    int len = strlen(str);
    while (len > 0 && (str[len - 1] == '\n' || str[len - 1] == ' '))
        str[--len] = '\0';
}

void print_and_save_ticket(struct Bus bus,
                           struct Passenger passengers[],
                           int startIndex,
                           int numPassengers,
                           const char *date,
                           float totalFare,
                           const char *paymentType)
{
    FILE *fp = fopen("ticket.txt", "w");
    if (!fp) {
        printf("Error creating ticket.txt\n");
        return;
    }

    // Both terminal and file use the same output
    FILE *out[2] = { stdout, fp };

    for (int t = 0; t < 2; t++) {

        fprintf(out[t],
        "============================================================\n"
        "                       BUS TICKET\n"
        "============================================================\n"
        " Ticket ID     : %s\n"
        " Date          : %s\n"
        "------------------------------------------------------------\n"
        " Bus Details\n"
        "------------------------------------------------------------\n"
        " Bus Number    : %s\n"
        " Bus Name      : %s\n"
        " Route         : %s  ->  %s\n"
        " Departure     : %s\n"
        " Arrival       : %s\n"
        "------------------------------------------------------------\n"
        " Fare Details\n"
        "------------------------------------------------------------\n"
        " Ticket Price  : %.2f\n"
        " Total Fare    : %.2f\n"
        " Payment Mode  : %s\n"
        "------------------------------------------------------------\n"
        " Passenger List\n"
        "------------------------------------------------------------\n",
        passengers[startIndex].booking_password,
        date,
        bus.busNo,
        bus.bus_name,
        bus.source,
        bus.destination,
        bus.departure,
        bus.arrival,
        bus.price,
        totalFare,
        paymentType
        );
        for (int i = startIndex; i < startIndex + numPassengers; i++) {
            fprintf(out[t],
            " Name      : %s\n"
            " Age       : %d\n"
            " Gender    : %s\n"
            " Phone     : %s\n"
            " Seat No   : %d\n"
            " Password  : %s\n"
            "------------------------------------------------------------\n",
            passengers[i].name,
            passengers[i].age,
            passengers[i].gender,
            passengers[i].phone,
            passengers[i].seat_no,
            passengers[i].booking_password
            );
        }

        fprintf(out[t],
        "============================================================\n"
        "           THANK YOU — HAVE A SAFE JOURNEY\n"
        "============================================================\n");
    }

    fclose(fp);
    printf("\nTicket saved to ticket.txt\n");
}
int process_payment() {
    return (rand() % 100) < 80;
}

int is_empty(const char *str) {
    while (*str) {
        if (!isspace((unsigned char)*str))
            return 0;
        str++;
    }
    return 1;
}

/****************** Receipt Function ******************/

void print_receipt(struct Passenger p, struct Bus bus, float fare, const char* paymentType) {
    printf("\n========== BOOKING RECEIPT ==========\n");
    printf("Booking ID: %s\n", p.booking_password);
    printf("Passenger: %s\n", p.name);
    printf("Age: %d | Gender: %s\n", p.age, p.gender);
    printf("Phone: %s\n", p.phone);
    printf("Bus: %s (%s)\n", bus.bus_name, bus.busNo);
    printf("Route: %s to %s\n", bus.source, bus.destination);
    printf("Date: %s\n", p.journey_date);
    printf("Seat: %d\n", p.seat_no);
    printf("Departure: %s | Arrival: %s\n", bus.departure, bus.arrival);
    printf("Fare: %.2f\n", fare);
    printf("Payment Mode: %s\n", paymentType);
    printf("Status: CONFIRMED\n");
    printf("=====================================\n");
}

/****************** Seat Assignment Function ******************/

int assignSeat(struct Bus *bus, int window_pref, int seater_pref) {
    for (int i = 0; i < MAX_SEATS; i++) {
        if (bus->seat_map[i] == 0) {
            int col = i % 5;
            int is_window = (col == 0 || col == 4);
            int is_2seater = (col < 2);
            int is_3seater = (col >= 2);

            if ((window_pref && is_window && seater_pref == 2 && is_2seater) ||
                (window_pref && is_window && seater_pref == 3 && is_3seater) ||
                (!window_pref && !is_window && seater_pref == 2 && is_2seater) ||
                (!window_pref && !is_window && seater_pref == 3 && is_3seater)) {

                bus->seat_map[i] = 1;
                bus->available_seats--;
                return i + 1;
            }
        }
    }

    printf("\nPreferred seat not available. Assigning random seat.\n");

    for (int i = 0; i < MAX_SEATS; i++) {
        if (bus->seat_map[i] == 0) {
            bus->seat_map[i] = 1;
            bus->available_seats--;
            return i + 1;
        }
    }

    return -1;
}

/****************** Booking Function ******************/

int bookTicketFunction() {
    system("cls");
    printf("=== BUS TICKET BOOKING ===\n\n");

    struct Bus buses[MAX_BUSES];
    struct Passenger passengers[MAX_PASSENGERS];
    int passenger_count = 0, busCount = 0;
    char validBusNos[MAX_BUSES][20];
    FILE *fp = fopen("Buses_schedule.csv", "r");
    if (!fp) {
        printf("Error: Could not open Buses_schedule.csv\n");
        printf("\nPress any key to continue...");
        getch();
        return 1;
    }

    char line[5120];
    while (fgets(line, sizeof(line), fp)) {
        if (sscanf(line, "%[^,],%[^,],%[^,],%[^,],%[^,],%d,%f,%[^,],%[^\n]",
                       buses[busCount].busNo,
                       buses[busCount].source,
                       buses[busCount].destination,
                       buses[busCount].departure,
                       buses[busCount].arrival,
                       &buses[busCount].available_seats,
                       &buses[busCount].price,
                       buses[busCount].bus_name,
                       buses[busCount].bus_type) == 9) {
            trim(buses[busCount].source);
            trim(buses[busCount].destination);
            busCount++;
        }
    }
    fclose(fp);

    FILE *fp2 = fopen("SeatMap.csv", "r");
    if (fp2) {
        char line2[5120];
        while (fgets(line2, sizeof(line2), fp2)) {
            char busId[20];
            sscanf(line2, "%[^,],", busId);
            for (int i = 0; i < busCount; i++) {
                if (strcmp(busId, buses[i].busNo) == 0) {
                    char *ptr = strchr(line2, ',') + 1;
                    for (int k = 0; k < MAX_SEATS; k++) {
                        sscanf(ptr, "%d", &buses[i].seat_map[k]);
                        ptr = strchr(ptr, ' ');
                        if (!ptr) break;
                        ptr++;
                    }
                }
            }
        }
        fclose(fp2);
    }
    else {
        srand(time(NULL));
        for (int b = 0; b < busCount; b++) {
            for (int i = 0; i < MAX_SEATS; i++)
                buses[b].seat_map[i] = 1;
            int empty = buses[b].available_seats;
            while (empty > 0) {
                int index = rand() % MAX_SEATS;
                if (buses[b].seat_map[index] == 1) {
                    buses[b].seat_map[index] = 0;
                    empty--;
                }
            }
        }
        fp2 = fopen("SeatMap.csv", "w");
        for (int i = 0; i < busCount; i++) {
            fprintf(fp2, "%s,", buses[i].busNo);
            for (int s = 0; s < MAX_SEATS; s++)
                fprintf(fp2, "%d ", buses[i].seat_map[s]);
            fprintf(fp2, "\n");
        }
        fclose(fp2);
    }

    char date[20], src[50], dest[50];
    
    // Date input with validation loop - FIXED VERSION
    int validDate = 0;
    while (!validDate) {
        printf("Enter Date (DD/MM/YYYY): ");
        scanf("%s", date);
        int d,m,y;
        if (sscanf(date,"%d/%d/%d",&d,&m,&y) != 3) {
            printf("Invalid date format! Please use DD/MM/YYYY.\n\n");
            continue;
        }
        time_t now=time(NULL);
        struct tm*current=localtime(&now);
        int currentDay=current->tm_mday;
        int currentMonth=current->tm_mon+1;
        int currentYear=current->tm_year+1900;
        
        // Fix the date validation logic
        if (y < currentYear) {
            printf("Error: You cannot book for a past year!\n\n");
        } else if (y == currentYear && m < currentMonth) {
            printf("Error: You cannot book for a past month!\n\n");
        } else if (y == currentYear && m == currentMonth && d < currentDay) {
            printf("Error: You cannot book for a past date!\n\n");
        } else if (m < 1 || m > 12 || d < 1 || d > 31) {
            printf("Error: Invalid date!\n\n");
        } else {
            validDate = 1;
        }
    }

    // Source and destination with route validation loop
    int validRoute = 0;
    while (!validRoute) {
        printf("Enter Source: ");
        scanf("%s", src);
        printf("Enter Destination: ");
        scanf("%s", dest);
         
        char srcLower[50], destLower[50];
        strcpy(srcLower, src);
        strcpy(destLower, dest);
        toLower(srcLower);
        toLower(destLower);

        printf("\n------------------ MATCHING BUSES ------------------\n");
        printf("BusNo   Source        Destination   Departure   Arrival   Seats   Price\n");
        int matchCount = 0;
        for (int i = 0; i < busCount; i++) {
            char s1[50], s2[50];
            strcpy(s1, buses[i].source);
            strcpy(s2, buses[i].destination);
            toLower(s1);
            toLower(s2);
            if (strcmp(s1, srcLower) == 0 && strcmp(s2, destLower) == 0) {
            int d,m,y;
            sscanf(date,"%d/%d/%d",&d,&m,&y);
            time_t now=time(NULL);
            struct tm*current=localtime(&now);
            int currentDay=current->tm_mday;
            int currentMonth=current->tm_mon+1;
            int currentYear=current->tm_year+1900;
            int currentMinutes=current->tm_hour*60+current->tm_min;
            if(d==currentDay && m==currentMonth && y==currentYear){
                int h,min;
                    sscanf(buses[i].departure,"%d:%d",&h,&min);
                    int depMinutes=h*60+min;
                    if(depMinutes<=currentMinutes){
                        continue;
                    }
            }
           printf("%-6s %-12s %-13s %-10s %-9s %-6d %.2f\n",
           buses[i].busNo,
           buses[i].source,
           buses[i].destination,
           buses[i].departure,
           buses[i].arrival,
           buses[i].available_seats,
           buses[i].price);
           strcpy(validBusNos[matchCount],buses[i].busNo);
                matchCount++;
            }
        }

        if (matchCount == 0) {
            printf("\nNo buses found for this route. Please enter different source and destination.\n\n");
        } else {
            validRoute = 1;
            
            char chosenBus[20];
            int numTickets;
            
            // Bus selection with validation loop
            int validBus = 0;
            while (!validBus) {
                printf("\nEnter Bus No to book: ");
                scanf("%s", chosenBus);
                
                int isValidForRoute = 0;
                for (int i = 0; i < matchCount; i++) {
                    if (strcmp(chosenBus,validBusNos[i])==0) {
                        isValidForRoute = 1;
                        break;
                    }
                }
                if (!isValidForRoute) {
                    printf("\nInvalid Bus Number for this route! Please choose from the list above.\n");
                } else {
                    validBus = 1;
                }
            }

            int busIndex = -1;
            for (int i = 0; i < busCount; i++)
                if (strcmp(buses[i].busNo, chosenBus) == 0)
                    busIndex = i;

            // Ticket quantity with validation loop
            int validTickets = 0;
            while (!validTickets) {
                printf("Number of Pasengers: ");
                scanf("%d", &numTickets);
                
                if (buses[busIndex].available_seats < numTickets) {
                    printf("Only %d seats available. Please enter a smaller number.\n", buses[busIndex].available_seats);
                } else if (numTickets <= 0) {
                    printf("Number of passengers must be at least 1.\n");
                } else {
                    validTickets = 1;
                }
            }

            char busType_booked[50];
            printf("Enter Bus Type for %s (e.g., Sleeper, Seater): ", chosenBus);
            scanf("%s", busType_booked);

            float totalFare = numTickets * buses[busIndex].price;
            char paymentType[30];
            int paymentSuccess = 0;
            int initial_passenger_count = passenger_count;
            srand(time(NULL));

            // --- 1. COLLECT PASSENGER DETAILS FIRST ---
            printf("\n=== ENTER PASSENGER DETAILS ===\n");
            for (int j = 0; j < numTickets; j++) {
                struct Passenger p;
                printf("\nPassenger %d Name: ", j + 1);
                scanf("%s", p.name);
                printf("Age: ");
                scanf("%d", &p.age);
                strcpy(p.journey_date, date);
                strcpy(p.busNo, buses[busIndex].busNo);
                
                // Window preference with validation
                int validWindow = 0;
                while (!validWindow) {
                    printf("Window seat? (1 Yes / 0 No): ");
                    scanf("%d", &p.window_pref);
                    if(p.window_pref!=0&&p.window_pref!=1){
                        printf("Invalid choice!! Please enter 1 for Yes or 0 for No.\n");
                    } else {
                        validWindow = 1;
                    }
                }
                
                // Seater preference with validation
                int validSeater = 0;
                while (!validSeater) {
                    printf("2-seater or 3-seater side? (2/3): ");
                    scanf("%d", &p.seater_pref);
                    if(p.seater_pref!=2&&p.seater_pref!=3){
                        printf("Invalid choice!! Please enter 2 or 3.\n");
                    } else {
                        validSeater = 1;
                    }
                }
                
                printf("Gender: ");
                scanf("%s", p.gender);
                


            char phone[20];

            while (1) {   // repeat until correct
            int valid = 1;

            printf("Phone: ");
            scanf("%19s", phone);

            int len = strlen(phone);

            // Length must be exactly 10
            if (len != 10) {
            printf("Error: Phone number must be exactly 10 digits. Try again.\n");
            continue;   // ask again
            }

           // Check each character is a digit
           for (int i = 0; i < 10; i++) {
            if (!isdigit(phone[i])) {
            printf("Error: Phone number must contain only digits. Try again.\n");
            valid = 0;
            break;
            }
           }

         if (valid) {
         strcpy(p.phone,phone);
         break;   // number is correct → exit loop
        }
        }


                p.seat_no = assignSeat(&buses[busIndex], p.window_pref, p.seater_pref);
                generate_booking_password(p.booking_password, 6);
                p.payment_success = 0;
                
                printf("Seat %d temporarily reserved. Booking Password: %s\n", p.seat_no, p.booking_password);
                passengers[passenger_count++] = p;
            }
            system("cls");
            // --- 2. NOW PROCESS PAYMENT ---
            printf("\n=== PAYMENT PROCESSING ===\n");
            
            int paymentChoice = 0;
            int dummyPin = 0;
            char upiID[100];
            char cardNumber[30];
            char loginId[50];
            char pass[20];
            
            printf("Processing payment for %d tickets...\n", numTickets);
            printf("Total amount due: %.2f\n", totalFare);

            paymentSuccess = process_payment(); 
            
            if (paymentSuccess) {
                while (paymentChoice == 0) {
                    printf("\n------------------- PAYMENT METHODS -------------------\n");
                    printf("Choose Payment Method:\n");
                    printf("1. Card (Credit/Debit)\n");
                    printf("2. UPI (Google Pay, PhonePe, etc.)\n");
                    printf("3. Net Banking\n");
                    printf("Enter choice (1-3): ");
                    scanf("%d", &paymentChoice);
                    
                    switch (paymentChoice) {
                        case 1:
                            strcpy(paymentType, "Card");

                            while (1) {
                            printf("Enter the card number of 16-digit:");
                            scanf(" %19[^\n]", cardNumber);

                            int len = strlen(cardNumber);

                            // Total length must be 19 (16 digits + 3 spaces)
                            if (len != 19) {
                            printf("Error: Card number must be in this format: XXXX XXXX XXXX XXXX\n");
                            continue;
                            }

                           int valid = 1;

                            // Check characters one by one
                            for (int i = 0; i < 19; i++) {

                            // Spaces must be at positions 4, 9, 14
                            if (i == 4 || i == 9 || i == 14) {
                            if (cardNumber[i] != ' ') {
                            printf("Error: Incorrect spacing. Use format XXXX XXXX XXXX XXXX\n");
                            valid = 0;
                            break;
                           }
                           }
                            else {
                            // Other positions must be digits
                            if (!isdigit(cardNumber[i])) {
                            printf("Error: All characters must be digits except spaces.\n");
                            valid = 0;
                            break;
                            }
                            }
                            }

                            if (valid) {
                            break;  // valid card number → exit loop
                            }
                            }
                            char cvv[10];

                            while (1) {
                            printf("Enter CVV (3 digits): ");
                            scanf("%9s", cvv);

                            int len = strlen(cvv);

                            // length must be 3
                            if (len != 3) {
                            printf("Error: CVV must be exactly 3 digits.\n");
                            continue;
                             }

                            int valid = 1;

                            // all characters must be digits
                            for (int i = 0; i < 3; i++) {
                            if (!isdigit(cvv[i])) {
                            printf("Error: CVV must contain only digits (0–9).\n");
                            valid = 0;
                             break;
                             }
                             }

                            if (valid)
                             break;
                            }

                            printf("Processing Card Payment... ");
                            sleep(2);
                            break;
                        case 2:
                            strcpy(paymentType,"UPI");
                           char upiID[50];

                           while (1) {
                           printf("Enter UPI ID (e.g., example@bank): ");
                           scanf("%49s", upiID);

                           int len = strlen(upiID);

                           // Cannot start or end with '@'
                           if (upiID[0] == '@' || upiID[len - 1] == '@') {
                           printf("Error: UPI ID cannot start or end with '@'.\n");
                           continue;
                           }

                           int atCount = 0;

                           // Count '@' and check for spaces
                           for (int i = 0; i < len; i++) {
                            if (upiID[i] == '@')
                                atCount++;

                            if (upiID[i] == ' ') {
                                printf("Error: UPI ID cannot contain spaces.\n");
                                    atCount = 2; // force invalid
                                    break;
                                     }
                              }

                           // Must contain exactly 1 '@'
                           if (atCount != 1) {
                           printf("Error: UPI ID must contain exactly one '@' (example@bank).\n");
                           continue;
                           }

                           break; // valid
                           }
                            
                            char pin[20];
                            int dummyPin;

                            while (1) {
                            printf("Enter UPI PIN: ");
                            scanf("%19s", pin);

                            int len = strlen(pin);

                            // Check length: must be 1 to 6
                            if (len == 0 || len > 6) {
                            printf("Error: UPI PIN must be between 1 and 6 digits.\n");
                            continue;
                            }

                             int valid = 1;

                            // Check all characters are digits
                            for (int i = 0; i < len; i++) {
                            if (!isdigit(pin[i])) {
                            printf("Error: UPI PIN must contain only digits.\n");
                            valid = 0;
                            break;
                             }
                            }

                            if (valid)
                             break;  // PIN is valid
                            }

                            // ✔ Save into dummyPin as integer
                            dummyPin = atoi(pin);

                            printf("Processing UPI Payment... ");
                            sleep(2);
                            break;
                        case 3:
                            strcpy(paymentType, "Net Banking");
                            printf("Enter Bank Login ID: ");
                            scanf("%s", loginId);
                            printf("Enter Net Banking password: ");
                            scanf("%s", pass);
                            printf("Processing Payment... ");
                            sleep(2);
                            break;
                        default:
                            printf("Invalid choice. Please select 1, 2, or 3.\n");
                            paymentChoice = 0;
                    }
                }
                printf("Payment Successful! All tickets are confirmed!\n");
            } else {
                printf("Payment failed! Cancellation of booking in progress.\n");
                for (int i = initial_passenger_count; i < passenger_count; i++) {
                     int seat_idx = passengers[i].seat_no - 1; 
                     if (seat_idx >= 0 && seat_idx < MAX_SEATS) {
                         buses[busIndex].seat_map[seat_idx] = 0;
                         buses[busIndex].available_seats++;
                     }
                }
                
                printf("\nPress any key to try again...");
                getch();
                return bookTicketFunction();
            }
            
            // --- 3. DATA PERSISTENCE (ONLY IF PAYMENT WAS SUCCESSFUL) ---

            for (int i = initial_passenger_count; i < passenger_count; i++) {
                struct Passenger p = passengers[i];
                p.payment_success = paymentSuccess;
                FILE *booking_fp = fopen("booking.csv", "a");
                if (booking_fp) {
                    char datetime[30];
                    char seat_str[10];
                    time_t now = time(NULL);
                    struct tm *t = localtime(&now);
                    sprintf(seat_str, "%d", p.seat_no);
                    fseek(booking_fp, 0, SEEK_END);
                    long file_size = ftell(booking_fp);
                    if (file_size == 0) {
                        fprintf(booking_fp, "Name,Age,Gender,Seat_Num,Phone_Number,Bus_Name,Bus_Num,seat_type,Arrival_Time,Destination,Booking_DateTime,paymentmode,password,price\n");
                    }
                    fprintf(booking_fp, "%s,%d,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%.2f\n",
                                p.name, p.age, p.gender, seat_str, p.phone, 
                                buses[busIndex].bus_name, p.busNo, busType_booked, buses[busIndex].arrival, 
                                buses[busIndex].destination, date, paymentType, p.booking_password,buses[busIndex].price);
                    fclose(booking_fp);
                } else {
                    printf("Error: Could not open booking.csv for writing\n");
                    printf("\nPress any key to continue...");
                    getch();
                    return 1;
                }
                if (p.payment_success) {
                    print_receipt(p, buses[busIndex], buses[busIndex].price, paymentType);
                }
            }

            printf("\nTotal Fare: %.2f\n", totalFare);
            fp2 = fopen("SeatMap.csv", "w");
            for (int i = 0; i < busCount; i++) {
                fprintf(fp2, "%s,", buses[i].busNo);
                for (int s = 0; s < MAX_SEATS; s++) {
                    fprintf(fp2, "%d ", buses[i].seat_map[s]);
                }
                fprintf(fp2, "\n");
            }
            fclose(fp2);
        system("cls");
          
            print_and_save_ticket(
            buses[busIndex],
            passengers,
            passenger_count - numTickets,
           numTickets,
           date,
           totalFare,
           paymentType);
            fp = fopen("Buses_schedule.csv", "w");
            if (!fp) {
                printf("Error: Could not update Buses_schedule.csv\n");
                printf("\nPress any key to continue...");
                getch();
                return 1;
            }
            for (int i = 0; i < busCount; i++) {
                fprintf(fp, "%s,%s,%s,%s,%s,%d,%.2f,%s,%s\n",
                        buses[i].busNo,
                        buses[i].source,
                        buses[i].destination,
                        buses[i].departure,
                        buses[i].arrival,
                        buses[i].available_seats,
                        buses[i].price,
                        buses[i].bus_name,
                        buses[i].bus_type);
            }
            fclose(fp);
        }
    }
    
    printf("\nPress any key to continue...");
    getch();
    busbookingmenu();
}

/****************** Cancellation Function ******************/

int cancelTicketFunction() {
    system("cls");
    char name[50], bus_number[20], password[30];
    int seat_no;

    printf("=== TICKET CANCELLATION ===\n");
    printf("Enter passenger name: ");
    scanf("%49s", name);
    printf("Enter seat number: ");
    scanf("%d", &seat_no);
    printf("Enter bus number: ");
    scanf("%19s", bus_number);
    printf("Enter booking password: ");
    scanf("%29s", password);

    while (getchar() != '\n');

    FILE *book_fp = fopen("booking.csv", "r");
    if (!book_fp) {
        printf("Error: No bookings file found!\n");
        getch();
        return 1;
    }

    char line[5120], found_line[5120] = "";
    int found = 0;

    fgets(line, sizeof(line), book_fp);  // skip header

    int d, m, y;
    char busno[20] = "";
    char departure[20] = "";

    while (fgets(line, sizeof(line), book_fp)) {

        char file_name[50], file_pass[30], file_gender[10], file_phone[15];
        char file_bus_name[50], file_bus_type[20], file_arrival[20], file_dest[20];
        char file_datetime[30], file_payment[20];
        int file_seat, file_age;
        char file_bus_num[20];
        float file_price;

        if (sscanf(line,
            "%49[^,],%d,%9[^,],%d,%14[^,],%49[^,],%19[^,],%19[^,],%19[^,],%19[^,],%29[^,],%19[^,],%29[^,],%f",
            file_name, &file_age, file_gender, &file_seat, file_phone,
            file_bus_name, file_bus_num, file_bus_type, file_arrival, file_dest,
            file_datetime, file_payment, file_pass, &file_price) >= 13) {

            sscanf(file_datetime, "%d/%d/%d", &d, &m, &y);

            if (strcmp(file_name, name) == 0 &&
                file_seat == seat_no &&
                strcmp(file_bus_num, bus_number) == 0 &&
                strcmp(file_pass, password) == 0) {

                strcpy(found_line, line);
                strcpy(busno, file_bus_num);
                found = 1;
                printf("Booking found and verified!\n");
                break;
            }
        }
    }
    fclose(book_fp);

    if (!found) {
        printf("\nError: No matching booking found!\n");
        getch();
        return 1;
    }

    FILE *BOOKING = fopen("Buses_schedule.csv", "r");
    if (!BOOKING) return 1;

    struct Bus buses[MAX_BUSES];
    int busCount = 0;
    char line2[5120];

    while (fgets(line2, sizeof(line2), BOOKING)) {
        if (sscanf(line2, "%[^,],%[^,],%[^,],%[^,],%[^,],%d,%f,%[^,],%[^\n]",
            buses[busCount].busNo, buses[busCount].source, buses[busCount].destination,
            buses[busCount].departure, buses[busCount].arrival,
            &buses[busCount].available_seats, &buses[busCount].price,
            buses[busCount].bus_name, buses[busCount].bus_type) == 9) {

            if (strcmp(busno, buses[busCount].busNo) == 0) {
                strcpy(departure, buses[busCount].departure);
                buses[busCount].available_seats++;
                buses[busCount].seat_map[seat_no]=0;
            }
            busCount++;
        }
    }
    fclose(BOOKING);

    // ===== TIME VALIDATION =====
    time_t now = time(NULL);
    struct tm *current = localtime(&now);

    int currentDay = current->tm_mday;
    int currentMonth = current->tm_mon + 1;
    int currentYear = current->tm_year + 1900;
    int currentMinutes = current->tm_hour * 60 + current->tm_min;

    if (y < currentYear ||
        (y == currentYear && m < currentMonth) ||
        (y == currentYear && m == currentMonth && d < currentDay)) {
        printf("\nError: Ticket cancellation not allowed for past date!\n");
        getch();
        return 1;
    }

    if (y == currentYear && m == currentMonth && d == currentDay) {
        int h, min;
        sscanf(departure, "%d:%d", &h, &min);
        int depMinutes = h * 60 + min;

        if (depMinutes <= currentMinutes) {
            printf("\nError: Ticket cancellation closed (departure time passed)!\n");
            getch();
            return 1;
        }
    }

    // ===== WRITE CANCEL DATA AND REMOVE ORIGINAL =====
    FILE *cancel_fp = fopen("cancel_booking.csv", "a");
    fprintf(cancel_fp, "%s", found_line);
    fclose(cancel_fp);

    book_fp = fopen("booking.csv", "r");
    FILE *temp = fopen("temp_booking.csv", "w");

    fgets(line, sizeof(line), book_fp);
    fprintf(temp, "%s", line);

    while (fgets(line, sizeof(line), book_fp)) {
        if (strcmp(line, found_line) != 0) {
            fprintf(temp, "%s", line);
        }
    }

    fclose(book_fp);
    fclose(temp);
   FILE *fp = fopen("Buses_schedule.csv", "w");
            if (!fp) {
                printf("Error: Could not update Buses_schedule.csv\n");
                printf("\nPress any key to continue...");
                getch();
                return 1;
            }
            for (int i = 0; i < busCount; i++) {
                fprintf(fp, "%s,%s,%s,%s,%s,%d,%.2f,%s,%s\n",
                        buses[i].busNo,
                        buses[i].source,
                        buses[i].destination,
                        buses[i].departure,
                        buses[i].arrival,
                        buses[i].available_seats,
                        buses[i].price,
                        buses[i].bus_name,
                        buses[i].bus_type);
            }
            fclose(fp);
           FILE *fp2 = fopen("SeatMap.csv", "w");
        for (int i = 0; i < busCount; i++) {
            fprintf(fp2, "%s,", buses[i].busNo);
            for (int s = 0; s < MAX_SEATS; s++)
                fprintf(fp2, "%d ", buses[i].seat_map[s]);
            fprintf(fp2, "\n");
        }
        fclose(fp2);
    remove("booking.csv");
    rename("temp_booking.csv", "booking.csv");

    printf("\nCANCELLATION SUCCESSFUL!\nRefund will be processed soon.\n");
    getch();
    busbookingmenu();
}

/****************** Main Menu Function ******************/

int busbookingmenu() {
    int choice;
    
    system("cls");
    printf("=================================\n");
    printf("     BUS BOOKING SYSTEM\n");
    printf("=================================\n");
    printf("\nChoose an option:\n");
    printf("1. Book Ticket\n");
    printf("2. Cancel Ticket\n");
    printf("3. Exit\n");
    printf("\nEnter your choice (1-3): ");
    
    scanf("%d", &choice);
    
    switch (choice) {
        case 1:
            return bookTicketFunction();
        case 2:
            return cancelTicketFunction();
        case 3:
            printf("Thank you for using our service!\n");
            return 0;
        default:
            printf("Invalid choice! Please try again.\n");
            printf("\nPress any key to continue...");
            getch();
            return 1;
    }
}