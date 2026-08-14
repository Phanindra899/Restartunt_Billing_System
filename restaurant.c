#include "restaurant.h"

static FoodItem g_menu[MAX_MENU_ITEMS];
static int g_menu_count = 0;

static Table g_tables[MAX_TABLES];
static int g_table_count = 0;

static Order g_orders[MAX_ORDERS];
static int g_order_count = 0;

static int g_next_order_id = INITIAL_ORDER_ID;

/* Initialize complete realistic demo dataset */
void initRestaurantData(void) {
    g_menu_count = 0;

    /* 1. STARTERS (101-108) */
    g_menu[g_menu_count++] = (FoodItem){101, "Veg Manchurian", "Starters", 140.00, 1};
    g_menu[g_menu_count++] = (FoodItem){102, "Paneer 65", "Starters", 160.00, 1};
    g_menu[g_menu_count++] = (FoodItem){103, "Paneer Tikka", "Starters", 190.00, 1};
    g_menu[g_menu_count++] = (FoodItem){104, "Gobi 65", "Starters", 130.00, 1};
    g_menu[g_menu_count++] = (FoodItem){105, "French Fries", "Starters", 100.00, 1};
    g_menu[g_menu_count++] = (FoodItem){106, "Chicken Tikka", "Starters", 210.00, 1};
    g_menu[g_menu_count++] = (FoodItem){107, "Chicken 65", "Starters", 200.00, 1};
    g_menu[g_menu_count++] = (FoodItem){108, "Chicken Manchurian", "Starters", 210.00, 1};

    /* 2. SOUPS (151-155) */
    g_menu[g_menu_count++] = (FoodItem){151, "Tomato Soup", "Soups", 90.00, 1};
    g_menu[g_menu_count++] = (FoodItem){152, "Sweet Corn Soup", "Soups", 100.00, 1};
    g_menu[g_menu_count++] = (FoodItem){153, "Hot & Sour Soup", "Soups", 110.00, 1};
    g_menu[g_menu_count++] = (FoodItem){154, "Chicken Clear Soup", "Soups", 130.00, 1};
    g_menu[g_menu_count++] = (FoodItem){155, "Chicken Manchow Soup", "Soups", 140.00, 1};

    /* 3. MAIN COURSE (201-208) */
    g_menu[g_menu_count++] = (FoodItem){201, "Paneer Butter Masala", "Main Course", 190.00, 1};
    g_menu[g_menu_count++] = (FoodItem){202, "Kadai Paneer", "Main Course", 200.00, 1};
    g_menu[g_menu_count++] = (FoodItem){203, "Dal Tadka", "Main Course", 150.00, 1};
    g_menu[g_menu_count++] = (FoodItem){204, "Dal Makhani", "Main Course", 170.00, 1};
    g_menu[g_menu_count++] = (FoodItem){205, "Veg Curry", "Main Course", 150.00, 1};
    g_menu[g_menu_count++] = (FoodItem){206, "Chicken Curry", "Main Course", 220.00, 1};
    g_menu[g_menu_count++] = (FoodItem){207, "Butter Chicken", "Main Course", 240.00, 1};
    g_menu[g_menu_count++] = (FoodItem){208, "Kadai Chicken", "Main Course", 230.00, 1};

    /* 4. BIRYANI (301-306) */
    g_menu[g_menu_count++] = (FoodItem){301, "Veg Biryani", "Biryani", 180.00, 1};
    g_menu[g_menu_count++] = (FoodItem){302, "Egg Biryani", "Biryani", 190.00, 1};
    g_menu[g_menu_count++] = (FoodItem){303, "Paneer Biryani", "Biryani", 200.00, 1};
    g_menu[g_menu_count++] = (FoodItem){304, "Chicken Biryani", "Biryani", 220.00, 1};
    g_menu[g_menu_count++] = (FoodItem){305, "Chicken Fry Piece Biryani", "Biryani", 250.00, 1};
    g_menu[g_menu_count++] = (FoodItem){306, "Mutton Biryani", "Biryani", 280.00, 1};

    /* 5. INDIAN BREADS (351-355) */
    g_menu[g_menu_count++] = (FoodItem){351, "Plain Naan", "Indian Breads", 30.00, 1};
    g_menu[g_menu_count++] = (FoodItem){352, "Butter Naan", "Indian Breads", 40.00, 1};
    g_menu[g_menu_count++] = (FoodItem){353, "Garlic Naan", "Indian Breads", 60.00, 1};
    g_menu[g_menu_count++] = (FoodItem){354, "Tandoori Roti", "Indian Breads", 30.00, 1};
    g_menu[g_menu_count++] = (FoodItem){355, "Butter Roti", "Indian Breads", 35.00, 1};

    /* 6. CHINESE (401-406) */
    g_menu[g_menu_count++] = (FoodItem){401, "Veg Fried Rice", "Chinese", 150.00, 1};
    g_menu[g_menu_count++] = (FoodItem){402, "Chicken Fried Rice", "Chinese", 200.00, 1};
    g_menu[g_menu_count++] = (FoodItem){403, "Veg Noodles", "Chinese", 150.00, 1};
    g_menu[g_menu_count++] = (FoodItem){404, "Chicken Noodles", "Chinese", 190.00, 1};
    g_menu[g_menu_count++] = (FoodItem){405, "Schezwan Fried Rice", "Chinese", 180.00, 1};
    g_menu[g_menu_count++] = (FoodItem){406, "Schezwan Chicken Noodles", "Chinese", 210.00, 1};

    /* 7. BEVERAGES (501-508) */
    g_menu[g_menu_count++] = (FoodItem){501, "Coke", "Beverages", 50.00, 1};
    g_menu[g_menu_count++] = (FoodItem){502, "Sprite", "Beverages", 50.00, 1};
    g_menu[g_menu_count++] = (FoodItem){503, "Fresh Lime Soda", "Beverages", 60.00, 1};
    g_menu[g_menu_count++] = (FoodItem){504, "Fresh Lime Water", "Beverages", 50.00, 1};
    g_menu[g_menu_count++] = (FoodItem){505, "Coffee", "Beverages", 70.00, 1};
    g_menu[g_menu_count++] = (FoodItem){506, "Tea", "Beverages", 40.00, 1};
    g_menu[g_menu_count++] = (FoodItem){507, "Fresh Juice", "Beverages", 90.00, 1};
    g_menu[g_menu_count++] = (FoodItem){508, "Mineral Water", "Beverages", 20.00, 1};

    /* 8. DESSERTS (601-605) */
    g_menu[g_menu_count++] = (FoodItem){601, "Ice Cream", "Desserts", 80.00, 1};
    g_menu[g_menu_count++] = (FoodItem){602, "Gulab Jamun", "Desserts", 90.00, 1};
    g_menu[g_menu_count++] = (FoodItem){603, "Brownie", "Desserts", 120.00, 1};
    g_menu[g_menu_count++] = (FoodItem){604, "Sizzling Brownie", "Desserts", 140.00, 1};
    g_menu[g_menu_count++] = (FoodItem){605, "Fruit Salad", "Desserts", 100.00, 1};

    /* Initialize Sample Tables (T01 to T10) */
    g_table_count = 10;
    int capacities[] = {2, 4, 4, 6, 2, 4, 6, 4, 2, 8};
    for (int i = 0; i < g_table_count; i++) {
        g_tables[i].id = i + 1;
        g_tables[i].capacity = capacities[i];
        g_tables[i].status = TABLE_AVAILABLE;
        g_tables[i].current_order_id = 0;
        g_tables[i].reserved_for[0] = '\0';
    }

    /* Set Table Statuses to Match Active Demo Data */
    g_tables[1].status = TABLE_OCCUPIED; g_tables[1].current_order_id = 1018; /* T02: Order #1018 */
    g_tables[3].status = TABLE_RESERVED; strncpy(g_tables[3].reserved_for, "Priya Sharma", 49); /* T04 */
    g_tables[4].status = TABLE_OCCUPIED; g_tables[4].current_order_id = 1019; /* T05: Order #1019 */
    g_tables[6].status = TABLE_OCCUPIED; g_tables[6].current_order_id = 1020; /* T07: Order #1020 */
    g_tables[8].status = TABLE_RESERVED; strncpy(g_tables[8].reserved_for, "Sneha Rao", 49); /* T09 */
    g_tables[9].status = TABLE_OCCUPIED; g_tables[9].current_order_id = 1017; /* T10: Order #1017 */

    /* Seed Historical & Active Orders (#1001 to #1020) */
    g_order_count = 0;
    g_next_order_id = INITIAL_ORDER_ID;

    /* Helper structure for order seeding */
    typedef struct {
        int table_id;
        const char *customer;
        OrderStatus status;
        int item_ids[5];
        int quantities[5];
        int item_cnt;
    } SeedOrder;

    SeedOrder seeds[] = {
        {0, "Walk-in Customer", STATUS_COMPLETED, {201, 352, 501, 0, 0}, {1, 2, 1, 0, 0}, 3},       /* #1001 */
        {2, "Rahul Kumar", STATUS_COMPLETED, {304, 501, 602, 0, 0}, {2, 2, 1, 0, 0}, 3},           /* #1002 */
        {0, "Sneha Rao", STATUS_COMPLETED, {301, 503, 601, 0, 0}, {1, 1, 1, 0, 0}, 3},             /* #1003 */
        {4, "Arjun Reddy", STATUS_COMPLETED, {306, 106, 502, 0, 0}, {2, 1, 2, 0, 0}, 3},           /* #1004 */
        {0, "Priya Sharma", STATUS_COMPLETED, {101, 403, 505, 0, 0}, {1, 1, 2, 0, 0}, 3},          /* #1005 */
        {6, "Karthik Kumar", STATUS_COMPLETED, {207, 353, 501, 0, 0}, {1, 3, 2, 0, 0}, 3},         /* #1006 */
        {0, "Anjali Singh", STATUS_COMPLETED, {204, 354, 503, 0, 0}, {1, 4, 1, 0, 0}, 3},          /* #1007 */
        {8, "Vikram Patel", STATUS_COMPLETED, {305, 107, 508, 0, 0}, {2, 1, 2, 0, 0}, 3},          /* #1008 */
        {0, "Sanjay Reddy", STATUS_COMPLETED, {102, 401, 406, 0, 0}, {1, 1, 1, 0, 0}, 3},          /* #1009 */
        {1, "Meena Rao", STATUS_COMPLETED, {152, 205, 351, 0, 0}, {2, 1, 3, 0, 0}, 3},             /* #1010 */
        {0, "Amit Kumar", STATUS_COMPLETED, {155, 206, 352, 0, 0}, {1, 1, 2, 0, 0}, 3},            /* #1011 */
        {3, "Walk-in Customer", STATUS_COMPLETED, {302, 504, 0, 0, 0}, {2, 2, 0, 0, 0}, 2},         /* #1012 */
        {0, "Rahul Kumar", STATUS_COMPLETED, {202, 353, 604, 0, 0}, {1, 2, 1, 0, 0}, 3},           /* #1013 */
        {5, "Sneha Rao", STATUS_COMPLETED, {104, 203, 354, 506}, {1, 1, 3, 2}, 4},                 /* #1014 */
        {0, "Arjun Reddy", STATUS_COMPLETED, {108, 404, 501, 0, 0}, {1, 1, 2, 0, 0}, 3},           /* #1015 */
        {7, "Priya Sharma", STATUS_COMPLETED, {151, 208, 352, 603}, {2, 1, 2, 1}, 4},               /* #1016 */

        /* ACTIVE ORDERS (#1017 - #1020) */
        {10, "Arjun Reddy", STATUS_SERVED, {306, 107, 503, 0, 0}, {2, 1, 2, 0, 0}, 3},            /* #1017 (T10) */
        {2, "Rahul Kumar", STATUS_PREPARING, {304, 501, 0, 0, 0}, {2, 2, 0, 0, 0}, 2},             /* #1018 (T02) */
        {5, "Anjali Singh", STATUS_READY, {103, 352, 0, 0, 0}, {1, 2, 0, 0, 0}, 2},                /* #1019 (T05) */
        {7, "Vikram Patel", STATUS_PENDING, {306, 503, 0, 0, 0}, {2, 2, 0, 0, 0}, 2}               /* #1020 (T07) */
    };

    for (size_t s = 0; s < sizeof(seeds) / sizeof(seeds[0]); s++) {
        Order ord;
        memset(&ord, 0, sizeof(Order));
        ord.order_id = g_next_order_id++;
        ord.table_id = seeds[s].table_id;
        strncpy(ord.customer_name, seeds[s].customer, sizeof(ord.customer_name) - 1);
        ord.status = seeds[s].status;
        ord.discount_rate = DISCOUNT_RATE;
        ord.tax_rate = TAX_RATE;

        /* Assign realistic timestamp */
        snprintf(ord.timestamp, sizeof(ord.timestamp), "2026-08-14 %02d:%02d:00",
                 12 + (int)(s / 2), (int)(s * 7) % 60);

        for (int k = 0; k < seeds[s].item_cnt; k++) {
            const FoodItem *food = findFoodItemById(seeds[s].item_ids[k]);
            if (food) {
                OrderItem oi;
                oi.item = *food;
                oi.quantity = seeds[s].quantities[k];
                oi.amount = food->price * oi.quantity;
                ord.items[ord.item_count++] = oi;
            }
        }
        calculateTotals(&ord);
        g_orders[g_order_count++] = ord;
    }
}

