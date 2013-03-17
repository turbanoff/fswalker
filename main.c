#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "node.h"

final_node * next_level(final_node *node);

int main() {
    arr_elem root = {.filename = "/", .is_dir = 1, .size = 0};
    final_node *true_root = malloc(sizeof(final_node));
    true_root->child_count = 1;
    true_root->fs_elements = &root;
    true_root->next_sibling = true_root;
    true_root->parent = NULL;
    true_root->parent_index = 0;
    
    size_t child_count;
    
    arr_elem *root_elems = list_dir_elem(root.filename, &root, &child_count, &root.size);
    final_node *iter_node = malloc(sizeof(final_node));
    iter_node->child_count = child_count;
    iter_node->fs_elements = root_elems;
    iter_node->next_sibling = iter_node;
    iter_node->parent_index = 0;
    iter_node->parent = true_root;
    
    int level = 1;
    while (iter_node != NULL) {
        printf("\"%s\" level = %d size in bytes = %zd\n", root.filename, level, root.size);
        iter_node = next_level(iter_node);
        level++;
    }
}

final_node * next_level(final_node *node) {
    final_node *first = node;
    
    size_t alloc_size = 0;
    char *full_name = malloc(0);

    final_node *prev_child = NULL;
    final_node *first_in_next_level = NULL;
    do {
        if (node->child_count == 0) {
            node = node->next_sibling;
            continue;
        }
        
        //создадим полный путь к родителю
        size_t parent_len = 0;
        char *parent_path = malloc(1);
        parent_path[0] = '\0';
        final_node *travers_to_root = node;
        while (travers_to_root->parent != NULL) {
            arr_elem *next = travers_to_root->parent->fs_elements + travers_to_root->parent_index;
            size_t next_part_len = strlen(next->filename);
            size_t new_parent_len = next_part_len + parent_len + 1; //1 символ слеша
            char *new_parent_path = malloc(sizeof(char) * (new_parent_len + 1)); //1 завершающий ноль
            
            memcpy(new_parent_path, next->filename, sizeof(char) * next_part_len);
            new_parent_path[next_part_len] = '/';
            memcpy(new_parent_path + next_part_len + 1, parent_path, sizeof(char) * (parent_len + 1) );
            free(parent_path);
            parent_len = new_parent_len;
            parent_path = new_parent_path;
            travers_to_root = travers_to_root->parent;
        }
        
        for (size_t i=0; i<node->child_count; i++) {

            arr_elem *elem = &node->fs_elements[i];
            if (!elem->is_dir) {
                elem->child = NULL;
                continue;
            }
            
            elem->child = malloc(sizeof(final_node));
            if (first_in_next_level == NULL) {
                first_in_next_level = elem->child;
            }
            elem->child->parent = node;
            elem->child->parent_index = i;
            elem->child->next_sibling = NULL;

            if (prev_child != NULL) {
                prev_child->next_sibling = elem->child;
            }

            size_t next_file_len = strlen(elem->filename);
            if (parent_len + next_file_len + 1 > alloc_size) {
                alloc_size = parent_len + next_file_len + 1;
                free(full_name);
                full_name = malloc(alloc_size * sizeof(char));
            }
            strcpy(full_name, parent_path);
            strcpy(full_name + parent_len, elem->filename);

            elem->child->fs_elements = list_dir_elem(full_name, elem, &elem->child->child_count, &elem->size);
            //необходимо прибавить полученный размер в байтах ко всем родительским папкам
            final_node *to_root_for_size = node;
            if (elem->size != 0) {
                while (to_root_for_size->parent != NULL) {
                    to_root_for_size->parent->fs_elements[to_root_for_size->parent_index].size += elem->size;
                    to_root_for_size = to_root_for_size->parent;
                }
            }

            prev_child = elem->child;
        }
        
        node = node->next_sibling;
    } while (node != first);
    free(full_name);
    if (prev_child != NULL) {
        prev_child->next_sibling = first_in_next_level;
    }
    return first_in_next_level;
}