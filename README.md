# Spice Bavarchi - Restaurant Management System (GTK Desktop GUI in C)

A modern, native **Desktop GUI Restaurant Management and Billing System** built in C using **GTK 4**. Designed for college project demonstrations and viva presentations, this application transforms core restaurant billing business logic into a real-world desktop application window with a polished graphical interface.

---

## 1. Project Overview

The **Spice Bavarchi Restaurant Management System** simulates point-of-sale (POS) billing operations used inside a restaurant. The system replaces terminal console prompts with a native desktop window ($1250 \times 780$ pixels) that allows cashiers to:
- Browse a 17-item restaurant menu organized across 5 categories (Starters, Main Course, Biryani, Beverages, Desserts) with item prices in Indian Rupees (`Rs.`).
- Add items to an active customer order with a single click (`+ Add`).
- Automatically merge duplicate menu items and increase quantities in real-time.
- Adjust item quantities dynamically using interactive quantity controls (`[-]` `Qty` `[+]`) or remove items (`✕`).
- Dynamically calculate Subtotal, Discount (5.0%), Net Taxable Amount, GST (5.0%), and Grand Total with cent-level 2-decimal rounding.
- Display a professional, formatted tax invoice receipt inside a native GTK dialog window.
- Reset orders and start new customer transactions with auto-incrementing Order IDs (`#1001`, `#1002`, ...).

---

## 2. Key Features

- **Native GTK Desktop Interface**: Runs in a standalone GTK 4 desktop application window without requiring terminal interaction during normal usage.
- **Categorized Menu Notebook**: Organized tabbed menu (`GtkNotebook`) for Starters, Main Course, Biryani, Beverages, and Desserts.
- **Real-Time Order List**: Dynamic order view featuring item names, unit prices, quantity spin controls, item amounts, and item removal buttons.
- **Automatic Duplicate Item Merging**: Clicking `+ Add` on an existing item increments its quantity instead of creating duplicate rows.
- **Live Billing Calculations**: Automatically recalculates Subtotal, 5% Discount, Taxable Amount, 5% GST Tax, and Grand Total whenever the order changes.
- **GUI Receipt Generation**: Renders a tax invoice receipt window complete with restaurant branding, Order ID, timestamp, and itemized breakdown.
- **GUI Validation & Alert Dialogs**: Displays native GTK alert dialogs for empty order billing attempts and confirmation prompts when starting a new order.
- **Custom GTK CSS Styling**: Polished dark slate blue header banner (`#0f172a`), emerald price tags (`#059669`), vibrant primary action buttons (`#16a34a`), and slate red removal buttons (`#ef4444`).

---

## 3. Technologies Used

- **Programming Language**: C (C11 / C99 Standard)
- **GUI Framework**: GTK 4 (`libgtk-4`)
- **Build Utilities**: GCC (`gcc`), `pkg-config`
- **Libraries**: Standard C runtime (`stdio.h`, `stdlib.h`, `string.h`, `time.h`, `math.h`), GLib, GDK, Pango

---

## 4. C & GTK Concepts Demonstrated

- **Structures (`struct`)**: Encapsulates `FoodItem`, `OrderItem`, and `Order` composite data types.
- **Modular Code Architecture**: Clear separation of data models (`restaurant.h`/`restaurant.c`), GUI builder & event handlers (`gui.h`/`gui.c`), and startup (`main.c`).
- **Pointers & Pass-by-Reference**: Passing pointers (`Order *`, `GtkWidget *`) efficiently between callbacks and business logic.
- **Event-Driven Architecture**: GTK signal handling (`g_signal_connect`) for button clicks, window signals, and UI state refresh.
- **GTK Widgets & Containers**: `GtkApplicationWindow`, `GtkBox`, `GtkNotebook`, `GtkScrolledWindow`, `GtkLabel`, `GtkButton`, `GtkTextView`, `GtkSeparator`.
- **GTK CSS Styling (`GtkCssProvider`)**: Custom application theme injection via CSS string providers.
- **Math & Currency Handling**: Cent-accurate 2-decimal place currency rounding using `round()` from `<math.h>`.

---

## 5. Project Structure

```text
Restaurant_billing_System/
├── restaurant.h      # Data structures, constants, & business logic declarations
├── restaurant.c      # Menu dataset, order math, & order state management
├── gui.h             # GTK GUI launcher interface declarations
├── gui.c             # GTK 4 layout construction, CSS styles, & signal callbacks
├── main.c            # Application entry point (gtk_application_run)
├── README.md         # Comprehensive project documentation & viva guide
└── .gitignore        # Git ignore rules for compiled binary output
```

---

## 6. Dependencies & Installation Instructions

### Prerequisites for macOS (Apple Silicon & Intel)
The application requires `pkg-config` and `gtk4`.

Install dependencies via Homebrew:
```bash
brew install pkg-config gtk4
```

---

## 7. How to Compile

Use `pkg-config` to automatically pull GTK 4 compiler flags and library paths:

```bash
gcc -Wall -Wextra -std=c11 main.c restaurant.c gui.c $(pkg-config --cflags --libs gtk4) -lm -o restaurant
```

*For Apple Silicon Macs using Homebrew at `/opt/homebrew`:*
```bash
gcc -Wall -Wextra -std=c11 main.c restaurant.c gui.c $(/opt/homebrew/bin/pkg-config --cflags --libs gtk4) -lm -o restaurant
```

