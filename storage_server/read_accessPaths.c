#include "read_accesPath.h"

int read_access_paths(SServer_details* SS){
    // Open the file for reading
   
    // Open the file for reading
    FILE *file = fopen("input.txt", "r");

    // Check if the file opened successfully
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

    // Initialize a buffer to store each line
    char line[1024];
    char *result = NULL;
    size_t result_size = 0;

    // Read lines from the file and concatenate them
    while (fgets(line, sizeof(line), file) != NULL) {
        // Remove the newline character from the line
        size_t line_length = strlen(line);
        if (line_length > 0 && line[line_length - 1] == '\n') {
            line[line_length - 1] = '\0';
        }

        // Append the line to the result string with a space separator
        if (result == NULL) {
            // If this is the first line, just duplicate it
            result = strdup(line);
            result_size = strlen(line);
        } else {
            // Allocate memory for the new result string
            result_size += line_length + 1; // +1 for the space separator
            result = (char *)realloc(result, result_size + 1); // +1 for the null terminator

            // Concatenate the line with a space
            strcat(result, " ");
            strcat(result, line);
        }
    }

    // Close the file
    fclose(file);
    strcpy(SS->accessible_paths,result);

    // Print the concatenated result
    // printf("Concatenated Result: %s\n", result);

    // Free the allocated memory
    free(result);

    return 0;
}




