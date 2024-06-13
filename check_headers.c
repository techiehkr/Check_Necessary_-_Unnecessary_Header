#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

// List of required headers
const char* required_headers[] = {
    "stdio.h",
    "stdlib.h",
    "string.h",
    // Add other required headers here
};

const int num_required_headers = sizeof(required_headers) / sizeof(required_headers[0]);

// Function to check if a header is in the list of required headers
int is_required_header(const char* header) {
    for (int i = 0; i < num_required_headers; i++) {
        if (strcmp(header, required_headers[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

// Function to process a single C file
void process_file(const char* filepath) {
    FILE* file = fopen(filepath, "r");
    if (!file) {
        perror("fopen");
        return;
    }

    char line[1024];
    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "#include", 8) == 0) {
            char header[256];
            if (sscanf(line, "#include <%255[^>]> ", header) == 1 || sscanf(line, "#include \"%255[^\"]\" ", header) == 1) {
                if (!is_required_header(header)) {
                    printf("File: %s, Unnecessary header: %s\n", filepath, header);
                }
            }
        }
    }

    fclose(file);
}

// Function to process all C files in a directory
void process_directory(const char* dirname) {
    struct dirent* entry;
    DIR* dp = opendir(dirname);

    if (dp == NULL) {
        perror("opendir");
        return;
    }

    while ((entry = readdir(dp))) {
        if (entry->d_type == DT_REG) {
            const char* filename = entry->d_name;
            const char* extension = strrchr(filename, '.');

            if (extension && strcmp(extension, ".c") == 0) {
                char filepath[1024];
                snprintf(filepath, sizeof(filepath), "%s/%s", dirname, filename);
                process_file(filepath);
            }
        }
    }

    closedir(dp);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <directory>\n", argv[0]);
        return 1;
    }

    const char* directory = argv[1];
    process_directory(directory);

    return 0;
}
