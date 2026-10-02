void load_fenics_mesh(char *rel_file_path, int32 *out_vertex_count, int32 *out_tetrahedron_count, int32 *out_triangle_count, vec32i3f **vertf, vec32i3d **vertd, vec32i3i128 **vert128, vec5i32 **tetrahedra, vec4i32 **triangles, uint32 *tri_index_cnt) {
    char *file_path = NULL;
    SDL_asprintf(&file_path, "%s" "%s", SDL_GetBasePath(), rel_file_path);
    char *raw_data = SDL_LoadFile(file_path, NULL);
    int file_char_count = SDL_strlen(raw_data);
    SDL_Log("%" SDL_PRIu32 " characters loaded from file : \"%s\"", file_char_count, file_path);
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
                SDL_Log("Vertex Count : %" SDL_PRIu32, raw_vertex_count);
                (*tri_index_cnt) = raw_vertex_count;
                vertices_revert_char_index = char_cursor;
                vertices_revert_line_index = line_index;
                *vertf = (vec32i3f *) SDL_malloc(sizeof(vec32i3f) * raw_vertex_count); // Note (?)
                *vertd = (vec32i3d *) SDL_malloc(sizeof(vec32i3d) * raw_vertex_count);
                *vert128 = (vec32i3i128 *) SDL_malloc(sizeof(vec32i3i128) * raw_vertex_count);
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
                *triangles = (vec4i32 *) SDL_malloc(sizeof(vec4i32) * tetrahedron_count * 4);
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
                                    (*vertd)[i].w = SDL_atoi(number_string);
                                }
                                if (j == 1) {
                                    (*vertd)[i].x = SDL_strtod(number_string, NULL);
                                }
                                if (j == 2) {
                                    (*vertd)[i].y = SDL_strtod(number_string, NULL);
                                }
                                if (j == 3) {
                                    (*vertd)[i].z = SDL_strtod(number_string, NULL);
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
    for (int i = 0; i < tetrahedron_count; i++) {
        (*triangles)[4*i + 0].w = 4*i + 0;
        (*triangles)[4*i + 0].x = (*tetrahedra)[i].b; // v0
        (*triangles)[4*i + 0].y = (*tetrahedra)[i].x; // v1
        (*triangles)[4*i + 0].z = (*tetrahedra)[i].y; // v2

        (*triangles)[4*i + 1].w = 4*i + 1;
        (*triangles)[4*i + 1].x = (*tetrahedra)[i].z; // v3
        (*triangles)[4*i + 1].y = (*tetrahedra)[i].y; // v2
        (*triangles)[4*i + 1].z = (*tetrahedra)[i].x; // v1

        (*triangles)[4*i + 2].w = 4*i + 2;
        (*triangles)[4*i + 2].x = (*tetrahedra)[i].b; // v0
        (*triangles)[4*i + 2].y = (*tetrahedra)[i].y; // v2
        (*triangles)[4*i + 2].z = (*tetrahedra)[i].z; // v3

        (*triangles)[4*i + 3].w = 4*i + 3;
        (*triangles)[4*i + 3].x = (*tetrahedra)[i].z; // v3
        (*triangles)[4*i + 3].y = (*tetrahedra)[i].x; // v1
        (*triangles)[4*i + 3].z = (*tetrahedra)[i].b; // v0
    }
    //SDL_Log("hmm %" SDL_PRIu32, (*triangles)[5].y);
    //int jack = 2184;
    //SDL_Log("%" SDL_PRIu32 " %" SDL_PRIu32 " %" SDL_PRIu32 " %" SDL_PRIu32 " %" SDL_PRIu32, (unsigned int) (*tetrahedra)[jack].a, (unsigned int) (*tetrahedra)[jack].b, (unsigned int) (*tetrahedra)[jack].x, (unsigned int) (*tetrahedra)[jack].y, (unsigned int) (*tetrahedra)[jack].z);

    //int black = 392;
    //SDL_Log("%" SDL_PRIu32 " %.3f" " %.3f" " %.3f", (unsigned int) (*vertices)[black].i, (*vertices)[black].x, (*vertices)[black].y, (*vertices)[black].z);

    (*out_tetrahedron_count) = tetrahedron_count;
    (*out_vertex_count) = raw_vertex_count;
    (*out_triangle_count) = tetrahedron_count * 4;

    SDL_free(number_string);
    SDL_free(raw_data);
    SDL_free(file_path);
}

