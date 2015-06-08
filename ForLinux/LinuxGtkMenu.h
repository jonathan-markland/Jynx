
#pragma once

#include <gtk/gtk.h>
#include <stdint.h>
#include <string>
#include <map>
#include <memory>
#include <vector>


class InterfaceForMenuItemClickHandling
{
public:
    virtual void NotifyMenuItemClicked( uint32_t menuItemID ) = 0;
};



class LinuxGtkMenu;




class LinuxGtkMenuBar
{
public:

    LinuxGtkMenuBar();

    // Create a sub menu, and return it so caller can add items.
    std::shared_ptr<LinuxGtkMenu> CreateMenu( const char *optionText, InterfaceForMenuItemClickHandling *eventHandler );

    // Search all the sub menus and set the state of the item found:
    // Does NOT raise event.
    void EnableMenuItem( uint32_t menuID, bool isEnabled );
    void CheckMenuItem(  uint32_t menuID, bool isChecked );

    GtkWidget *GetWidget() { return _menuBar; }

private:

    GtkWidget *_menuBar = nullptr;
    std::vector<std::shared_ptr<LinuxGtkMenu>>  _subMenus;

};






class LinuxGtkMenu
{
public:

    // Helps make a menu on GTK
    // - optionText can contain '&' accelerator indicators.
    // - menuID is the Microsoft-style ID number of the menu option.
    // - eventHandler handles ALL of the menu option clicks in WM_COMMAND style.

    LinuxGtkMenu( GtkWidget *menubar, const char *optionText, InterfaceForMenuItemClickHandling *eventHandler );

    void AddItem( const char *optionText, uint32_t menuID );
    void AddTick( const char *optionText, uint32_t menuID );
    void AddSeparator();

    // Set state without raising event:
    void EnableMenuItem( uint32_t menuID, bool isEnabled );
    void CheckMenuItem( uint32_t menuID, bool isChecked );

private:

    std::string  StripAccelerators( const char *optionText );
    void AppendItem( GtkWidget *menuItemWidget, uint32_t menuID );
    static void GenericMenuItemHandler( GtkWidget *menuItemWidget, gpointer thisMenuObject );
    void MenuItemClicked( GtkWidget *menuItemWidget );
    GtkWidget *GetWidgetByID( uint32_t menuID ) const;

    GtkWidget *_menuObject = nullptr;
    GtkWidget *_barButton  = nullptr;
    InterfaceForMenuItemClickHandling *_onClickEventHandler = nullptr;
    std::map<GtkWidget *, uint32_t> _widgetToMenuItemIdMap;
    std::map<uint32_t, GtkWidget *> _menuItemIdMapToWidget;
    bool _inhibitMenuEvents = false;

};
