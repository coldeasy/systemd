/*-*- Mode: C; c-basic-offset: 8; indent-tabs-mode: nil -*-*/

/***
  This file is part of systemd.

  Copyright 2010 Lennart Poettering

  systemd is free software; you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by
  the Free Software Foundation; either version 2.1 of the License, or
  (at your option) any later version.

  systemd is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with systemd; If not, see <http://www.gnu.org/licenses/>.
***/

#include "unit.h"
#include "mount.h"
#include "dbus-unit.h"
#include "dbus-execute.h"
#include "dbus-kill.h"
#include "dbus-cgroup.h"
#include "dbus-mount.h"
#include "bus-util.h"

static int property_get_what(
                sd_bus *bus,
                const char *path,
                const char *interface,
                const char *property,
                sd_bus_message *reply,
                void *userdata,
                sd_bus_error *error) {

        Mount *m = userdata;
        const char *d;

        assert(bus);
        assert(reply);
        assert(m);

        if (m->from_proc_self_mountinfo && m->parameters_proc_self_mountinfo.what)
                d = m->parameters_proc_self_mountinfo.what;
        else if (m->from_fragment && m->parameters_fragment.what)
                d = m->parameters_fragment.what;
        else
                d = "";

        return sd_bus_message_append(reply, "s", d);
}

static int property_get_options(
                sd_bus *bus,
                const char *path,
                const char *interface,
                const char *property,
                sd_bus_message *reply,
                void *userdata,
                sd_bus_error *error) {

        Mount *m = userdata;
        const char *d;

        assert(bus);
        assert(reply);
        assert(m);

        if (m->from_proc_self_mountinfo && m->parameters_proc_self_mountinfo.options)
                d = m->parameters_proc_self_mountinfo.options;
        else if (m->from_fragment && m->parameters_fragment.options)
                d = m->parameters_fragment.options;
        else
                d = "";

        return sd_bus_message_append(reply, "s", d);
}

static int property_get_type(
                sd_bus *bus,
                const char *path,
                const char *interface,
                const char *property,
                sd_bus_message *reply,
                void *userdata,
                sd_bus_error *error) {

        Mount *m = userdata;
        const char *d;

        assert(bus);
        assert(reply);
        assert(m);

        if (m->from_proc_self_mountinfo && m->parameters_proc_self_mountinfo.fstype)
                d = m->parameters_proc_self_mountinfo.fstype;
        else if (m->from_fragment && m->parameters_fragment.fstype)
                d = m->parameters_fragment.fstype;
        else
                d = "";

        return sd_bus_message_append(reply, "s", d);
}

static BUS_DEFINE_PROPERTY_GET_ENUM(property_get_result, mount_result, MountResult);

const sd_bus_vtable bus_mount_vtable[] = {
        SD_BUS_VTABLE_START(0),
        SD_BUS_PROPERTY("Where", "s", NULL, offsetof(Mount, where), 0),
        SD_BUS_PROPERTY("What", "s", property_get_what, 0, SD_BUS_VTABLE_PROPERTY_EMITS_CHANGE),
        SD_BUS_PROPERTY("Options","s", property_get_options, 0, SD_BUS_VTABLE_PROPERTY_EMITS_CHANGE),
        SD_BUS_PROPERTY("Type", "s", property_get_type, 0, SD_BUS_VTABLE_PROPERTY_EMITS_CHANGE),
        SD_BUS_PROPERTY("TimeoutUSec", "t", bus_property_get_usec, offsetof(Mount, timeout_usec), 0),
        SD_BUS_PROPERTY("ControlPID", "u", bus_property_get_pid, offsetof(Mount, control_pid), SD_BUS_VTABLE_PROPERTY_EMITS_CHANGE),
        SD_BUS_PROPERTY("DirectoryMode", "u", bus_property_get_mode, offsetof(Mount, directory_mode), 0),
        SD_BUS_PROPERTY("Result", "s", property_get_result, offsetof(Mount, result), SD_BUS_VTABLE_PROPERTY_EMITS_CHANGE),
        BUS_EXEC_COMMAND_VTABLE("ExecMount", offsetof(Mount, exec_command[MOUNT_EXEC_MOUNT]), 0),
        BUS_EXEC_COMMAND_VTABLE("ExecUnmount", offsetof(Mount, exec_command[MOUNT_EXEC_UNMOUNT]), 0),
        BUS_EXEC_COMMAND_VTABLE("ExecRemount", offsetof(Mount, exec_command[MOUNT_EXEC_REMOUNT]), 0),
        SD_BUS_VTABLE_END
};

const char * const bus_mount_changing_properties[] = {
        "What",
        "Options",
        "Type",
        "ControlPID",
        "Result",
        NULL
};

int bus_mount_set_property(
                Unit *u,
                const char *name,
                sd_bus_message *message,
                UnitSetPropertiesMode mode,
                sd_bus_error *error) {

        Mount *m = MOUNT(u);

        assert(m);
        assert(name);
        assert(message);

        return bus_cgroup_set_property(u, &m->cgroup_context, name, message, mode, error);
}

int bus_mount_commit_properties(Unit *u) {
        assert(u);

        unit_realize_cgroup(u);
        return 0;
}
