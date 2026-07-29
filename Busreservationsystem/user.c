/*
  user.c
  User Management Module
  Handles registration, login, password change, deletion, and viewing users.

  Data stored in: Users.csv
  Format: UserID,Name,Email,Password,Phone
*/

#include "user.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <windows.h>

#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif

#define CSV_FORMAT "%9[^,],%63[^,],%63[^,],%63[^,],%63[^\n]"

/* ==================== Cursor Helpers (Basic Console Tools) ==================== */

void clearLine() {
    printf("\r\033[K");
}

/* ==================== File Handling ==================== */

void initializeUser()
{
     FILE *fp = fopen("Users.csv", "r");
    if (!fp) {
        fp = fopen("Users.csv", "w");
        if (fp) {
            fprintf(fp, "UserID,Name,Email,Password,Phone\n");
            fclose(fp);
        }
    } else {
        fclose(fp);
    }
}

int loadUsers(User users[]) {
    initializeUser();
    FILE *fp = fopen("Users.csv", "r");
    if (!fp) return 0;

    char line[256];
    int count = 0;
    fgets(line, sizeof(line), fp); // skip header

    while (fgets(line, sizeof(line), fp) && count < MAX_USERS) {
        sscanf(line, CSV_FORMAT,
               users[count].user_id, users[count].name,
               users[count].email, users[count].password,
               users[count].phone); //sscanf is like scanf, but its input is what we provide as a string and stroes in to different variable
        count++;
    }
    fclose(fp);
    return count;
}

void saveUsers(User users[], int count) {
    FILE *fp = fopen("Users.csv", "w");
    if (!fp) return;

    fprintf(fp, "UserID,Name,Email,Password,Phone\n");
    for (int i = 0; i < count; i++) {
        fprintf(fp, "%s,%s,%s,%s,%s\n",
                users[i].user_id, users[i].name,
                users[i].email, users[i].password,
                users[i].phone);
    }
    fclose(fp);
}

/* ==================== ID Generator ==================== */

void generateRandomUserID(char *user_id, User users[], int count) {
    int unique = 0;
    while (!unique) {
        sprintf(user_id, "U%05d", rand() % 100000); //sprintf() writes formatted text into a string
        unique = 1;
        for (int i = 0; i < count; i++) {
            if (strcmp(users[i].user_id, user_id) == 0) {
                unique = 0;
                break;
            } //strcmp() compares two strings.if both same it gives 0.
        }
    }
}

/* ==================== Registration ==================== */

void registerUser(const char *name, const char *email, const char *password, const char *phone) {
    initializeUser();
    User users[MAX_USERS];
    int count = loadUsers(users);

    // check if email already registered
    for (int i = 0; i < count; i++) {
        if (strcmp(users[i].email, email) == 0) {
            printf("Email already registered.\n");
            return;
        }
    }

    char user_id[10];
    generateRandomUserID(user_id, users, count);

    FILE *fp = fopen("Users.csv", "a+");
    if (!fp) {
        printf("Cannot open Users.csv\n");
        return;
    }

    // if file is empty, write header
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    if (size == 0) {
        fprintf(fp, "UserID,Name,Email,Password,Phone\n");
    }

    fprintf(fp, "%s,%s,%s,%s,%s\n", user_id, name, email, password, phone);
    fclose(fp);

    printf("User Registered: %s\n", user_id);
}

/* ==================== Login ==================== */

int loginUser(const char *user_or_email, const char *password, User *loggedInUser) {
    User users[MAX_USERS];
    int count = loadUsers(users);

    for (int i = 0; i < count; i++) {
        if (strcmp(users[i].user_id, user_or_email) == 0 ||
            strcmp(users[i].email, user_or_email) == 0) {
            if (strcmp(users[i].password, password) == 0) {
                *loggedInUser = users[i];
                return 1;
            }//strcmp compare 2 strings.
            return 0;
        }
    }
    return 0;
}

/* ==================== Add User (Admin use) ==================== */

void addUser() {
    initializeUser();
    system("cls");
    char name[64], email[64], password[64], phone[64];

    printf("Enter Name: ");
    scanf("%63s", name);
    printf("Enter Email: ");
    scanf("%63s", email);
    printf("Enter Password: ");
    scanf("%63s", password);
    printf("Enter Phone: ");
    scanf("%63s", phone);

    registerUser(name, email, password, phone);
    printf("User added successfully!\n");
    system("pause");
}

/* ==================== User Selection (Number + Enter) ==================== */

int selectUserInteractive(User users[], int count) {
    if (count == 0) return -1;

    system("cls");
    printf("=== Select User ===\n\n");

    printf("+-------------------------------------------------------------------------------------------------+\n");
    printf("| %-3s | %-10s | %-20s | %-30s | %-15s |\n",
           "No", "UserID", "Name", "Email", "Phone");
    printf("+-------------------------------------------------------------------------------------------------+\n");

    for (int i = 0; i < count; i++) {
        printf("| %-3d | %-10s | %-20s | %-30s | %-15s |\n",
               i + 1,
               users[i].user_id,
               users[i].name,
               users[i].email,
               users[i].phone);
    }

    printf("+-------------------------------------------------------------------------------------------------+\n");

    int choice;
    printf("\nEnter user number (1-%d) or 0 to cancel: ", count);
    scanf("%d", &choice);

    if (choice < 1 || choice > count) return -1;
    return choice - 1;
}


/* ==================== Delete User ==================== */

void deleteUser() {
    system("cls");
    User users[MAX_USERS];
    int count = loadUsers(users);

    if (count == 0) {
        printf("No users available.\n");
        system("pause");
        return;
    }

    int selected = selectUserInteractive(users, count);
    if (selected == -1) return;

    for (int j = selected; j < count - 1; j++)
        users[j] = users[j + 1];

    count--;
    saveUsers(users, count);
    printf("User deleted successfully!\n");
    system("pause");
}

/* ==================== Change Password ==================== */

void changeUserPassword() {
    system("cls");
    User users[MAX_USERS];
    int count = loadUsers(users);

    if (count == 0) {
        printf("No users available.\n");
        system("pause");
        return;
    }

    int selected = selectUserInteractive(users, count);
    if (selected == -1) return;

    printf("Enter new password for %s: ", users[selected].name);
    scanf("%63s", users[selected].password);

    saveUsers(users, count);
    printf("Password updated successfully!\n");
    system("pause");
}

/* ==================== View All Users ==================== */

void viewAllUsers() {
    system("cls");
    FILE *fp = fopen("Users.csv", "r");
    if (!fp) {
        printf("Cannot open Users.csv\n");
        return;
    }

    char line[256];
    fgets(line, sizeof(line), fp); // skip header

    printf("=== All Users ===\n\n");

    printf("+-----------------------------------------------------------------------------------------------------------+\n");
    printf("| %-10s | %-20s | %-25s | %-15s | %-15s |\n",
           "UserID", "Name", "Email", "Password", "Phone");
    printf("+-----------------------------------------------------------------------------------------------------------+\n");

    while (fgets(line, sizeof(line), fp)) {
        char user_id[16], name[64], email[64], password[64], phone[32];

        sscanf(line, "%15[^,],%63[^,],%63[^,],%63[^,],%31[^\n]",
               user_id, name, email, password, phone);

        printf("| %-10s | %-20s | %-25s | %-15s | %-15s |\n",
               user_id, name, email, password, phone);
    }

    printf("+-----------------------------------------------------------------------------------------------------------+\n");

    fclose(fp);

    printf("\nPress any key to continue...");
    getch();
}


/* ==================== End of File ==================== */
