#ifndef AM_PACKED_ARRAY
#define AM_PACKED_ARRAY

#include "dynamic_array.h"

#define packed_array(type)\
    struct {\
        dyn_array(unsigned int) indices;\
        dyn_array(type) elements;\
        unsigned int next_id;\
    }*

#define packed_array_get_idx(array, id)         ((id) < (array)->next_id ? (array)->indices[(id)] : 0xFFFFFFFF)
#define packed_array_get(array, id)             (((id) < (array)->next_id) && ((array)->indices[(id)] != 0xFFFFFFFF) ? (array)->elements[(array)->indices[(id)]] : 0)
#define packed_array_has(array, id)             (((id) < (array)->next_id) && (packed_array_get_idx((array), (id)) != 0xFFFFFFFF) ? 1 : 0)
#define packed_array_init(array, value_size)    (packed_array_alloc((void**)&(array), sizeof(*(array))), dyn_array_init((void**)&((array)->indices), sizeof(unsigned int)), dyn_array_init((void**)&((array)->elements), (value_size)))
#define packed_array_clear(array)               (dyn_array_clear((array)->indices), dyn_array_clear((array)->elements), free((void*)(array)))
#define packed_array_add(array, value)          (dyn_array_push((array)->indices, dyn_array_get_count((array)->indices)), dyn_array_push((array)->elements, value), (array)->next_id++)

void packed_array_alloc(void **array, size_t size) {
    if (*array == NULL) {
        *array = malloc(size);
        memset(*array, 0, size);
    };
};

#define packed_array_erase(array, id)\
    do {\
        unsigned int index = packed_array_get_idx(array, id);\
        if (index == 0xFFFFFFFF) {\
            printf("[WARN] Element with ID %d does not exist!\n", id);\
            break;\
        };\
        if (!packed_array_has((array), (id))) {\
            printf("[WARN] Invalid id %d!\n", id);\
            break;\
        };\
        int last_element_id = -1;\
        if (packed_array_get_idx((array), (id)) == dyn_array_get_count((array)->elements) - 1) last_element_id = id;\
        else for (int i = 0; i < dyn_array_get_size(array) / sizeof(unsigned int); i++)\
            if (packed_array_get_idx((array), i) == dyn_array_get_count((array)->elements) - 1) {\
                last_element_id = i;\
                break;\
            };\
        dyn_array_replace((array)->elements, &((array)->elements[dyn_array_get_count((array)->elements) - 1]), index*sizeof((array)->elements[0]), sizeof((array)->elements[0]));\
        array->indices[last_element_id] = index;\
        array->indices[id] = 0xFFFFFFFF;\
        dyn_array_get_size((array)->elements) -= sizeof((array)->elements[0]);\
    } while(0);

#endif
