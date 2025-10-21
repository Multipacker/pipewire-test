global Pipewire_State *pipewire_state;

internal B32 pipewire_object_is_nil(Pipewire_Object *object) {
    B32 result = !object || object == &pipewire_nil_object;
    return result;
}

internal Void pipewire_add_child(Pipewire_Object *parent, Pipewire_Object *child) {
    child->parent = parent;
    dll_insert_next_previous_zero(parent->first, parent->last, parent->last, child, next, previous, &pipewire_nil_object);
}

internal Void pipewire_remove_child(Pipewire_Object *parent, Pipewire_Object *child) {
    child->parent = &pipewire_nil_object;
    dll_remove_next_previous_zero(parent->first, parent->last, child, next, previous, &pipewire_nil_object);
}

internal Pipewire_Object *pipewire_create_object(U32 id) {
    Pipewire_Object *object = pipewire_state->object_freelist;
    if (object) {
        sll_stack_pop(object);
    } else {
        object = arena_push_struct(pipewire_state->arena, Pipewire_Object);
    }

    dll_insert_next_previous_zero(pipewire_state->first_object, pipewire_state->last_object, pipewire_state->last_object, object, all_next, all_previous, 0);
    object->id = id;

    object->parent   = &pipewire_nil_object;
    object->first    = &pipewire_nil_object;
    object->last     = &pipewire_nil_object;
    object->next     = &pipewire_nil_object;
    object->previous = &pipewire_nil_object;

    return object;
}

internal Void pipewire_destroy_object(Pipewire_Object *object) {
    if (!pipewire_object_is_nil(object->parent)) {
        pipewire_remove_child(object->parent, object);
    }

    if (!pipewire_object_is_nil(object->first)) {
        for (Pipewire_Object *child = object->first, *next = &pipewire_nil_object; !pipewire_object_is_nil(child); child = next) {
            next = child->next;
            pipewire_remove_child(object, child);
        }
    }

    ++object->generation;
    dll_remove_next_previous_zero(pipewire_state->first_object, pipewire_state->last_object, object, all_next, all_previous, 0);
    sll_stack_push(pipewire_state->object_freelist, object);
}

internal Pipewire_Handle pipewire_handle_from_object(Pipewire_Object *object) {
    Pipewire_Handle handle = { 0 };
    handle.u64[0] = integer_from_pointer(object);
    handle.u64[1] = object->generation;
    return handle;
}

internal Pipewire_Object *pipewire_object_from_handle(Pipewire_Handle handle) {
    Pipewire_Object *object = (Pipewire_Object *) pointer_from_integer(handle.u64[0]);
    if (!object || object->generation != handle.u64[1]) {
        object = &pipewire_nil_object;
    }
    return object;
}

internal Pipewire_Object *pipewire_object_from_id(U32 id) {
    Pipewire_Object *result = &pipewire_nil_object;
    for (Pipewire_Object *object = pipewire_state->first_object; object; object = object->all_next) {
        if (object->id == id) {
            result = object;
            break;
        }
    }
    return result;
}



internal Void pipewire_client_info(Void *data, const struct pw_client_info *info) {
    Pipewire_Object *object = (Pipewire_Object *) data;

    printf("client: id:%u\n", info->id);
    printf("  change mask:");
    if (info->change_mask & PW_CLIENT_CHANGE_MASK_PROPS)  printf(" PROPS");
    printf("\n");
    printf("  props:\n");
    const struct spa_dict_item *item = 0;
    spa_dict_for_each(item, info->props) {
        printf("    %s: \"%s\"\n", item->key, item->value);
    }
}



internal Void pipewire_node_info(Void *data, const struct pw_node_info *info) {
    Pipewire_Object *node = (Pipewire_Object *) data;

    printf("node: id:%u\n", info->id);
    printf("  max input ports:  %u\n", info->max_input_ports);
    printf("  max output ports: %u\n", info->max_output_ports);
    printf("  change mask:");
    if (info->change_mask & PW_NODE_CHANGE_MASK_INPUT_PORTS)  printf(" INPUT_PORTS");
    if (info->change_mask & PW_NODE_CHANGE_MASK_OUTPUT_PORTS) printf(" OUTPUT_PORTS");
    if (info->change_mask & PW_NODE_CHANGE_MASK_STATE)        printf(" STATE");
    if (info->change_mask & PW_NODE_CHANGE_MASK_PROPS)        printf(" PROPS");
    if (info->change_mask & PW_NODE_CHANGE_MASK_PARAMS)       printf(" PARAMS");
    printf("\n");
    printf("  n input ports:    %u\n", info->n_input_ports);
    printf("  n output ports:   %u\n", info->n_output_ports);
    printf("  state:            %s\n", pw_node_state_as_string(info->state));
    printf("  props:\n");
    const struct spa_dict_item *item = 0;
    spa_dict_for_each(item, info->props) {
        printf("    %s: \"%s\"\n", item->key, item->value);
    }

    const struct spa_dict_item *device_id_item = spa_dict_lookup_item(info->props, PW_KEY_DEVICE_ID);
    const struct spa_dict_item *client_id_item = spa_dict_lookup_item(info->props, PW_KEY_CLIENT_ID);

    if (node->parent) {
        pipewire_remove_child(node->parent, node);
    }

    if (device_id_item) {
        U32 device_id = 0;
        if (spa_atou32(device_id_item->value, &device_id, 10)) {
            Pipewire_Object *device = pipewire_object_from_id(device_id);
            if (!pipewire_object_is_nil(device)) {
                pipewire_add_child(device, node);
                printf("attached node:%u to device:%u\n", node->id, device->id);
            }
        }
    } else if (client_id_item) {
        U32 client_id = 0;
        if (spa_atou32(client_id_item->value, &client_id, 10)) {
            Pipewire_Object *client = pipewire_object_from_id(client_id);
            if (!pipewire_object_is_nil(client)) {
                pipewire_add_child(client, node);
                printf("attached node:%u to client:%u\n", node->id, client->id);
            }
        }
    }
}