void vertd_i_to_vert128_scaled(int32 *vertex_count, vec32i3d **vertd, vec32i3i128 **vert128, double scale) {
    for (int i = 0; i < (*vertex_count); i++) {
        double tmp_x = (double) (*vertd)[i].x * scale;
        double tmp_y = (double) (*vertd)[i].y * scale;
        double tmp_z = (double) (*vertd)[i].z * scale;
        (*vert128)[i].x = (int128) tmp_x;
        (*vert128)[i].y = (int128) tmp_y;
        (*vert128)[i].z = (int128) tmp_z;
        //SDL_Log("HYI - %.3f", (float) (*vert128)[i].x);
    }
    //SDL_Log("HYI - %.3f", (float) (*vert128)[6].x);
    return;
}

void vert128_translate(int32 *vertex_count, vec32i3i128 **vert128, vec3i128 translate) {
    for (int i = 0; i < (*vertex_count); i++) {
        (*vert128)[i].x = (int128) (*vert128)[i].x + translate.x;
        (*vert128)[i].y = (int128) (*vert128)[i].y + translate.y;
        (*vert128)[i].z = (int128) (*vert128)[i].z + translate.z;
    }
    return;
}

void vert128_to_verf_i(int32 *vertex_count, vec32i3i128 **vert128, vec32i3f **vertf) {
    //SDL_Log("%" SDL_PRIu32, (*vertex_count));
    for (int i = 0; i < (*vertex_count); i++) {
        (*vertf)[i].x = (float) (*vert128)[i].x;
        (*vertf)[i].y = (float) (*vert128)[i].y;
        (*vertf)[i].z = (float) (*vert128)[i].z;
    }
    return;
}

void vert128_to_verf(int32 *vertex_count, vec32i3i128 **vert128, vec3f **vertf) {
    //SDL_Log("%" SDL_PRIu32, (*vertex_count));
    for (int i = 0; i < (*vertex_count); i++) {
        (*vertf)[i].x = (float) ((double) (*vert128)[i].x/SCALE);
        (*vertf)[i].y = (float) ((double) (*vert128)[i].y/SCALE);
        (*vertf)[i].z = (float) ((double) (*vert128)[i].z/SCALE);
    }
    return;
}

void vec3f_to_vert128(int32 *vertex_count, vec3f **vertf, vec32i3i128 **vert128, double scale) {
    for (int i = 0; i < (*vertex_count); i++) {
        float tmp_x = (float) (*vertf)[i].x * scale;
        float tmp_y = (float) (*vertf)[i].y * scale;
        float tmp_z = (float) (*vertf)[i].z * scale;
        (*vert128)[i].x = (int128) tmp_x;
        (*vert128)[i].y = (int128) tmp_y;
        (*vert128)[i].z = (int128) tmp_z;
        //SDL_Log("HYI - %.3f", (float) (*vert128)[i].x);
    }
    //SDL_Log("HYI - %.3f", (float) (*vert128)[6].x);
    return;
}

void sort3i32(int32 *a, int32 *b, int32 *c) { // 1, 2, ... (increasing order); abc, acb, cab, bac, bca, cba
    int32 at = (*a);
    int32 bt = (*b);
    int32 ct = (*c);
    if ((a < b) && (b < c)) {           // abc
        return;
    } else if ((a < c) && (c < b)) {    // acb ; to-do tree optimize?
        (*a) = at;
        (*b) = ct;
        (*c) = bt;
        return;
    } else if ((c < a) && (a < b)) {    // cab
        (*a) = ct;
        (*b) = at;
        (*c) = bt;
        return;
    } else if ((b < a) && (a < c)) {    // bac
        (*a) = bt;
        (*b) = at;
        (*c) = ct;
        return;
    } else if ((b < c) && (c < a)) {    // bca
        (*a) = bt;
        (*b) = ct;
        (*c) = at;
        return;
    } else if ((c < b) && (b < a)) {    // cba
        (*a) = ct;
        (*b) = bt;
        (*c) = at;
        return;
    } else {
        return; // shouldn't happen
    }
}

int comp32i(const void *a, const void *b) { // https://www.geeksforgeeks.org/c/qsort-function-in-c/
    int x = *(const int *)a;
    int y = *(const int *)b;

    if (x < y)
        return -1;
    if (x > y)
        return 1;
    return 0;
}

int compd(const void *a, const void *b) {
    double x = *(const double *)a;
    double y = *(const double *)b;

    if (x < y)
        return -1;
    if (x > y)
        return 1;
    return 0;
}

int comp128ui(const void *a, const void *b) {
    uint128 x = *(const uint128 *)a;
    uint128 y = *(const uint128 *)b;

    if (x < y)
        return -1;
    if (x > y)
        return 1;
    return 0;
}