/* File Persistence Functions */
void saveData(void) {
    FILE *fp_menu = fopen("menu.dat", "wb");
    if (fp_menu) {
        fwrite(&g_menu_count, sizeof(int), 1, fp_menu);
        if (g_menu_count > 0) {
            fwrite(g_menu, sizeof(FoodItem), g_menu_count, fp_menu);
        }
        fclose(fp_menu);
    }

    FILE *fp_tables = fopen("tables.dat", "wb");
    if (fp_tables) {
        fwrite(&g_table_count, sizeof(int), 1, fp_tables);
        if (g_table_count > 0) {
            fwrite(g_tables, sizeof(Table), g_table_count, fp_tables);
        }
        fclose(fp_tables);
    }

    FILE *fp_orders = fopen("orders.dat", "wb");
    if (fp_orders) {
        fwrite(&g_order_count, sizeof(int), 1, fp_orders);
        fwrite(&g_next_order_id, sizeof(int), 1, fp_orders);
        if (g_order_count > 0) {
            fwrite(g_orders, sizeof(Order), g_order_count, fp_orders);
        }
        fclose(fp_orders);
    }
}

void loadData(void) {
    int loaded = 0;

    FILE *fp_menu = fopen("menu.dat", "rb");
    if (fp_menu) {
        if (fread(&g_menu_count, sizeof(int), 1, fp_menu) == 1 && g_menu_count > 0) {
            if (g_menu_count > MAX_MENU_ITEMS) g_menu_count = MAX_MENU_ITEMS;
            fread(g_menu, sizeof(FoodItem), g_menu_count, fp_menu);
            loaded = 1;
        }
        fclose(fp_menu);
    }

    FILE *fp_tables = fopen("tables.dat", "rb");
    if (fp_tables) {
        if (fread(&g_table_count, sizeof(int), 1, fp_tables) == 1 && g_table_count > 0) {
            if (g_table_count > MAX_TABLES) g_table_count = MAX_TABLES;
            fread(g_tables, sizeof(Table), g_table_count, fp_tables);
        }
        fclose(fp_tables);
    }

    FILE *fp_orders = fopen("orders.dat", "rb");
    if (fp_orders) {
        if (fread(&g_order_count, sizeof(int), 1, fp_orders) == 1) {
            fread(&g_next_order_id, sizeof(int), 1, fp_orders);
            if (g_order_count > MAX_ORDERS) g_order_count = MAX_ORDERS;
            if (g_order_count > 0) {
                fread(g_orders, sizeof(Order), g_order_count, fp_orders);
            }
        }
        fclose(fp_orders);
    }

    if (!loaded || g_menu_count < 20) {
        initRestaurantData();
        saveData();
    }
}