internal Void pipewire_node_param(Void *data, S32 seq, U32 id, U32 index, U32 next, const struct spa_pod *param) {
}



internal Void pipewire_port_info(Void *data, const struct pw_port_info *info) {
    Pipewire_Object *port = (Pipewire_Object *) data;

    const char *direction = "";
    switch (info->direction) {
        case PW_DIRECTION_INPUT:  direction = "input";  break;
        case PW_DIRECTION_OUTPUT: direction = "output"; break;
    }
    printf("port: id:%u\n", info->id);
    printf("  direction:            %s\n", direction);
    printf("  change mask:");
    if (info->change_mask & PW_PORT_CHANGE_MASK_PROPS)  printf(" PROPS");
    if (info->change_mask & PW_PORT_CHANGE_MASK_PARAMS) printf(" PARAMS");
    printf("\n");
    printf("  props:\n");
    const struct spa_dict_item *item = 0;
    spa_dict_for_each(item, info->props) {
        printf("    %s: \"%s\"\n", item->key, item->value);
    }

    const struct spa_dict_item *node_id_item = spa_dict_lookup_item(info->props, PW_KEY_NODE_ID);

    if (port->parent) {
        pipewire_remove_child(port->parent, port);
    }

    if (node_id_item) {
        U32 node_id = 0;
        if (spa_atou32(node_id_item->value, &node_id, 10)) {
            Pipewire_Object *node = pipewire_object_from_id(node_id);
            if (!pipewire_object_is_nil(node)) {
                pipewire_add_child(node, port);
                printf("attached port:%u to node:%u\n", port->id, node->id);
            }
        }
    }
}

internal Void pipewire_port_param(Void *data, S32 seq, U32 id, U32 index, U32 next, const struct spa_pod *param) {
}



internal Void pipewire_device_info(Void *data, const struct pw_device_info *info) {
    Pipewire_Object *device = data;

    printf("device: id:%u\n", info->id);
    printf("  change mask:");
    if (info->change_mask & PW_DEVICE_CHANGE_MASK_PROPS)  printf(" PROPS");
    if (info->change_mask & PW_DEVICE_CHANGE_MASK_PARAMS) printf(" PARAMS");
    printf("\n");
    printf("  props:\n");
    const struct spa_dict_item *item = 0;
    spa_dict_for_each(item, info->props) {
        printf("    %s: \"%s\"\n", item->key, item->value);
    }
}

internal Void pipewire_device_param(Void *data, S32 seq, U32 id, U32 index, U32 next, const struct spa_pod *param) {
}



internal Void pipewire_link_info(Void *data, const struct pw_link_info *info) {
    Pipewire_Object *link = (Pipewire_Object *) data;

    printf("link: id:%u\n", info->id);
    printf("  output node id: %u\n", info->output_node_id);
    printf("  output port id: %u\n", info->output_port_id);
    printf("  input node id:  %u\n", info->input_node_id);
    printf("  input port id:  %u\n", info->input_port_id);
    printf("  change mask:");
    if (info->change_mask & PW_LINK_CHANGE_MASK_STATE)  printf(" STATE");
    if (info->change_mask & PW_LINK_CHANGE_MASK_FORMAT) printf(" FORMAT");
    if (info->change_mask & PW_LINK_CHANGE_MASK_PROPS)  printf(" PROPS");
    printf("\n");
    printf("  state:          %s\n", pw_link_state_as_string(info->state));
    printf("  props:\n");
    const struct spa_dict_item *item = 0;
    spa_dict_for_each(item, info->props) {
        printf("    %s: \"%s\"\n", item->key, item->value);
    }
}



