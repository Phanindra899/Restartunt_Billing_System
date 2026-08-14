#include "restaurant.h"

static FoodItem g_menu[MAX_MENU_ITEMS];
static int g_menu_count = 0;

/* Populates the restaurant menu with realistic food items */
void initMenu(void) {
    g_menu_count = 0;

    /* Starters (100s) */
    g_menu[g_menu_count++] = (FoodItem){101, "Veg Manchurian", "Starters", 140.00};
    g_menu[g_menu_count++] = (FoodItem){102, "Paneer 65", "Starters", 160.00};
    g_menu[g_menu_count++] = (FoodItem){103, "French Fries", "Starters", 100.00};
    g_menu[g_menu_count++] = (FoodItem){104, "Chicken Tikka", "Starters", 210.00};

    /* Main Course (200s) */
    g_menu[g_menu_count++] = (FoodItem){201, "Veg Noodles", "Main Course", 150.00};
    g_menu[g_menu_count++] = (FoodItem){202, "Fried Rice", "Main Course", 160.00};
    g_menu[g_menu_count++] = (FoodItem){203, "Paneer Butter Masala", "Main Course", 190.00};
    g_menu[g_menu_count++] = (FoodItem){204, "Dal Makhani", "Main Course", 170.00};
    g_menu[g_menu_count++] = (FoodItem){205, "Butter Naan", "Main Course", 40.00};

    /* Biryani (300s) */
    g_menu[g_menu_count++] = (FoodItem){301, "Chicken Biryani", "Biryani", 220.00};
    g_menu[g_menu_count++] = (FoodItem){302, "Veg Biryani", "Biryani", 180.00};
    g_menu[g_menu_count++] = (FoodItem){303, "Mutton Biryani", "Biryani", 280.00};

    /* Beverages (400s) */
    g_menu[g_menu_count++] = (FoodItem){401, "Coke", "Beverages", 50.00};
    g_menu[g_menu_count++] = (FoodItem){402, "Fresh Lime Soda", "Beverages", 60.00};
    g_menu[g_menu_count++] = (FoodItem){403, "Coffee", "Beverages", 70.00};
    g_menu[g_menu_count++] = (FoodItem){404, "Mineral Water", "Beverages", 20.00};

    /* Desserts (500s) */
    g_menu[g_menu_count++] = (FoodItem){501, "Ice Cream", "Desserts", 80.00};
    g_menu[g_menu_count++] = (FoodItem){502, "Gulab Jamun", "Desserts", 90.00};
    g_menu[g_menu_count++] = (FoodItem){503, "Sizzling Brownie", "Desserts", 140.00};
}

int getMenuCount(void) {
    return g_menu_count;
}

const FoodItem* getMenuItem(int index) {
    if (index >= 0 && index < g_menu_count) {
        return &g_menu[index];
    }
    return NULL;
}

const FoodItem* findFoodItemById(int id) {
    for (int i = 0; i < g_menu_count; i++) {
        if (g_menu[i].id == id) {
            return &g_menu[i];
        }
    }
    return NULL;
}

void initOrder(Order *order, int order_id) {
    order->order_id = order_id;
    order->item_count = 0;
    order->subtotal = 0.0;
    order->discount_rate = DISCOUNT_RATE;
    order->discount_amount = 0.0;
    order->tax_rate = TAX_RATE;
    order->tax_amount = 0.0;
    order->grand_total = 0.0;
    order->timestamp[0] = '\0';
}

void getCurrentTimestamp(char *buffer, size_t size) {
    time_t rawtime;
    struct tm *timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    if (timeinfo != NULL) {
        strftime(buffer, size, "%Y-%m-%d %H:%M:%S", timeinfo);
    } else {
        snprintf(buffer, size, "N/A");
    }
}

/* Adds item to order or merges quantity if item already exists */
int addItemToOrder(Order *order, int item_id, int quantity, char *err_msg, size_t err_size) {
    const FoodItem *food = findFoodItemById(item_id);
    if (food == NULL) {
        if (err_msg) snprintf(err_msg, err_size, "Invalid Item ID (%d). Item not found in menu!", item_id);
        return 0;
    }

    if (quantity <= 0) {
        if (err_msg) snprintf(err_msg, err_size, "Quantity must be greater than 0!");
        return 0;
    }

    /* Check if item is already present in current order */
    for (int i = 0; i < order->item_count; i++) {
        if (order->items[i].item.id == item_id) {
            order->items[i].quantity += quantity;
            order->items[i].amount = order->items[i].quantity * order->items[i].item.price;
            calculateTotals(order);
            return 1;
        }
    }

    /* Add as new item */
    if (order->item_count >= MAX_ORDER_ITEMS) {
        if (err_msg) snprintf(err_msg, err_size, "Order item capacity reached (Max %d items)!", MAX_ORDER_ITEMS);
        return 0;
    }

    OrderItem newItem;
    newItem.item = *food;
    newItem.quantity = quantity;
    newItem.amount = food->price * quantity;

    order->items[order->item_count++] = newItem;
    calculateTotals(order);
    return 1;
}

int updateItemQuantity(Order *order, int item_id, int new_quantity, char *err_msg, size_t err_size) {
    if (new_quantity <= 0) {
        return removeItemFromOrder(order, item_id, err_msg, err_size);
    }

    for (int i = 0; i < order->item_count; i++) {
        if (order->items[i].item.id == item_id) {
            order->items[i].quantity = new_quantity;
            order->items[i].amount = order->items[i].quantity * order->items[i].item.price;
            calculateTotals(order);
            return 1;
        }
    }

    if (err_msg) snprintf(err_msg, err_size, "Item ID %d not found in active order.", item_id);
    return 0;
}

int removeItemFromOrder(Order *order, int item_id, char *err_msg, size_t err_size) {
    int found_index = -1;
    for (int i = 0; i < order->item_count; i++) {
        if (order->items[i].item.id == item_id) {
            found_index = i;
            break;
        }
    }

    if (found_index == -1) {
        if (err_msg) snprintf(err_msg, err_size, "Item ID %d is not in the current order.", item_id);
        return 0;
    }

    /* Shift elements left */
    for (int i = found_index; i < order->item_count - 1; i++) {
        order->items[i] = order->items[i + 1];
    }
    order->item_count--;

    calculateTotals(order);
    return 1;
}

void calculateTotals(Order *order) {
    order->subtotal = 0.0;
    for (int i = 0; i < order->item_count; i++) {
        order->subtotal += order->items[i].amount;
    }

    /* 5% Discount rounded to 2 decimal places */
    order->discount_amount = round(order->subtotal * order->discount_rate * 100.0) / 100.0;

    /* Taxable amount after discount */
    double taxable_amount = order->subtotal - order->discount_amount;

    /* 5% GST Tax rounded to 2 decimal places */
    order->tax_amount = round(taxable_amount * order->tax_rate * 100.0) / 100.0;

    /* Final Grand Total = Subtotal - Discount + Tax */
    order->grand_total = order->subtotal - order->discount_amount + order->tax_amount;
}

void startNewOrder(Order *order, int *current_order_id) {
    (*current_order_id)++;
    initOrder(order, *current_order_id);
}