/* Menu API Implementation */
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

int addMenuItem(int id, const char *name, const char *category, double price, int available, char *err_msg, size_t err_size) {
    if (id <= 0) {
        if (err_msg) snprintf(err_msg, err_size, "Item ID must be a positive integer.");
        return 0;
    }
    if (name == NULL || strlen(name) == 0) {
        if (err_msg) snprintf(err_msg, err_size, "Item name cannot be empty.");
        return 0;
    }
    if (price < 0.0) {
        if (err_msg) snprintf(err_msg, err_size, "Price cannot be negative.");
        return 0;
    }
    if (findFoodItemById(id) != NULL) {
        if (err_msg) snprintf(err_msg, err_size, "Item ID %d already exists.", id);
        return 0;
    }
    if (g_menu_count >= MAX_MENU_ITEMS) {
        if (err_msg) snprintf(err_msg, err_size, "Menu capacity reached (Max %d items).", MAX_MENU_ITEMS);
        return 0;
    }

    FoodItem item;
    item.id = id;
    strncpy(item.name, name, sizeof(item.name) - 1);
    item.name[sizeof(item.name) - 1] = '\0';
    strncpy(item.category, category ? category : "General", sizeof(item.category) - 1);
    item.category[sizeof(item.category) - 1] = '\0';
    item.price = price;
    item.available = available ? 1 : 0;

    g_menu[g_menu_count++] = item;
    saveData();
    return 1;
}

