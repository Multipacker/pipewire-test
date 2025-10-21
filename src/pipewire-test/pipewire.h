#ifndef PIPEWIRE_INCLUDE_H
#define PIPEWIRE_INCLUDE_H

typedef struct Pipewire_Handle Pipewire_Handle;
struct Pipewire_Handle {
    U64 u64[2];
};

typedef enum {
    PipeWire_Object_Client,
    PipeWire_Object_Device,
    PipeWire_Object_Node,
    PipeWire_Object_Port,
    PipeWire_Object_Link,
} PipeWire_ObjectKind;

typedef struct Pipewire_Object Pipewire_Object;
struct Pipewire_Object {
    Pipewire_Object *all_next;
    Pipewire_Object *all_previous;
    U64             generation;

    Pipewire_Object *parent;
    Pipewire_Object *first;
    Pipewire_Object *last;
    Pipewire_Object *next;
    Pipewire_Object *previous;

    U32 id;

    PipeWire_ObjectKind kind;

    struct pw_client *client;
    struct spa_hook   client_listener;

    struct pw_device *device;
    struct spa_hook   device_listener;

    struct pw_node  *node;
    struct spa_hook  node_listener;

    struct pw_port  *port;
    struct spa_hook  port_listener;

    struct pw_link  *link;
    struct spa_hook  link_listener;
};

global Pipewire_Object pipewire_nil_object = {
    .parent   = &pipewire_nil_object,
    .first    = &pipewire_nil_object,
    .last     = &pipewire_nil_object,
    .next     = &pipewire_nil_object,
    .previous = &pipewire_nil_object,
};

typedef struct Pipewire_State Pipewire_State;
struct Pipewire_State {
    Arena *arena;

    Pipewire_Object *first_object;
    Pipewire_Object *last_object;
    Pipewire_Object *object_freelist;

    struct pw_main_loop *loop;
    struct pw_context   *context;
    struct pw_core      *core;

    struct pw_registry  *registry;
    struct spa_hook      registry_listener;
};

internal B32 pipewire_object_is_nil(Pipewire_Object *object);

internal Void pipewire_add_child(Pipewire_Object *parent, Pipewire_Object *child);
internal Void pipewire_remove_child(Pipewire_Object *parent, Pipewire_Object *child);

internal Pipewire_Object *pipewire_create_object(U32 id);
internal Void             pipewire_destroy_object(Pipewire_Object *object);

internal Pipewire_Handle  pipewire_handle_from_object(Pipewire_Object *object);
internal Pipewire_Object *pipewire_object_from_handle(Pipewire_Handle handle);

internal Pipewire_Object *pipewire_object_from_id(U32 id);



internal Void pipewire_client_info(Void *data, const struct pw_client_info *info);

global const struct pw_client_events client_events = {
    PW_VERSION_CLIENT_EVENTS,
    .info = pipewire_client_info,
};



internal Void pipewire_node_info(Void *data, const struct pw_node_info *info);
internal Void pipewire_node_param(Void *data, S32 seq, U32 id, U32 index, U32 next, const struct spa_pod *param);

global const struct pw_node_events node_events = {
    PW_VERSION_CLIENT_EVENTS,
    .info  = pipewire_node_info,
    .param = pipewire_node_param,
};



internal Void pipewire_port_info(Void *data, const struct pw_port_info *info);
internal Void pipewire_port_param(Void *data, S32 seq, U32 id, U32 index, U32 next, const struct spa_pod *param);

global const struct pw_port_events port_events = {
    PW_VERSION_PORT_EVENTS,
    .info  = pipewire_port_info,
    .param = pipewire_port_param,
};



internal Void pipewire_device_info(Void *data, const struct pw_device_info *info);
internal Void pipewire_device_param(Void *data, S32 seq, U32 id, U32 index, U32 next, const struct spa_pod *param);

global const struct pw_device_events device_events = {
    PW_VERSION_DEVICE_EVENTS,
    .info  = pipewire_device_info,
    .param = pipewire_device_param,
};



internal Void pipewire_link_info(Void *data, const struct pw_link_info *info);

global const struct pw_link_events link_events = {
    PW_VERSION_LINK_EVENTS,
    .info  = pipewire_link_info,
};



internal Void pipewire_registry_global(Void *data, U32 id, U32 permissions, const char *type, U32 version, const struct spa_dict *props);
internal Void pipewire_registry_global_remove(Void *data, U32 id);

global const struct pw_registry_events registry_events = {
#undef global
    PW_VERSION_REGISTRY_EVENTS,
    .global        = pipewire_registry_global,
    .global_remove = pipewire_registry_global_remove,
#define global static
};



typedef struct Pipewire_Roundtrip Pipewire_Roundtrip;
struct Pipewire_Roundtrip {
    S32                  pending;
    struct pw_main_loop *loop;
};

internal Void pipewire_core_roundtrip_done(Void *data, U32 id, S32 seq);

global const struct pw_core_events pipewire_core_roundtrip_events = {
    PW_VERSION_CORE_EVENTS,
    .done = pipewire_core_roundtrip_done,
};

#endif // PIPEWIRE_INCLUDE_H
