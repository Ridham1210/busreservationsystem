// auth_login.c
// Authentication Menu with Email + Password validation and simple password hashing

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <ctype.h>
#include <windows.h>

#include "admin.h"
#include "user.h"
#include "booking.h"
#include "buses.h"
#include "Bus_booking.h"
#include "reports.h"

void runAdminMenu();
int busbookingmenu();

User CURRENT_USER;
   // stores logged-in user


// ---------- Simple Hash Function ----------
void hashPassword(const char *password, char *hashed) {
    // Simple demo hash (not secure, just obfuscates)
    unsigned long hash = 5381;
    int c;
    while ((c = *password++))
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    sprintf(hashed, "%lu", hash);
}

// ---------- Validation Functions ----------
int isValidEmail(const char *email) {
    const char *at = strchr(email, '@');
    const char *dot = strrchr(email, '.');
    return at && dot && at < dot && at != email && *(dot + 1) != '\0';
}

int isValidPassword(const char *pass) {
    int len = strlen(pass);
    if (len < 8) return 0;

    int hasUpper = 0, hasDigit = 0, hasSpecial = 0;
    for (int i = 0; i < len; i++) {
        if (isupper(pass[i])) hasUpper = 1;
        else if (isdigit(pass[i])) hasDigit = 1;
        else if (!isalnum(pass[i])) hasSpecial = 1;
    }
    return hasUpper && hasDigit && hasSpecial;
}

// ---------- Login Menu ----------
void main() {
    User loggedInUser;
    char name[64], email[64], password[64], phone[64], hash[64];

    while (1) {
        system("cls");
        printf("========= LOGIN MENU =========\n");
        printf("1. Register\n");
        printf("2. Login\n");
        printf("3. Change Password\n");
        printf("4. Exit\n");
        printf("\nEnter choice: ");

        int choice = getch();

        switch (choice) {
        case '1':
            system("cls");
            printf("=== Register User ===\n");
            printf("Enter Name: ");
            scanf("%63s", name);
            printf("Enter Email: ");
            scanf("%63s", email);

            if (!isValidEmail(email)) {
                printf("❌ Invalid email format.\nPress any key...");
                getch();
                break;
            }

            printf("Enter Phone: ");
            while (getchar() != '\n');  // Clear input buffer

            while (1) {
                scanf("%63s", phone);
                
                int len = strlen(phone);
                
                // Length must be exactly 10
                if (len != 10) {
                    printf("Error: Phone number must be exactly 10 digits. Try again.\n");
                    printf("Enter Phone: ");
                    while (getchar() != '\n');  // Clear buffer after invalid input
                    continue;
                }
                
                int valid = 1;
                // Check each character is a digit
                for (int i = 0; i < 10; i++) {
                    if (!isdigit(phone[i])) {
                        printf("Error: Phone number must contain only digits. Try again.\n");
                        printf("Enter Phone: ");
                        valid = 0;
                        break;
                    }
                }
                
                if (!valid) {
                    while (getchar() != '\n');  // Clear buffer after invalid input
                    continue;
                }
                
                if (valid) {
                    while (getchar() != '\n');  // Clear buffer after valid input
                    break;  // Phone number is valid, exit loop
                }
            }
            printf("Enter Password: ");
            scanf("%63s", password);

            if (!isValidPassword(password)) {
                printf("❌ Password must have:\n");
                printf("   - At least 8 characters\n");
                printf("   - One uppercase letter\n");
                printf("   - One number\n");
                printf("   - One special character\n");
                printf("\nPress any key...");
                getch();
                break;
            }

            hashPassword(password, hash);
            registerUser(name, email, hash, phone); // store hashed password
            printf("\n✅ Registration successful!\nPress any key...");
            getch();
            break;

        case '2': {
            int success = 0;
            int attempts = 0;

            while (!success) {
                system("cls");
                printf("=== Login ===\n");
                printf("Enter Email or UserID : ");
                scanf("%63s", email);

                // user types 'back' to return
                if (strcmp(email, "0") == 0)
                    break;

                printf("Enter Password: ");
                scanf("%63s", password);
                hashPassword(password, hash);
                if (strcmp(email, "admin@admin.com") == 0 &&
                        strcmp(password, "admin123") == 0) {
                        printf("\nWelcome Admin!\n");
                        Sleep(1000);
                        runAdminMenu();
                        }

                if (loginUser(email, hash, &loggedInUser)) {
                    success = 1;

                    // Admin check
                    char adminHash[64];
                    hashPassword("admin123", adminHash);

                    if (strcmp(loggedInUser.email, "admin@admin.com") == 0 &&
                        strcmp(loggedInUser.password, "admin123") == 0) {
                        printf("\nWelcome Admin!\n");
                        Sleep(1000);
                        runAdminMenu();
                    } else {
                        CURRENT_USER = loggedInUser;

                        printf("\nWelcome %s!\n", CURRENT_USER.name);
                        Sleep(1000);

                        // open user menu or booking menu
                        busbookingmenu();

                    }
                } else {
                    attempts++;
                    printf("\n❌ Invalid email or password.\n");
                    printf("Type 'back' as email to return to menu.\n");
                    printf("Press any key to retry...");
                    getch();
                }
            }
            break;
        }


        case '3':
            changeUserPassword();
            break;

        case '4':
            printf("\nExiting program...\n");
            Sleep(500);
            return;

        default:
            printf("\nInvalid choice! Try again.\n");
            Sleep(800);
        }
    }
}

// ---------- Main ----------