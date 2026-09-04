void render_mesh(meshf *mesh, vec3f position) {
    glTranslatef(position.x, position.y, position.z);
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < mesh->element_count; i++) {
        glColor4f(mesh->colors[i].x, mesh->colors[i].y, mesh->colors[i].z, mesh->colors[i].w);
        glNormal3f(mesh->normals[i].x, mesh->normals[i].y, mesh->normals[i].z);
        glVertex3f(mesh->vertices[i].x, mesh->vertices[i].y, mesh->vertices[i].z);
    }
    glEnd();
    glTranslatef(-position.x, -position.y, -position.z);
}

vec3f generate_normal(vec32i3f a1, vec32i3f b1, vec32i3f c1) {
    vec3f a2 = {a1.x, a1.y, a1.z};
    vec3f b2 = {b1.x, b1.y, b1.z};
    vec3f c2 = {c1.x, c1.y, c1.z};

    vec3f normal = triangle_normal3f(a2, b2, c2);
    normal = unit_vector3f(normal);
    return normal;
}

void render_tetrahedra(vec32i3f **nodes, vec5i32 **cells, int32 cell_count, vec3f position, bool debug) {
    glTranslatef(position.x, position.y, position.z);
    if (!debug) {glEnable(GL_CULL_FACE); glCullFace(GL_BACK);}

    /*int VBO;
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);*/

    for (int i = 0; i < cell_count; i++) {
        vec3f normal0 = generate_normal((*nodes)[(*cells)[i].b], (*nodes)[(*cells)[i].x], (*nodes)[(*cells)[i].y]);
        vec3f normal1 = generate_normal((*nodes)[(*cells)[i].z], (*nodes)[(*cells)[i].y], (*nodes)[(*cells)[i].x]); // flipped
        vec3f normal2 = generate_normal((*nodes)[(*cells)[i].b], (*nodes)[(*cells)[i].y], (*nodes)[(*cells)[i].z]);
        vec3f normal3 = generate_normal((*nodes)[(*cells)[i].z], (*nodes)[(*cells)[i].x], (*nodes)[(*cells)[i].b]); // flipped
        float temp_normal_data[36] = {
            normal0.x, normal0.y, normal0.z, // v0 v1 v2
            normal0.x, normal0.y, normal0.z,
            normal0.x, normal0.y, normal0.z,

            normal1.x, normal1.y, normal1.z, // v3 v2 v1
            normal1.x, normal1.y, normal1.z,
            normal1.x, normal1.y, normal1.z,

            normal2.x, normal2.y, normal2.z, // v0 v2 v3
            normal2.x, normal2.y, normal2.z,
            normal2.x, normal2.y, normal2.z,

            normal3.x, normal3.y, normal3.z, // v3
            normal3.x, normal3.y, normal3.z, // v1
            normal3.x, normal3.y, normal3.z  // v0
        };
        float transparency_;
        if (debug) {
            transparency_ = 0.5;
        } else {
            transparency_ = 1.0;
        }
        float temp_color_data[48] = {
            1.0,0.0,0.0,transparency_,
            1.0,0.0,0.0,transparency_,
            1.0,0.0,0.0,transparency_,

            0.0,1.0,0.0,transparency_,
            0.0,1.0,0.0,transparency_,
            0.0,1.0,0.0,transparency_,

            0.0,0.0,1.0,transparency_,
            0.0,0.0,1.0,transparency_,
            0.0,0.0,1.0,transparency_,

            1.0,0.0,1.0,transparency_,
            1.0,0.0,1.0,transparency_,
            1.0,0.0,1.0,transparency_
        };
        float temp_vertex_data[36] = {
            (*nodes)[(*cells)[i].b].x, (*nodes)[(*cells)[i].b].y, (*nodes)[(*cells)[i].b].z, // v0
            (*nodes)[(*cells)[i].x].x, (*nodes)[(*cells)[i].x].y, (*nodes)[(*cells)[i].x].z, // v1
            (*nodes)[(*cells)[i].y].x, (*nodes)[(*cells)[i].y].y, (*nodes)[(*cells)[i].y].z, // v2

            (*nodes)[(*cells)[i].z].x, (*nodes)[(*cells)[i].z].y, (*nodes)[(*cells)[i].z].z, // v3
            (*nodes)[(*cells)[i].y].x, (*nodes)[(*cells)[i].y].y, (*nodes)[(*cells)[i].y].z, // v2
            (*nodes)[(*cells)[i].x].x, (*nodes)[(*cells)[i].x].y, (*nodes)[(*cells)[i].x].z, // v1

            (*nodes)[(*cells)[i].b].x, (*nodes)[(*cells)[i].b].y, (*nodes)[(*cells)[i].b].z, // v0
            (*nodes)[(*cells)[i].y].x, (*nodes)[(*cells)[i].y].y, (*nodes)[(*cells)[i].y].z, // v2
            (*nodes)[(*cells)[i].z].x, (*nodes)[(*cells)[i].z].y, (*nodes)[(*cells)[i].z].z, // v3

            (*nodes)[(*cells)[i].z].x, (*nodes)[(*cells)[i].z].y, (*nodes)[(*cells)[i].z].z, // v3
            (*nodes)[(*cells)[i].x].x, (*nodes)[(*cells)[i].x].y, (*nodes)[(*cells)[i].x].z, // v1
            (*nodes)[(*cells)[i].b].x, (*nodes)[(*cells)[i].b].y, (*nodes)[(*cells)[i].b].z  // v0
        };
        for (int j = 0; j < 36; j++) {
            normal_data[j + 36 * i] = temp_normal_data[j];
        }
        for (int j = 0; j < 48; j++) {
            color_data[j + 48 * i] = temp_color_data[j];
        }
        for (int j = 0; j < 36; j++) {
            vertex_data[j + 36 * i] = temp_vertex_data[j];
        }
    }
    glNormalPointer(GL_FLOAT, 0, normal_data);

    glColorPointer(4, GL_FLOAT, 0, color_data);

    glVertexPointer(3, GL_FLOAT, 0, vertex_data);

    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY);
    glDrawArrays(GL_TRIANGLES, 0, 12 * cell_count);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);

    /*glDisableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);*/

    if (!debug) {glDisable(GL_CULL_FACE);}
    glTranslatef(-position.x, -position.y, -position.z);
}