int updateMenuItem(int id, const char *name, const char *category, double price, int available, char *err_msg, size_t err_size) {
    FoodItem *item = NULL;
    for (int i = 0; i < g_menu_count; i++) {
        if (g_menu[i].id == id) {
            item = &g_menu[i];
            break;
        }
    }

    if (item == NULL) {
        if (err_msg) snprintf(err_msg, err_size, "Item ID %d not found.", id);
        return 0;
    }
    if (name == NULL || strlen(name) == 0) {
        if (err_msg) snprintf(err_msg, err_size, "Item name cannot be empty.");
        return 0;
    }
    if (price < 0.0) {
        if (err_msg) snprintf(err_msg, err_size, "Price cannot be negative.");
        return 0;
    }

    strncpy(item->name, name, sizeof(item->name) - 1);
    item->name[sizeof(item->name) - 1] = '\0';
    strncpy(item->category, category ? category : "General", sizeof(item->category) - 1);
    item->category[sizeof(item->category) - 1] = '\0';
    item->price = price;
    item->available = available ? 1 : 0;

    saveData();
    return 1;
}

int deleteMenuItem(int id, char *err_msg, size_t err_size) {
    int index = -1;
    for (int i = 0; i < g_menu_count; i++) {
        if (g_menu[i].id == id) {
            index = i;
            break;
        }
    }

    if (index == -1) {
        if (err_msg) snprintf(err_msg, err_size, "Item ID %d not found.", id);
        return 0;
    }

    for (int i = index; i < g_menu_count - 1; i++) {
        g_menu[i] = g_menu[i + 1];
    }
    g_menu_count--;

    saveData();
    return 1;
}