void i128_to_binary_string(int128 N, char *str) { // https://www.geeksforgeeks.org/c/how-to-convert-an-integer-to-a-string-in-c/
    int i = 0;

    // Save the copy of the number for sign
    int128 sign = N;

    // If the number is negative, make it positive
    if (N < 0)
        N = -N;

    // Extract digits from the number and add them to the
    // string
    while (N > 0) {

        // Convert integer digit to character and store
        // it in the str
        str[i++] = N % 2 + '0';
        N /= 2;
    }

    // If the number was negative, add a minus sign to the
    // string
    if (sign < 0) {
        str[i++] = '-';
    }

    // Null-terminate the string
    str[i] = '\0';

    // Reverse the string to get the correct order
    for (int j = 0, k = i - 1; j < k; j++, k--) {
        char temp = str[j];
        str[j] = str[k];
        str[k] = temp;
    }
}

//int SDLCALL compare(const void *a, const void *b) // https://wiki.libsdl.org/SDL3/SDL_qsort

void remove_shared_faces (int32 triangle_count, vec4i32 **triangles) {
    uint128 triangle_ids[triangle_count];
    for (int i = 0; i < triangle_count; i++) {
        uint32 ai = (*triangles)[i].x; // a,b,c,d > 0
        uint32 bi = (*triangles)[i].y;
        uint32 ci = (*triangles)[i].z;
        uint32 di = (*triangles)[i].w;
        uint32 i_arr[3] = {ai, bi, ci};
        SDL_qsort(i_arr, 3, sizeof(i_arr[0]), comp32i);
        triangle_ids[i] = (uint128) ((int128) di) + ((int128) i_arr[0])*((int128) 1<<32) + ((int128) i_arr[1])*((int128) 1<<64) + ((int128) i_arr[2])*((int128) 1<<96);
    }
    /*char bufferg[129];
    i128_to_binary_string((int128) (*triangles)[63].x, bufferg);
    SDL_Log("X I : %s", bufferg);
    i128_to_binary_string((int128) (*triangles)[63].y, bufferg);
    SDL_Log("Y I : %s", bufferg);
    i128_to_binary_string((int128) (*triangles)[63].z, bufferg);
    SDL_Log("Z I : %s", bufferg);
    i128_to_binary_string((int128) (*triangles)[63].w, bufferg);
    SDL_Log("W I : %s", bufferg);
    i128_to_binary_string((int128) triangle_ids[63], bufferg);
    SDL_Log("HMM I : %s", bufferg);*/

    SDL_qsort(triangle_ids, triangle_count, sizeof(triangle_ids[0]), comp128ui); // x y z w (largest to smallest)
    int32 ch = 0;
    while (ch < triangle_count - 1) {
        uint32 *id_1 = (uint32*) (triangle_ids + (ch)); // smallest value = .w^(above)
        uint32 *id_2 = (uint32*) (triangle_ids + (ch + 1));
        uint128 triangle_1 = triangle_ids[ch];// (*id_1);
        uint128 triangle_2 = triangle_ids[ch+1];
        bool *mod1 = (bool*) &triangle_1;
        bool *mod2 = (bool*) &triangle_2;
        for (int i = 0; i < sizeof(uint32); i++) { // clear index value from triangle id's
            *(mod1 + i) = 0;
            *(mod2 + i) = 0;
        }
        /*if ((*id_1) == 63) {
            i128_to_binary_string((int128) triangle_1, bufferg);
            SDL_Log("11111 : %s", bufferg);
            i128_to_binary_string((int128) triangle_2, bufferg);
            SDL_Log("77777 : %s", bufferg);
        }
        if ((*id_2 == 63)) {
            i128_to_binary_string((int128) triangle_2, bufferg);
            SDL_Log("22222 : %s", bufferg);
            i128_to_binary_string((int128) triangle_1, bufferg);
            SDL_Log("88888 : %s", bufferg);
        }*/
        /*SDL_Log("id1 # : %d", (*id_1));
        SDL_Log("id2 # : %d", (*id_2));
        char buffer1[129];
        char buffer2[129];
        i128_to_binary_string((int128) triangle_1, buffer1);
        //i128_to_binary_string((int128) triangle_id_1 * ((int128) 2<<32), buffer2);
        i128_to_binary_string((int128) triangle_2, buffer2);
        SDL_Log("ye I : %s", buffer1);
        SDL_Log("no i : %s", buffer2);
        SDL_Log("");*/
        if (triangle_1 == triangle_2) {
            (*triangles)[(*id_1)].w = -(*triangles)[(*id_1)].w - 1;
            (*triangles)[(*id_2)].w = -(*triangles)[(*id_2)].w - 1;
            ch += 2;
        } else {
            ch += 1;
        }
    }
    /*for (int i = 0; i < triangle_count; i++) {
        int32 ai = (*triangles)[i].x;
        int32 bi = (*triangles)[i].y;
        int32 ci = (*triangles)[i].z;
        int32 i_arr[3] = {ai, bi, ci};
        SDL_qsort(i_arr, 3, sizeof(i_arr[0]), comp32i);
        int128 triangle_id_i = (int128) ((int128) i_arr[0]) + ((int128) i_arr[1])*((int128) 1<<32) + ((int128) i_arr[2])*((int128) 1<<64);
        int128 triangle_id_i_rm = -triangle_id_i - 1;
        /*for (int j = 0; j < triangle_count; j++) { //(O(n^yikes))
            if (j != i) {
                int32 aj = (*triangles)[j].x;
                int32 bj = (*triangles)[j].y;
                int32 cj = (*triangles)[j].z;
                int32 j_arr[3] = {aj, bj, cj};
                SDL_qsort(j_arr, 3, sizeof(j_arr[0]), comp32i);
                int128 triangle_id_j = (int128) ((int128) j_arr[0]) + ((int128) j_arr[1])*((int128) 1<<32) + ((int128) j_arr[2])*((int128) 1<<64);
                if (((*triangles)[i].w >= 0) && (triangle_id_i == triangle_id_j)) {
                    (*triangles)[i].w = -(*triangles)[i].w - 1;
                }
                /*if (triangle_ids[j] == triangle_id_i_rm) {
                    (*triangles)[i].w = -(*triangles)[i].w - 1;
                }*/
            /*}
        }
    }*/
}

