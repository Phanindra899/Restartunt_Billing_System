#ifndef RESTAURANT_H
#define RESTAURANT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define MAX_MENU_ITEMS 30
#define MAX_ORDER_ITEMS 50
#define DISCOUNT_RATE 0.05  /* 5% Discount */
#define TAX_RATE 0.05       /* 5% GST Tax */
#define INITIAL_ORDER_ID 1001

/* Represents a single food item in the restaurant menu */
typedef struct {
    int id;
    char name[50];
    char category[30];
    double price;
} FoodItem;

/* Represents an item added to a customer's order */
typedef struct {
    FoodItem item;
    int quantity;
    double amount;  /* item.price * quantity */
} OrderItem;

/* Represents the complete order structure */
typedef struct {
    int order_id;
    OrderItem items[MAX_ORDER_ITEMS];
    int item_count;
    double subtotal;
    double discount_rate;
    double discount_amount;
    double tax_rate;
    double tax_amount;
    double grand_total;
    char timestamp[30];
} Order;

/* Business Logic Functions */
void initMenu(void);
int getMenuCount(void);
const FoodItem* getMenuItem(int index);
const FoodItem* findFoodItemById(int id);

void initOrder(Order *order, int order_id);
int addItemToOrder(Order *order, int item_id, int quantity, char *err_msg, size_t err_size);
int updateItemQuantity(Order *order, int item_id, int new_quantity, char *err_msg, size_t err_size);
int removeItemFromOrder(Order *order, int item_id, char *err_msg, size_t err_size);
void calculateTotals(Order *order);
void getCurrentTimestamp(char *buffer, size_t size);
void startNewOrder(Order *order, int *current_order_id);

#endif /* RESTAURANT_H */
