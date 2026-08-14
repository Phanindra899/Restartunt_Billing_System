#include "gui.h"

/* Global Order State */
static Order g_order;
static int g_current_order_id = INITIAL_ORDER_ID;

/* GTK Widgets for Live UI Updates */
static GtkWidget *g_main_window = NULL;
static GtkWidget *g_lbl_order_id = NULL;
static GtkWidget *g_box_order_items = NULL;
static GtkWidget *g_lbl_subtotal = NULL;
static GtkWidget *g_lbl_discount = NULL;
static GtkWidget *g_lbl_taxable = NULL;
static GtkWidget *g_lbl_gst = NULL;
static GtkWidget *g_lbl_grand_total = NULL;

/* Forward Declarations */
static void refresh_order_ui(void);
static void apply_custom_css(void);
static void show_info_alert(const char *title, const char *message);

/* ============================================================================
 * HELPER DIALOG ROUTINES (GTK 4.10+ Compliant)
 * ============================================================================
 */

static void show_info_alert(const char *title, const char *message) {
    GtkWidget *dialog = gtk_window_new();
    gtk_window_set_title(GTK_WINDOW(dialog), title);
    gtk_window_set_transient_for(GTK_WINDOW(dialog), GTK_WINDOW(g_main_window));
    gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
    gtk_window_set_default_size(GTK_WINDOW(dialog), 380, 180);

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 14);
    gtk_widget_set_margin_start(box, 20);
    gtk_widget_set_margin_end(box, 20);
    gtk_widget_set_margin_top(box, 20);
    gtk_widget_set_margin_bottom(box, 20);

    GtkWidget *lbl = gtk_label_new(message);
    gtk_label_set_wrap(GTK_LABEL(lbl), TRUE);
    gtk_box_append(GTK_BOX(box), lbl);

    GtkWidget *btn_ok = gtk_button_new_with_label("OK");
    gtk_widget_add_css_class(btn_ok, "btn-primary");
    g_signal_connect_swapped(btn_ok, "clicked", G_CALLBACK(gtk_window_destroy), dialog);
    gtk_box_append(GTK_BOX(box), btn_ok);

    gtk_window_set_child(GTK_WINDOW(dialog), box);
    gtk_window_present(GTK_WINDOW(dialog));
}

/* ============================================================================
 * EVENT CALLBACKS
 * ============================================================================
 */

/* Callback when user clicks [ + Add ] button on a menu item */
static void on_add_menu_item_clicked(GtkButton *btn, gpointer user_data) {
    (void)btn;
    int item_id = GPOINTER_TO_INT(user_data);
    char err_msg[100];

    if (!addItemToOrder(&g_order, item_id, 1, err_msg, sizeof(err_msg))) {
        show_info_alert("Error Adding Item", err_msg);
        return;
    }

    refresh_order_ui();
}

/* Callback when user clicks [+] in order list */
static void on_qty_increment_clicked(GtkButton *btn, gpointer user_data) {
    (void)btn;
    int item_id = GPOINTER_TO_INT(user_data);
    addItemToOrder(&g_order, item_id, 1, NULL, 0);
    refresh_order_ui();
}

/* Callback when user clicks [-] in order list */
static void on_qty_decrement_clicked(GtkButton *btn, gpointer user_data) {
    (void)btn;
    int item_id = GPOINTER_TO_INT(user_data);

    /* Find current item quantity */
    for (int i = 0; i < g_order.item_count; i++) {
        if (g_order.items[i].item.id == item_id) {
            int current_qty = g_order.items[i].quantity;
            if (current_qty <= 1) {
                removeItemFromOrder(&g_order, item_id, NULL, 0);
            } else {
                updateItemQuantity(&g_order, item_id, current_qty - 1, NULL, 0);
            }
            break;
        }
    }
    refresh_order_ui();
}

/* Callback when user clicks [ Remove ] button */
static void on_remove_item_clicked(GtkButton *btn, gpointer user_data) {
    (void)btn;
    int item_id = GPOINTER_TO_INT(user_data);
    removeItemFromOrder(&g_order, item_id, NULL, 0);
    refresh_order_ui();
}