void generate_normals(meshf *mesh) {
    if (mesh->element_count % 3 == 0) {
        int triangle_count = mesh->element_count / 3;
        for (int i = 0; i < triangle_count; i++) {
            vec3f normal = triangle_normal3f(mesh->vertices[3*i + 0], mesh->vertices[3*i + 1], mesh->vertices[3*i + 2]);
            normal = unit_vector3f(normal);

            mesh->normals[3*i + 0] = normal;
            mesh->normals[3*i + 1] = normal;
            mesh->normals[3*i + 2] = normal;
        }
    }
}

void generate_bounding_box(meshf *mesh) {
    for (int i = 0; i < (mesh->element_count); i++) {
        if (mesh->vertices[i].x < (mesh->bounding_box.a)) {
            mesh->bounding_box.a = mesh->vertices[i].x;

        } else if (mesh->vertices[i].x > (mesh->bounding_box.x)) {
            mesh->bounding_box.x = mesh->vertices[i].x;
        }
        if (mesh->vertices[i].y < (mesh->bounding_box.b)) {
            mesh->bounding_box.b = mesh->vertices[i].y;
        } else if (mesh->vertices[i].y > (mesh->bounding_box.y)) {
            mesh->bounding_box.y = mesh->vertices[i].y;
        }
        if (mesh->vertices[i].z < (mesh->bounding_box.c)) {
            mesh->bounding_box.c = mesh->vertices[i].z;
        } else if (mesh->vertices[i].z > (mesh->bounding_box.z)) {
            mesh->bounding_box.z = mesh->vertices[i].z;
        }
    }
}

