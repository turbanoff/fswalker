#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "node.h"

void show_arr_elems(arr_elem* arr, size_t size) {
    for (int i=0; i<size; i++) {
        printf("is_dir=%d size=%7zd %s\n", arr[i].is_dir, arr[i].size, arr[i].filename);
    }
}

int main() {
    arr_elem root = {.filename = "/", .is_dir = 1, .size = 0};
    
    size_t child_count;
    
    arr_elem* root_elems = list_dir_elem("/", &root, &child_count, &root.size);
    final_node* root_node = malloc(sizeof(final_node));
    root_node->child_count = child_count;
    root_node->fs_elements = root_elems;
    root_node->next_sibling = root_node;
    root_node->parent_index = 0;
    root_node->parent = NULL;
    
    show_arr_elems(root_elems, child_count);
    printf("\"%s\" size in bytes = %zd\n", root.filename, root.size);
}

void next_level(final_node* node, const char* parent_path) {
    final_node* first = node;
    
    size_t alloc_size = 0;
    char *full_name = NULL;
    size_t parent_size = strlen(parent_path);
    
    final_node *prev_child = NULL;
    do {        
        for (size_t i=0; i<node->child_count; i++) {

            arr_elem* elem = &node->fs_elements[i];
            if (!elem->is_dir) {
                elem->child = NULL;
                continue;
            }
            
            elem->child = malloc(sizeof(final_node));
            elem->child->parent = node;
            elem->child->parent_index = i;
            elem->child->next_sibling = NULL;

            if (prev_child != NULL) {
                prev_child->next_sibling = elem->child;
            }

            size_t next_file_len = strlen(elem->filename);
            if (parent_size + next_file_len + 1 > alloc_size) {
                alloc_size = parent_size + next_file_len + 1;
                free(full_name);
                full_name = malloc(alloc_size * sizeof(char));
            }
            strcpy(full_name, parent_path);
            strcpy(full_name + parent_size, elem->filename);

            printf("%s\n", full_name);            

//            elem->child->fs_elements = list_dir_elem(elem, &elem->child->child_count);

            prev_child = elem->child;
        }
        
        node = node->next_sibling;
    } while (node != first);
    prev_child->next_sibling = node->fs_elements[0].child;
    free(full_name);
}