/* Modal Window for Bill Receipt */
static void show_receipt_dialog(void) {
    if (strlen(g_order.timestamp) == 0) {
        getCurrentTimestamp(g_order.timestamp, sizeof(g_order.timestamp));
    }
    calculateTotals(&g_order);

    GtkWidget *dialog = gtk_window_new();
    gtk_window_set_title(GTK_WINDOW(dialog), "Tax Invoice Receipt");
    gtk_window_set_transient_for(GTK_WINDOW(dialog), GTK_WINDOW(g_main_window));
    gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
    gtk_window_set_default_size(GTK_WINDOW(dialog), 520, 680);

    GtkWidget *content_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 15);
    gtk_widget_set_margin_start(content_box, 20);
    gtk_widget_set_margin_end(content_box, 20);
    gtk_widget_set_margin_top(content_box, 20);
    gtk_widget_set_margin_bottom(content_box, 20);

    /* Receipt Buffer Formatting */
    char receipt_buf[4096];
    size_t offset = 0;

    offset += snprintf(receipt_buf + offset, sizeof(receipt_buf) - offset,
        "============================================================\n"
        "                    SPICE BAVARCHI RESTAURANT\n"
        "                   FINAL TAX INVOICE / RECEIPT\n"
        "============================================================\n"
        "  Order ID : #%-10d Date/Time: %s\n"
        "------------------------------------------------------------\n"
        "  %-24s %-5s %-9s %-10s\n"
        "------------------------------------------------------------\n",
        g_order.order_id, g_order.timestamp, "Item Description", "Qty", "Rate", "Amount");

    for (int i = 0; i < g_order.item_count; i++) {
        offset += snprintf(receipt_buf + offset, sizeof(receipt_buf) - offset,
            "  %-24s %-5d Rs. %-6.2f Rs. %-7.2f\n",
            g_order.items[i].item.name,
            g_order.items[i].quantity,
            g_order.items[i].item.price,
            g_order.items[i].amount);
    }

    double taxable_amount = g_order.subtotal - g_order.discount_amount;

    snprintf(receipt_buf + offset, sizeof(receipt_buf) - offset,
        "------------------------------------------------------------\n"
        "  Subtotal                                  Rs. %8.2f\n"
        "  Discount ( 5.0%%)                           -Rs. %8.2f\n"
        "  Taxable Amount                            Rs. %8.2f\n"
        "  GST ( 5.0%%)                                +Rs. %8.2f\n"
        "------------------------------------------------------------\n"
        "  GRAND TOTAL                               Rs. %8.2f\n"
        "============================================================\n"
        "              Thank You! Please Visit Again!\n"
        "============================================================\n",
        g_order.subtotal, g_order.discount_amount, taxable_amount,
        g_order.tax_amount, g_order.grand_total);

    /* Text View with monospace font */
    GtkWidget *scrolled = gtk_scrolled_window_new();
    gtk_widget_set_vexpand(scrolled, TRUE);

    GtkWidget *text_view = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(text_view), FALSE);
    gtk_text_view_set_monospace(GTK_TEXT_VIEW(text_view), TRUE);

    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
    gtk_text_buffer_set_text(buffer, receipt_buf, -1);

    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled), text_view);
    gtk_box_append(GTK_BOX(content_box), scrolled);

    /* Close Button */
    GtkWidget *btn_close = gtk_button_new_with_label("Close Receipt");
    gtk_widget_add_css_class(btn_close, "btn-primary");
    g_signal_connect_swapped(btn_close, "clicked", G_CALLBACK(gtk_window_destroy), dialog);
    gtk_box_append(GTK_BOX(content_box), btn_close);

    gtk_window_set_child(GTK_WINDOW(dialog), content_box);
    gtk_window_present(GTK_WINDOW(dialog));
}

/* Callback for Generate Bill */
static void on_generate_bill_clicked(GtkButton *btn, gpointer user_data) {
    (void)btn;
    (void)user_data;

    if (g_order.item_count == 0) {
        show_info_alert("Empty Order Warning",
                         "Your order is empty.\nPlease select items from the menu before generating a bill.");
        return;
    }

    show_receipt_dialog();
}