---

## 8. How to Run

Launch the compiled desktop GUI application:

```bash
./restaurant
```

---

## 9. Billing Calculation Formula

1. **Item Amount**:
   $$\text{Item Amount} = \text{Item Price} \times \text{Quantity}$$

2. **Subtotal**:
   $$\text{Subtotal} = \sum \text{Item Amount}$$

3. **Discount (5.0%)**:
   $$\text{Discount} = \text{round}(\text{Subtotal} \times 0.05)$$

4. **Taxable Amount**:
   $$\text{Taxable Amount} = \text{Subtotal} - \text{Discount}$$

5. **GST Tax (5.0%)**:
   $$\text{GST Tax} = \text{round}(\text{Taxable Amount} \times 0.05)$$

6. **Grand Total**:
   $$\text{Grand Total} = \text{Subtotal} - \text{Discount} + \text{GST Tax}$$

---

## 10. Sample Receipt Output (GUI Dialog Window)

```text
============================================================
                    SPICE BAVARCHI RESTAURANT
                   FINAL TAX INVOICE / RECEIPT
============================================================
  Order ID : #1001       Date/Time: 2026-08-14 10:07:00
------------------------------------------------------------
  Item Description         Qty   Rate      Amount
------------------------------------------------------------
  Chicken Biryani          2     Rs. 220.00 Rs. 440.00
  Coke                     2     Rs. 50.00  Rs. 100.00
  Gulab Jamun              1     Rs. 90.00  Rs. 90.00
------------------------------------------------------------
  Subtotal                                  Rs.   630.00
  Discount ( 5.0%)                           -Rs.    31.50
  Taxable Amount                            Rs.   598.50
  GST ( 5.0%)                                +Rs.    29.93
------------------------------------------------------------
  GRAND TOTAL                               Rs.   628.43
============================================================
              Thank You! Please Visit Again!
============================================================
```

---

## 11. Testing & Verification Performed

- **TEST 1 (App Launch)**: Window opens cleanly titled `"Spice Bavarchi - Restaurant Management System"`.
- **TEST 2 (Menu Navigation)**: Category tabs (`Starters`, `Main Course`, `Biryani`, `Beverages`, `Desserts`) switch smoothly displaying all 17 items.
- **TEST 3 (Item Addition)**: Added Chicken Biryani ($\times 2$), Coke ($\times 2$), Gulab Jamun ($\times 1$).
- **TEST 4 (Duplicate Item Merging)**: Clicking `+ Add` on an existing item increases quantity from 2 to 3.
- **TEST 5 (Quantity Controls)**: Clicking `[-]` decrements quantity; clicking `[+]` increments quantity.
- **TEST 6 (Item Removal)**: Clicking `✕` removes item, shifts array elements, and updates totals immediately.
- **TEST 7 (Empty Order Billing Warning)**: Clicking `GENERATE BILL` on an empty order triggers an alert dialog.
- **TEST 8 (Bill Generation)**: Tax invoice receipt window displays formatted itemization, Subtotal (`Rs. 630.00`), Discount (`Rs. 31.50`), Taxable Amount (`Rs. 598.50`), GST (`Rs. 29.93`), and Grand Total (`Rs. 628.43`).
- **TEST 9 (New Order Reset)**: Clicking `NEW ORDER` prompts confirmation, clears order, and advances Order ID badge to `#1002`.

---

## 12. Project Limitations

- **No External Database**: Data is managed in memory for standalone runtime performance.
- **Single Cashier Session**: No multi-tenant networking or cloud storage.
- **Cashier POS Interface**: Designed strictly for counter-billing simulation.

---

## 13. Student Viva Explanation Guide

- **Q1: How does the GTK GUI communicate with the underlying C business logic?**
  - *Answer*: GTK widgets emit signals (e.g. `"clicked"`). Signal handler callbacks in `gui.c` invoke C business logic functions in `restaurant.c` (such as `addItemToOrder` or `calculateTotals`). After updating the global `Order` struct, `refresh_order_ui()` rebuilds the GTK order list and updates label widgets.
- **Q2: Why split the code into `restaurant.c`, `gui.c`, and `main.c`?**
  - *Answer*: This follows the **Model-View-Controller (MVC)** architectural pattern. `restaurant.c` serves as the data model, `gui.c` serves as the view and controller, and `main.c` serves as the application launcher. This keeps the code modular, readable, and easy to maintain.
- **Q3: How are GTK CSS styles applied in C?**
  - *Answer*: We create a `GtkCssProvider` in C, load a CSS style string containing rules for `.header-banner`, `.menu-card`, `.btn-primary`, etc., and add it to the default display provider via `gtk_style_context_add_provider_for_display()`.
- **Q4: How does duplicate item detection work in GTK?**
  - *Answer*: When `+ Add` is clicked, `addItemToOrder()` loops through `g_order.items[]` to check if `item.id` matches. If found, it increments `quantity` and updates `amount`; if not, it appends a new `OrderItem`.
- **Q5: How is rounding handled for financial accuracy?**
  - *Answer*: We use `round(val * 100.0) / 100.0` from `<math.h>` to round currency calculations cent-accurately to 2 decimal places.
