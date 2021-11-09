/*
 * Copyright (C) 2019-2021 Canonical, Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef MULTIPASS_SETTINGS_H
#define MULTIPASS_SETTINGS_H

#include "settings_handler.h"

#include <multipass/exceptions/settings_exceptions.h>
#include <multipass/singleton.h>

#include <QString>
#include <QVariant>

#include <set>

#define MP_SETTINGS multipass::Settings::instance()

namespace multipass
{
class Settings : public Singleton<Settings>
{
public:
    Settings(const Singleton<Settings>::PrivatePass&);

    virtual void register_handler(std::unique_ptr<SettingsHandler> handler);

    /**
     * Obtain the keys, or key templates, that this Settings singleton knows about.
     * @warning Templates are meant for human interpretation (e.g. <tt>local.@<instance@>.cpus</tt>). They cannot be
     * used in get/set as actual keys.
     * @return The set of keys or key templates that this Settings singleton knows about.
     */
    std::set<QString> keys() const;
    virtual QString get(const QString& key) const;            // throws on unknown key
    virtual void set(const QString& key, const QString& val); // throws on unknown key or bad settings

    /**
     * Obtain a setting as a certain type
     * @tparam T The type to obtain the setting as
     * @param key The key that identifies the setting
     * @return The value that the setting converts to, when interpreted as type @p T. Follows the conversion rules used
     * by @c QVariant.
     * If the current @em value cannot be converted, a default @p T value is returned.
     * @throws UnsupportedSettingValueType<T> If QVariant cannot convert from a @c QString to type @c T. Note that
     * this is only thrown when the type itself can't be converted to. The actual value may still fail to convert, in
     * which case a default @c T value is returned.
     */
    template <typename T>
    T get_as(const QString& key) const;

private:
    std::vector<std::unique_ptr<SettingsHandler>> handlers;
};
} // namespace multipass

template <typename T>
T multipass::Settings::get_as(const QString& key) const
{
    auto var = QVariant{get(key)};
    if (var.canConvert<T>()) // Note: this only checks if the types themselves are convertible, not the values
        return var.value<T>();
    throw UnsupportedSettingValueType<T>(key);
}

#endif // MULTIPASS_SETTINGS_H
