#include <gtk/gtk.h>
#include "gui.h"
#include <ctype.h>

/* Global UI References */
static GtkWidget *g_main_window = NULL;
static GtkWidget *g_stack = NULL;

/* Module Containers */
static GtkWidget *g_dashboard_container = NULL;
static GtkWidget *g_menu_container = NULL;
static GtkWidget *g_tables_container = NULL;
static GtkWidget *g_orders_container = NULL;
static GtkWidget *g_kitchen_container = NULL;
static GtkWidget *g_billing_container = NULL;
static GtkWidget *g_reports_container = NULL;

/* Navigation Buttons for Active State Highlighting */
static GtkWidget *g_nav_btn_dashboard = NULL;
static GtkWidget *g_nav_btn_menu = NULL;
static GtkWidget *g_nav_btn_tables = NULL;
static GtkWidget *g_nav_btn_orders = NULL;
static GtkWidget *g_nav_btn_kitchen = NULL;
static GtkWidget *g_nav_btn_billing = NULL;
static GtkWidget *g_nav_btn_reports = NULL;

/* State Selection */
static int g_selected_order_id = 0;
static char g_selected_menu_category[30] = "ALL";
static char g_menu_search_query[60] = "";

/* Forward Declarations */
static void refresh_all_views(void);
static void switch_to_page(const char *page_name);
static void show_info_alert(GtkWidget *parent, const char *title, const char *message);
static void show_error_alert(GtkWidget *parent, const char *title, const char *message);

static void refresh_dashboard_view(void);
static void refresh_menu_view(void);
static void refresh_tables_view(void);
static void refresh_orders_view(void);
static void refresh_kitchen_view(void);
static void refresh_billing_view(void);
static void refresh_reports_view(void);

/* Helper to safely clear container children in GTK3 */
static void clear_container(GtkWidget *container) {
    if (!container || !GTK_IS_CONTAINER(container)) return;
    GList *children = gtk_container_get_children(GTK_CONTAINER(container));
    for (GList *iter = children; iter != NULL; iter = g_list_next(iter)) {
        gtk_widget_destroy(GTK_WIDGET(iter->data));
    }
    g_list_free(children);
}

/* Alert Dialog Helpers */
static void show_info_alert(GtkWidget *parent, const char *title, const char *message) {
    GtkWidget *dialog = gtk_message_dialog_new(
        GTK_WINDOW(parent ? parent : g_main_window),
        GTK_DIALOG_MODAL,
        GTK_MESSAGE_INFO,
        GTK_BUTTONS_OK,
        "%s", message
    );
    gtk_window_set_title(GTK_WINDOW(dialog), title);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

static void show_error_alert(GtkWidget *parent, const char *title, const char *message) {
    GtkWidget *dialog = gtk_message_dialog_new(
        GTK_WINDOW(parent ? parent : g_main_window),
        GTK_DIALOG_MODAL,
        GTK_MESSAGE_ERROR,
        GTK_BUTTONS_OK,
        "%s", message
    );
    gtk_window_set_title(GTK_WINDOW(dialog), title);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

/* Navigation Active State */
static void set_nav_active(GtkWidget *btn) {
    GtkWidget *nav_btns[] = {
        g_nav_btn_dashboard, g_nav_btn_menu, g_nav_btn_tables,
        g_nav_btn_orders, g_nav_btn_kitchen, g_nav_btn_billing, g_nav_btn_reports
    };
    for (int i = 0; i < 7; i++) {
        if (nav_btns[i]) {
            GtkStyleContext *ctx = gtk_widget_get_style_context(nav_btns[i]);
            gtk_style_context_remove_class(ctx, "nav-btn-active");
        }
    }
    if (btn) {
        GtkStyleContext *ctx = gtk_widget_get_style_context(btn);
        gtk_style_context_add_class(ctx, "nav-btn-active");
    }
}

static void switch_to_page(const char *page_name) {
    if (!g_stack) return;
    gtk_stack_set_visible_child_name(GTK_STACK(g_stack), page_name);

    if (strcmp(page_name, "dashboard") == 0) set_nav_active(g_nav_btn_dashboard);
    else if (strcmp(page_name, "menu") == 0) set_nav_active(g_nav_btn_menu);
    else if (strcmp(page_name, "tables") == 0) set_nav_active(g_nav_btn_tables);
    else if (strcmp(page_name, "orders") == 0) set_nav_active(g_nav_btn_orders);
    else if (strcmp(page_name, "kitchen") == 0) set_nav_active(g_nav_btn_kitchen);
    else if (strcmp(page_name, "billing") == 0) set_nav_active(g_nav_btn_billing);
    else if (strcmp(page_name, "reports") == 0) set_nav_active(g_nav_btn_reports);

    refresh_all_views();
    gtk_widget_show_all(g_stack);
    gtk_widget_show_all(g_main_window);
}

static void on_nav_clicked(GtkButton *btn, gpointer user_data) {
    (void)btn;
    const char *page_name = (const char*)user_data;
    switch_to_page(page_name);
}

static void on_exit_clicked(GtkButton *btn, gpointer user_data) {
    (void)btn;
    (void)user_data;
    saveData();
    gtk_main_quit();
}

static void refresh_all_views(void) {
    refresh_dashboard_view();
    refresh_menu_view();
    refresh_tables_view();
    refresh_orders_view();
    refresh_kitchen_view();
    refresh_billing_view();
    refresh_reports_view();
}

/* ============================================================================
 * MODULE 1: DASHBOARD VIEW
 * ============================================================================
 */
static void refresh_dashboard_view(void) {
    if (!g_dashboard_container) return;
    clear_container(g_dashboard_container);

    DashboardStats stats;
    getDashboardStats(&stats);

    GtkWidget *root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 16);
    gtk_widget_set_margin_start(root, 20);
    gtk_widget_set_margin_end(root, 20);
    gtk_widget_set_margin_top(root, 20);
    gtk_widget_set_margin_bottom(root, 20);
    gtk_widget_set_vexpand(root, TRUE);
    gtk_widget_set_hexpand(root, TRUE);

    /* Title */
    GtkWidget *lbl_head = gtk_label_new("Restaurant Executive Dashboard & Analytics");
    gtk_widget_set_halign(lbl_head, GTK_ALIGN_START);
    gtk_style_context_add_class(gtk_widget_get_style_context(lbl_head), "page-title");
    gtk_box_pack_start(GTK_BOX(root), lbl_head, FALSE, FALSE, 0);

    /* Grid for 8 Metrics Cards */
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 16);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 16);

    char buf[128];

    /* Card 1: Total Orders */
    GtkWidget *c1 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    gtk_style_context_add_class(gtk_widget_get_style_context(c1), "stat-card");
    GtkWidget *t1 = gtk_label_new("Total Orders");
    gtk_widget_set_halign(t1, GTK_ALIGN_START);
    gtk_style_context_add_class(gtk_widget_get_style_context(t1), "stat-title");
    snprintf(buf, sizeof(buf), "%d", stats.total_orders);
    GtkWidget *v1 = gtk_label_new(buf);
    gtk_widget_set_halign(v1, GTK_ALIGN_START);
    gtk_style_context_add_class(gtk_widget_get_style_context(v1), "stat-value");
    gtk_box_pack_start(GTK_BOX(c1), t1, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(c1), v1, FALSE, FALSE, 0);
    gtk_widget_set_hexpand(c1, TRUE);
    gtk_grid_attach(GTK_GRID(grid), c1, 0, 0, 1, 1);

    /* Card 2: Active Orders */
    GtkWidget *c2 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    gtk_style_context_add_class(gtk_widget_get_style_context(c2), "stat-card");
    GtkWidget *t2 = gtk_label_new("Active Orders");
    gtk_widget_set_halign(t2, GTK_ALIGN_START);
    gtk_style_context_add_class(gtk_widget_get_style_context(t2), "stat-title");
    snprintf(buf, sizeof(buf), "%d", stats.active_orders);
    GtkWidget *v2 = gtk_label_new(buf);
    gtk_widget_set_halign(v2, GTK_ALIGN_START);
    gtk_style_context_add_class(gtk_widget_get_style_context(v2), "stat-value");
    gtk_box_pack_start(GTK_BOX(c2), t2, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(c2), v2, FALSE, FALSE, 0);
    gtk_widget_set_hexpand(c2, TRUE);
    gtk_grid_attach(GTK_GRID(grid), c2, 1, 0, 1, 1);

    /* Card 3: Available Tables */
    GtkWidget *c3 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    gtk_style_context_add_class(gtk_widget_get_style_context(c3), "stat-card");
    GtkWidget *t3 = gtk_label_new("Available Tables");
    gtk_widget_set_halign(t3, GTK_ALIGN_START);
    gtk_style_context_add_class(gtk_widget_get_style_context(t3), "stat-title");
    snprintf(buf, sizeof(buf), "%d", stats.available_tables);
    GtkWidget *v3 = gtk_label_new(buf);
    gtk_widget_set_halign(v3, GTK_ALIGN_START);
    gtk_style_context_add_class(gtk_widget_get_style_context(v3), "stat-value");
    gtk_box_pack_start(GTK_BOX(c3), t3, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(c3), v3, FALSE, FALSE, 0);
    gtk_widget_set_hexpand(c3, TRUE);
    gtk_grid_attach(GTK_GRID(grid), c3, 2, 0, 1, 1);

    /* Card 4: Occupied Tables */
    GtkWidget *c4 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    gtk_style_context_add_class(gtk_widget_get_style_context(c4), "stat-card");
    GtkWidget *t4 = gtk_label_new("Occupied Tables");
    gtk_widget_set_halign(t4, GTK_ALIGN_START);
    gtk_style_context_add_class(gtk_widget_get_style_context(t4), "stat-title");
    snprintf(buf, sizeof(buf), "%d", stats.occupied_tables);
    GtkWidget *v4 = gtk_label_new(buf);
    gtk_widget_set_halign(v4, GTK_ALIGN_START);
    gtk_style_context_add_class(gtk_widget_get_style_context(v4), "stat-value");
    gtk_box_pack_start(GTK_BOX(c4), t4, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(c4), v4, FALSE, FALSE, 0);
    gtk_widget_set_hexpand(c4, TRUE);
    gtk_grid_attach(GTK_GRID(grid), c4, 3, 0, 1, 1);

    /* Card 5: Reserved Tables */
    GtkWidget *c5 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    gtk_style_context_add_class(gtk_widget_get_style_context(c5), "stat-card");
    GtkWidget *t5 = gtk_label_new("Reserved Tables");
    gtk_widget_set_halign(t5, GTK_ALIGN_START);
    gtk_style_context_add_class(gtk_widget_get_style_context(t5), "stat-title");
    snprintf(buf, sizeof(buf), "%d", stats.reserved_tables);
    GtkWidget *v5 = gtk_label_new(buf);
    gtk_widget_set_halign(v5, GTK_ALIGN_START);
    gtk_style_context_add_class(gtk_widget_get_style_context(v5), "stat-value");
    gtk_box_pack_start(GTK_BOX(c5), t5, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(c5), v5, FALSE, FALSE, 0);
    gtk_widget_set_hexpand(c5, TRUE);
    gtk_grid_attach(GTK_GRID(grid), c5, 0, 1, 1, 1);

    /* Card 6: Table Occupancy Rate */
    GtkWidget *c6 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    gtk_style_context_add_class(gtk_widget_get_style_context(c6), "stat-card");
    GtkWidget *t6 = gtk_label_new("Table Occupancy %");
    gtk_widget_set_halign(t6, GTK_ALIGN_START);
    gtk_style_context_add_class(gtk_widget_get_style_context(t6), "stat-title");
    snprintf(buf, sizeof(buf), "%.1f%%", stats.occupancy_rate);
    GtkWidget *v6 = gtk_label_new(buf);
    gtk_widget_set_halign(v6, GTK_ALIGN_START);
    gtk_style_context_add_class(gtk_widget_get_style_context(v6), "stat-value");
    gtk_box_pack_start(GTK_BOX(c6), t6, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(c6), v6, FALSE, FALSE, 0);
    gtk_widget_set_hexpand(c6, TRUE);
    gtk_grid_attach(GTK_GRID(grid), c6, 1, 1, 1, 1);

    /* Card 7: Average Order Value */
    GtkWidget *c7 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    gtk_style_context_add_class(gtk_widget_get_style_context(c7), "stat-card");
    GtkWidget *t7 = gtk_label_new("Avg Order Value");
    gtk_widget_set_halign(t7, GTK_ALIGN_START);
    gtk_style_context_add_class(gtk_widget_get_style_context(t7), "stat-title");
    snprintf(buf, sizeof(buf), "Rs. %.2f", stats.avg_order_value);
    GtkWidget *v7 = gtk_label_new(buf);
    gtk_widget_set_halign(v7, GTK_ALIGN_START);
    gtk_style_context_add_class(gtk_widget_get_style_context(v7), "stat-value");
    gtk_box_pack_start(GTK_BOX(c7), t7, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(c7), v7, FALSE, FALSE, 0);
    gtk_widget_set_hexpand(c7, TRUE);
    gtk_grid_attach(GTK_GRID(grid), c7, 2, 1, 1, 1);

    /* Card 8: Total Revenue Banner */
    GtkWidget *c8 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    gtk_style_context_add_class(gtk_widget_get_style_context(c8), "stat-card");
    GtkWidget *t8 = gtk_label_new("Today's Total Revenue Realized");
    gtk_widget_set_halign(t8, GTK_ALIGN_START);
    gtk_style_context_add_class(gtk_widget_get_style_context(t8), "stat-title");
    snprintf(buf, sizeof(buf), "Rs. %.2f", stats.today_revenue);
    GtkWidget *v8 = gtk_label_new(buf);
    gtk_widget_set_halign(v8, GTK_ALIGN_START);
    gtk_style_context_add_class(gtk_widget_get_style_context(v8), "stat-value");
    gtk_box_pack_start(GTK_BOX(c8), t8, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(c8), v8, FALSE, FALSE, 0);
    gtk_widget_set_hexpand(c8, TRUE);
    gtk_grid_attach(GTK_GRID(grid), c8, 3, 1, 1, 1);

    gtk_box_pack_start(GTK_BOX(root), grid, FALSE, FALSE, 0);

    /* Quick Shortcuts Panel with High Contrast Buttons */
    GtkWidget *panel = gtk_box_new(GTK_ORIENTATION_VERTICAL, 14);
    gtk_style_context_add_class(gtk_widget_get_style_context(panel), "card-panel");

    GtkWidget *lbl_p = gtk_label_new("Quick Management Actions");
    gtk_widget_set_halign(lbl_p, GTK_ALIGN_START);
    gtk_style_context_add_class(gtk_widget_get_style_context(lbl_p), "page-title");
    gtk_box_pack_start(GTK_BOX(panel), lbl_p, FALSE, FALSE, 0);

    GtkWidget *btn_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);

    GtkWidget *b_menu = gtk_button_new_with_label("🍽️ Menu Management");
    gtk_style_context_add_class(gtk_widget_get_style_context(b_menu), "btn-primary");
    g_signal_connect(b_menu, "clicked", G_CALLBACK(on_nav_clicked), (gpointer)"menu");
    gtk_box_pack_start(GTK_BOX(btn_box), b_menu, TRUE, TRUE, 0);

    GtkWidget *b_tbl = gtk_button_new_with_label("🪑 Table Management");
    gtk_style_context_add_class(gtk_widget_get_style_context(b_tbl), "btn-primary");
    g_signal_connect(b_tbl, "clicked", G_CALLBACK(on_nav_clicked), (gpointer)"tables");
    gtk_box_pack_start(GTK_BOX(btn_box), b_tbl, TRUE, TRUE, 0);

    GtkWidget *b_ord = gtk_button_new_with_label("📋 Create / View Orders");
    gtk_style_context_add_class(gtk_widget_get_style_context(b_ord), "btn-success");
    g_signal_connect(b_ord, "clicked", G_CALLBACK(on_nav_clicked), (gpointer)"orders");
    gtk_box_pack_start(GTK_BOX(btn_box), b_ord, TRUE, TRUE, 0);

    GtkWidget *b_kitch = gtk_button_new_with_label("👨‍🍳 Kitchen Display");
    gtk_style_context_add_class(gtk_widget_get_style_context(b_kitch), "btn-warning");
    g_signal_connect(b_kitch, "clicked", G_CALLBACK(on_nav_clicked), (gpointer)"kitchen");
    gtk_box_pack_start(GTK_BOX(btn_box), b_kitch, TRUE, TRUE, 0);

    GtkWidget *b_bill = gtk_button_new_with_label("🧾 Billing & Receipt");
    gtk_style_context_add_class(gtk_widget_get_style_context(b_bill), "btn-primary");
    g_signal_connect(b_bill, "clicked", G_CALLBACK(on_nav_clicked), (gpointer)"billing");
    gtk_box_pack_start(GTK_BOX(btn_box), b_bill, TRUE, TRUE, 0);

    gtk_box_pack_start(GTK_BOX(panel), btn_box, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(root), panel, FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(g_dashboard_container), root, TRUE, TRUE, 0);
    gtk_widget_show_all(g_dashboard_container);
}