int toggleItemAvailability(int id) {
    for (int i = 0; i < g_menu_count; i++) {
        if (g_menu[i].id == id) {
            g_menu[i].available = !g_menu[i].available;
            saveData();
            return 1;
        }
    }
    return 0;
}

/* Table API Implementation */
int getTableCount(void) {
    return g_table_count;
}

Table* getTable(int index) {
    if (index >= 0 && index < g_table_count) {
        return &g_tables[index];
    }
    return NULL;
}

Table* findTableById(int id) {
    for (int i = 0; i < g_table_count; i++) {
        if (g_tables[i].id == id) {
            return &g_tables[i];
        }
    }
    return NULL;
}

int assignTable(int table_id, int order_id, char *err_msg, size_t err_size) {
    Table *tbl = findTableById(table_id);
    if (tbl == NULL) {
        if (err_msg) snprintf(err_msg, err_size, "Table T%02d not found.", table_id);
        return 0;
    }
    if (tbl->status == TABLE_OCCUPIED && tbl->current_order_id != order_id && tbl->current_order_id != 0) {
        if (err_msg) snprintf(err_msg, err_size, "Table T%02d is already occupied by Order #%d.", table_id, tbl->current_order_id);
        return 0;
    }

    tbl->status = TABLE_OCCUPIED;
    tbl->current_order_id = order_id;
    tbl->reserved_for[0] = '\0';
    saveData();
    return 1;
}