internal Void pipewire_registry_global(Void *data, U32 id, U32 permissions, const char *type, U32 version, const struct spa_dict *props) {
    printf("object: id:%u type:%s/%u\n", id, type, version);

    if (strcmp(type, PW_TYPE_INTERFACE_Client) == 0) {
        Pipewire_Object *object = pipewire_create_object(id);
        object->kind = PipeWire_Object_Client;
        object->client = pw_registry_bind(pipewire_state->registry, id, type, PW_VERSION_CLIENT, 0);
        pw_client_add_listener(object->client, &object->client_listener, &client_events, object);
    } else if (strcmp(type, PW_TYPE_INTERFACE_Device) == 0) {
        Pipewire_Object *object = pipewire_create_object(id);
        object->kind = PipeWire_Object_Device;
        object->device = pw_registry_bind(pipewire_state->registry, id, type, PW_VERSION_DEVICE, 0);
        pw_device_add_listener(object->device, &object->device_listener, &device_events, object);
    } else if (strcmp(type, PW_TYPE_INTERFACE_Node) == 0) {
        Pipewire_Object *object = pipewire_create_object(id);
        object->kind = PipeWire_Object_Node;
        object->node = pw_registry_bind(pipewire_state->registry, id, type, PW_VERSION_NODE, 0);
        pw_node_add_listener(object->node, &object->node_listener, &node_events, object);
    } else if (strcmp(type, PW_TYPE_INTERFACE_Port) == 0) {
        Pipewire_Object *object = pipewire_create_object(id);
        object->kind = PipeWire_Object_Port;
        object->port = pw_registry_bind(pipewire_state->registry, id, type, PW_VERSION_PORT, 0);
        pw_port_add_listener(object->port, &object->port_listener, &port_events, object);
    } else if (strcmp(type, PW_TYPE_INTERFACE_Link) == 0) {
        Pipewire_Object *object = pipewire_create_object(id);
        object->kind = PipeWire_Object_Link;
        object->link = pw_registry_bind(pipewire_state->registry, id, type, PW_VERSION_LINK, 0);
        pw_link_add_listener(object->link, &object->link_listener, &link_events, object);
    }

    //for (U32 i = 0; i < props->n_items; ++i) {
        //const struct spa_dict_item *item = &props->items[i];
        //printf("  %s: %s\n", item->key, item->value);
    //}
}

internal Void pipewire_registry_global_remove(Void *data, U32 id) {
    Pipewire_Object *object = pipewire_object_from_id(id);
    if (!pipewire_object_is_nil(object)) {
        printf("remove object id:%u\n", id);
        switch (object->kind) {
            case PipeWire_Object_Client: {
                pw_proxy_destroy((struct pw_proxy *) object->client);
            } break;
            case PipeWire_Object_Device: {
                pw_proxy_destroy((struct pw_proxy *) object->device);
            } break;
            case PipeWire_Object_Node: {
                pw_proxy_destroy((struct pw_proxy *) object->node);
            } break;
            case PipeWire_Object_Port: {
                pw_proxy_destroy((struct pw_proxy *) object->port);
            } break;
            case PipeWire_Object_Link: {
                pw_proxy_destroy((struct pw_proxy *) object->link);
            } break;
        }
        pipewire_destroy_object(object);
    }
}



internal Void pipewire_core_roundtrip_done(Void *data, U32 id, S32 seq) {
    Pipewire_Roundtrip *roundtrip = (Pipewire_Roundtrip *) data;
    if (id == PW_ID_CORE && seq == roundtrip->pending) {
        pw_main_loop_quit(roundtrip->loop);
    }
}



internal Void pipewire_roundtrip(struct pw_core *core, struct pw_main_loop *loop) {
    Pipewire_Roundtrip roundtrip = { 0 };
    struct spa_hook core_listener = { 0 };

    pw_core_add_listener(core, &core_listener, &pipewire_core_roundtrip_events, &roundtrip);

    roundtrip.loop    = loop;
    roundtrip.pending = pw_core_sync(core, PW_ID_CORE, 0);

    int error = pw_main_loop_run(loop);
    if (error < 0) {
        fprintf(stderr, "main_loop_run error: %d\n", error);
    }

    spa_hook_remove(&core_listener);
}



internal Void pipewire_init(Void) {
    Arena *arena = arena_create();
    pipewire_state = arena_push_struct(arena, Pipewire_State);
    pipewire_state->arena = arena;

    pw_init(0, 0);

    pipewire_state->loop     = pw_main_loop_new(0);
    pipewire_state->context  = pw_context_new(pw_main_loop_get_loop(pipewire_state->loop), 0, 0);
    pipewire_state->core     = pw_context_connect(pipewire_state->context, 0, 0);
    pipewire_state->registry = pw_core_get_registry(pipewire_state->core, PW_VERSION_REGISTRY, 0);
    
    pw_registry_add_listener(pipewire_state->registry, &pipewire_state->registry_listener, &registry_events, 0);

    // NOTE(simon): Register all globals.
    pipewire_roundtrip(pipewire_state->core, pipewire_state->loop);

    // NOTE(simon): Gather all events from the globals.
    pipewire_roundtrip(pipewire_state->core, pipewire_state->loop);

    pw_proxy_destroy((struct pw_proxy *) pipewire_state->registry);
    pw_core_disconnect(pipewire_state->core);
    pw_context_destroy(pipewire_state->context);
    pw_main_loop_destroy(pipewire_state->loop);

    pw_deinit();
}