/* ============================================================================
 * MODULE 2: MENU MANAGEMENT VIEW
 * ============================================================================
 */
static void show_add_edit_menu_item_dialog(const FoodItem *edit_item) {
    GtkWidget *dialog = gtk_dialog_new_with_buttons(
        edit_item ? "Edit Menu Item" : "Add New Menu Item",
        GTK_WINDOW(g_main_window),
        GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
        "Cancel", GTK_RESPONSE_CANCEL,
        "Save", GTK_RESPONSE_ACCEPT,
        NULL
    );
    gtk_window_set_default_size(GTK_WINDOW(dialog), 380, 320);

    GtkWidget *content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 12);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 12);
    gtk_widget_set_margin_start(grid, 20);
    gtk_widget_set_margin_end(grid, 20);
    gtk_widget_set_margin_top(grid, 20);
    gtk_widget_set_margin_bottom(grid, 20);

    /* ID */
    GtkWidget *lbl_id = gtk_label_new("Item ID:");
    GtkWidget *entry_id = gtk_entry_new();
    char id_buf[32];
    if (edit_item) {
        snprintf(id_buf, sizeof(id_buf), "%d", edit_item->id);
        gtk_entry_set_text(GTK_ENTRY(entry_id), id_buf);
        gtk_widget_set_sensitive(entry_id, FALSE);
    } else {
        snprintf(id_buf, sizeof(id_buf), "%d", 100 + getMenuCount() + 1);
        gtk_entry_set_text(GTK_ENTRY(entry_id), id_buf);
    }
    gtk_grid_attach(GTK_GRID(grid), lbl_id, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), entry_id, 1, 0, 1, 1);

    /* Name */
    GtkWidget *lbl_name = gtk_label_new("Item Name:");
    GtkWidget *entry_name = gtk_entry_new();
    if (edit_item) gtk_entry_set_text(GTK_ENTRY(entry_name), edit_item->name);
    gtk_grid_attach(GTK_GRID(grid), lbl_name, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), entry_name, 1, 1, 1, 1);

    /* Category */
    GtkWidget *lbl_cat = gtk_label_new("Category:");
    GtkWidget *combo_cat = gtk_combo_box_text_new();
    const char *cats[] = {"Starters", "Soups", "Main Course", "Biryani", "Indian Breads", "Chinese", "Beverages", "Desserts"};
    for (int i = 0; i < 8; i++) {
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_cat), cats[i]);
    }
    if (edit_item) {
        GtkTreeModel *model = gtk_combo_box_get_model(GTK_COMBO_BOX(combo_cat));
        GtkTreeIter iter;
        int idx = 0;
        if (gtk_tree_model_get_iter_first(model, &iter)) {
            do {
                gchar *val = NULL;
                gtk_tree_model_get(model, &iter, 0, &val, -1);
                if (val && strcmp(val, edit_item->category) == 0) {
                    gtk_combo_box_set_active(GTK_COMBO_BOX(combo_cat), idx);
                    g_free(val);
                    break;
                }
                g_free(val);
                idx++;
            } while (gtk_tree_model_iter_next(model, &iter));
        }
    } else {
        gtk_combo_box_set_active(GTK_COMBO_BOX(combo_cat), 0);
    }
    gtk_grid_attach(GTK_GRID(grid), lbl_cat, 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), combo_cat, 1, 2, 1, 1);

    /* Price */
    GtkWidget *lbl_price = gtk_label_new("Price (Rs.):");
    GtkWidget *entry_price = gtk_entry_new();
    if (edit_item) {
        snprintf(id_buf, sizeof(id_buf), "%.2f", edit_item->price);
        gtk_entry_set_text(GTK_ENTRY(entry_price), id_buf);
    }
    gtk_grid_attach(GTK_GRID(grid), lbl_price, 0, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), entry_price, 1, 3, 1, 1);

    /* Available */
    GtkWidget *chk_avail = gtk_check_button_new_with_label("Available for Ordering");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(chk_avail), edit_item ? edit_item->available : 1);
    gtk_grid_attach(GTK_GRID(grid), chk_avail, 1, 4, 1, 1);

    gtk_container_add(GTK_CONTAINER(content), grid);
    gtk_widget_show_all(dialog);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        int id = atoi(gtk_entry_get_text(GTK_ENTRY(entry_id)));
        const char *name = gtk_entry_get_text(GTK_ENTRY(entry_name));
        gchar *cat = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(combo_cat));
        double price = atof(gtk_entry_get_text(GTK_ENTRY(entry_price)));
        int avail = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(chk_avail));
        char err[128];

        int success = 0;
        if (edit_item) {
            success = updateMenuItem(id, name, cat, price, avail, err, sizeof(err));
        } else {
            success = addMenuItem(id, name, cat, price, avail, err, sizeof(err));
        }

        if (!success) {
            show_error_alert(g_main_window, "Menu Operation Error", err);
        } else {
            refresh_all_views();
        }
        g_free(cat);
    }

    gtk_widget_destroy(dialog);
}

static void on_menu_category_filter_changed(GtkComboBox *combo, gpointer user_data) {
    (void)user_data;
    gchar *active = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(combo));
    if (active) {
        strncpy(g_selected_menu_category, active, sizeof(g_selected_menu_category) - 1);
        g_free(active);
        refresh_menu_view();
    }
}

static void on_menu_search_changed(GtkEntry *entry, gpointer user_data) {
    (void)user_data;
    const char *text = gtk_entry_get_text(entry);
    if (text) {
        strncpy(g_menu_search_query, text, sizeof(g_menu_search_query) - 1);
    } else {
        g_menu_search_query[0] = '\0';
    }
    refresh_menu_view();
}

static void on_add_menu_clicked(GtkButton *btn, gpointer user_data) {
    (void)btn;
    (void)user_data;
    show_add_edit_menu_item_dialog(NULL);
}

static void on_toggle_avail_clicked(GtkButton *btn, gpointer user_data) {
    (void)btn;
    int id = GPOINTER_TO_INT(user_data);
    toggleItemAvailability(id);
    refresh_all_views();
}

static void on_edit_menu_clicked(GtkButton *btn, gpointer user_data) {
    (void)btn;
    int id = GPOINTER_TO_INT(user_data);
    const FoodItem *item = findFoodItemById(id);
    if (item) {
        show_add_edit_menu_item_dialog(item);
    }
}

static void on_delete_menu_clicked(GtkButton *btn, gpointer user_data) {
    (void)btn;
    int id = GPOINTER_TO_INT(user_data);
    char err[128];
    if (!deleteMenuItem(id, err, sizeof(err))) {
        show_error_alert(g_main_window, "Delete Error", err);
    } else {
        refresh_all_views();
    }
}

