#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "folders.h"
#include "node.h"

void show_arr_elems(arr_elem* arr, size_t size) {
    for (int i=0; i<size; i++) {
        printf("is_dir=%d size=%7zd %s\n", arr[i].is_dir, arr[i].size, arr[i].filename);
    }
}

int main() {
    arr_elem root;
    strcpy(root.filename, "/");
    root.is_dir = 1;
    root.size = 0;

    size_t child_count;
    arr_elem* root_childes = list_dir_elem(&root, &child_count);
    
    final_node fs;
    fs.child_count = child_count;
    fs.fs_elements = root_childes;
    fs.parent = NULL;
    fs.next_sibling = &fs;
    
    size_t parent_size = 1;
    size_t alloc_size = 0;
    char *full_name = NULL;
    
    for (size_t i=0; i<child_count; i++) {
        
        arr_elem* elem = &fs.fs_elements[i];

        size_t next_file_len = strlen(elem->filename);
        if (parent_size + next_file_len + 1 > alloc_size) {
            alloc_size = parent_size + next_file_len + 1;
            free(full_name);
            full_name = malloc(alloc_size * sizeof(char));
        }
        strcpy(full_name, "/");
        strcpy(full_name + parent_size, elem->filename);

        printf("%s\n", full_name);
        
        struct stat buf;
        lstat(full_name, &buf);
        
        if (S_ISDIR(buf.st_mode)) {
            elem->is_dir = 1;
            elem->size = 0;
        } else if (S_ISREG(buf.st_mode)) {
            elem->is_dir = 0;
            elem->size = buf.st_size;
        } else {
            elem->is_dir = 0;
            elem->size = 0;
        }
    }
    
    show_arr_elems(root_childes, child_count);
}