static void on_confirm_new_order_yes(GtkButton *btn, gpointer user_data) {
    (void)btn;
    GtkWidget *dialog = GTK_WIDGET(user_data);
    startNewOrder(&g_order, &g_current_order_id);
    refresh_order_ui();
    gtk_window_destroy(GTK_WINDOW(dialog));
}

/* Callback for New Order */
static void on_new_order_clicked(GtkButton *btn, gpointer user_data) {
    (void)btn;
    (void)user_data;

    if (g_order.item_count > 0) {
        GtkWidget *dialog = gtk_window_new();
        gtk_window_set_title(GTK_WINDOW(dialog), "New Order Confirmation");
        gtk_window_set_transient_for(GTK_WINDOW(dialog), GTK_WINDOW(g_main_window));
        gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
        gtk_window_set_default_size(GTK_WINDOW(dialog), 380, 180);

        GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 14);
        gtk_widget_set_margin_start(box, 20);
        gtk_widget_set_margin_end(box, 20);
        gtk_widget_set_margin_top(box, 20);
        gtk_widget_set_margin_bottom(box, 20);

        char buf[128];
        snprintf(buf, sizeof(buf), "Start a new order?\nCurrent Order #%d will be cleared.", g_order.order_id);
        GtkWidget *lbl = gtk_label_new(buf);
        gtk_label_set_wrap(GTK_LABEL(lbl), TRUE);
        gtk_box_append(GTK_BOX(box), lbl);

        GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
        gtk_widget_set_halign(hbox, GTK_ALIGN_END);

        GtkWidget *btn_no = gtk_button_new_with_label("No");
        gtk_widget_add_css_class(btn_no, "btn-secondary");
        g_signal_connect_swapped(btn_no, "clicked", G_CALLBACK(gtk_window_destroy), dialog);
        gtk_box_append(GTK_BOX(hbox), btn_no);

        GtkWidget *btn_yes = gtk_button_new_with_label("Yes, Start New");
        gtk_widget_add_css_class(btn_yes, "btn-primary");
        g_signal_connect(btn_yes, "clicked", G_CALLBACK(on_confirm_new_order_yes), dialog);
        gtk_box_append(GTK_BOX(hbox), btn_yes);

        gtk_box_append(GTK_BOX(box), hbox);
        gtk_window_set_child(GTK_WINDOW(dialog), box);
        gtk_window_present(GTK_WINDOW(dialog));
    } else {
        startNewOrder(&g_order, &g_current_order_id);
        refresh_order_ui();
    }
}

/* ============================================================================
 * UI REFRESH ROUTINE
 * ============================================================================
 */