static void refresh_menu_view(void) {
    if (!g_menu_container) return;
    clear_container(g_menu_container);

    GtkWidget *root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 16);
    gtk_widget_set_margin_start(root, 20);
    gtk_widget_set_margin_end(root, 20);
    gtk_widget_set_margin_top(root, 20);
    gtk_widget_set_margin_bottom(root, 20);
    gtk_widget_set_vexpand(root, TRUE);
    gtk_widget_set_hexpand(root, TRUE);

    /* Header Bar */
    GtkWidget *hbar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
    GtkWidget *lbl_head = gtk_label_new("Restaurant Menu Management");
    gtk_style_context_add_class(gtk_widget_get_style_context(lbl_head), "page-title");
    gtk_box_pack_start(GTK_BOX(hbar), lbl_head, FALSE, FALSE, 0);

    /* Search Bar */
    GtkWidget *entry_search = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_search), "🔍 Search items...");
    if (strlen(g_menu_search_query) > 0) {
        gtk_entry_set_text(GTK_ENTRY(entry_search), g_menu_search_query);
    }
    g_signal_connect(entry_search, "changed", G_CALLBACK(on_menu_search_changed), NULL);
    gtk_box_pack_start(GTK_BOX(hbar), entry_search, FALSE, FALSE, 0);

    /* Category Filter */
    GtkWidget *lbl_filter = gtk_label_new("Category:");
    GtkWidget *combo_filter = gtk_combo_box_text_new();
    const char *cats[] = {"ALL", "Starters", "Soups", "Main Course", "Biryani", "Indian Breads", "Chinese", "Beverages", "Desserts"};
    for (int i = 0; i < 9; i++) {
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_filter), cats[i]);
    }
    
    /* Set active filter */
    GtkTreeModel *model = gtk_combo_box_get_model(GTK_COMBO_BOX(combo_filter));
    GtkTreeIter iter;
    int idx = 0;
    if (gtk_tree_model_get_iter_first(model, &iter)) {
        do {
            gchar *val = NULL;
            gtk_tree_model_get(model, &iter, 0, &val, -1);
            if (val && strcmp(val, g_selected_menu_category) == 0) {
                gtk_combo_box_set_active(GTK_COMBO_BOX(combo_filter), idx);
                g_free(val);
                break;
            }
            g_free(val);
            idx++;
        } while (gtk_tree_model_iter_next(model, &iter));
    }
    g_signal_connect(combo_filter, "changed", G_CALLBACK(on_menu_category_filter_changed), NULL);

    gtk_box_pack_start(GTK_BOX(hbar), lbl_filter, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbar), combo_filter, FALSE, FALSE, 0);

    GtkWidget *spacer = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_pack_start(GTK_BOX(hbar), spacer, TRUE, TRUE, 0);

    GtkWidget *btn_add = gtk_button_new_with_label("➕ Add New Item");
    gtk_style_context_add_class(gtk_widget_get_style_context(btn_add), "btn-success");
    g_signal_connect(btn_add, "clicked", G_CALLBACK(on_add_menu_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(hbar), btn_add, FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(root), hbar, FALSE, FALSE, 0);

    /* Scrolled Window */
    GtkWidget *scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_vexpand(scrolled, TRUE);
    gtk_widget_set_hexpand(scrolled, TRUE);

    GtkWidget *card = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_style_context_add_class(gtk_widget_get_style_context(card), "card-panel");

    /* Table Header */
    GtkWidget *header_row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
    GtkWidget *h_id = gtk_label_new("ID"); gtk_widget_set_size_request(h_id, 60, -1);
    GtkWidget *h_name = gtk_label_new("ITEM NAME"); gtk_widget_set_hexpand(h_name, TRUE); gtk_widget_set_halign(h_name, GTK_ALIGN_START);
    GtkWidget *h_cat = gtk_label_new("CATEGORY"); gtk_widget_set_size_request(h_cat, 140, -1); gtk_widget_set_halign(h_cat, GTK_ALIGN_START);
    GtkWidget *h_price = gtk_label_new("PRICE"); gtk_widget_set_size_request(h_price, 90, -1); gtk_widget_set_halign(h_price, GTK_ALIGN_END);
    GtkWidget *h_stat = gtk_label_new("STATUS"); gtk_widget_set_size_request(h_stat, 110, -1);
    GtkWidget *h_act = gtk_label_new("ACTIONS"); gtk_widget_set_size_request(h_act, 230, -1);

    gtk_box_pack_start(GTK_BOX(header_row), h_id, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(header_row), h_name, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(header_row), h_cat, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(header_row), h_price, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(header_row), h_stat, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(header_row), h_act, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(card), header_row, FALSE, FALSE, 0);

    GtkWidget *sep = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_box_pack_start(GTK_BOX(card), sep, FALSE, FALSE, 0);

    /* Items rows */
    int count = getMenuCount();
    char buf[64];
    int shown = 0;
    for (int i = 0; i < count; i++) {
        const FoodItem *item = getMenuItem(i);
        if (!item) continue;

        if (strcmp(g_selected_menu_category, "ALL") != 0 && strcmp(item->category, g_selected_menu_category) != 0) {
            continue;
        }

        if (strlen(g_menu_search_query) > 0) {
            char lower_name[100], lower_query[100];
            strncpy(lower_name, item->name, sizeof(lower_name) - 1); lower_name[99] = '\0';
            strncpy(lower_query, g_menu_search_query, sizeof(lower_query) - 1); lower_query[99] = '\0';
            for (char *p = lower_name; *p; ++p) *p = tolower(*p);
            for (char *p = lower_query; *p; ++p) *p = tolower(*p);
            if (strstr(lower_name, lower_query) == NULL) {
                continue;
            }
        }

        shown++;

        GtkWidget *row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
        gtk_widget_set_margin_top(row, 4);
        gtk_widget_set_margin_bottom(row, 4);

        snprintf(buf, sizeof(buf), "%d", item->id);
        GtkWidget *l_id = gtk_label_new(buf); gtk_widget_set_size_request(l_id, 60, -1);

        GtkWidget *l_name = gtk_label_new(item->name);
        gtk_widget_set_hexpand(l_name, TRUE);
        gtk_widget_set_halign(l_name, GTK_ALIGN_START);

        GtkWidget *l_cat = gtk_label_new(item->category);
        gtk_widget_set_size_request(l_cat, 140, -1);
        gtk_widget_set_halign(l_cat, GTK_ALIGN_START);

        snprintf(buf, sizeof(buf), "Rs. %.2f", item->price);
        GtkWidget *l_price = gtk_label_new(buf);
        gtk_widget_set_size_request(l_price, 90, -1);
        gtk_widget_set_halign(l_price, GTK_ALIGN_END);

        GtkWidget *l_stat = gtk_label_new(item->available ? "Available" : "Unavailable");
        gtk_widget_set_size_request(l_stat, 110, -1);
        gtk_style_context_add_class(gtk_widget_get_style_context(l_stat),
                                   item->available ? "badge-available" : "badge-occupied");

        GtkWidget *b_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
        gtk_widget_set_size_request(b_box, 230, -1);

        GtkWidget *b_toggle = gtk_button_new_with_label(item->available ? "Disable" : "Enable");
        if (item->available) {
            gtk_style_context_add_class(gtk_widget_get_style_context(b_toggle), "btn-secondary");
        } else {
            gtk_style_context_add_class(gtk_widget_get_style_context(b_toggle), "btn-success");
        }
        g_signal_connect(b_toggle, "clicked", G_CALLBACK(on_toggle_avail_clicked), GINT_TO_POINTER(item->id));

        GtkWidget *b_edit = gtk_button_new_with_label("Edit");
        gtk_style_context_add_class(gtk_widget_get_style_context(b_edit), "btn-primary");
        g_signal_connect(b_edit, "clicked", G_CALLBACK(on_edit_menu_clicked), GINT_TO_POINTER(item->id));

        GtkWidget *b_del = gtk_button_new_with_label("Delete");
        gtk_style_context_add_class(gtk_widget_get_style_context(b_del), "btn-danger");
        g_signal_connect(b_del, "clicked", G_CALLBACK(on_delete_menu_clicked), GINT_TO_POINTER(item->id));

        gtk_box_pack_start(GTK_BOX(b_box), b_toggle, FALSE, FALSE, 0);
        gtk_box_pack_start(GTK_BOX(b_box), b_edit, FALSE, FALSE, 0);
        gtk_box_pack_start(GTK_BOX(b_box), b_del, FALSE, FALSE, 0);

        gtk_box_pack_start(GTK_BOX(row), l_id, FALSE, FALSE, 0);
        gtk_box_pack_start(GTK_BOX(row), l_name, TRUE, TRUE, 0);
        gtk_box_pack_start(GTK_BOX(row), l_cat, FALSE, FALSE, 0);
        gtk_box_pack_start(GTK_BOX(row), l_price, FALSE, FALSE, 0);
        gtk_box_pack_start(GTK_BOX(row), l_stat, FALSE, FALSE, 0);
        gtk_box_pack_start(GTK_BOX(row), b_box, FALSE, FALSE, 0);

        gtk_box_pack_start(GTK_BOX(card), row, FALSE, FALSE, 0);
    }

    if (shown == 0) {
        GtkWidget *l_none = gtk_label_new("No menu items match your search or filter.");
        gtk_box_pack_start(GTK_BOX(card), l_none, TRUE, TRUE, 0);
    }

    gtk_container_add(GTK_CONTAINER(scrolled), card);
    gtk_box_pack_start(GTK_BOX(root), scrolled, TRUE, TRUE, 0);

    gtk_box_pack_start(GTK_BOX(g_menu_container), root, TRUE, TRUE, 0);
    gtk_widget_show_all(g_menu_container);
}

/* ============================================================================
 * MODULE 3: TABLE MANAGEMENT VIEW (RELIABLE EXPANSION & VIEWPORT FIX)
 * ============================================================================
 */
static void on_table_assign_clicked(GtkButton *btn, gpointer user_data) {
    (void)btn;
    int table_id = GPOINTER_TO_INT(user_data);
    char err[128];
    if (!assignTable(table_id, 0, err, sizeof(err))) {
        show_error_alert(g_main_window, "Table Assignment Error", err);
    } else {
        refresh_all_views();
    }
}

static void on_table_reserve_clicked(GtkButton *btn, gpointer user_data) {
    (void)btn;
    int table_id = GPOINTER_TO_INT(user_data);
    
    GtkWidget *dialog = gtk_dialog_new_with_buttons(
        "Reserve Table",
        GTK_WINDOW(g_main_window),
        GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
        "Cancel", GTK_RESPONSE_CANCEL,
        "Reserve", GTK_RESPONSE_ACCEPT,
        NULL
    );
    gtk_window_set_default_size(GTK_WINDOW(dialog), 340, 180);

    GtkWidget *content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_margin_start(box, 16);
    gtk_widget_set_margin_end(box, 16);
    gtk_widget_set_margin_top(box, 16);
    gtk_widget_set_margin_bottom(box, 16);

    GtkWidget *lbl = gtk_label_new("Guest / Reservation Name:");
    GtkWidget *entry = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(entry), "Reserved Guest");

    gtk_box_pack_start(GTK_BOX(box), lbl, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box), entry, FALSE, FALSE, 0);
    gtk_container_add(GTK_CONTAINER(content), box);
    gtk_widget_show_all(dialog);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        const char *guest = gtk_entry_get_text(GTK_ENTRY(entry));
        char err[128];
        if (!reserveTable(table_id, guest, err, sizeof(err))) {
            show_error_alert(g_main_window, "Reservation Error", err);
        } else {
            refresh_all_views();
        }
    }
    gtk_widget_destroy(dialog);
}

static void on_table_release_clicked(GtkButton *btn, gpointer user_data) {
    (void)btn;
    int table_id = GPOINTER_TO_INT(user_data);
    char err[128];
    if (!releaseTable(table_id, err, sizeof(err))) {
        show_error_alert(g_main_window, "Table Release Error", err);
    } else {
        refresh_all_views();
    }
}

static void on_table_view_order_clicked(GtkButton *btn, gpointer user_data) {
    (void)btn;
    int order_id = GPOINTER_TO_INT(user_data);
    if (order_id > 0) {
        g_selected_order_id = order_id;
        switch_to_page("orders");
    }
}

