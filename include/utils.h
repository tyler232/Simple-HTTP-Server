#ifndef UTILS_H
#define UTILS_H

/**
 * print_usage - Prints the usage instructions for the program.
 *
 * This function prints a message to stderr that describes how to use the program,
 * including the accepted command-line options and their descriptions.
 * It is typically called when the user provides invalid arguments or requests help.
 */
void print_usage();

/**
 * is_numeric - Checks if a given string is numeric.
 *
 * @param str: The string to check.
 * @return: 1 if the string is numeric (contains only digits), 0 otherwise.
 *
 * This function iterates through each character in the string and checks if it
 * is a digit. If all characters are digits, the function returns 1. Otherwise,
 * it returns 0.
 */
int is_numeric(const char *str);

/**
 * local_path_create - Create a local path from a rooted path
 *
 * @param path: The rooted path.
 * @return: A pointer to the local path string.
 *
 * This function adjusts the given path to a local path format. If the path is "/",
 * it returns a pointer to ".". Otherwise, it returns a pointer to the string starting
 * from the second character, effectively stripping the leading '/'.
 */
char *local_path_create(const char *path);

/**
 * sort_string_array - Sorts an array of strings in case-insensitive alphabetical order.
 *
 * @param array: The array of strings to sort.
 * @param size: The number of elements in the array.
 *
 * This function uses the qsort function to sort the array of strings.
 * The sorting is case-insensitive and is done in alphabetical order.
 */
void sort_string_array(char **array, int size);

#endif