static void refresh_order_ui(void) {
    /* Clear current order list box */
    GtkWidget *child;
    while ((child = gtk_widget_get_first_child(g_box_order_items)) != NULL) {
        gtk_box_remove(GTK_BOX(g_box_order_items), child);
    }

    /* Update Order ID Badge */
    char buf[100];
    snprintf(buf, sizeof(buf), "Order #%d", g_order.order_id);
    gtk_label_set_text(GTK_LABEL(g_lbl_order_id), buf);

    /* Calculate Totals */
    calculateTotals(&g_order);

    if (g_order.item_count == 0) {
        GtkWidget *empty_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
        gtk_widget_set_valign(empty_box, GTK_ALIGN_CENTER);
        gtk_widget_set_halign(empty_box, GTK_ALIGN_CENTER);
        gtk_widget_set_margin_top(empty_box, 40);
        gtk_widget_set_margin_bottom(empty_box, 40);

        GtkWidget *lbl_empty_icon = gtk_label_new("🛒");
        gtk_widget_add_css_class(lbl_empty_icon, "empty-icon");

        GtkWidget *lbl_empty = gtk_label_new("Your order is empty.");
        gtk_widget_add_css_class(lbl_empty, "empty-title");

        GtkWidget *lbl_sub = gtk_label_new("Select items from the menu on the left to get started.");
        gtk_widget_add_css_class(lbl_sub, "empty-subtitle");

        gtk_box_append(GTK_BOX(empty_box), lbl_empty_icon);
        gtk_box_append(GTK_BOX(empty_box), lbl_empty);
        gtk_box_append(GTK_BOX(empty_box), lbl_sub);
        gtk_box_append(GTK_BOX(g_box_order_items), empty_box);
    } else {
        for (int i = 0; i < g_order.item_count; i++) {
            GtkWidget *row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
            gtk_widget_add_css_class(row, "order-row");

            /* Item Info */
            GtkWidget *vbox_info = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
            gtk_widget_set_hexpand(vbox_info, TRUE);

            GtkWidget *lbl_name = gtk_label_new(g_order.items[i].item.name);
            gtk_widget_set_halign(lbl_name, GTK_ALIGN_START);
            gtk_widget_add_css_class(lbl_name, "order-item-name");

            snprintf(buf, sizeof(buf), "Rs. %.2f each", g_order.items[i].item.price);
            GtkWidget *lbl_price = gtk_label_new(buf);
            gtk_widget_set_halign(lbl_price, GTK_ALIGN_START);
            gtk_widget_add_css_class(lbl_price, "order-item-price");

            gtk_box_append(GTK_BOX(vbox_info), lbl_name);
            gtk_box_append(GTK_BOX(vbox_info), lbl_price);
            gtk_box_append(GTK_BOX(row), vbox_info);

            /* Quantity Controls */
            GtkWidget *hbox_qty = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
            gtk_widget_set_valign(hbox_qty, GTK_ALIGN_CENTER);

            GtkWidget *btn_minus = gtk_button_new_with_label("-");
            gtk_widget_add_css_class(btn_minus, "btn-qty");
            g_signal_connect(btn_minus, "clicked", G_CALLBACK(on_qty_decrement_clicked),
                             GINT_TO_POINTER(g_order.items[i].item.id));

            snprintf(buf, sizeof(buf), "%d", g_order.items[i].quantity);
            GtkWidget *lbl_qty = gtk_label_new(buf);
            gtk_widget_add_css_class(lbl_qty, "lbl-qty-badge");

            GtkWidget *btn_plus = gtk_button_new_with_label("+");
            gtk_widget_add_css_class(btn_plus, "btn-qty");
            g_signal_connect(btn_plus, "clicked", G_CALLBACK(on_qty_increment_clicked),
                             GINT_TO_POINTER(g_order.items[i].item.id));

            gtk_box_append(GTK_BOX(hbox_qty), btn_minus);
            gtk_box_append(GTK_BOX(hbox_qty), lbl_qty);
            gtk_box_append(GTK_BOX(hbox_qty), btn_plus);
            gtk_box_append(GTK_BOX(row), hbox_qty);

            /* Item Amount */
            snprintf(buf, sizeof(buf), "Rs. %.2f", g_order.items[i].amount);
            GtkWidget *lbl_amt = gtk_label_new(buf);
            gtk_widget_set_valign(lbl_amt, GTK_ALIGN_CENTER);
            gtk_widget_add_css_class(lbl_amt, "order-item-amount");
            gtk_box_append(GTK_BOX(row), lbl_amt);

            /* Remove Button */
            GtkWidget *btn_rem = gtk_button_new_with_label("✕");
            gtk_widget_set_valign(btn_rem, GTK_ALIGN_CENTER);
            gtk_widget_add_css_class(btn_rem, "btn-remove");
            g_signal_connect(btn_rem, "clicked", G_CALLBACK(on_remove_item_clicked),
                             GINT_TO_POINTER(g_order.items[i].item.id));
            gtk_box_append(GTK_BOX(row), btn_rem);

            gtk_box_append(GTK_BOX(g_box_order_items), row);
        }
    }

    /* Update Summary Labels */
    snprintf(buf, sizeof(buf), "Rs. %.2f", g_order.subtotal);
    gtk_label_set_text(GTK_LABEL(g_lbl_subtotal), buf);

    snprintf(buf, sizeof(buf), "-Rs. %.2f", g_order.discount_amount);
    gtk_label_set_text(GTK_LABEL(g_lbl_discount), buf);

    double taxable = g_order.subtotal - g_order.discount_amount;
    snprintf(buf, sizeof(buf), "Rs. %.2f", taxable);
    gtk_label_set_text(GTK_LABEL(g_lbl_taxable), buf);

    snprintf(buf, sizeof(buf), "+Rs. %.2f", g_order.tax_amount);
    gtk_label_set_text(GTK_LABEL(g_lbl_gst), buf);

    snprintf(buf, sizeof(buf), "Rs. %.2f", g_order.grand_total);
    gtk_label_set_text(GTK_LABEL(g_lbl_grand_total), buf);
}