static void refresh_tables_view(void) {
    if (!g_tables_container) return;
    clear_container(g_tables_container);

    GtkWidget *root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 16);
    gtk_widget_set_margin_start(root, 20);
    gtk_widget_set_margin_end(root, 20);
    gtk_widget_set_margin_top(root, 20);
    gtk_widget_set_margin_bottom(root, 20);
    gtk_widget_set_vexpand(root, TRUE);
    gtk_widget_set_hexpand(root, TRUE);

    GtkWidget *lbl_head = gtk_label_new("Table Management & Floor Seating Overview");
    gtk_style_context_add_class(gtk_widget_get_style_context(lbl_head), "page-title");
    gtk_box_pack_start(GTK_BOX(root), lbl_head, FALSE, FALSE, 0);

    /* Scrolled Window with explicit expansion */
    GtkWidget *scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_vexpand(scrolled, TRUE);
    gtk_widget_set_hexpand(scrolled, TRUE);

    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 16);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 16);
    gtk_widget_set_vexpand(grid, TRUE);
    gtk_widget_set_hexpand(grid, TRUE);

    int count = getTableCount();
    char buf[128];
    for (int i = 0; i < count; i++) {
        Table *tbl = getTable(i);
        if (!tbl) continue;

        GtkWidget *card = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
        gtk_style_context_add_class(gtk_widget_get_style_context(card), "table-card");
        gtk_widget_set_size_request(card, 300, 190);

        /* Table ID Badge & Capacity */
        GtkWidget *top_row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
        snprintf(buf, sizeof(buf), "TABLE T%02d", tbl->id);
        GtkWidget *l_tid = gtk_label_new(buf);
        gtk_style_context_add_class(gtk_widget_get_style_context(l_tid), "stat-title");

        snprintf(buf, sizeof(buf), "%d Seats", tbl->capacity);
        GtkWidget *l_cap = gtk_label_new(buf);
        gtk_widget_set_halign(l_cap, GTK_ALIGN_END);
        GtkWidget *sp = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
        gtk_widget_set_hexpand(sp, TRUE);

        gtk_box_pack_start(GTK_BOX(top_row), l_tid, FALSE, FALSE, 0);
        gtk_box_pack_start(GTK_BOX(top_row), sp, TRUE, TRUE, 0);
        gtk_box_pack_start(GTK_BOX(top_row), l_cap, FALSE, FALSE, 0);
        gtk_box_pack_start(GTK_BOX(card), top_row, FALSE, FALSE, 0);

        /* Status Badge */
        const char *st_str = getTableStatusString(tbl->status);
        GtkWidget *l_status = gtk_label_new(st_str);
        gtk_widget_set_halign(l_status, GTK_ALIGN_START);
        if (tbl->status == TABLE_AVAILABLE) {
            gtk_style_context_add_class(gtk_widget_get_style_context(l_status), "badge-available");
        } else if (tbl->status == TABLE_OCCUPIED) {
            gtk_style_context_add_class(gtk_widget_get_style_context(l_status), "badge-occupied");
        } else {
            gtk_style_context_add_class(gtk_widget_get_style_context(l_status), "badge-reserved");
        }
        gtk_box_pack_start(GTK_BOX(card), l_status, FALSE, FALSE, 0);

        /* Info Line: Current Order or Guest */
        if (tbl->status == TABLE_OCCUPIED && tbl->current_order_id > 0) {
            Order *ord = findOrderById(tbl->current_order_id);
            snprintf(buf, sizeof(buf), "Order #%d (%s)", tbl->current_order_id, ord ? ord->customer_name : "Active Guest");
            GtkWidget *l_info = gtk_label_new(buf);
            gtk_widget_set_halign(l_info, GTK_ALIGN_START);
            gtk_box_pack_start(GTK_BOX(card), l_info, FALSE, FALSE, 0);
        } else if (tbl->status == TABLE_RESERVED && strlen(tbl->reserved_for) > 0) {
            snprintf(buf, sizeof(buf), "Reserved for: %s", tbl->reserved_for);
            GtkWidget *l_info = gtk_label_new(buf);
            gtk_widget_set_halign(l_info, GTK_ALIGN_START);
            gtk_box_pack_start(GTK_BOX(card), l_info, FALSE, FALSE, 0);
        } else {
            GtkWidget *l_info = gtk_label_new("Ready for walk-in guests");
            gtk_widget_set_halign(l_info, GTK_ALIGN_START);
            gtk_style_context_add_class(gtk_widget_get_style_context(l_info), "stat-title");
            gtk_box_pack_start(GTK_BOX(card), l_info, FALSE, FALSE, 0);
        }

        /* Action Buttons */
        GtkWidget *btn_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
        if (tbl->status == TABLE_AVAILABLE) {
            GtkWidget *b_assign = gtk_button_new_with_label("Assign Table");
            gtk_style_context_add_class(gtk_widget_get_style_context(b_assign), "btn-success");
            g_signal_connect(b_assign, "clicked", G_CALLBACK(on_table_assign_clicked), GINT_TO_POINTER(tbl->id));

            GtkWidget *b_res = gtk_button_new_with_label("Reserve");
            gtk_style_context_add_class(gtk_widget_get_style_context(b_res), "btn-warning");
            g_signal_connect(b_res, "clicked", G_CALLBACK(on_table_reserve_clicked), GINT_TO_POINTER(tbl->id));

            gtk_box_pack_start(GTK_BOX(btn_box), b_assign, TRUE, TRUE, 0);
            gtk_box_pack_start(GTK_BOX(btn_box), b_res, TRUE, TRUE, 0);
        } else if (tbl->status == TABLE_OCCUPIED) {
            GtkWidget *b_view = gtk_button_new_with_label("View Order");
            gtk_style_context_add_class(gtk_widget_get_style_context(b_view), "btn-primary");
            g_signal_connect(b_view, "clicked", G_CALLBACK(on_table_view_order_clicked), GINT_TO_POINTER(tbl->current_order_id));

            GtkWidget *b_rel = gtk_button_new_with_label("Release");
            gtk_style_context_add_class(gtk_widget_get_style_context(b_rel), "btn-danger");
            g_signal_connect(b_rel, "clicked", G_CALLBACK(on_table_release_clicked), GINT_TO_POINTER(tbl->id));

            gtk_box_pack_start(GTK_BOX(btn_box), b_view, TRUE, TRUE, 0);
            gtk_box_pack_start(GTK_BOX(btn_box), b_rel, TRUE, TRUE, 0);
        } else {
            GtkWidget *b_assign = gtk_button_new_with_label("Occupy Guest");
            gtk_style_context_add_class(gtk_widget_get_style_context(b_assign), "btn-success");
            g_signal_connect(b_assign, "clicked", G_CALLBACK(on_table_assign_clicked), GINT_TO_POINTER(tbl->id));

            GtkWidget *b_rel = gtk_button_new_with_label("Cancel Reservation");
            gtk_style_context_add_class(gtk_widget_get_style_context(b_rel), "btn-secondary");
            g_signal_connect(b_rel, "clicked", G_CALLBACK(on_table_release_clicked), GINT_TO_POINTER(tbl->id));

            gtk_box_pack_start(GTK_BOX(btn_box), b_assign, TRUE, TRUE, 0);
            gtk_box_pack_start(GTK_BOX(btn_box), b_rel, TRUE, TRUE, 0);
        }

        gtk_box_pack_start(GTK_BOX(card), btn_box, FALSE, FALSE, 0);

        int row_idx = i / 3;
        int col_idx = i % 3;
        gtk_grid_attach(GTK_GRID(grid), card, col_idx, row_idx, 1, 1);
    }

    gtk_container_add(GTK_CONTAINER(scrolled), grid);
    gtk_box_pack_start(GTK_BOX(root), scrolled, TRUE, TRUE, 0);

    /* Use pack_start for g_tables_container to guarantee full expansion */
    gtk_box_pack_start(GTK_BOX(g_tables_container), root, TRUE, TRUE, 0);
    gtk_widget_show_all(g_tables_container);
}

/* ============================================================================
 * MODULE 4: ORDER MANAGEMENT VIEW
 * ============================================================================
 */
static void show_create_order_dialog(void) {
    GtkWidget *dialog = gtk_dialog_new_with_buttons(
        "Create New Customer Order",
        GTK_WINDOW(g_main_window),
        GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
        "Cancel", GTK_RESPONSE_CANCEL,
        "Create Order", GTK_RESPONSE_ACCEPT,
        NULL
    );
    gtk_window_set_default_size(GTK_WINDOW(dialog), 380, 240);

    GtkWidget *content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 12);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 12);
    gtk_widget_set_margin_start(grid, 20);
    gtk_widget_set_margin_end(grid, 20);
    gtk_widget_set_margin_top(grid, 20);
    gtk_widget_set_margin_bottom(grid, 20);

    /* Table Selector */
    GtkWidget *lbl_tbl = gtk_label_new("Assign Table:");
    GtkWidget *combo_tbl = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_tbl), "None (Walk-in / Takeaway)");

    int t_count = getTableCount();
    char buf[64];
    int valid_table_ids[MAX_TABLES];
    int valid_count = 0;

    for (int i = 0; i < t_count; i++) {
        Table *t = getTable(i);
        if (t && t->status == TABLE_AVAILABLE) {
            snprintf(buf, sizeof(buf), "Table T%02d (%d seats)", t->id, t->capacity);
            gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_tbl), buf);
            valid_table_ids[valid_count++] = t->id;
        }
    }
    gtk_combo_box_set_active(GTK_COMBO_BOX(combo_tbl), 0);

    gtk_grid_attach(GTK_GRID(grid), lbl_tbl, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), combo_tbl, 1, 0, 1, 1);

    /* Customer Name */
    GtkWidget *lbl_cust = gtk_label_new("Customer Name:");
    GtkWidget *entry_cust = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(entry_cust), "Walk-in Customer");
    gtk_grid_attach(GTK_GRID(grid), lbl_cust, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), entry_cust, 1, 1, 1, 1);

    gtk_container_add(GTK_CONTAINER(content), grid);
    gtk_widget_show_all(dialog);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        int sel = gtk_combo_box_get_active(GTK_COMBO_BOX(combo_tbl));
        int table_id = 0;
        if (sel > 0 && sel <= valid_count) {
            table_id = valid_table_ids[sel - 1];
        }
        const char *cname = gtk_entry_get_text(GTK_ENTRY(entry_cust));
        char err[128];
        int new_id = createOrder(table_id, cname, err, sizeof(err));
        if (new_id > 0) {
            g_selected_order_id = new_id;
            refresh_all_views();
        } else {
            show_error_alert(g_main_window, "Order Creation Error", err);
        }
    }

    gtk_widget_destroy(dialog);
}

static void on_create_order_clicked(GtkButton *btn, gpointer user_data) {
    (void)btn;
    (void)user_data;
    show_create_order_dialog();
}

static void on_select_order_row(GtkButton *btn, gpointer user_data) {
    (void)btn;
    g_selected_order_id = GPOINTER_TO_INT(user_data);
    refresh_orders_view();
}

static void on_add_item_to_order_clicked(GtkButton *btn, gpointer user_data) {
    (void)btn;
    GtkWidget *combo = GTK_WIDGET(user_data);
    gchar *active_str = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(combo));
    if (!active_str) return;

    int item_id = 0;
    sscanf(active_str, "[%d]", &item_id);
    g_free(active_str);

    Order *ord = findOrderById(g_selected_order_id);
    if (ord && item_id > 0) {
        char err[128];
        if (!addItemToOrder(ord, item_id, 1, err, sizeof(err))) {
            show_error_alert(g_main_window, "Add Item Error", err);
        } else {
            refresh_all_views();
        }
    }
}

static void on_order_qty_inc(GtkButton *btn, gpointer user_data) {
    (void)btn;
    int item_id = GPOINTER_TO_INT(user_data);
    Order *ord = findOrderById(g_selected_order_id);
    if (ord) {
        addItemToOrder(ord, item_id, 1, NULL, 0);
        refresh_all_views();
    }
}

static void on_order_qty_dec(GtkButton *btn, gpointer user_data) {
    (void)btn;
    int item_id = GPOINTER_TO_INT(user_data);
    Order *ord = findOrderById(g_selected_order_id);
    if (ord) {
        for (int i = 0; i < ord->item_count; i++) {
            if (ord->items[i].item.id == item_id) {
                updateItemQuantity(ord, item_id, ord->items[i].quantity - 1, NULL, 0);
                break;
            }
        }
        refresh_all_views();
    }
}

static void on_order_item_remove(GtkButton *btn, gpointer user_data) {
    (void)btn;
    int item_id = GPOINTER_TO_INT(user_data);
    Order *ord = findOrderById(g_selected_order_id);
    if (ord) {
        removeItemFromOrder(ord, item_id, NULL, 0);
        refresh_all_views();
    }
}