int reserveTable(int table_id, const char *customer_name, char *err_msg, size_t err_size) {
    Table *tbl = findTableById(table_id);
    if (tbl == NULL) {
        if (err_msg) snprintf(err_msg, err_size, "Table T%02d not found.", table_id);
        return 0;
    }
    if (tbl->status == TABLE_OCCUPIED) {
        if (err_msg) snprintf(err_msg, err_size, "Cannot reserve table T%02d because it is currently occupied.", table_id);
        return 0;
    }

    tbl->status = TABLE_RESERVED;
    if (customer_name && strlen(customer_name) > 0) {
        strncpy(tbl->reserved_for, customer_name, sizeof(tbl->reserved_for) - 1);
        tbl->reserved_for[sizeof(tbl->reserved_for) - 1] = '\0';
    } else {
        strcpy(tbl->reserved_for, "Reserved Guest");
    }
    saveData();
    return 1;
}

int releaseTable(int table_id, char *err_msg, size_t err_size) {
    Table *tbl = findTableById(table_id);
    if (tbl == NULL) {
        if (err_msg) snprintf(err_msg, err_size, "Table T%02d not found.", table_id);
        return 0;
    }

    tbl->status = TABLE_AVAILABLE;
    tbl->current_order_id = 0;
    tbl->reserved_for[0] = '\0';
    saveData();
    return 1;
}

/* Helper Timestamps & Formatters */
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

const char* getOrderStatusString(OrderStatus status) {
    switch (status) {
        case STATUS_PENDING:   return "PENDING";
        case STATUS_PREPARING: return "PREPARING";
        case STATUS_READY:     return "READY";
        case STATUS_SERVED:    return "SERVED";
        case STATUS_COMPLETED: return "COMPLETED";
        case STATUS_CANCELLED: return "CANCELLED";
        default:               return "UNKNOWN";
    }
}

const char* getTableStatusString(TableStatus status) {
    switch (status) {
        case TABLE_AVAILABLE: return "AVAILABLE";
        case TABLE_OCCUPIED:  return "OCCUPIED";
        case TABLE_RESERVED:  return "RESERVED";
        default:              return "UNKNOWN";
    }
}

/* Order Totals Math - Preserving Exact Formula */
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

/* Order API Implementation */
int getOrderCount(void) {
    return g_order_count;
}

Order* getOrder(int index) {
    if (index >= 0 && index < g_order_count) {
        return &g_orders[index];
    }
    return NULL;
}

Order* findOrderById(int order_id) {
    for (int i = 0; i < g_order_count; i++) {
        if (g_orders[i].order_id == order_id) {
            return &g_orders[i];
        }
    }
    return NULL;
}

