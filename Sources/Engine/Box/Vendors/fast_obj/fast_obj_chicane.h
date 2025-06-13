#include "fast_obj.h"

fastObjMesh* fast_obj_read_memory_with_callbacks(const char* start, const char* end, const fastObjCallbacks* callbacks, void* user_data)
{
    fastObjData  data;
    fastObjMesh* m;

    /* Empty mesh */
    m = (fastObjMesh*)(memory_realloc(0, sizeof(fastObjMesh)));

    if (!m)
        return 0;

    m->positions      = 0;
    m->texcoords      = 0;
    m->normals        = 0;
    m->colors         = 0;
    m->face_vertices  = 0;
    m->face_materials = 0;
    m->face_lines     = 0;
    m->indices        = 0;
    m->materials      = 0;
    m->textures       = 0;
    m->objects        = 0;
    m->groups         = 0;

    /* Add dummy position/texcoord/normal/texture */
    array_push(m->positions, 0.0f);
    array_push(m->positions, 0.0f);
    array_push(m->positions, 0.0f);

    array_push(m->texcoords, 0.0f);
    array_push(m->texcoords, 0.0f);

    array_push(m->normals, 0.0f);
    array_push(m->normals, 0.0f);
    array_push(m->normals, 1.0f);

    array_push(m->textures, map_default());

    /* Data needed during parsing */
    data.mesh     = m;
    data.object   = object_default();
    data.group    = group_default();
    data.material = 0;
    data.line     = 1;
    data.base     = 0;

    parse_buffer(&data, start, end + 1, callbacks, 0);

    /* Flush final object/group */
    flush_object(&data);
    object_clean(&data.object);

    flush_group(&data);
    group_clean(&data.group);

    m->position_count = array_size(m->positions) / 3;
    m->texcoord_count = array_size(m->texcoords) / 2;
    m->normal_count   = array_size(m->normals) / 3;
    m->color_count    = array_size(m->colors) / 3;
    m->face_count     = array_size(m->face_vertices);
    m->index_count    = array_size(m->indices);
    m->material_count = array_size(m->materials);
    m->texture_count  = array_size(m->textures);
    m->object_count   = array_size(m->objects);
    m->group_count    = array_size(m->groups);

    /* Clean up */
    memory_dealloc(data.base);

    return m;
}

fastObjMesh* fast_obj_read_memory(const char* start, const char* end)
{
    fastObjCallbacks callbacks;
    callbacks.file_open  = file_open;
    callbacks.file_close = file_close;
    callbacks.file_read  = file_read;
    callbacks.file_size  = file_size;

    return fast_obj_read_memory_with_callbacks(start, end, &callbacks, 0);
}