/*
  bus_schedule_module.h
  Single header for Bus + Daily Schedule module.

  CSV schema (keep this exact order):
  Buses.csv:
    BusName,BusNo,BusType,MaxSeats,BasePrice

  Buses_schedule.csv:
    BusNo,Source,Destination,Departure,Arrival,AvailableSeats,Price,BusName,BusType
*/

#ifndef BUS_SCHEDULE_MODULE_H
#define BUS_SCHEDULE_MODULE_H

/* ===== Limits ===== */
#define MAX_BUSES       100
#define MAX_SCHEDULES   200
#define MAX_OVERRIDES   200
#define STR_LEN         50

/* ===== Structures ===== */

typedef struct {
    char  name[STR_LEN];
    int   bus_no;
    char  type[STR_LEN];
    int   max_seats;
    float base_price;
} Bus;

typedef struct {
    int   busNo;
    char  source[STR_LEN];
    char  destination[STR_LEN];
    char  departure[20];
    char  arrival[20];
    int   availableSeats;
    float price;
    char  busName[STR_LEN];
    char  busType[STR_LEN];
} Schedule;

/* ===== Utilities (simple I/O helpers) ===== */
void clear_console(void);
void press_any_key(void);
void remove_newline(char *s);

/* ===== File handling ===== */
void load_buses(void);
void save_buses(void);
void load_daily_schedules(void);
void save_daily_schedules(void);

/* ===== Bus management ===== */
void addBus(void);
void viewAllBuses(void);
void editOrDeleteBus(void);

/* ===== Schedule management ===== */
void addDailySchedule(void);
void viewAllSchedules(void);
void updateDailySchedule(void);

/* ===== Overrides (stub if unused) ===== */
void ensure_files_exist(void);     /* optional, create files with headers */
void cancelTripForDate(void);      /* declare only if you implement it */

#endif /* BUS_SCHEDULE_MODULE_H */
