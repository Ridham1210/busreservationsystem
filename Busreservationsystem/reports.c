#include <stdio.h>
#include <string.h>

/*
 booking.csv format (your current system):
 name, age, gender, seat, phone,
 bus_name, bus_no, bus_type,
 arrival, dest, datetime,
 payment_mode, price
*/

/* ============================
   SHOW BUS WISE REVENUE (TABLE)
   ============================ */
void showBusRevenue() {

    FILE *fp = fopen("booking.csv", "r");
    if (!fp) {
        printf("Cannot open booking.csv\n");
        return;
    }

    struct R { int busNo; int tickets; float revenue; } rec[200];
    int used = 0;

    for (int i = 0; i < 200; i++) {
        rec[i].busNo = 0;
        rec[i].tickets = 0;
        rec[i].revenue = 0;
    }

    char line[500];
    fgets(line, sizeof(line), fp); // skip header

    while (fgets(line, sizeof(line), fp)) {

        int busno = 0;
        float price = 0;

        /*
           Correct format:
           name,age,gender,seat,phone,
           bus_name,bus_no,bus_type,
           arrival,dest,datetime,
           paymode,password,price ← column 14
        */

        int ok = sscanf(line,
            "%*[^,],%*[^,],%*[^,],%*[^,],%*[^,],"  // 1…5
            "%*[^,],%d,%*[^,],"                    // bus_no = 7
            "%*[^,],%*[^,],%*[^,],"                // 8,9,10
            "%*[^,],%*[^,],%f",                    // paymode, password, PRICE
            &busno, &price
        );

        if (ok == 2) {

            int found = 0;
            for (int i = 0; i < used; i++) {
                if (rec[i].busNo == busno) {
                    rec[i].tickets++;
                    rec[i].revenue += price;
                    found = 1;
                    break;
                }
            }

            if (!found) {
                rec[used].busNo = busno;
                rec[used].tickets = 1;
                rec[used].revenue = price;
                used++;
            }
        }
    }

    fclose(fp);

    printf("\n================ BUS REVENUE REPORT ================\n\n");

    printf("+-----------------------------------------------------------+\n");
    printf("| %-8s | %-10s | %-12s |\n", "Bus No", "Tickets", "Revenue");
    printf("+-----------------------------------------------------------+\n");

    float totalR = 0;
    int totalT = 0;

    for (int i = 0; i < used; i++) {
        printf("| %-8d | %-10d | %-12.2f |\n",
               rec[i].busNo,
               rec[i].tickets,
               rec[i].revenue);
        totalR += rec[i].revenue;
        totalT += rec[i].tickets;
    }

    printf("+-----------------------------------------------------------+\n");
    printf("| %-8s | %-10d | %-12.2f |\n", "TOTAL", totalT, totalR);
    printf("+-----------------------------------------------------------+\n");
}

void showCancelReport() {

    FILE *fp = fopen("cancel_booking.csv", "r");
    if (!fp) {
        printf("Cannot open cancel_booking.csv\n");
        return;
    }

    struct C { int busNo; int tickets; float loss; } rec[200];
    int used = 0;

    for (int i = 0; i < 200; i++) {
        rec[i].busNo = 0;
        rec[i].tickets = 0;
        rec[i].loss = 0;
    }

    char line[500];
    fgets(line, sizeof(line), fp);

    while (fgets(line, sizeof(line), fp)) {

        int busno = 0;
        float price = 0;

        int ok = sscanf(line,
            "%*[^,],%*[^,],%*[^,],%*[^,],%*[^,],"  // 1..5
            "%*[^,],%d,%*[^,],"                    // bus number = 7
            "%*[^,],%*[^,],%*[^,],"                // 8,9,10
            "%*[^,],%*[^,],%f",                    // paymode,password, PRICE
            &busno, &price
        );

        if (ok == 2) {

            int found = 0;
            for (int i = 0; i < used; i++) {
                if (rec[i].busNo == busno) {
                    rec[i].tickets++;
                    rec[i].loss += price;
                    found = 1;
                    break;
                }
            }

            if (!found) {
                rec[used].busNo = busno;
                rec[used].tickets = 1;
                rec[used].loss = price;
                used++;
            }
        }
    }

    fclose(fp);

    printf("\n================ CANCELLED TICKETS REPORT ================\n\n");

    printf("+-----------------------------------------------------------+\n");
    printf("| %-8s | %-10s | %-12s |\n", "Bus No", "Tickets", "Loss");
    printf("+-----------------------------------------------------------+\n");

    int totalT = 0;
    float totalL = 0;

    for (int i = 0; i < used; i++) {
        printf("| %-8d | %-10d | %-12.2f |\n",
               rec[i].busNo,
               rec[i].tickets,
               rec[i].loss);
        totalT += rec[i].tickets;
        totalL += rec[i].loss;
    }

    printf("+-----------------------------------------------------------+\n");
    printf("| %-8s | %-10d | %-12.2f |\n", "TOTAL", totalT, totalL);
    printf("+-----------------------------------------------------------+\n");
}
