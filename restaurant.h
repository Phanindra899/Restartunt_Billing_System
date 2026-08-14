#ifndef RESTAURANT_H
#define RESTAURANT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define MAX_MENU_ITEMS 100
#define MAX_ORDER_ITEMS 50
#define MAX_TABLES 20
#define MAX_ORDERS 200

#define DISCOUNT_RATE 0.05  /* 5% Discount */
#define TAX_RATE 0.05       /* 5% GST Tax */
#define INITIAL_ORDER_ID 1001

/* Table Statuses */
typedef enum {
    TABLE_AVAILABLE = 0,
    TABLE_OCCUPIED,
    TABLE_RESERVED
} TableStatus;

/* Order Statuses */
typedef enum {
    STATUS_PENDING = 0,
    STATUS_PREPARING,
    STATUS_READY,
    STATUS_SERVED,
    STATUS_COMPLETED,
    STATUS_CANCELLED
} OrderStatus;

/* Food Item structure */
typedef struct {
    int id;
    char name[60];
    char category[30];
    double price;
    int available; /* 1 = Available, 0 = Unavailable */
} FoodItem;

/* Table structure */
typedef struct {
    int id;                /* e.g., 1 for T01 */
    int capacity;          /* e.g., 2, 4, 6 */
    TableStatus status;
    int current_order_id;  /* Order ID occupying this table, or 0 */
    char reserved_for[50]; /* Name for reservation, if RESERVED */
} Table;

/* Order Item structure */
typedef struct {
    FoodItem item;
    int quantity;
    double amount;      /* price * quantity */
} OrderItem;

/* Order structure */
typedef struct {
    int order_id;
    int table_id;       /* 0 if walk-in / no table */
    char customer_name[50];
    OrderItem items[MAX_ORDER_ITEMS];
    int item_count;
    OrderStatus status;
    double subtotal;
    double discount_rate;
    double discount_amount;
    double tax_rate;
    double tax_amount;
    double grand_total;
    char timestamp[30];
} Order;

/* Dashboard & Reports Statistics */
typedef struct {
    int total_orders;
    int active_orders;
    int completed_orders;
    int pending_orders;
    int preparing_orders;
    int ready_orders;
    int served_orders;
    int cancelled_orders;
    int available_tables;
    int occupied_tables;
    int reserved_tables;
    double today_revenue;
    double avg_order_value;
    double occupancy_rate; /* percentage e.g. 40.0% */
} DashboardStats;

typedef struct {
    int item_id;
    char item_name[60];
    int total_quantity_sold;
    double total_revenue_generated;
} TopItemStat;

/* System Initialization & Persistence */
void initRestaurantData(void);
void loadData(void);
void saveData(void);

/* Menu API */
int getMenuCount(void);
const FoodItem* getMenuItem(int index);
const FoodItem* findFoodItemById(int id);
int addMenuItem(int id, const char *name, const char *category, double price, int available, char *err_msg, size_t err_size);
int updateMenuItem(int id, const char *name, const char *category, double price, int available, char *err_msg, size_t err_size);
int deleteMenuItem(int id, char *err_msg, size_t err_size);
int toggleItemAvailability(int id);

/* Table API */
int getTableCount(void);
Table* getTable(int index);
Table* findTableById(int id);
int assignTable(int table_id, int order_id, char *err_msg, size_t err_size);
int reserveTable(int table_id, const char *customer_name, char *err_msg, size_t err_size);
int releaseTable(int table_id, char *err_msg, size_t err_size);

/* Order API */
int getOrderCount(void);
Order* getOrder(int index);
Order* findOrderById(int order_id);
int createOrder(int table_id, const char *customer_name, char *err_msg, size_t err_size);
int addItemToOrder(Order *order, int item_id, int quantity, char *err_msg, size_t err_size);
int updateItemQuantity(Order *order, int item_id, int new_quantity, char *err_msg, size_t err_size);
int removeItemFromOrder(Order *order, int item_id, char *err_msg, size_t err_size);
int updateOrderStatus(int order_id, OrderStatus status, char *err_msg, size_t err_size);
void calculateTotals(Order *order);

/* Metrics & Helper API */
void getCurrentTimestamp(char *buffer, size_t size);
const char* getOrderStatusString(OrderStatus status);
const char* getTableStatusString(TableStatus status);
void getDashboardStats(DashboardStats *stats);
int getTopSellingItems(TopItemStat *top_items, int max_items);

#endif /* RESTAURANT_H */
