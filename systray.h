//  systray.cpp
void load_tray_menu(WORD menuID);
void attach_tray_icon(HWND hwnd, char const * const szClassName, WORD iconID);
BOOL respond_to_tray_clicks(HWND hwnd, LPARAM lParam);
void release_systray_resource(void);