void transform_mesh(meshf *mesh, vec3f transform_point, vec3f scale, vec3f rotate, vec3f translate) { // scales then rotates then translates
    for (int i = 0; i < mesh->element_count; i++) {

        mesh->vertices[i] = vector_addition3f(mesh->vertices[i], translate);
    }
    generate_normals(mesh);
}

void expand_mesh(meshf *addition, meshf *mesh) {
    int old_element_count = mesh->element_count;
    mesh->element_count += addition->element_count;
    mesh->vertices = SDL_realloc(mesh->vertices, sizeof(vec3f) * mesh->element_count);
    mesh->normals = SDL_realloc(mesh->normals, sizeof(vec3f) * mesh->element_count);
    mesh->colors = SDL_realloc(mesh->colors, sizeof(vec3f) * mesh->element_count);
    int j = 0;
    for (int i = old_element_count; i < (mesh->element_count); i++) {
        mesh->vertices[i] = addition->vertices[j];
        mesh->normals[i] = addition->normals[j];
        mesh->colors[i] = addition->colors[j];
        j++;
    }
}

void shrink_mesh(int reduction, meshf *mesh) {

}

void set_mesh_color(vec4f color, meshf *mesh) {
    for (int i = 0; i < (mesh->element_count); i++) {
        mesh->colors[i] = color;
    }
}

void create_grid() {

}

meshf *create_UV_sphere_mesh(float radius, int sph_h, int sph_v) { // have this return a mesh instead (modify)
    if (sph_h >= 3 && sph_v >= 2) {
        int triangle_count = 2 * sph_h + (sph_v-2) * sph_h * 2;
        int vertex_count = triangle_count * 3;
        meshf *mesh = SDL_malloc(sizeof(meshf));
        mesh->element_count = vertex_count;
        mesh->vertices = SDL_malloc(sizeof(vec3f) * mesh->element_count);
        mesh->normals = SDL_malloc(sizeof(vec3f) * mesh->element_count);
        mesh->colors = SDL_malloc(sizeof(vec4f) * mesh->element_count);
        float p_loop_r;
        float loop_r;
        loop_r = radius * SDL_sinf(SDL_PI_F/sph_v); // sph_v = 6 --> r = 0.5
        for (int i = 0; i < sph_h; i++) { // top loop
            vec3f vert1 = {0, 0, radius};
            vec3f vert2 = {loop_r * SDL_sinf(2*(i-1)*SDL_PI_F/sph_h), loop_r * SDL_cosf(2*(i-1)*SDL_PI_F/sph_h), radius * SDL_cosf(SDL_PI_F/sph_v)};
            vec3f vert3 = {loop_r * SDL_sinf(2*(i)*SDL_PI_F/sph_h), loop_r * SDL_cosf(2*(i)*SDL_PI_F/sph_h), radius * SDL_cosf(SDL_PI_F/sph_v)};

            mesh->vertices[3*i + 0] = vert1;
            mesh->vertices[3*i + 1] = vert2;
            mesh->vertices[3*i + 2] = vert3;
        }
        for (int i = sph_h; i < 2*sph_h; i++) { // bottom loop
            vec3f vert1 = {0, 0, -radius};
            vec3f vert2 = {loop_r * SDL_sinf(2*(i)*SDL_PI_F/sph_h), loop_r * SDL_cosf(2*(i)*SDL_PI_F/sph_h), -radius * SDL_cosf(SDL_PI_F/sph_v)};
            vec3f vert3 = {loop_r * SDL_sinf(2*(i-1)*SDL_PI_F/sph_h), loop_r * SDL_cosf(2*(i-1)*SDL_PI_F/sph_h), -radius * SDL_cosf(SDL_PI_F/sph_v)};

            mesh->vertices[3*i + 0] = vert1;
            mesh->vertices[3*i + 1] = vert2;
            mesh->vertices[3*i + 2] = vert3;
        }
        for (int j = 2; j < sph_v; j++) { // "2" because of top&bottom triangle "caps" to UV sphere, so discounting those loops
            p_loop_r = radius * SDL_sinf((j-1)*SDL_PI_F/sph_v);
            loop_r = radius * SDL_sinf((j)*SDL_PI_F/sph_v);
            for (int i = 0; i < sph_h; i++) {
                vec3f vert1 = {p_loop_r * SDL_sinf(2*(i)*SDL_PI_F/sph_h), p_loop_r * SDL_cosf(2*(i)*SDL_PI_F/sph_h), radius * SDL_cosf((j-1)*SDL_PI_F/sph_v)};
                vec3f vert2 = {p_loop_r * SDL_sinf(2*(i-1)*SDL_PI_F/sph_h), p_loop_r * SDL_cosf(2*(i-1)*SDL_PI_F/sph_h), radius * SDL_cosf((j-1)*SDL_PI_F/sph_v)};
                vec3f vert3 = {loop_r * SDL_sinf(2*(i)*SDL_PI_F/sph_h), loop_r * SDL_cosf(2*(i)*SDL_PI_F/sph_h), radius * SDL_cosf((j)*SDL_PI_F/sph_v)};

                vec3f vert4 = {loop_r * SDL_sinf(2*(i)*SDL_PI_F/sph_h), loop_r * SDL_cosf(2*(i)*SDL_PI_F/sph_h), radius * SDL_cosf((j)*SDL_PI_F/sph_v)};
                vec3f vert5 = {p_loop_r * SDL_sinf(2*(i-1)*SDL_PI_F/sph_h), p_loop_r * SDL_cosf(2*(i-1)*SDL_PI_F/sph_h), radius * SDL_cosf((j-1)*SDL_PI_F/sph_v)};
                vec3f vert6 = {loop_r * SDL_sinf(2*(i-1)*SDL_PI_F/sph_h), loop_r * SDL_cosf(2*(i-1)*SDL_PI_F/sph_h), radius * SDL_cosf((j)*SDL_PI_F/sph_v)};

                int prev_tris = 2*sph_h + (j-2)*sph_h*2;

                mesh->vertices[prev_tris*3 + 3*(2*i) + 0] = vert1;
                mesh->vertices[prev_tris*3 + 3*(2*i) + 1] = vert2;
                mesh->vertices[prev_tris*3 + 3*(2*i) + 2] = vert3;

                mesh->vertices[prev_tris*3 + 3*(2*i+1) + 0] = vert4;
                mesh->vertices[prev_tris*3 + 3*(2*i+1) + 1] = vert5;
                mesh->vertices[prev_tris*3 + 3*(2*i+1) + 2] = vert6;
            }
        }
        generate_normals(mesh);
        generate_bounding_box(mesh);
        set_mesh_color(debug_color, mesh);
        return mesh;
    } else {
        SDL_Log("Failed to create UV sphere mesh (impossible splicing parameters)");
        return NULL;
    }
}

