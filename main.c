/*
 * ============================================================================
 * Project: Restaurant Management System (GTK3 Desktop Application)
 * Language: C (C99 Standard)
 * Framework: GTK3 Desktop GUI
 * Description: B.Tech College Project GTK Desktop Application for Restaurant Management.
 * ============================================================================
 */

#include "gui.h"
#include "restaurant.h"

int main(int argc, char *argv[]) {
    loadData();
    int status = run_gui_app(argc, argv);
    saveData();
    return status;
}