static void on_order_status_change(GtkButton *btn, gpointer user_data) {
    (void)btn;
    OrderStatus new_st = (OrderStatus)GPOINTER_TO_INT(user_data);
    updateOrderStatus(g_selected_order_id, new_st, NULL, 0);
    refresh_all_views();
}

static void refresh_orders_view(void) {
    if (!g_orders_container) return;
    clear_container(g_orders_container);

    GtkWidget *root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 16);
    gtk_widget_set_margin_start(root, 20);
    gtk_widget_set_margin_end(root, 20);
    gtk_widget_set_margin_top(root, 20);
    gtk_widget_set_margin_bottom(root, 20);
    gtk_widget_set_vexpand(root, TRUE);
    gtk_widget_set_hexpand(root, TRUE);

    /* Header Bar */
    GtkWidget *hbar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
    GtkWidget *lbl_head = gtk_label_new("Order Management & Order History");
    gtk_style_context_add_class(gtk_widget_get_style_context(lbl_head), "page-title");
    gtk_box_pack_start(GTK_BOX(hbar), lbl_head, FALSE, FALSE, 0);

    GtkWidget *sp = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_pack_start(GTK_BOX(hbar), sp, TRUE, TRUE, 0);

    GtkWidget *btn_new = gtk_button_new_with_label("➕ Create New Order");
    gtk_style_context_add_class(gtk_widget_get_style_context(btn_new), "btn-success");
    g_signal_connect(btn_new, "clicked", G_CALLBACK(on_create_order_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(hbar), btn_new, FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(root), hbar, FALSE, FALSE, 0);

    /* Split Container: Left = Orders List (Active & History), Right = Selected Order Details */
    GtkWidget *body = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 16);
    gtk_widget_set_vexpand(body, TRUE);
    gtk_widget_set_hexpand(body, TRUE);

    /* Left Orders List */
    GtkWidget *left_panel = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_size_request(left_panel, 360, -1);
    gtk_style_context_add_class(gtk_widget_get_style_context(left_panel), "card-panel");

    GtkWidget *sc_left = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(sc_left), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_vexpand(sc_left, TRUE);

    GtkWidget *vbox_orders = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);

    int o_count = getOrderCount();
    if (g_selected_order_id == 0 && o_count > 0) {
        g_selected_order_id = getOrder(o_count - 1)->order_id;
    }

    /* Active Section Header */
    GtkWidget *l_act_head = gtk_label_new("⚡ ACTIVE ORDERS");
    gtk_widget_set_halign(l_act_head, GTK_ALIGN_START);
    gtk_style_context_add_class(gtk_widget_get_style_context(l_act_head), "stat-title");
    gtk_box_pack_start(GTK_BOX(vbox_orders), l_act_head, FALSE, FALSE, 0);

    char buf[128];
    for (int i = o_count - 1; i >= 0; i--) {
        Order *ord = getOrder(i);
        if (!ord) continue;

        if (ord->status != STATUS_COMPLETED && ord->status != STATUS_CANCELLED) {
            snprintf(buf, sizeof(buf), "#%d | T%02d | %s | Rs.%.2f [%s]",
                     ord->order_id, ord->table_id, ord->customer_name, ord->grand_total, getOrderStatusString(ord->status));
            GtkWidget *btn_ord = gtk_button_new_with_label(buf);

            if (ord->order_id == g_selected_order_id) {
                gtk_style_context_add_class(gtk_widget_get_style_context(btn_ord), "btn-primary");
            } else {
                gtk_style_context_add_class(gtk_widget_get_style_context(btn_ord), "btn-warning");
            }
            g_signal_connect(btn_ord, "clicked", G_CALLBACK(on_select_order_row), GINT_TO_POINTER(ord->order_id));
            gtk_box_pack_start(GTK_BOX(vbox_orders), btn_ord, FALSE, FALSE, 0);
        }
    }

    GtkWidget *sep_left = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_widget_set_margin_top(sep_left, 8);
    gtk_widget_set_margin_bottom(sep_left, 8);
    gtk_box_pack_start(GTK_BOX(vbox_orders), sep_left, FALSE, FALSE, 0);

    /* History Section Header */
    GtkWidget *l_hist_head = gtk_label_new("📜 ORDER HISTORY (COMPLETED)");
    gtk_widget_set_halign(l_hist_head, GTK_ALIGN_START);
    gtk_style_context_add_class(gtk_widget_get_style_context(l_hist_head), "stat-title");
    gtk_box_pack_start(GTK_BOX(vbox_orders), l_hist_head, FALSE, FALSE, 0);

    for (int i = o_count - 1; i >= 0; i--) {
        Order *ord = getOrder(i);
        if (!ord) continue;

        if (ord->status == STATUS_COMPLETED || ord->status == STATUS_CANCELLED) {
            snprintf(buf, sizeof(buf), "#%d | %s | Rs.%.2f [%s]",
                     ord->order_id, ord->customer_name, ord->grand_total, getOrderStatusString(ord->status));
            GtkWidget *btn_ord = gtk_button_new_with_label(buf);

            if (ord->order_id == g_selected_order_id) {
                gtk_style_context_add_class(gtk_widget_get_style_context(btn_ord), "btn-primary");
            } else {
                gtk_style_context_add_class(gtk_widget_get_style_context(btn_ord), "btn-secondary");
            }
            g_signal_connect(btn_ord, "clicked", G_CALLBACK(on_select_order_row), GINT_TO_POINTER(ord->order_id));
            gtk_box_pack_start(GTK_BOX(vbox_orders), btn_ord, FALSE, FALSE, 0);
        }
    }

    gtk_container_add(GTK_CONTAINER(sc_left), vbox_orders);
    gtk_box_pack_start(GTK_BOX(left_panel), sc_left, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(body), left_panel, FALSE, FALSE, 0);

    /* Right Order Details */
    GtkWidget *right_panel = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
    gtk_style_context_add_class(gtk_widget_get_style_context(right_panel), "card-panel");
    gtk_widget_set_vexpand(right_panel, TRUE);

    Order *sel_ord = findOrderById(g_selected_order_id);
    if (sel_ord) {
        /* Top Banner */
        snprintf(buf, sizeof(buf), "Order #%d Details | Table T%02d | Customer: %s | Date: %s",
                 sel_ord->order_id, sel_ord->table_id, sel_ord->customer_name, sel_ord->timestamp);
        GtkWidget *lbl_dt = gtk_label_new(buf);
        gtk_widget_set_halign(lbl_dt, GTK_ALIGN_START);
        gtk_style_context_add_class(gtk_widget_get_style_context(lbl_dt), "page-title");
        gtk_box_pack_start(GTK_BOX(right_panel), lbl_dt, FALSE, FALSE, 0);

        /* Status Controls */
        GtkWidget *st_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
        GtkWidget *l_st = gtk_label_new("Order Status:");
        snprintf(buf, sizeof(buf), "[ %s ]", getOrderStatusString(sel_ord->status));
        GtkWidget *badge_st = gtk_label_new(buf);
        gtk_style_context_add_class(gtk_widget_get_style_context(badge_st), "badge-status");

        gtk_box_pack_start(GTK_BOX(st_box), l_st, FALSE, FALSE, 0);
        gtk_box_pack_start(GTK_BOX(st_box), badge_st, FALSE, FALSE, 0);

        if (sel_ord->status != STATUS_COMPLETED && sel_ord->status != STATUS_CANCELLED) {
            GtkWidget *b_prep = gtk_button_new_with_label("Mark Preparing");
            gtk_style_context_add_class(gtk_widget_get_style_context(b_prep), "btn-warning");
            g_signal_connect(b_prep, "clicked", G_CALLBACK(on_order_status_change), GINT_TO_POINTER(STATUS_PREPARING));

            GtkWidget *b_ready = gtk_button_new_with_label("Mark Ready");
            gtk_style_context_add_class(gtk_widget_get_style_context(b_ready), "btn-success");
            g_signal_connect(b_ready, "clicked", G_CALLBACK(on_order_status_change), GINT_TO_POINTER(STATUS_READY));

            GtkWidget *b_serv = gtk_button_new_with_label("Mark Served");
            gtk_style_context_add_class(gtk_widget_get_style_context(b_serv), "btn-primary");
            g_signal_connect(b_serv, "clicked", G_CALLBACK(on_order_status_change), GINT_TO_POINTER(STATUS_SERVED));

            gtk_box_pack_start(GTK_BOX(st_box), b_prep, FALSE, FALSE, 0);
            gtk_box_pack_start(GTK_BOX(st_box), b_ready, FALSE, FALSE, 0);
            gtk_box_pack_start(GTK_BOX(st_box), b_serv, FALSE, FALSE, 0);
        }

        gtk_box_pack_start(GTK_BOX(right_panel), st_box, FALSE, FALSE, 0);

        /* Add Item to Order Row (if active) */
        if (sel_ord->status != STATUS_COMPLETED && sel_ord->status != STATUS_CANCELLED) {
            GtkWidget *add_row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
            GtkWidget *combo_items = gtk_combo_box_text_new();
            int m_count = getMenuCount();
            for (int i = 0; i < m_count; i++) {
                const FoodItem *fi = getMenuItem(i);
                if (fi && fi->available) {
                    snprintf(buf, sizeof(buf), "[%d] %s - Rs.%.2f", fi->id, fi->name, fi->price);
                    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_items), buf);
                }
            }
            gtk_combo_box_set_active(GTK_COMBO_BOX(combo_items), 0);
            gtk_box_pack_start(GTK_BOX(add_row), combo_items, TRUE, TRUE, 0);

            GtkWidget *btn_add_itm = gtk_button_new_with_label("➕ Add Item to Order");
            gtk_style_context_add_class(gtk_widget_get_style_context(btn_add_itm), "btn-primary");
            g_signal_connect(btn_add_itm, "clicked", G_CALLBACK(on_add_item_to_order_clicked), combo_items);
            gtk_box_pack_start(GTK_BOX(add_row), btn_add_itm, FALSE, FALSE, 0);

            gtk_box_pack_start(GTK_BOX(right_panel), add_row, FALSE, FALSE, 0);
        }

        /* Items List */
        GtkWidget *sc_items = gtk_scrolled_window_new(NULL, NULL);
        gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(sc_items), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
        gtk_widget_set_vexpand(sc_items, TRUE);

        GtkWidget *vbox_itms = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);

        for (int i = 0; i < sel_ord->item_count; i++) {
            GtkWidget *i_row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);

            GtkWidget *l_iname = gtk_label_new(sel_ord->items[i].item.name);
            gtk_widget_set_hexpand(l_iname, TRUE);
            gtk_widget_set_halign(l_iname, GTK_ALIGN_START);

            snprintf(buf, sizeof(buf), "Rs. %.2f each", sel_ord->items[i].item.price);
            GtkWidget *l_iprice = gtk_label_new(buf);

            /* Qty controls */
            GtkWidget *b_dec = gtk_button_new_with_label("-");
            gtk_style_context_add_class(gtk_widget_get_style_context(b_dec), "btn-danger");
            g_signal_connect(b_dec, "clicked", G_CALLBACK(on_order_qty_dec), GINT_TO_POINTER(sel_ord->items[i].item.id));

            snprintf(buf, sizeof(buf), " %d ", sel_ord->items[i].quantity);
            GtkWidget *l_qty = gtk_label_new(buf);

            GtkWidget *b_inc = gtk_button_new_with_label("+");
            gtk_style_context_add_class(gtk_widget_get_style_context(b_inc), "btn-success");
            g_signal_connect(b_inc, "clicked", G_CALLBACK(on_order_qty_inc), GINT_TO_POINTER(sel_ord->items[i].item.id));

            snprintf(buf, sizeof(buf), "Rs. %.2f", sel_ord->items[i].amount);
            GtkWidget *l_amt = gtk_label_new(buf);
            gtk_style_context_add_class(gtk_widget_get_style_context(l_amt), "stat-title");

            GtkWidget *b_rem = gtk_button_new_with_label("✕");
            gtk_style_context_add_class(gtk_widget_get_style_context(b_rem), "btn-danger");
            g_signal_connect(b_rem, "clicked", G_CALLBACK(on_order_item_remove), GINT_TO_POINTER(sel_ord->items[i].item.id));

            gtk_box_pack_start(GTK_BOX(i_row), l_iname, TRUE, TRUE, 0);
            gtk_box_pack_start(GTK_BOX(i_row), l_iprice, FALSE, FALSE, 0);
            if (sel_ord->status != STATUS_COMPLETED && sel_ord->status != STATUS_CANCELLED) {
                gtk_box_pack_start(GTK_BOX(i_row), b_dec, FALSE, FALSE, 0);
                gtk_box_pack_start(GTK_BOX(i_row), l_qty, FALSE, FALSE, 0);
                gtk_box_pack_start(GTK_BOX(i_row), b_inc, FALSE, FALSE, 0);
            } else {
                gtk_box_pack_start(GTK_BOX(i_row), l_qty, FALSE, FALSE, 0);
            }
            gtk_box_pack_start(GTK_BOX(i_row), l_amt, FALSE, FALSE, 0);
            if (sel_ord->status != STATUS_COMPLETED && sel_ord->status != STATUS_CANCELLED) {
                gtk_box_pack_start(GTK_BOX(i_row), b_rem, FALSE, FALSE, 0);
            }

            gtk_box_pack_start(GTK_BOX(vbox_itms), i_row, FALSE, FALSE, 0);
        }

        gtk_container_add(GTK_CONTAINER(sc_items), vbox_itms);
        gtk_box_pack_start(GTK_BOX(right_panel), sc_items, TRUE, TRUE, 0);

        /* Totals Card */
        calculateTotals(sel_ord);
        snprintf(buf, sizeof(buf), "Subtotal: Rs.%.2f | Discount (5%%): -Rs.%.2f | GST (5%%): +Rs.%.2f | Grand Total: Rs.%.2f",
                 sel_ord->subtotal, sel_ord->discount_amount, sel_ord->tax_amount, sel_ord->grand_total);
        GtkWidget *l_totals = gtk_label_new(buf);
        gtk_style_context_add_class(gtk_widget_get_style_context(l_totals), "stat-value");
        gtk_box_pack_start(GTK_BOX(right_panel), l_totals, FALSE, FALSE, 0);
    } else {
        GtkWidget *l_empty = gtk_label_new("No order selected. Click 'Create New Order' or pick an order from the list.");
        gtk_box_pack_start(GTK_BOX(right_panel), l_empty, TRUE, TRUE, 0);
    }

    gtk_box_pack_start(GTK_BOX(body), right_panel, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(root), body, TRUE, TRUE, 0);

    gtk_box_pack_start(GTK_BOX(g_orders_container), root, TRUE, TRUE, 0);
    gtk_widget_show_all(g_orders_container);
}

