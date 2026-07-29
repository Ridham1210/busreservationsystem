/*
  user.h
  Header file for user management module.

  Supports:
    - Registration
    - Login
    - Password change
    - Deletion
    - Viewing all users
*/

#ifndef USER_H
#define USER_H

#include <windows.h>

#define MAX_USERS 100

typedef struct {
    char user_id[10];
    char name[64];
    char email[64];
    char password[64];
    char phone[64];
} User;

/* === Cursor Helpers === */
COORD getCursorPosition();
void setCursorPosition(COORD pos);
void clearLine();

/* === File Handling === */
int loadUsers(User users[]);
void saveUsers(User users[], int count);

/* === ID Generation === */
void generateRandomUserID(char *user_id, User users[], int count);

/* === Registration & Login === */
void registerUser(const char *name, const char *email, const char *password, const char *phone);
int loginUser(const char *user_or_email, const char *password, User *loggedInUser);

/* === User Management === */
void addUser();
int selectUserInteractive(User users[], int count);
void deleteUser();
void changeUserPassword();
void viewAllUsers();

#endif