int createOrder(int table_id, const char *customer_name, char *err_msg, size_t err_size) {
    if (g_order_count >= MAX_ORDERS) {
        if (err_msg) snprintf(err_msg, err_size, "Maximum order capacity reached (%d).", MAX_ORDERS);
        return 0;
    }

    if (table_id > 0) {
        Table *tbl = findTableById(table_id);
        if (tbl == NULL) {
            if (err_msg) snprintf(err_msg, err_size, "Table T%02d does not exist.", table_id);
            return 0;
        }
        if (tbl->status == TABLE_OCCUPIED) {
            if (err_msg) snprintf(err_msg, err_size, "Table T%02d is already occupied by Order #%d.", table_id, tbl->current_order_id);
            return 0;
        }
    }

    Order newOrder;
    memset(&newOrder, 0, sizeof(Order));
    newOrder.order_id = g_next_order_id++;
    newOrder.table_id = table_id;

    if (customer_name && strlen(customer_name) > 0) {
        strncpy(newOrder.customer_name, customer_name, sizeof(newOrder.customer_name) - 1);
        newOrder.customer_name[sizeof(newOrder.customer_name) - 1] = '\0';
    } else {
        strcpy(newOrder.customer_name, "Walk-in Customer");
    }

    newOrder.item_count = 0;
    newOrder.status = STATUS_PENDING;
    newOrder.discount_rate = DISCOUNT_RATE;
    newOrder.tax_rate = TAX_RATE;
    getCurrentTimestamp(newOrder.timestamp, sizeof(newOrder.timestamp));
    calculateTotals(&newOrder);

    if (table_id > 0) {
        assignTable(table_id, newOrder.order_id, NULL, 0);
    }

    g_orders[g_order_count++] = newOrder;
    saveData();
    return newOrder.order_id;
}

int addItemToOrder(Order *order, int item_id, int quantity, char *err_msg, size_t err_size) {
    if (order == NULL) {
        if (err_msg) snprintf(err_msg, err_size, "No active order specified.");
        return 0;
    }

    const FoodItem *food = findFoodItemById(item_id);
    if (food == NULL) {
        if (err_msg) snprintf(err_msg, err_size, "Invalid Item ID (%d).", item_id);
        return 0;
    }
    if (!food->available) {
        if (err_msg) snprintf(err_msg, err_size, "Item '%s' is currently unavailable.", food->name);
        return 0;
    }

    if (quantity <= 0) {
        if (err_msg) snprintf(err_msg, err_size, "Quantity must be greater than 0.");
        return 0;
    }

    /* Merge if item already present */
    for (int i = 0; i < order->item_count; i++) {
        if (order->items[i].item.id == item_id) {
            order->items[i].quantity += quantity;
            order->items[i].amount = order->items[i].quantity * order->items[i].item.price;
            calculateTotals(order);
            saveData();
            return 1;
        }
    }

    if (order->item_count >= MAX_ORDER_ITEMS) {
        if (err_msg) snprintf(err_msg, err_size, "Max item capacity reached for this order.");
        return 0;
    }

    OrderItem newItem;
    newItem.item = *food;
    newItem.quantity = quantity;
    newItem.amount = food->price * quantity;

    order->items[order->item_count++] = newItem;
    calculateTotals(order);
    saveData();
    return 1;
}

int updateItemQuantity(Order *order, int item_id, int new_quantity, char *err_msg, size_t err_size) {
    if (order == NULL) return 0;
    if (new_quantity <= 0) {
        return removeItemFromOrder(order, item_id, err_msg, err_size);
    }

    for (int i = 0; i < order->item_count; i++) {
        if (order->items[i].item.id == item_id) {
            order->items[i].quantity = new_quantity;
            order->items[i].amount = order->items[i].quantity * order->items[i].item.price;
            calculateTotals(order);
            saveData();
            return 1;
        }
    }

    if (err_msg) snprintf(err_msg, err_size, "Item ID %d not found in order.", item_id);
    return 0;
}

int removeItemFromOrder(Order *order, int item_id, char *err_msg, size_t err_size) {
    if (order == NULL) return 0;
    int index = -1;
    for (int i = 0; i < order->item_count; i++) {
        if (order->items[i].item.id == item_id) {
            index = i;
            break;
        }
    }

    if (index == -1) {
        if (err_msg) snprintf(err_msg, err_size, "Item ID %d not in order.", item_id);
        return 0;
    }

    for (int i = index; i < order->item_count - 1; i++) {
        order->items[i] = order->items[i + 1];
    }
    order->item_count--;

    calculateTotals(order);
    saveData();
    return 1;
}

