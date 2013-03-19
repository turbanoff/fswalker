#ifndef NODE_H
#define	NODE_H

struct final_node;

struct arr_elem {
    char *filename;
    int is_dir;
    unsigned long long size;
    struct final_node *child;
};
typedef struct arr_elem arr_elem;

struct final_node {
    struct final_node *next_sibling;
    struct final_node *parent;
    size_t parent_index;
    arr_elem *fs_elements;
    size_t child_count;
};
typedef struct final_node final_node;

arr_elem * list_dir_elem(char *parent_full_path, arr_elem *parent, size_t *dir_size, unsigned long long *size_in_bytes);
#endif	/* NODE_H */