/* ============================================================================
 * MODULE 5: KITCHEN MANAGEMENT VIEW
 * ============================================================================
 */
static void on_kitchen_status_clicked(GtkButton *btn, gpointer user_data) {
    (void)btn;
    int order_id = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(btn), "order_id"));
    OrderStatus status = (OrderStatus)GPOINTER_TO_INT(user_data);

    updateOrderStatus(order_id, status, NULL, 0);
    refresh_all_views();
}

static void refresh_kitchen_view(void) {
    if (!g_kitchen_container) return;
    clear_container(g_kitchen_container);

    GtkWidget *root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 16);
    gtk_widget_set_margin_start(root, 20);
    gtk_widget_set_margin_end(root, 20);
    gtk_widget_set_margin_top(root, 20);
    gtk_widget_set_margin_bottom(root, 20);
    gtk_widget_set_vexpand(root, TRUE);
    gtk_widget_set_hexpand(root, TRUE);

    GtkWidget *lbl_head = gtk_label_new("Kitchen Display System (KDS - Active Orders)");
    gtk_style_context_add_class(gtk_widget_get_style_context(lbl_head), "page-title");
    gtk_box_pack_start(GTK_BOX(root), lbl_head, FALSE, FALSE, 0);

    GtkWidget *scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_vexpand(scrolled, TRUE);

    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 16);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 16);

    int o_count = getOrderCount();
    int active_k_count = 0;
    char buf[128];

    for (int i = 0; i < o_count; i++) {
        Order *ord = getOrder(i);
        if (!ord) continue;

        /* Kitchen view displays orders with PENDING, PREPARING, READY, or SERVED status */
        if (ord->status != STATUS_COMPLETED && ord->status != STATUS_CANCELLED) {
            GtkWidget *card = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
            gtk_style_context_add_class(gtk_widget_get_style_context(card), "table-card");
            gtk_widget_set_size_request(card, 300, 220);

            snprintf(buf, sizeof(buf), "ORDER #%d  (Table T%02d)", ord->order_id, ord->table_id);
            GtkWidget *l_title = gtk_label_new(buf);
            gtk_style_context_add_class(gtk_widget_get_style_context(l_title), "stat-title");

            snprintf(buf, sizeof(buf), "STATUS: %s", getOrderStatusString(ord->status));
            GtkWidget *l_st = gtk_label_new(buf);
            gtk_style_context_add_class(gtk_widget_get_style_context(l_st), "badge-status");

            gtk_box_pack_start(GTK_BOX(card), l_title, FALSE, FALSE, 0);
            gtk_box_pack_start(GTK_BOX(card), l_st, FALSE, FALSE, 0);

            GtkWidget *sep = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
            gtk_box_pack_start(GTK_BOX(card), sep, FALSE, FALSE, 0);

            /* Items */
            for (int it = 0; it < ord->item_count; it++) {
                snprintf(buf, sizeof(buf), "• %s  x %d", ord->items[it].item.name, ord->items[it].quantity);
                GtkWidget *l_item = gtk_label_new(buf);
                gtk_widget_set_halign(l_item, GTK_ALIGN_START);
                gtk_box_pack_start(GTK_BOX(card), l_item, FALSE, FALSE, 0);
            }

            /* Action Buttons */
            GtkWidget *b_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);

            GtkWidget *b_prep = gtk_button_new_with_label("Preparing");
            gtk_style_context_add_class(gtk_widget_get_style_context(b_prep), "btn-warning");
            g_object_set_data(G_OBJECT(b_prep), "order_id", GINT_TO_POINTER(ord->order_id));
            g_signal_connect(b_prep, "clicked", G_CALLBACK(on_kitchen_status_clicked), GINT_TO_POINTER(STATUS_PREPARING));

            GtkWidget *b_ready = gtk_button_new_with_label("Ready");
            gtk_style_context_add_class(gtk_widget_get_style_context(b_ready), "btn-success");
            g_object_set_data(G_OBJECT(b_ready), "order_id", GINT_TO_POINTER(ord->order_id));
            g_signal_connect(b_ready, "clicked", G_CALLBACK(on_kitchen_status_clicked), GINT_TO_POINTER(STATUS_READY));

            GtkWidget *b_serv = gtk_button_new_with_label("Served");
            gtk_style_context_add_class(gtk_widget_get_style_context(b_serv), "btn-primary");
            g_object_set_data(G_OBJECT(b_serv), "order_id", GINT_TO_POINTER(ord->order_id));
            g_signal_connect(b_serv, "clicked", G_CALLBACK(on_kitchen_status_clicked), GINT_TO_POINTER(STATUS_SERVED));

            gtk_box_pack_start(GTK_BOX(b_box), b_prep, TRUE, TRUE, 0);
            gtk_box_pack_start(GTK_BOX(b_box), b_ready, TRUE, TRUE, 0);
            gtk_box_pack_start(GTK_BOX(b_box), b_serv, TRUE, TRUE, 0);
            gtk_box_pack_start(GTK_BOX(card), b_box, FALSE, FALSE, 0);

            int r = active_k_count / 3;
            int c = active_k_count % 3;
            gtk_grid_attach(GTK_GRID(grid), card, c, r, 1, 1);
            active_k_count++;
        }
    }

    if (active_k_count == 0) {
        GtkWidget *l_empty = gtk_label_new("No active orders in kitchen queue.");
        gtk_box_pack_start(GTK_BOX(root), l_empty, TRUE, TRUE, 0);
    } else {
        gtk_container_add(GTK_CONTAINER(scrolled), grid);
        gtk_box_pack_start(GTK_BOX(root), scrolled, TRUE, TRUE, 0);
    }

    gtk_box_pack_start(GTK_BOX(g_kitchen_container), root, TRUE, TRUE, 0);
    gtk_widget_show_all(g_kitchen_container);
}

/* ============================================================================
 * MODULE 6: BILLING VIEW
 * ============================================================================
 */
static void show_receipt_dialog(Order *ord) {
    if (!ord) return;
    calculateTotals(ord);

    GtkWidget *dialog = gtk_dialog_new_with_buttons(
        "Final Tax Invoice Receipt",
        GTK_WINDOW(g_main_window),
        GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
        "Close Receipt", GTK_RESPONSE_CLOSE,
        NULL
    );
    gtk_window_set_default_size(GTK_WINDOW(dialog), 540, 680);

    GtkWidget *content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget *scrolled = gtk_scrolled_window_new(NULL, NULL);

    char receipt_buf[4096];
    size_t offset = 0;

    offset += snprintf(receipt_buf + offset, sizeof(receipt_buf) - offset,
        "============================================================\n"
        "                    SPICE BAVARCHI RESTAURANT\n"
        "                  \"Great food. Great moments.\"\n"
        "                   FINAL TAX INVOICE / RECEIPT\n"
        "============================================================\n"
        "  Order ID : #%-10d Date/Time: %s\n"
        "  Table    : T%02d        Customer : %s\n"
        "------------------------------------------------------------\n"
        "  %-24s %-5s %-9s %-10s\n"
        "------------------------------------------------------------\n",
        ord->order_id, ord->timestamp, ord->table_id, ord->customer_name,
        "Item Description", "Qty", "Rate", "Amount");

    for (int i = 0; i < ord->item_count; i++) {
        offset += snprintf(receipt_buf + offset, sizeof(receipt_buf) - offset,
            "  %-24s %-5d Rs. %-6.2f Rs. %-7.2f\n",
            ord->items[i].item.name,
            ord->items[i].quantity,
            ord->items[i].item.price,
            ord->items[i].amount);
    }

    double taxable_amount = ord->subtotal - ord->discount_amount;

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
        ord->subtotal, ord->discount_amount, taxable_amount,
        ord->tax_amount, ord->grand_total);

    GtkWidget *text_view = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(text_view), FALSE);
    gtk_text_view_set_monospace(GTK_TEXT_VIEW(text_view), TRUE);

    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
    gtk_text_buffer_set_text(buffer, receipt_buf, -1);

    gtk_container_add(GTK_CONTAINER(scrolled), text_view);
    gtk_container_add(GTK_CONTAINER(content), scrolled);
    gtk_widget_show_all(dialog);

    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

static void on_billing_combo_changed(GtkComboBox *combo, gpointer user_data) {
    (void)user_data;
    gchar *active_text = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(combo));
    if (active_text) {
        int order_id = 0;
        if (sscanf(active_text, "Order #%d", &order_id) == 1 && order_id > 0) {
            g_selected_order_id = order_id;
            refresh_billing_view();
        }
        g_free(active_text);
    }
}

static void on_view_receipt_clicked(GtkButton *btn, gpointer user_data) {
    (void)btn;
    (void)user_data;
    Order *ord = findOrderById(g_selected_order_id);
    if (ord) {
        show_receipt_dialog(ord);
    }
}

static void on_save_receipt_clicked(GtkButton *btn, gpointer user_data) {
    (void)btn;
    (void)user_data;
    Order *ord = findOrderById(g_selected_order_id);
    if (!ord) return;

    calculateTotals(ord);
    char fname[64];
    snprintf(fname, sizeof(fname), "receipt_%d.txt", ord->order_id);
    FILE *fp = fopen(fname, "w");
    if (fp) {
        fprintf(fp,
            "============================================================\n"
            "                    SPICE BAVARCHI RESTAURANT\n"
            "                  \"Great food. Great moments.\"\n"
            "                   FINAL TAX INVOICE / RECEIPT\n"
            "============================================================\n"
            "  Order ID : #%-10d Date/Time: %s\n"
            "  Table    : T%02d        Customer : %s\n"
            "------------------------------------------------------------\n"
            "  %-24s %-5s %-9s %-10s\n"
            "------------------------------------------------------------\n",
            ord->order_id, ord->timestamp, ord->table_id, ord->customer_name,
            "Item Description", "Qty", "Rate", "Amount");

        for (int i = 0; i < ord->item_count; i++) {
            fprintf(fp, "  %-24s %-5d Rs. %-6.2f Rs. %-7.2f\n",
                ord->items[i].item.name, ord->items[i].quantity,
                ord->items[i].item.price, ord->items[i].amount);
        }

        double taxable_amount = ord->subtotal - ord->discount_amount;
        fprintf(fp,
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
            ord->subtotal, ord->discount_amount, taxable_amount,
            ord->tax_amount, ord->grand_total);
        fclose(fp);

        char msg[128];
        snprintf(msg, sizeof(msg), "Receipt successfully saved to file: %s", fname);
        show_info_alert(g_main_window, "Receipt Saved", msg);
    }
}