int updateOrderStatus(int order_id, OrderStatus status, char *err_msg, size_t err_size) {
    Order *ord = findOrderById(order_id);
    if (ord == NULL) {
        if (err_msg) snprintf(err_msg, err_size, "Order #%d not found.", order_id);
        return 0;
    }

    ord->status = status;

    /* Free table automatically if order is completed or cancelled */
    if ((status == STATUS_COMPLETED || status == STATUS_CANCELLED) && ord->table_id > 0) {
        releaseTable(ord->table_id, NULL, 0);
    }

    saveData();
    return 1;
}

/* Dashboard Statistics & Analytics Calculation */
void getDashboardStats(DashboardStats *stats) {
    memset(stats, 0, sizeof(DashboardStats));

    stats->total_orders = g_order_count;
    stats->today_revenue = 0.0;
    stats->avg_order_value = 0.0;
    stats->occupancy_rate = 0.0;

    for (int i = 0; i < g_order_count; i++) {
        switch (g_orders[i].status) {
            case STATUS_PENDING:
                stats->pending_orders++;
                stats->active_orders++;
                break;
            case STATUS_PREPARING:
                stats->preparing_orders++;
                stats->active_orders++;
                break;
            case STATUS_READY:
                stats->ready_orders++;
                stats->active_orders++;
                break;
            case STATUS_SERVED:
                stats->served_orders++;
                stats->active_orders++;
                break;
            case STATUS_COMPLETED:
                stats->completed_orders++;
                stats->today_revenue += g_orders[i].grand_total;
                break;
            case STATUS_CANCELLED:
                stats->cancelled_orders++;
                break;
        }
    }

    if (stats->completed_orders > 0) {
        stats->avg_order_value = stats->today_revenue / stats->completed_orders;
    }

    for (int i = 0; i < g_table_count; i++) {
        switch (g_tables[i].status) {
            case TABLE_AVAILABLE: stats->available_tables++; break;
            case TABLE_OCCUPIED:  stats->occupied_tables++;  break;
            case TABLE_RESERVED:  stats->reserved_tables++;  break;
        }
    }

    if (g_table_count > 0) {
        stats->occupancy_rate = (stats->occupied_tables * 100.0) / g_table_count;
    }
}

int getTopSellingItems(TopItemStat *top_items, int max_items) {
    if (max_items <= 0) return 0;

    TopItemStat list[MAX_MENU_ITEMS];
    int count = 0;

    for (int i = 0; i < g_menu_count; i++) {
        list[count].item_id = g_menu[i].id;
        strncpy(list[count].item_name, g_menu[i].name, sizeof(list[count].item_name) - 1);
        list[count].item_name[sizeof(list[count].item_name) - 1] = '\0';
        list[count].total_quantity_sold = 0;
        list[count].total_revenue_generated = 0.0;

        for (int o = 0; o < g_order_count; o++) {
            /* Include completed and active non-cancelled orders */
            if (g_orders[o].status != STATUS_CANCELLED) {
                for (int it = 0; it < g_orders[o].item_count; it++) {
                    if (g_orders[o].items[it].item.id == g_menu[i].id) {
                        list[count].total_quantity_sold += g_orders[o].items[it].quantity;
                        list[count].total_revenue_generated += g_orders[o].items[it].amount;
                    }
                }
            }
        }
        count++;
    }

    /* Sort list descending by total_quantity_sold */
    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (list[j].total_quantity_sold > list[i].total_quantity_sold) {
                TopItemStat temp = list[i];
                list[i] = list[j];
                list[j] = temp;
            }
        }
    }

    int result_count = (count < max_items) ? count : max_items;
    for (int i = 0; i < result_count; i++) {
        top_items[i] = list[i];
    }

    return result_count;
}