void rotate_128i(int32 vertex_count, vec32i3i128 **vert128, vec3i128 CoR, vec3f rotation_axis, float angle) {
    hold_rendering = true;
    for (int i = 0; i < vertex_count; i++) {
        (*vert128)[i].x = (*vert128)[i].x - CoR.x;
        (*vert128)[i].y = (*vert128)[i].y - CoR.y;
        (*vert128)[i].z = (*vert128)[i].z - CoR.z;
    }

    vec3f *vert3flocal = (vec3f *) SDL_malloc(sizeof(vec3f) * vertex_count);
    vert128_to_verf(&vertex_count, vert128, &vert3flocal);

    float rotation_matrix[4][4] = {{1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1}};
    for (int i = 0; i < vertex_count; i++) {
        glm_rotate_make(rotation_matrix, angle, (float *) &rotation_axis);
    }

    for (int i = 0; i < vertex_count; i++) { // https://en.wikipedia.org/wiki/Rotation_matrix#General_3D_rotations
        float x_prev = vert3flocal[i].x;
        float y_prev = vert3flocal[i].y;
        float z_prev = vert3flocal[i].z;

        vert3flocal[i].x = x_prev * rotation_matrix[0][0] + y_prev * rotation_matrix[0][1] + z_prev * rotation_matrix[0][2];
        vert3flocal[i].y = x_prev * rotation_matrix[1][0] + y_prev * rotation_matrix[1][1] + z_prev * rotation_matrix[1][2];
        vert3flocal[i].z = x_prev * rotation_matrix[2][0] + y_prev * rotation_matrix[2][1] + z_prev * rotation_matrix[2][2];
    }

    vec3f_to_vert128(&vertex_count, &vert3flocal, vert128, SCALE);
    SDL_free(vert3flocal);

    for (int i = 0; i < vertex_count; i++) {
        (*vert128)[i].x = (*vert128)[i].x + CoR.x;
        (*vert128)[i].y = (*vert128)[i].y + CoR.y;
        (*vert128)[i].z = (*vert128)[i].z + CoR.z;
    }
    hold_rendering = false;
    return;
}

void copy_128mesh(int32 node_count, vec32i3i128 **source, vec32i3i128 **destination) {
    SDL_free(*destination);
    *destination = (vec32i3i128 *) SDL_malloc(sizeof(vec32i3i128) * node_count);
    for (int i = 0; i < node_count; i++) {
        (*destination)[i].w = (*source)[i].w;
        (*destination)[i].x = (*source)[i].x;
        (*destination)[i].y = (*source)[i].y;
        (*destination)[i].z = (*source)[i].z;
    }
    return;
}