static void on_complete_payment_clicked(GtkButton *btn, gpointer user_data) {
    (void)btn;
    (void)user_data;
    Order *ord = findOrderById(g_selected_order_id);
    if (!ord) return;

    if (ord->status == STATUS_COMPLETED) {
        show_info_alert(g_main_window, "Already Paid", "This order is already marked as COMPLETED.");
        return;
    }

    updateOrderStatus(ord->order_id, STATUS_COMPLETED, NULL, 0);
    show_info_alert(g_main_window, "Payment Complete", "Payment received successfully! Order completed & Table released.");
    refresh_all_views();
}

static void refresh_billing_view(void) {
    if (!g_billing_container) return;
    clear_container(g_billing_container);

    GtkWidget *root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 16);
    gtk_widget_set_margin_start(root, 20);
    gtk_widget_set_margin_end(root, 20);
    gtk_widget_set_margin_top(root, 20);
    gtk_widget_set_margin_bottom(root, 20);
    gtk_widget_set_vexpand(root, TRUE);
    gtk_widget_set_hexpand(root, TRUE);

    GtkWidget *hbar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
    GtkWidget *lbl_head = gtk_label_new("Billing & Cashier Terminal");
    gtk_style_context_add_class(gtk_widget_get_style_context(lbl_head), "page-title");
    gtk_box_pack_start(GTK_BOX(hbar), lbl_head, FALSE, FALSE, 0);

    /* Selector Combo for selecting order to bill */
    GtkWidget *lbl_sel = gtk_label_new("Select Order:");
    GtkWidget *combo_orders = gtk_combo_box_text_new();
    int o_count = getOrderCount();
    char buf[128];

    int active_idx = 0;
    int cur_idx = 0;

    for (int i = o_count - 1; i >= 0; i--) {
        Order *o = getOrder(i);
        if (o) {
            snprintf(buf, sizeof(buf), "Order #%d - %s (Rs.%.2f, %s)",
                     o->order_id, o->customer_name, o->grand_total, getOrderStatusString(o->status));
            gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_orders), buf);
            if (o->order_id == g_selected_order_id) {
                active_idx = cur_idx;
            }
            cur_idx++;
        }
    }
    if (cur_idx > 0) {
        gtk_combo_box_set_active(GTK_COMBO_BOX(combo_orders), active_idx);
    }
    g_signal_connect(combo_orders, "changed", G_CALLBACK(on_billing_combo_changed), NULL);

    gtk_box_pack_start(GTK_BOX(hbar), lbl_sel, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbar), combo_orders, FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(root), hbar, FALSE, FALSE, 0);

    Order *ord = findOrderById(g_selected_order_id);
    if (!ord && getOrderCount() > 0) {
        ord = getOrder(getOrderCount() - 1);
        if (ord) g_selected_order_id = ord->order_id;
    }

    if (ord) {
        calculateTotals(ord);
        GtkWidget *card = gtk_box_new(GTK_ORIENTATION_VERTICAL, 14);
        gtk_style_context_add_class(gtk_widget_get_style_context(card), "card-panel");
        gtk_widget_set_vexpand(card, TRUE);

        snprintf(buf, sizeof(buf), "Tax Invoice for Order #%d | Table T%02d | Customer: %s | Status: %s",
                 ord->order_id, ord->table_id, ord->customer_name, getOrderStatusString(ord->status));
        GtkWidget *l_title = gtk_label_new(buf);
        gtk_style_context_add_class(gtk_widget_get_style_context(l_title), "stat-title");
        gtk_box_pack_start(GTK_BOX(card), l_title, FALSE, FALSE, 0);

        /* Table breakdown */
        GtkWidget *sc = gtk_scrolled_window_new(NULL, NULL);
        gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(sc), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
        gtk_widget_set_vexpand(sc, TRUE);

        GtkWidget *v_items = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);

        for (int i = 0; i < ord->item_count; i++) {
            snprintf(buf, sizeof(buf), "%-32s x%-3d @ Rs.%-7.2f  = Rs.%.2f",
                     ord->items[i].item.name, ord->items[i].quantity,
                     ord->items[i].item.price, ord->items[i].amount);
            GtkWidget *l_it = gtk_label_new(buf);
            gtk_widget_set_halign(l_it, GTK_ALIGN_START);
            gtk_box_pack_start(GTK_BOX(v_items), l_it, FALSE, FALSE, 0);
        }
        gtk_container_add(GTK_CONTAINER(sc), v_items);
        gtk_box_pack_start(GTK_BOX(card), sc, TRUE, TRUE, 0);

        GtkWidget *sep = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
        gtk_box_pack_start(GTK_BOX(card), sep, FALSE, FALSE, 0);

        /* Summary Math */
        snprintf(buf, sizeof(buf),
            "Subtotal: Rs. %.2f   |   Discount (5.0%%): -Rs. %.2f   |   Taxable: Rs. %.2f   |   GST (5.0%%): +Rs. %.2f   |   GRAND TOTAL: Rs. %.2f",
            ord->subtotal, ord->discount_amount,
            ord->subtotal - ord->discount_amount,
            ord->tax_amount, ord->grand_total);
        GtkWidget *l_math = gtk_label_new(buf);
        gtk_widget_set_halign(l_math, GTK_ALIGN_START);
        gtk_style_context_add_class(gtk_widget_get_style_context(l_math), "stat-value");
        gtk_box_pack_start(GTK_BOX(card), l_math, FALSE, FALSE, 0);

        /* Action Buttons */
        GtkWidget *b_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);

        GtkWidget *b_view = gtk_button_new_with_label("🧾 View Printable Receipt");
        gtk_style_context_add_class(gtk_widget_get_style_context(b_view), "btn-primary");
        g_signal_connect(b_view, "clicked", G_CALLBACK(on_view_receipt_clicked), NULL);

        GtkWidget *b_save = gtk_button_new_with_label("💾 Save Receipt to File");
        gtk_style_context_add_class(gtk_widget_get_style_context(b_save), "btn-secondary");
        g_signal_connect(b_save, "clicked", G_CALLBACK(on_save_receipt_clicked), NULL);

        GtkWidget *b_pay = gtk_button_new_with_label("✅ Complete Payment & Clear Table");
        gtk_style_context_add_class(gtk_widget_get_style_context(b_pay), "btn-success");
        g_signal_connect(b_pay, "clicked", G_CALLBACK(on_complete_payment_clicked), NULL);

        gtk_box_pack_start(GTK_BOX(b_box), b_view, TRUE, TRUE, 0);
        gtk_box_pack_start(GTK_BOX(b_box), b_save, TRUE, TRUE, 0);
        gtk_box_pack_start(GTK_BOX(b_box), b_pay, TRUE, TRUE, 0);

        gtk_box_pack_start(GTK_BOX(card), b_box, FALSE, FALSE, 0);
        gtk_box_pack_start(GTK_BOX(root), card, TRUE, TRUE, 0);
    } else {
        GtkWidget *l_empty = gtk_label_new("No order available for billing.");
        gtk_box_pack_start(GTK_BOX(root), l_empty, TRUE, TRUE, 0);
    }

    gtk_box_pack_start(GTK_BOX(g_billing_container), root, TRUE, TRUE, 0);
    gtk_widget_show_all(g_billing_container);
}

/* ============================================================================
 * MODULE 7: REPORTS VIEW
 * ============================================================================
 */
static void refresh_reports_view(void) {
    if (!g_reports_container) return;
    clear_container(g_reports_container);

    GtkWidget *root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 16);
    gtk_widget_set_margin_start(root, 20);
    gtk_widget_set_margin_end(root, 20);
    gtk_widget_set_margin_top(root, 20);
    gtk_widget_set_margin_bottom(root, 20);
    gtk_widget_set_vexpand(root, TRUE);
    gtk_widget_set_hexpand(root, TRUE);

    GtkWidget *lbl_head = gtk_label_new("Sales & Operations Performance Reports");
    gtk_style_context_add_class(gtk_widget_get_style_context(lbl_head), "page-title");
    gtk_box_pack_start(GTK_BOX(root), lbl_head, FALSE, FALSE, 0);

    DashboardStats stats;
    getDashboardStats(&stats);
    char buf[256];

    /* Stat Overview Card */
    GtkWidget *card1 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_style_context_add_class(gtk_widget_get_style_context(card1), "card-panel");

    snprintf(buf, sizeof(buf),
        "Total Orders Processed : %d\n"
        "  • Completed Orders   : %d\n"
        "  • Active Orders      : %d (Pending: %d, Preparing: %d, Ready: %d, Served: %d)\n"
        "  • Cancelled Orders   : %d\n\n"
        "Table Seating Occupancy: %.1f%%\n"
        "Total Revenue Realized : Rs. %.2f\n"
        "Average Order Value    : Rs. %.2f",
        stats.total_orders, stats.completed_orders, stats.active_orders,
        stats.pending_orders, stats.preparing_orders, stats.ready_orders, stats.served_orders,
        stats.cancelled_orders, stats.occupancy_rate, stats.today_revenue, stats.avg_order_value);

    GtkWidget *l_stats = gtk_label_new(buf);
    gtk_widget_set_halign(l_stats, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(card1), l_stats, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(root), card1, FALSE, FALSE, 0);

    /* Top Selling Items Card */
    GtkWidget *card2 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_style_context_add_class(gtk_widget_get_style_context(card2), "card-panel");
    gtk_widget_set_vexpand(card2, TRUE);

    GtkWidget *l_toptitle = gtk_label_new("Top Selling Menu Items Ranking");
    gtk_widget_set_halign(l_toptitle, GTK_ALIGN_START);
    gtk_style_context_add_class(gtk_widget_get_style_context(l_toptitle), "stat-title");
    gtk_box_pack_start(GTK_BOX(card2), l_toptitle, FALSE, FALSE, 0);

    TopItemStat top[15];
    int top_count = getTopSellingItems(top, 15);

    GtkWidget *sc = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(sc), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_vexpand(sc, TRUE);

    GtkWidget *v_top = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);

    for (int i = 0; i < top_count; i++) {
        if (top[i].total_quantity_sold > 0) {
            snprintf(buf, sizeof(buf), "%2d. %-32s | Total Sold: %-4d units | Revenue: Rs. %.2f",
                     i + 1, top[i].item_name, top[i].total_quantity_sold, top[i].total_revenue_generated);
            GtkWidget *l_row = gtk_label_new(buf);
            gtk_widget_set_halign(l_row, GTK_ALIGN_START);
            gtk_box_pack_start(GTK_BOX(v_top), l_row, FALSE, FALSE, 0);
        }
    }
    if (top_count == 0 || top[0].total_quantity_sold == 0) {
        GtkWidget *l_none = gtk_label_new("No items sold yet.");
        gtk_box_pack_start(GTK_BOX(v_top), l_none, FALSE, FALSE, 0);
    }

    gtk_container_add(GTK_CONTAINER(sc), v_top);
    gtk_box_pack_start(GTK_BOX(card2), sc, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(root), card2, TRUE, TRUE, 0);

    gtk_box_pack_start(GTK_BOX(g_reports_container), root, TRUE, TRUE, 0);
    gtk_widget_show_all(g_reports_container);
}

/* ============================================================================
 * GTK CSS STYLING PROVIDER (HIGH CONTRAST & MODERN DESKTOP THEME)
 * ============================================================================
 */
