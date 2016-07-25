/*
  gnomekeyring.cpp

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2015-2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

  Author: Frank Osterfeld <frank.osterfeld@kdab.com>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "gnomekeyring_p.h"

const char* GnomeKeyring::GNOME_KEYRING_DEFAULT = 0;

bool GnomeKeyring::isAvailable()
{
    const GnomeKeyring& keyring = instance();
    return keyring.isLoaded() &&
           keyring.NETWORK_PASSWORD &&
           keyring.is_available &&
           keyring.find_password &&
           keyring.store_password &&
           keyring.delete_password &&
           keyring.is_available();
}

GnomeKeyring::gpointer GnomeKeyring::store_network_password( const gchar* keyring, const gchar* display_name,
                                               const gchar* user, const gchar* server, const gchar* password,
                                               OperationDoneCallback callback, gpointer data, GDestroyNotify destroy_data )
{
    if ( !isAvailable() )
        return 0;
    return instance().store_password( instance().NETWORK_PASSWORD,
                                      keyring, display_name, password, callback, data, destroy_data,
                                      "user", user, "server", server, static_cast<char*>(0) );
}

GnomeKeyring::gpointer GnomeKeyring::find_network_password( const gchar* user, const gchar* server,
                                              OperationGetStringCallback callback, gpointer data, GDestroyNotify destroy_data )
{
    if ( !isAvailable() )
        return 0;
    return instance().find_password( instance().NETWORK_PASSWORD,
                                     callback, data, destroy_data,
                                     "user", user, "server", server, static_cast<char*>(0) );
}

GnomeKeyring::gpointer GnomeKeyring::delete_network_password( const gchar* user,
                                                       const gchar* server,
                                                       OperationDoneCallback callback,
                                                       gpointer data,
                                                       GDestroyNotify destroy_data )
{
    if ( !isAvailable() )
        return 0;
    return instance().delete_password( instance().NETWORK_PASSWORD,
                                       callback, data, destroy_data,
                                       "user", user, "server", server, static_cast<char*>(0) );
}

GnomeKeyring::GnomeKeyring()
    : QLibrary(QStringLiteral("gnome-keyring"), 0)
{
    static const PasswordSchema schema = {
        ITEM_NETWORK_PASSWORD,
        {{ "user",   ATTRIBUTE_TYPE_STRING },
         { "server", ATTRIBUTE_TYPE_STRING },
         { 0,     static_cast<AttributeType>( 0 ) }}
    };

    NETWORK_PASSWORD = &schema;
    is_available = reinterpret_cast<is_available_fn*>( resolve( "gnome_keyring_is_available" ) );
    find_password = reinterpret_cast<find_password_fn*>( resolve( "gnome_keyring_find_password" ) );
    store_password = reinterpret_cast<store_password_fn*>( resolve( "gnome_keyring_store_password" ) );
    delete_password = reinterpret_cast<delete_password_fn*>( resolve( "gnome_keyring_delete_password" ) );
}

GnomeKeyring& GnomeKeyring::instance() {
    static GnomeKeyring keyring;
    return keyring;
}
