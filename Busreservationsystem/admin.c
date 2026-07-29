#include "admin.h"
#include <stdio.h>
#include <windows.h>
#include <conio.h>
#include "User.h"
#include "Booking.h"
#include "buses.h"
#include "reports.h"

// -------------------- Utility Functions --------------------

void clearScreen() { system("cls"); }

// void SetColor(int ForgC, int BackC) {
//     WORD wColor = ((BackC & 0x0F) << 4) + (ForgC & 0x0F);
//     SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), wColor);
// }

// -------------------- Generic Menu Function --------------------

int drawMenu(const char *title, const char *modules[], const char *desc[], int count) {
    clearScreen();
    // SetColor(11, 0);
    printf("\n============================== %s ==============================\n\n", title);
    // SetColor(7, 0);

    printf(" %-4s %-35s %-50s\n", "No.", "Module Name", "What Admin Can Do");
    printf(" %-4s %-35s %-50s\n", "---", "-----------------------------------", "--------------------------------------------------");

    for (int i = 0; i < count; i++) {
        printf(" %-4d %-35s %-50s\n", i + 1, modules[i], desc[i]);
    }

    printf("\nSelect an option (1-%d) or 0 to go back: ", count);
    //fflush(stdout);

    while (1) {
        int ch = _getch();
        if (ch == '0') return -1;
        if (ch >= '1' && ch <= '0' + count) {
            printf("%c\n", ch);  // Show the selected number
            return ch - '1';     // return index
        }
    }
}

// -------------------- Sub Menus --------------------

void userManagement() {
    const char *opts[] = {
        "View all users",
        "Add user",
        "Remove user",
        "Reset password",
        "Back"
    };
    int n = 5, c;
    while (1) {
        c = drawMenu("User Management", opts, opts, n);
        if (c == -1 || c == n - 1) return;
        switch (c) {
            case 0: viewAllUsers(); break;
            case 1: addUser(); break;
            case 2: deleteUser(); break;
            case 3: changeUserPassword(); break;
        }
        // Only pause if an action was performed
        if (c >= 0 && c < n - 1) {
            printf("\nPress any key to continue...");
            _getch();
        }
    }
}

void busManagement() {
    const char *opts[] = {
        "View All Buses",
        "Add new bus",
        "Edit or Delete bus details",
        "Back"
    };
    int n = 4, c;
    while (1) {
        c = drawMenu("Bus Management", opts, opts, n);
        if (c == -1 || c == n - 1) return;
        switch (c) {
            case 0: viewAllBuses(); break;
            case 1: addBus(); break;
            case 2: editOrDeleteBus(); break;
        }
        // Only pause if an action was performed
        if (c >= 0 && c < n - 1) {
            printf("\nPress any key to continue...");
            _getch();
        }
    }
}

void routeScheduleManagement() {
    const char *opts[] = {
        "View All Shedule",
        "Add route for daily",
        "Update daily route",
        "Back"
    };
    int n = 4, c;
    while (1) {
        c = drawMenu("Route & Schedule Management", opts, opts, n);
        if (c == -1 || c == n - 1) return;
        switch (c) {
            case 0: viewAllSchedules(); break;
            case 1: addDailySchedule(); break;
            case 2: updateDailySchedule(); break;
        }
        // Only pause if an action was performed
        if (c >= 0 && c < n - 1) {
            printf("\nPress any key to continue...");
            _getch();
        }
    }
}

void bookingManagement() {
    const char *opts[] = {
        "View all bookings",
        "View canceled tickets",
        "Add booking",
        "Cancel booking",
        "Back"
    };
    int n = 5, c;
    while (1) {
        c = drawMenu("Booking Management", opts, opts, n);
        if (c == -1 || c == n - 1) return;
        switch (c) {
            case 0: view_bookings(); break;
            case 1: view_cancellations(); break;
            case 2: add_booking(); break;
            case 3: cancel_booking(); break;
        }
        // Only pause if an action was performed
        if (c >= 0 && c < n - 1) {
            printf("\nPress any key to continue...");
            _getch();
        }
    }
}

void reportsDashboard() {
    const char *opts[] = {
        "Show total revenue",
        "Show total cancelation",
        "Back"
    };
    int n = 3, c;
    while (1) {
        c = drawMenu("Reports / Summary Dashboard", opts, opts, n);
        if (c == -1 || c == n - 1) return;
        clearScreen();
        switch (c) {
            case 0: showBusRevenue(); break;
            case 1: showCancelReport(); break;
        }
        printf("\nPress any key to continue...");
        _getch();
    }
}

// -------------------- Main Admin Menu --------------------

void runAdminMenu() {
    const char *modules[] = {
        "User Management",
        "Bus Management",
        "Route & Schedule Management",
        "Booking Management",
        "Reports / Summary Dashboard",
        "Log Out"
    };

    const char *desc[] = {
        "Manage Users",
        "Manage Buses",
        "Manage daily schedules",
        "Manage all bookings and cancellations",
        "Show booking and revenue reports",
        "Exit the admin menu and return to login"
    };

    int n = 6, choice;

    while (1) {
        choice = drawMenu("Admin Control Modules", modules, desc, n);
        if (choice == -1 || choice == n - 1) {
            clearScreen();
            printf("\nLogging out...\n");
            return;  // Just exit for now as we're in admin mode

            printf("Press Enter to continue...");
            getchar();
            return;
        }

        switch (choice) {
            case 0: userManagement(); break;
            case 1: busManagement(); break;
            case 2: routeScheduleManagement(); break;
            case 3: bookingManagement(); break;
            case 4: reportsDashboard(); break;
        }
    }
}