static void apply_custom_css(void) {
    const char *css =
        "window { background-color: #f1f5f9; font-family: 'Segoe UI', Arial, sans-serif; }\n"
        ".header-banner { background-color: #0f172a; padding: 14px 24px; color: #ffffff; border-bottom: 2px solid #1e293b; }\n"
        ".app-title { font-size: 22px; font-weight: 800; color: #ffffff; }\n"
        ".app-subtitle { font-size: 13px; color: #cbd5e1; }\n"
        ".nav-bar { background-color: #1e293b; padding: 8px 16px; border-bottom: 2px solid #334155; }\n"

        /* Navigation Buttons: Solid Navy Slate buttons with White Bold Text & Clear Borders */
        "button.nav-btn { background-color: #334155; background-image: none; color: #ffffff; font-weight: 800; font-size: 13px; border: 1px solid #475569; padding: 8px 16px; border-radius: 6px; }\n"
        "button.nav-btn label { color: #ffffff; font-weight: 800; font-size: 13px; }\n"
        "button.nav-btn:hover { background-color: #2563eb; border-color: #60a5fa; }\n"
        "button.nav-btn:hover label { color: #ffffff; }\n"
        "button.nav-btn-active { background-color: #2563eb; background-image: none; color: #ffffff; font-weight: 900; border: 2px solid #ffffff; }\n"
        "button.nav-btn-active label { color: #ffffff; font-weight: 900; }\n"

        /* General Button Reset & Sub-label Rules */
        "button { background-image: none; text-shadow: none; box-shadow: none; border-radius: 6px; padding: 8px 16px; font-weight: 800; font-size: 13px; }\n"
        "button label { color: #ffffff; font-weight: 800; font-size: 13px; }\n"

        /* Stat & Card Panels */
        ".card-panel { background-color: #ffffff; border-radius: 10px; padding: 20px; border: 1px solid #cbd5e1; box-shadow: 0 2px 4px rgba(0,0,0,0.05); }\n"
        ".page-title { font-size: 20px; font-weight: 800; color: #0f172a; margin-bottom: 12px; }\n"
        ".stat-card { background-color: #ffffff; border-radius: 10px; padding: 18px; border: 2px solid #cbd5e1; }\n"
        ".stat-title { font-size: 13px; font-weight: 700; color: #334155; }\n"
        ".stat-value { font-size: 24px; font-weight: 800; color: #0f172a; margin-top: 4px; }\n"

        /* Status Badges */
        ".badge-available { background-color: #15803d; color: #ffffff; font-weight: 800; font-size: 12px; padding: 4px 10px; border-radius: 12px; }\n"
        ".badge-occupied { background-color: #b91c1c; color: #ffffff; font-weight: 800; font-size: 12px; padding: 4px 10px; border-radius: 12px; }\n"
        ".badge-reserved { background-color: #c2410c; color: #ffffff; font-weight: 800; font-size: 12px; padding: 4px 10px; border-radius: 12px; }\n"
        ".badge-status { background-color: #0369a1; color: #ffffff; font-weight: 800; font-size: 12px; padding: 4px 10px; border-radius: 12px; }\n"

        /* HIGH-CONTRAST ACTION BUTTONS (SOLID VISIBLE COLORS & DARK CONTRAST BORDERS) */
        /* Primary / Edit / View / Action -> Deep Royal Blue (#1D4ED8) with Crisp White Text */
        "button.btn-primary { background-color: #1d4ed8; background-image: none; color: #ffffff; font-weight: 800; font-size: 13px; border-radius: 6px; padding: 8px 16px; border: 2px solid #1e40af; }\n"
        "button.btn-primary label { color: #ffffff; font-weight: 800; }\n"
        "button.btn-primary:hover { background-color: #1e40af; border-color: #1e3a8a; }\n"

        /* Success / Add / Save / Complete / Occupy -> Deep Emerald Green (#15803D) with Crisp White Text */
        "button.btn-success { background-color: #15803d; background-image: none; color: #ffffff; font-weight: 800; font-size: 13px; border-radius: 6px; padding: 8px 16px; border: 2px solid #166534; }\n"
        "button.btn-success label { color: #ffffff; font-weight: 800; }\n"
        "button.btn-success:hover { background-color: #166534; border-color: #14532d; }\n"

        /* Warning / Preparing / Reserve -> Deep Burnt Amber (#C2410C) with Crisp White Text */
        "button.btn-warning { background-color: #c2410c; background-image: none; color: #ffffff; font-weight: 800; font-size: 13px; border-radius: 6px; padding: 8px 16px; border: 2px solid #9a3412; }\n"
        "button.btn-warning label { color: #ffffff; font-weight: 800; }\n"
        "button.btn-warning:hover { background-color: #9a3412; border-color: #7c2d12; }\n"

        /* Danger / Delete / Release / Exit -> Deep Crimson Red (#B91C1C) with Crisp White Text */
        "button.btn-danger { background-color: #b91c1c; background-image: none; color: #ffffff; font-weight: 800; font-size: 13px; border-radius: 6px; padding: 8px 16px; border: 2px solid #991b1b; }\n"
        "button.btn-danger label { color: #ffffff; font-weight: 800; }\n"
        "button.btn-danger:hover { background-color: #991b1b; border-color: #7f1d1d; }\n"

        /* Secondary / Cancel / Disable / History -> Deep Slate Charcoal (#1E293B) with Crisp White Text */
        "button.btn-secondary { background-color: #1e293b; background-image: none; color: #ffffff; font-weight: 800; font-size: 13px; border-radius: 6px; padding: 8px 16px; border: 2px solid #0f172a; }\n"
        "button.btn-secondary label { color: #ffffff; font-weight: 800; }\n"
        "button.btn-secondary:hover { background-color: #334155; border-color: #1e293b; }\n"

        ".table-card { background-color: #ffffff; border-radius: 10px; padding: 16px; border: 2px solid #cbd5e1; }\n";

    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider, css, -1, NULL);
    gtk_style_context_add_provider_for_screen(
        gdk_screen_get_default(),
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

    g_main_window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(g_main_window), "Spice Bavarchi - Restaurant Management System");
    gtk_window_set_default_size(GTK_WINDOW(g_main_window), 1280, 800);

    apply_custom_css();

    GtkWidget *root_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    /* 1. Header Banner */
    GtkWidget *header = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 15);
    gtk_style_context_add_class(gtk_widget_get_style_context(header), "header-banner");

    GtkWidget *vbox_title = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
    GtkWidget *lbl_title = gtk_label_new("SPICE BAVARCHI RESTAURANT");
    gtk_widget_set_halign(lbl_title, GTK_ALIGN_START);
    gtk_style_context_add_class(gtk_widget_get_style_context(lbl_title), "app-title");

    GtkWidget *lbl_sub = gtk_label_new("SPICE BAVARCHI — \"Great food. Great moments.\" | Restaurant Management System");
    gtk_widget_set_halign(lbl_sub, GTK_ALIGN_START);
    gtk_style_context_add_class(gtk_widget_get_style_context(lbl_sub), "app-subtitle");

    gtk_box_pack_start(GTK_BOX(vbox_title), lbl_title, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox_title), lbl_sub, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(header), vbox_title, FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(root_box), header, FALSE, FALSE, 0);

    /* 2. Top Navigation Bar */
    GtkWidget *navbar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    gtk_style_context_add_class(gtk_widget_get_style_context(navbar), "nav-bar");

    g_nav_btn_dashboard = gtk_button_new_with_label("📊 Dashboard");
    gtk_style_context_add_class(gtk_widget_get_style_context(g_nav_btn_dashboard), "nav-btn");
    g_signal_connect(g_nav_btn_dashboard, "clicked", G_CALLBACK(on_nav_clicked), (gpointer)"dashboard");
    gtk_box_pack_start(GTK_BOX(navbar), g_nav_btn_dashboard, FALSE, FALSE, 0);

    g_nav_btn_menu = gtk_button_new_with_label("🍽️ Menu");
    gtk_style_context_add_class(gtk_widget_get_style_context(g_nav_btn_menu), "nav-btn");
    g_signal_connect(g_nav_btn_menu, "clicked", G_CALLBACK(on_nav_clicked), (gpointer)"menu");
    gtk_box_pack_start(GTK_BOX(navbar), g_nav_btn_menu, FALSE, FALSE, 0);

    g_nav_btn_tables = gtk_button_new_with_label("🪑 Tables");
    gtk_style_context_add_class(gtk_widget_get_style_context(g_nav_btn_tables), "nav-btn");
    g_signal_connect(g_nav_btn_tables, "clicked", G_CALLBACK(on_nav_clicked), (gpointer)"tables");
    gtk_box_pack_start(GTK_BOX(navbar), g_nav_btn_tables, FALSE, FALSE, 0);

    g_nav_btn_orders = gtk_button_new_with_label("📋 Orders");
    gtk_style_context_add_class(gtk_widget_get_style_context(g_nav_btn_orders), "nav-btn");
    g_signal_connect(g_nav_btn_orders, "clicked", G_CALLBACK(on_nav_clicked), (gpointer)"orders");
    gtk_box_pack_start(GTK_BOX(navbar), g_nav_btn_orders, FALSE, FALSE, 0);

    g_nav_btn_kitchen = gtk_button_new_with_label("👨‍🍳 Kitchen");
    gtk_style_context_add_class(gtk_widget_get_style_context(g_nav_btn_kitchen), "nav-btn");
    g_signal_connect(g_nav_btn_kitchen, "clicked", G_CALLBACK(on_nav_clicked), (gpointer)"kitchen");
    gtk_box_pack_start(GTK_BOX(navbar), g_nav_btn_kitchen, FALSE, FALSE, 0);

    g_nav_btn_billing = gtk_button_new_with_label("🧾 Billing");
    gtk_style_context_add_class(gtk_widget_get_style_context(g_nav_btn_billing), "nav-btn");
    g_signal_connect(g_nav_btn_billing, "clicked", G_CALLBACK(on_nav_clicked), (gpointer)"billing");
    gtk_box_pack_start(GTK_BOX(navbar), g_nav_btn_billing, FALSE, FALSE, 0);

    g_nav_btn_reports = gtk_button_new_with_label("📈 Reports");
    gtk_style_context_add_class(gtk_widget_get_style_context(g_nav_btn_reports), "nav-btn");
    g_signal_connect(g_nav_btn_reports, "clicked", G_CALLBACK(on_nav_clicked), (gpointer)"reports");
    gtk_box_pack_start(GTK_BOX(navbar), g_nav_btn_reports, FALSE, FALSE, 0);

    GtkWidget *nav_sp = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_pack_start(GTK_BOX(navbar), nav_sp, TRUE, TRUE, 0);

    GtkWidget *btn_exit = gtk_button_new_with_label("❌ Exit System");
    gtk_style_context_add_class(gtk_widget_get_style_context(btn_exit), "btn-danger");
    g_signal_connect(btn_exit, "clicked", G_CALLBACK(on_exit_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(navbar), btn_exit, FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(root_box), navbar, FALSE, FALSE, 0);

    /* 3. GtkStack Container holding 7 Pages */
    g_stack = gtk_stack_new();
    gtk_stack_set_transition_type(GTK_STACK(g_stack), GTK_STACK_TRANSITION_TYPE_CROSSFADE);

    g_dashboard_container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    g_menu_container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    g_tables_container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    g_orders_container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    g_kitchen_container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    g_billing_container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    g_reports_container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    gtk_stack_add_named(GTK_STACK(g_stack), g_dashboard_container, "dashboard");
    gtk_stack_add_named(GTK_STACK(g_stack), g_menu_container, "menu");
    gtk_stack_add_named(GTK_STACK(g_stack), g_tables_container, "tables");
    gtk_stack_add_named(GTK_STACK(g_stack), g_orders_container, "orders");
    gtk_stack_add_named(GTK_STACK(g_stack), g_kitchen_container, "kitchen");
    gtk_stack_add_named(GTK_STACK(g_stack), g_billing_container, "billing");
    gtk_stack_add_named(GTK_STACK(g_stack), g_reports_container, "reports");

    gtk_box_pack_start(GTK_BOX(root_box), g_stack, TRUE, TRUE, 0);

    gtk_container_add(GTK_CONTAINER(g_main_window), root_box);

    switch_to_page("dashboard");
    gtk_widget_show_all(g_main_window);
}

int run_gui_app(int argc, char *argv[]) {
    GtkApplication *app = gtk_application_new("com.spicebavarchi.restaurant.system", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(on_app_activate), NULL);
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;
}