/* ============================================================================
 * GTK CSS STYLING PROVIDER
 * ============================================================================
 */
static void apply_custom_css(void) {
    const char *css =
        "window { background-color: #f1f5f9; font-family: 'Segoe UI', system-ui, sans-serif; }\n"
        ".header-banner { background-color: #0f172a; padding: 18px 24px; color: #ffffff; }\n"
        ".app-title { font-size: 22px; font-weight: 800; color: #ffffff; letter-spacing: 0.5px; }\n"
        ".app-subtitle { font-size: 13px; color: #94a3b8; }\n"
        ".order-badge { background-color: #2563eb; color: #ffffff; font-weight: 700; font-size: 14px; padding: 6px 14px; border-radius: 20px; }\n"
        ".card-panel { background-color: #ffffff; border-radius: 12px; padding: 16px; border: 1px solid #e2e8f0; }\n"
        ".panel-title { font-size: 16px; font-weight: 700; color: #1e293b; margin-bottom: 12px; }\n"
        ".menu-card { background-color: #ffffff; border-radius: 8px; padding: 12px; border: 1px solid #e2e8f0; margin-bottom: 8px; }\n"
        ".menu-card:hover { border-color: #3b82f6; background-color: #f8fafc; }\n"
        ".item-name { font-weight: 600; font-size: 14px; color: #0f172a; }\n"
        ".item-price { font-weight: 700; font-size: 14px; color: #059669; }\n"
        ".btn-add { background-color: #2563eb; color: #ffffff; font-weight: 600; font-size: 13px; border-radius: 6px; padding: 4px 12px; border: none; }\n"
        ".btn-add:hover { background-color: #1d4ed8; }\n"
        ".order-row { background-color: #f8fafc; border-radius: 8px; padding: 10px 12px; border: 1px solid #e2e8f0; margin-bottom: 6px; }\n"
        ".order-item-name { font-weight: 600; font-size: 14px; color: #1e293b; }\n"
        ".order-item-price { font-size: 12px; color: #64748b; }\n"
        ".order-item-amount { font-weight: 700; font-size: 14px; color: #0f172a; }\n"
        ".btn-qty { background-color: #e2e8f0; color: #1e293b; font-weight: 700; font-size: 13px; border-radius: 4px; min-width: 28px; min-height: 28px; border: none; padding: 0; }\n"
        ".btn-qty:hover { background-color: #cbd5e1; }\n"
        ".lbl-qty-badge { font-weight: 700; font-size: 13px; color: #0f172a; padding: 0 6px; }\n"
        ".btn-remove { background-color: #fee2e2; color: #ef4444; font-weight: 700; border-radius: 4px; min-width: 28px; min-height: 28px; border: none; padding: 0; }\n"
        ".btn-remove:hover { background-color: #fca5a5; color: #991b1b; }\n"
        ".summary-box { background-color: #f8fafc; border-radius: 10px; padding: 14px; border: 1px solid #e2e8f0; margin-top: 12px; }\n"
        ".summary-label { font-size: 14px; color: #64748b; }\n"
        ".summary-val { font-size: 14px; font-weight: 600; color: #1e293b; }\n"
        ".total-label { font-size: 18px; font-weight: 800; color: #0f172a; }\n"
        ".total-val { font-size: 20px; font-weight: 800; color: #2563eb; }\n"
        ".btn-primary { background-color: #16a34a; color: #ffffff; font-weight: 700; font-size: 15px; border-radius: 8px; padding: 12px 20px; border: none; }\n"
        ".btn-primary:hover { background-color: #15803d; }\n"
        ".btn-secondary { background-color: #64748b; color: #ffffff; font-weight: 600; font-size: 14px; border-radius: 8px; padding: 10px 16px; border: none; }\n"
        ".btn-secondary:hover { background-color: #475569; }\n"
        ".empty-icon { font-size: 36px; margin-bottom: 8px; }\n"
        ".empty-title { font-size: 15px; font-weight: 600; color: #64748b; }\n"
        ".empty-subtitle { font-size: 13px; color: #94a3b8; }\n";

    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_string(provider, css);

    gtk_style_context_add_provider_for_display(
        gdk_display_get_default(),
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
    );
}