void load_fenics_mesh(char *rel_file_path, int *out_vertex_count, int *out_tetrahedron_count, vec32i3f **vertices, vec5i32 **tetrahedra) {
    char *file_path = NULL;
    SDL_asprintf(&file_path, "%s" "%s", SDL_GetBasePath(), rel_file_path);
    char *raw_data = SDL_LoadFile(file_path, NULL);
    int file_char_count = SDL_strlen(raw_data);
    SDL_Log("%" SDL_PRIu32 " characters loaded from file", file_char_count);
    int char_cursor = 0;
    int line_index = 0;
    int line_char = 0;
    char *number_string = SDL_malloc(sizeof(char) * 32);
    int digit_count = 0;
    int raw_vertex_count = 0;
    //vec32i3f *used_vertices = NULL;
    int tetrahedron_count = 0;
    int tetrahedron_index = 0;
    int vertices_revert_char_index = 0;
    int vertices_revert_line_index = 0;
    bool has_tetrahedra_completed = false;
    bool has_reverted_to_vertices = false;
    int highest_used_vertex = 0;
    while (char_cursor < file_char_count) { // literally a Turing machine lol
        char current_char = raw_data[char_cursor];

        // new line handling
        if (current_char == '>') {
            line_index++;
            char_cursor += 2;
            current_char = raw_data[char_cursor];
        }

        if (!has_tetrahedra_completed) {
            // determining vertex count
            while ((line_index == 2) && (raw_data[char_cursor + 1] != '>')) { // if next char isn't the line end
                if (SDL_isdigit(current_char)) { // || current_char == '.'
                    number_string[digit_count] = current_char;
                    digit_count++;
                    char_cursor++;
                } else {
                    char_cursor++; // advance until number string is encountered
                }
                current_char = raw_data[char_cursor];
            } if ((line_index == 2) && (raw_data[char_cursor + 1] == '>')) { // once done recording number
                number_string[digit_count] = '\0';
                digit_count = 0;
                raw_vertex_count = SDL_atoi(number_string);
                SDL_Log("%" SDL_PRIu32, raw_vertex_count);
                vertices_revert_char_index = char_cursor;
                vertices_revert_line_index = line_index;
                *vertices = (vec32i3f *) SDL_malloc(sizeof(vec32i3f) * raw_vertex_count); // Note
            }

            // determining tetrahedra count
            while ((line_index == (raw_vertex_count + 4)) && (raw_data[char_cursor + 1] != '>')) {
                if (SDL_isdigit(current_char)) { // || current_char == '.'
                    number_string[digit_count] = current_char;
                    digit_count++;
                    char_cursor++;
                } else {
                    char_cursor++; // advance until number string is encountered
                }
                current_char = raw_data[char_cursor];
            } if ((line_index == (raw_vertex_count + 4)) && (raw_data[char_cursor + 1] == '>')) {
                number_string[digit_count] = '\0';
                digit_count = 0;
                tetrahedron_count = SDL_atoi(number_string);
                *tetrahedra = (vec5i32 *) SDL_malloc(sizeof(vec5i32) * tetrahedron_count);
            }

            // tetrahedra
            while ((raw_vertex_count + 4 < line_index) && (line_index < raw_vertex_count + 4 + tetrahedron_count)) { // like above blocks, only runs with cursor in certain range/data-section
                for (int i = 0; i < tetrahedron_count; i++) { // runs once for each "tetrahedron" entry
                    for (int j = 0; j < 5; j++) { // runs once for each datum in each "tetrahedron" entry (5x)
                        bool has_encountered_datum = false;
                        while (!has_encountered_datum) {
                            if (raw_data[char_cursor] == '"') {
                                has_encountered_datum = true;
                                char_cursor++;
                            } else {
                                char_cursor++;
                            }
                        }
                        bool is_number = true;
                        digit_count = 0;
                        while (is_number) {
                            if (raw_data[char_cursor] == '"') {
                                char_cursor++;
                                is_number = false;
                                number_string[digit_count] = '\0';
                                int number = SDL_atoi(number_string);
                                if (j == 0) {
                                    (*tetrahedra)[i].a = number;
                                }
                                if (j == 1) {
                                    (*tetrahedra)[i].b = number;
                                }
                                if (j == 2) {
                                    (*tetrahedra)[i].x = number;
                                }
                                if (j == 3) {
                                    (*tetrahedra)[i].y = number;
                                }
                                if (j == 4) {
                                    (*tetrahedra)[i].z = number;
                                }
                                if ((j != 0) && (number >= highest_used_vertex)) {
                                    highest_used_vertex = number;
                                }
                            } else {
                                number_string[digit_count] = raw_data[char_cursor];
                                digit_count++;
                                char_cursor++;
                            }
                        }
                    }
                    bool has_line_terminated = false;
                    while (!has_line_terminated) {
                        if (raw_data[char_cursor] == '>') {
                            line_index++;
                            char_cursor += 2;
                            current_char = raw_data[char_cursor];
                            has_line_terminated = true;
                        } else {
                            char_cursor++;
                        }
                    }
                    if (i + 1 == tetrahedron_count) { // if tetrahedron loop is done
                        has_tetrahedra_completed = true;
                    }
                }
            }
        }

        // vertices
        if (has_tetrahedra_completed) {
            if (!has_reverted_to_vertices) {
                char_cursor = vertices_revert_char_index;
                line_index = vertices_revert_line_index - 1; // idk why (else vert0 = "0,0,0"), (I sorta do though)
                has_reverted_to_vertices = true;
            }
            //SDL_Log("T");
            while ((vertices_revert_line_index <= line_index) && (line_index < vertices_revert_line_index + raw_vertex_count)) {
                for (int i = 0; i < raw_vertex_count; i++) {
                    for (int j = 0; j < 4; j++) {
                        bool has_encountered_datum = false;
                        while (!has_encountered_datum) {
                            if (raw_data[char_cursor] == '"') {
                                has_encountered_datum = true;
                                char_cursor++;
                            } else {
                                char_cursor++;
                            }
                        }
                        bool is_number = true;
                        digit_count = 0;
                        while (is_number) {
                            //SDL_Log("T");
                            if (raw_data[char_cursor] == '"') {
                                char_cursor++;
                                is_number = false;
                                number_string[digit_count] = '\0';
                                if (j == 0) {
                                    (*vertices)[i].i = SDL_atoi(number_string);
                                }
                                if (j == 1) {
                                    (*vertices)[i].x = SDL_atof(number_string);
                                }
                                if (j == 2) {
                                    (*vertices)[i].y = SDL_atof(number_string);
                                }
                                if (j == 3) {
                                    (*vertices)[i].z = SDL_atof(number_string);
                                }
                            } else {
                                number_string[digit_count] = raw_data[char_cursor];
                                digit_count++;
                                char_cursor++;
                            }
                        }
                    }
                    bool has_line_terminated = false;
                    while (!has_line_terminated) {
                        if (raw_data[char_cursor] == '>') {
                            line_index++;
                            char_cursor += 2;
                            current_char = raw_data[char_cursor];
                            has_line_terminated = true;
                        } else {
                            char_cursor++;
                        }
                    }
                }
            }
        }

        char_cursor++;
    }
    int jack = 2184;
    SDL_Log("%" SDL_PRIu32 " %" SDL_PRIu32 " %" SDL_PRIu32 " %" SDL_PRIu32 " %" SDL_PRIu32, (unsigned int) (*tetrahedra)[jack].a, (unsigned int) (*tetrahedra)[jack].b, (unsigned int) (*tetrahedra)[jack].x, (unsigned int) (*tetrahedra)[jack].y, (unsigned int) (*tetrahedra)[jack].z);

    int black = 392;
    SDL_Log("%" SDL_PRIu32 " %.3f" " %.3f" " %.3f", (unsigned int) (*vertices)[black].i, (*vertices)[black].x, (*vertices)[black].y, (*vertices)[black].z);

    (*out_tetrahedron_count) = tetrahedron_count;
    (*out_vertex_count) = raw_vertex_count;

    SDL_free(number_string);
    SDL_free(raw_data);
    SDL_free(file_path);
}

void connect_mesh(vec32i3f **nodes, vec5i32 **cells, int vertex_count, int node_count, int32 **nodes_in_cells) {
    /*
    v 0 = i x y z
    t0 = i v0 v1 v2 v3

    vert[i] = length t0 t1 t2 ... tn
    */
    SDL_free(nodes_in_cells); // just in case
    *nodes_in_cells = SDL_malloc(sizeof(*int32) * vertex_count); // array of pointers
    int32 node0, node1, node2, node3;
    for (int i = 0; i < node_count; i++) {
        node0 = *cells[i].b;
        if (nodes_in_cells[node0] == NULL) {
            nodes_in_cells[node0] = SDL_malloc(sizeof(int32) * 2);
            nodes_in_cells[node0][0] = 2; // number of entries in this (above pointer)'s array
            nodes_in_cells[node0][1] = i; // note offset
        } else {
            SDL_realloc(nodes_in_cells[node0], sizeof(int32) * nodes_in_cells);
        }
        node1 = *cells[i].x;
        node2 = *cells[i].y;
        node3 = *cells[i].z;
    }
}
