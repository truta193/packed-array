#ifndef AM_PACKED_ARRAY
#define AM_PACKED_ARRAY

//https://github.com/truta193/dynamic-array/blob/main/dynamic_array.h
#include "dynamic_array.h"

#define PA_INVALID_ID 0xFFFFFFFF
#define PA_INVALID_INDEX 0xFFFFFFFF

#define am_packed_array(type)\
    struct {\
        dyn_array(am_uint32) indices;\
        dyn_array(type) elements;\
        unsigned int next_id;\
    }*

#define packed_array_get_idx(array, id) ((id) < (array)->next_id ? (array)->indices[(id)] : PA_INVALID_INDEX)
//TODO: Some kind of guard for get_val needed
#define packed_array_get_val(array, id) ((array)->elements[(array)->indices[(id)]])
#define packed_array_get_ptr(array, id) (((id) < (array)->next_id) && ((array)->indices[(id)] != PA_INVALID_INDEX) ? &((array)->elements[(array)->indices[(id)]]) : NULL)
#define packed_array_get_count(array) (dyn_array_get_count((array)->elements))
#define packed_array_has(array, id) (((id) < (array)->next_id) && (packed_array_get_idx((array), (id)) != PA_INVALID_INDEX) ? 1 : 0)
#define packed_array_init(array, value_size) (packed_array_alloc((void**)&(array), sizeof(*(array))), (array)->next_id = 1, dyn_array_init((void**)&((array)->indices), sizeof(unsigned int)*2), dyn_array_get_header((array)->indices)->size += sizeof(unsigned int), dyn_array_init((void**)&((array)->elements), (value_size)))
#define packed_array_clear(array) (dyn_array_clear((array)->indices), dyn_array_clear((array)->elements))
#define packed_array_destroy(array) (dyn_array_destroy((array)->indices), dyn_array_destroy((array)->elements), free(array))
#define packed_array_add(array, value) (dyn_array_push((array)->indices, dyn_array_get_count((array)->indices)-1), dyn_array_push((array)->elements, value), (array)->next_id++)

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