/* ============================================================================
 * GTK APPLICATION ACTIVATION
 * ============================================================================
 */
static void on_app_activate(GtkApplication *app, gpointer user_data) {
    (void)user_data;

    initMenu();
    initOrder(&g_order, g_current_order_id);

    g_main_window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(g_main_window), "Spice Bavarchi - Restaurant Management System");
    gtk_window_set_default_size(GTK_WINDOW(g_main_window), 1250, 780);

    apply_custom_css();

    GtkWidget *root_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    /* 1. HEADER BANNER */
    GtkWidget *header = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 15);
    gtk_widget_add_css_class(header, "header-banner");

    GtkWidget *vbox_title = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
    GtkWidget *lbl_title = gtk_label_new("SPICE BAVARCHI RESTAURANT");
    gtk_widget_set_halign(lbl_title, GTK_ALIGN_START);
    gtk_widget_add_css_class(lbl_title, "app-title");

    GtkWidget *lbl_sub = gtk_label_new("Restaurant Management & Billing System");
    gtk_widget_set_halign(lbl_sub, GTK_ALIGN_START);
    gtk_widget_add_css_class(lbl_sub, "app-subtitle");

    gtk_box_append(GTK_BOX(vbox_title), lbl_title);
    gtk_box_append(GTK_BOX(vbox_title), lbl_sub);
    gtk_box_append(GTK_BOX(header), vbox_title);

    /* Flexible spacer */
    GtkWidget *spacer = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_hexpand(spacer, TRUE);
    gtk_box_append(GTK_BOX(header), spacer);

    /* Order Badge */
    g_lbl_order_id = gtk_label_new("Order #1001");
    gtk_widget_set_valign(g_lbl_order_id, GTK_ALIGN_CENTER);
    gtk_widget_add_css_class(g_lbl_order_id, "order-badge");
    gtk_box_append(GTK_BOX(header), g_lbl_order_id);

    gtk_box_append(GTK_BOX(root_box), header);

    /* 2. BODY SPLIT CONTAINER */
    GtkWidget *body = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 16);
    gtk_widget_set_margin_start(body, 16);
    gtk_widget_set_margin_end(body, 16);
    gtk_widget_set_margin_top(body, 16);
    gtk_widget_set_margin_bottom(body, 16);
    gtk_widget_set_vexpand(body, TRUE);

    /* LEFT SIDE: RESTAURANT MENU */
    GtkWidget *left_panel = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_hexpand(left_panel, TRUE);
    gtk_widget_add_css_class(left_panel, "card-panel");

    GtkWidget *lbl_menu_head = gtk_label_new("Restaurant Menu");
    gtk_widget_set_halign(lbl_menu_head, GTK_ALIGN_START);
    gtk_widget_add_css_class(lbl_menu_head, "panel-title");
    gtk_box_append(GTK_BOX(left_panel), lbl_menu_head);

    GtkWidget *notebook = gtk_notebook_new();
    gtk_widget_set_vexpand(notebook, TRUE);

    const char *categories[] = {"Starters", "Main Course", "Biryani", "Beverages", "Desserts"};
    int cat_count = 5;

    for (int c = 0; c < cat_count; c++) {
        GtkWidget *scrolled = gtk_scrolled_window_new();
        gtk_widget_set_vexpand(scrolled, TRUE);

        GtkWidget *cat_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
        gtk_widget_set_margin_start(cat_box, 10);
        gtk_widget_set_margin_end(cat_box, 10);
        gtk_widget_set_margin_top(cat_box, 10);
        gtk_widget_set_margin_bottom(cat_box, 10);

        int menu_total = getMenuCount();
        for (int m = 0; m < menu_total; m++) {
            const FoodItem *item = getMenuItem(m);
            if (item != NULL && strcmp(item->category, categories[c]) == 0) {
                GtkWidget *card = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
                gtk_widget_add_css_class(card, "menu-card");

                GtkWidget *vinfo = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
                gtk_widget_set_hexpand(vinfo, TRUE);

                GtkWidget *lbl_iname = gtk_label_new(item->name);
                gtk_widget_set_halign(lbl_iname, GTK_ALIGN_START);
                gtk_widget_add_css_class(lbl_iname, "item-name");

                char pbuf[50];
                snprintf(pbuf, sizeof(pbuf), "Rs. %.2f", item->price);
                GtkWidget *lbl_iprice = gtk_label_new(pbuf);
                gtk_widget_set_halign(lbl_iprice, GTK_ALIGN_START);
                gtk_widget_add_css_class(lbl_iprice, "item-price");

                gtk_box_append(GTK_BOX(vinfo), lbl_iname);
                gtk_box_append(GTK_BOX(vinfo), lbl_iprice);
                gtk_box_append(GTK_BOX(card), vinfo);

                GtkWidget *btn_add = gtk_button_new_with_label("+ Add");
                gtk_widget_set_valign(btn_add, GTK_ALIGN_CENTER);
                gtk_widget_add_css_class(btn_add, "btn-add");
                g_signal_connect(btn_add, "clicked", G_CALLBACK(on_add_menu_item_clicked),
                                 GINT_TO_POINTER(item->id));
                gtk_box_append(GTK_BOX(card), btn_add);

                gtk_box_append(GTK_BOX(cat_box), card);
            }
        }

        gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled), cat_box);
        gtk_notebook_append_page(GTK_NOTEBOOK(notebook), scrolled, gtk_label_new(categories[c]));
    }

    gtk_box_append(GTK_BOX(left_panel), notebook);
    gtk_box_append(GTK_BOX(body), left_panel);

    /* RIGHT SIDE: CURRENT ORDER & BILLING SUMMARY */
    GtkWidget *right_panel = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_size_request(right_panel, 520, -1);
    gtk_widget_add_css_class(right_panel, "card-panel");

    GtkWidget *lbl_order_head = gtk_label_new("Current Order");
    gtk_widget_set_halign(lbl_order_head, GTK_ALIGN_START);
    gtk_widget_add_css_class(lbl_order_head, "panel-title");
    gtk_box_append(GTK_BOX(right_panel), lbl_order_head);

    GtkWidget *scrolled_order = gtk_scrolled_window_new();
    gtk_widget_set_vexpand(scrolled_order, TRUE);

    g_box_order_items = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gtk_widget_set_margin_start(g_box_order_items, 6);
    gtk_widget_set_margin_end(g_box_order_items, 6);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled_order), g_box_order_items);
    gtk_box_append(GTK_BOX(right_panel), scrolled_order);

    /* BILLING SUMMARY FRAME */
    GtkWidget *summary_frame = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    gtk_widget_add_css_class(summary_frame, "summary-box");

    /* Subtotal */
    GtkWidget *box_sub = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    GtkWidget *lbl1 = gtk_label_new("Subtotal:");
    gtk_widget_add_css_class(lbl1, "summary-label");
    g_lbl_subtotal = gtk_label_new("Rs. 0.00");
    gtk_widget_add_css_class(g_lbl_subtotal, "summary-val");
    GtkWidget *s1 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_hexpand(s1, TRUE);
    gtk_box_append(GTK_BOX(box_sub), lbl1);
    gtk_box_append(GTK_BOX(box_sub), s1);
    gtk_box_append(GTK_BOX(box_sub), g_lbl_subtotal);
    gtk_box_append(GTK_BOX(summary_frame), box_sub);

    /* Discount */
    GtkWidget *box_disc = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    GtkWidget *lbl2 = gtk_label_new("Discount (5%):");
    gtk_widget_add_css_class(lbl2, "summary-label");
    g_lbl_discount = gtk_label_new("-Rs. 0.00");
    gtk_widget_add_css_class(g_lbl_discount, "summary-val");
    GtkWidget *s2 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_hexpand(s2, TRUE);
    gtk_box_append(GTK_BOX(box_disc), lbl2);
    gtk_box_append(GTK_BOX(box_disc), s2);
    gtk_box_append(GTK_BOX(box_disc), g_lbl_discount);
    gtk_box_append(GTK_BOX(summary_frame), box_disc);

    /* Taxable Amount */
    GtkWidget *box_taxable = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    GtkWidget *lbl3 = gtk_label_new("Taxable Amount:");
    gtk_widget_add_css_class(lbl3, "summary-label");
    g_lbl_taxable = gtk_label_new("Rs. 0.00");
    gtk_widget_add_css_class(g_lbl_taxable, "summary-val");
    GtkWidget *s3 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_hexpand(s3, TRUE);
    gtk_box_append(GTK_BOX(box_taxable), lbl3);
    gtk_box_append(GTK_BOX(box_taxable), s3);
    gtk_box_append(GTK_BOX(box_taxable), g_lbl_taxable);
    gtk_box_append(GTK_BOX(summary_frame), box_taxable);

    /* GST Tax */
    GtkWidget *box_gst = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    GtkWidget *lbl4 = gtk_label_new("GST (5%):");
    gtk_widget_add_css_class(lbl4, "summary-label");
    g_lbl_gst = gtk_label_new("+Rs. 0.00");
    gtk_widget_add_css_class(g_lbl_gst, "summary-val");
    GtkWidget *s4 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_hexpand(s4, TRUE);
    gtk_box_append(GTK_BOX(box_gst), lbl4);
    gtk_box_append(GTK_BOX(box_gst), s4);
    gtk_box_append(GTK_BOX(box_gst), g_lbl_gst);
    gtk_box_append(GTK_BOX(summary_frame), box_gst);

    /* Separator */
    GtkWidget *sep = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_widget_set_margin_top(sep, 6);
    gtk_widget_set_margin_bottom(sep, 6);
    gtk_box_append(GTK_BOX(summary_frame), sep);

    /* Grand Total */
    GtkWidget *box_tot = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    GtkWidget *lbl5 = gtk_label_new("GRAND TOTAL:");
    gtk_widget_add_css_class(lbl5, "total-label");
    g_lbl_grand_total = gtk_label_new("Rs. 0.00");
    gtk_widget_add_css_class(g_lbl_grand_total, "total-val");
    GtkWidget *s5 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_hexpand(s5, TRUE);
    gtk_box_append(GTK_BOX(box_tot), lbl5);
    gtk_box_append(GTK_BOX(box_tot), s5);
    gtk_box_append(GTK_BOX(box_tot), g_lbl_grand_total);
    gtk_box_append(GTK_BOX(summary_frame), box_tot);

    gtk_box_append(GTK_BOX(right_panel), summary_frame);

    /* ACTION BUTTONS */
    GtkWidget *action_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_widget_set_margin_top(action_box, 10);

    GtkWidget *btn_new = gtk_button_new_with_label("🔄 New Order");
    gtk_widget_add_css_class(btn_new, "btn-secondary");
    g_signal_connect(btn_new, "clicked", G_CALLBACK(on_new_order_clicked), NULL);
    gtk_box_append(GTK_BOX(action_box), btn_new);

    GtkWidget *btn_bill = gtk_button_new_with_label("🧾 GENERATE BILL");
    gtk_widget_set_hexpand(btn_bill, TRUE);
    gtk_widget_add_css_class(btn_bill, "btn-primary");
    g_signal_connect(btn_bill, "clicked", G_CALLBACK(on_generate_bill_clicked), NULL);
    gtk_box_append(GTK_BOX(action_box), btn_bill);

    gtk_box_append(GTK_BOX(right_panel), action_box);
    gtk_box_append(GTK_BOX(body), right_panel);

    gtk_box_append(GTK_BOX(root_box), body);

    gtk_window_set_child(GTK_WINDOW(g_main_window), root_box);
    refresh_order_ui();
    gtk_window_present(GTK_WINDOW(g_main_window));
}

int run_gui_app(int argc, char *argv[]) {
    GtkApplication *app = gtk_application_new("com.spicebavarchi.restaurant", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(on_app_activate), NULL);
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;
}
