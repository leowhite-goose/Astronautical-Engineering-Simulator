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

void vertd_to_vert128_scaled(int32 *vertex_count, vec32i3d **vertd, vec32i3i128 **vert128, double scale) {
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

void vert128_to_verf(int32 *vertex_count, vec32i3i128 **vert128, vec32i3f **vertf) {
    //SDL_Log("%" SDL_PRIu32, (*vertex_count));
    for (int i = 0; i < (*vertex_count); i++) {
        (*vertf)[i].x = (float) (*vert128)[i].x;
        (*vertf)[i].y = (float) (*vert128)[i].y;
        (*vertf)[i].z = (float) (*vert128)[i].z;
    }
    return;
}

void vert128_to_verf_graphics(int32 *vertex_count, vec32i3i128 **vert128, vec32i3f **vertf) {
    //SDL_Log("%" SDL_PRIu32, (*vertex_count));
    for (int i = 0; i < (*vertex_count); i++) {
        (*vertf)[i].x = (float) (*vert128)[i].x / 12;
        (*vertf)[i].y = (float) (*vert128)[i].y / 12;
        (*vertf)[i].z = (float) (*vert128)[i].z / 12;
    }
    return;
}

void sort3i32(int32 *a, int32 *b, int32 *c) { // 1, 2, ... (increasing order); abc, acb, cab, bac, bca, cba
    if ((a < b) && (b < c)) {           // abc
        return;
    } else if ((a < c) && (c < b)) {    // acb ; to-do tree optimize?
        (*a) = (*a);
        (*b) = (*c);
        (*c) = (*b);
        return;
    } else if ((c < a) && (a < b)) {    // cab
        (*a) = (*c);
        (*b) = (*a);
        (*c) = (*b);
        return;
    } else if ((b < a) && (a < c)) {    // bac
        (*a) = (*b);
        (*b) = (*a);
        (*c) = (*c);
        return;
    } else if ((b < c) && (c < a)) {    // bca
        (*a) = (*b);
        (*b) = (*c);
        (*c) = (*a);
        return;
    } else if ((c < b) && (b < a)) {    // cba
        (*a) = (*c);
        (*b) = (*b);
        (*c) = (*a);
        return;
    } else {
        return; // shouldn't happen
    }
}

int comp(const void *a, const void *b) { // https://www.geeksforgeeks.org/c/qsort-function-in-c/
    int x = *(const int *)a;
    int y = *(const int *)b;

    if (x < y)
        return -1;
    if (x > y)
        return 1;
    return 0;
}

//int SDLCALL compare(const void *a, const void *b) // https://wiki.libsdl.org/SDL3/SDL_qsort

void remove_shared_faces (int32 triangle_count, vec4i32 **triangles) {
    vec4i32 ordered_triangle_values[triangle_count];
    int32 triangle_ids[triangle_count*3];
    int48 triangle_prime_ids[triangle_count];
    //vec3i32 primes = {32771, 32779, 32783}; // must be fewer than just over 2^15 triangles; http://compoasso.free.fr/primelistweb/page/prime/liste_online_en.php
    vec3i32 primes = {2097169, 2097211, 2097223}; // 2097169 2^21
    for (int i = 0; i < triangle_count; i++) {
        int32 id = (*triangles)[i].w;
        int32 a = (*triangles)[i].x;
        int32 b = (*triangles)[i].y;
        int32 c = (*triangles)[i].z;
        sort3i32(&a, &b, &c);
        triangle_prime_ids[i] = a*primes.x + b*primes.y + c*primes.z;
        /*ordered_triangle_values[i].w = id;
        ordered_triangle_values[i].x = a;
        ordered_triangle_values[i].y = b;
        ordered_triangle_values[i].z = c;*/
        //SDL_Log("%" SDL_PRIu32 "J0 - %" SDL_PRIu64, i, triangle_ids[i]);
        //triangle_ids[i] = (int128) a + b<<35 + c<<67; //a,b,c need to be positive; padding = 3*
        triangle_ids[3*i + 0] = a;
        triangle_ids[3*i + 2] = b;
        triangle_ids[3*i + 1] = c;
        //SDL_Log("%" SDL_PRIu32 "Jabc - %" SDL_PRIu64, i, triangle_ids[i] - b<<35 - a - c<<67);
    } // all sets of same numbers are now identical: {1,2,3},{3,1,2} --> {1,2,3},{1,2,3} <-- both have same "id"
    char *triangleidchar;
    triangleidchar = (char *) triangle_ids;
    SDL_qsort(triangle_ids, triangle_count, sizeof(triangle_ids[0]*3), comp);
    int ch;
    while (ch < triangle_count - 1) {
        int96 triangle_id_1;
        int96 triangle_id_2;
        triangle_id_1 = (int96) *(triangle_ids + ch);
        triangle_id_2 = (int96) *(triangle_ids + ch + 1);
        if (triangle_id_1 == triangle_id_2) {
            if (triangle_id_1 >= 0) {
                triangle_id_1 = -triangle_id_1 - 1;
                //SDL_Log("%" SDL_PRIs32, i);
                SDL_Log("i1;%" SDL_PRIs32 " - %" SDL_PRIs32, ch, triangle_id_1);
            }
            if (triangle_id_2 >= 0) {
                triangle_id_2 = -triangle_id_2 - 1;
                //SDL_Log("%" SDL_PRIs32, i);
                SDL_Log("i2;%" SDL_PRIs32 " - %" SDL_PRIs32, ch, triangle_id_1);
            }
            ch += 2;
        } else {
            ch++;
        }
    }
    /*for (int i = 0; i < triangle_count - 1; i++) { // moving window
        //SDL_Log("i - %" SDL_PRIu32 "; id - %" SDL_PRIu32, i, triangle_prime_ids[i]);
        if ((triangle_prime_ids[i] == triangle_prime_ids[i+1]) || (triangle_prime_ids[i] == -triangle_prime_ids[i+1] - 1)) {
            if (triangle_prime_ids[i] >= 0) {
                triangle_prime_ids[i] = -triangle_prime_ids[i] - 1;
                //SDL_Log("%" SDL_PRIs32, i);
                SDL_Log("i1;%" SDL_PRIs32 " - %" SDL_PRIs32, i, triangle_prime_ids[i]);
            }
            if (triangle_prime_ids[i+1] >= 0) {
                triangle_prime_ids[i+1] = -triangle_prime_ids[i+1] - 1;
                //SDL_Log("%" SDL_PRIs32, i);
                SDL_Log("i2;%" SDL_PRIs32 " - %" SDL_PRIs32, i, triangle_prime_ids[i]);
            }
        }
    }*/
    for (int i = 0; i < triangle_count; i++) {
        int32 id = (*triangles)[i].w;
        int32 a = (*triangles)[i].x;
        int32 b = (*triangles)[i].y;
        int32 c = (*triangles)[i].z;
        sort3i32(&a, &b, &c);
        //int48 triangle_prime_id = -a*primes.x + b*primes.y + c*primes.z;
        //int128 triangle_id = (int128) a;
        //triangle_id += (int128) b<<35;
        //triangle_id += (int128) c<<67;
        int32 triangle_id32[3];
        triangle_id32[0] = a;
        triangle_id32[1] = b;
        triangle_id32[2] = c;
        int96 triangle_id;
        triangle_id = (int96) *(triangle_ids);
        for (int j = 0; j < triangle_count; j++) { //(O(n^yikes))
            int96 triangle_id_j = (int96) *(triangle_ids + j);
            if ((-triangle_id - 1 == triangle_id_j) && (id >= 0)) {
                (*triangles)[i].w = -i - 1;
                SDL_Log("t: %" SDL_PRIs32, (*triangles)[i].w);
            }
        }
    }
}
