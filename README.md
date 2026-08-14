# Spice Bavarchi - Restaurant Management System (C99 + GTK3)

A native, feature-complete **Desktop Restaurant Management System** built in C using **GTK3**.

**Tagline**: *"Great food. Great moments."*

---

## 1. Modules Overview

1. **Dashboard**: Executive metrics overview showing Total Orders, Active Orders, Available/Occupied/Reserved Tables, Occupancy Rate %, Today's Revenue Realized, Average Order Value, and Quick Actions shortcuts.
2. **Menu Management**: Manage 51+ menu items across 8 categories (Starters, Soups, Main Course, Biryani, Indian Breads, Chinese, Beverages, Desserts) with live search, category filtering, Add/Edit modals, availability toggling, and deletion.
3. **Table Management**: Interactive seating floor map for 10 tables (T01–T10) featuring status badges (`AVAILABLE` - Green, `OCCUPIED` - Red, `RESERVED` - Amber), current order details, customer names, and contextual actions (`Assign Table`, `View Order`, `Reserve`, `Release`).
4. **Order Management**: Multi-order processing supporting active orders, order history (#1001–#1020), customer names, item additions, quantity controls (`+`/`-`), and status progression (`PENDING` $\rightarrow$ `PREPARING` $\rightarrow$ `READY` $\rightarrow$ `SERVED` $\rightarrow$ `COMPLETED`).
5. **Kitchen Display System (KDS)**: Kitchen screen displaying active orders with item breakdowns and status progression controls (`Preparing`, `Ready`, `Served`).
6. **Billing & Receipts**: Cashier terminal calculating Subtotal, 5% Discount, 5% GST, and Grand Total with 2-decimal accuracy, printable receipt popup, receipt file export (`receipt_#1001.txt`), and payment completion (auto-releasing occupied tables).
7. **Reports & Analytics**: Sales performance reports, table occupancy rates, and top-selling items ranking.

---

## 2. Seeded Realistic Demo Dataset

- **51 Menu Items** across 8 Indian restaurant categories.
- **10 Tables**: 4 Available (T01, T03, T06, T08), 4 Occupied (T02 - Rahul Kumar, T05 - Anjali Singh, T07 - Vikram Patel, T10 - Arjun Reddy), 2 Reserved (T04 - Priya Sharma, T09 - Sneha Rao).
- **20 Orders (#1001–#1020)**: 4 Active Orders, 16 Completed Historical Orders.
- **Calculated Metrics**: Realized Revenue of **₹8,059.83** and Average Order Value of **₹503.74**.

---

## 3. Compilation & Launch

### Compile Command
```bash
gcc -Wall -Wextra -std=c99 main.c restaurant.c gui.c -o restaurant $(pkg-config --cflags --libs gtk+-3.0) -lm
```

### Run Command
```bash
./restaurant
```

---

## 4. Demonstration Guide for Viva / Presentation

1. **Dashboard**: Show live metric cards (Revenue: ₹8,059.83, Avg Order: ₹503.74, Occupancy: 40.0%).
2. **Tables**: Open `Tables`. Switch between tabs (`Dashboard` $\rightarrow$ `Tables` $\rightarrow$ `Dashboard`) and resize the window to demonstrate that the page never clips or becomes blank.
3. **Menu**: Search for `"Biryani"` in the search bar or filter by category.
4. **Kitchen & Orders**: Update order status from `PREPARING` to `READY` to `SERVED`.
5. **Billing**: Select Order #1018, click `🧾 View Printable Receipt` to show invoice math, then click `✅ Complete Payment & Clear Table` to release Table T02.
