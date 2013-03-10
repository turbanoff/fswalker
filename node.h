#ifndef NODE_H
#define	NODE_H

struct arr_elem {
    char filename[256];
    int is_dir;
    size_t size;
};
typedef struct arr_elem arr_elem;

struct final_node {
    struct final_node* next_sibling;
    struct final_node* parent;
    arr_elem* fs_elements;
    size_t child_count;
};
typedef struct final_node final_node;

arr_elem* list_dir_elem(arr_elem* parent, size_t* dir_size);
#endif	/* NODE_H */

