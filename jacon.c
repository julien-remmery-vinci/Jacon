#define JACON_IMPLEMENTATION
#include "jacon.h"
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void
print_error(Jacon_Error error)
{
    switch (error) { 
        case JACON_ERR_INDEX_OUT_OF_BOUND:
            puts("Jacon error: JACON_INDEX_OUT_OF_BOUND");
            break;
        case JACON_ERR_MEMORY_ALLOCATION:
            puts("Jacon error: JACON_ALLOC_ERROR");
            break;
        case JACON_ERR_CHAR_NOT_FOUND:
            puts("Jacon error: JACON_ERR_CHAR_NOT_FOUND");
            break;
        case JACON_ERR_INVALID_JSON:
            puts("Jacon error: JACON_ERR_INVALID_JSON");
            break;
        case JACON_NO_MORE_TOKENS:
            puts("Jacon error: JACON_NO_MORE_TOKENS");
            break;
        case JACON_ERR_NULL_PARAM:
            puts("Jacon error: JACON_NULL_PARAM");
            break;
        case JACON_ERR_INVALID_ESCAPE_SEQUENCE:
            puts("Jacon error: JACON_ERR_INVALID_ESCAPE_SEQUENCE");
            break;
        case JACON_ERR_UNREACHABLE_STATEMENT:
            puts("Jacon error: JACON_ERROR_UNREACHABLE_STATEMENT");
            break;
        case JACON_ERR_INVALID_VALUE_TYPE:
            puts("Jacon error: JACON_ERR_INVALID_VALUE_TYPE");
            break;
        case JACON_ERR_EMPTY_INPUT:
            puts("Jacon error: JACON_ERR_EMPTY_INPUT");
            break;
        case JACON_ERR_INVALID_SIZE:
            puts("Jacon error: JACON_ERR_INVALID_SIZE");
            break;
        case JACON_ERR_APPEND_FSTRING:
            puts("Jacon error: JACON_ERR_APPEND_FSTRING");
            break;
        case JACON_END_OF_INPUT:
        case JACON_OK:
        default:
            break;
    }
}

int
main(int argc, const char** argv)
{
    if (argc <= 1) {
        printf("Please specify an input file\n");
        exit(EXIT_SUCCESS);
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        perror("Failed to open file");
        return 1;
    }

    struct stat fileStat;
    if (fstat(fd, &fileStat) == -1) {
        perror("fstat failed");
        close(fd);
        return -1;
    }
    off_t file_size = fileStat.st_size;

    // Allocate a buffer according to the file size 
    // (dont use astronomically big files for testing or buy more ram)
    char* json_str = (char*)malloc(file_size + 1 * sizeof(char));
    if (json_str == NULL) {
        perror("malloc error");
        close(fd);
        return JACON_ERR_MEMORY_ALLOCATION;
    }

    ssize_t nrread = read(fd, json_str, sizeof(char) * file_size);
    if (nrread != file_size) {
        perror("Failed to read file");
        free(json_str);
        close(fd);
        return 1;
    }

    // TODO : move in parsing process
    const char *found = memchr(json_str, '\0', file_size);
    if (found != NULL) {
        free(json_str);
        close(fd);
        exit(EXIT_FAILURE);
    }
    json_str[file_size] = '\0';

    Jacon_content content;
    Jacon_init_content(&content);
    struct timeval start, end;
    gettimeofday(&start, NULL);
    int ret = Jacon_deserialize(&content, json_str);
    gettimeofday(&end, NULL);
    
    double parse_timing =
        (double)((end.tv_sec - start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec) / 1000;
    
    print_error(ret);    

    // Single values
    // char* string;
    // Jacon_get_string(&content, &string);
    // puts(string);
    // int i;
    // Jacon_get_int(&content, &i);
    // printf("%d\n", i);
    // double d;
    // Jacon_get_double(&content, &d);
    // printf("%f\n", d);
    // float f;
    // Jacon_get_float(&content, &f);
    // printf("%f\n", f);

    // Object based
    // char* string;
    // Jacon_get_string_by_name(&content, "string", &string);
    // puts(string);
    // free(string);
    // bool tb;
    // Jacon_get_bool_by_name(&content, "truebool", &tb);
    // printf("%s\n", tb ? "true" : "false");
    // bool fb;
    // Jacon_get_bool_by_name(&content, "falsebool", &fb);
    // printf("%s\n", fb ? "true" : "false");
    // int i;
    // Jacon_get_int_by_name(&content, "int", &i);
    // printf("%d\n", i);
    // float f;
    // Jacon_get_float_by_name(&content, "float", &f);
    // printf("%f\n", f);
    // double d;
    // Jacon_get_double_by_name(&content, "double", &d);
    // printf("%.15f\n", d);
    // char* object;
    // Jacon_get_string_by_name(&content, "object.property", &object);
    // puts(object);
    // free(object);
    // char* nested_str;
    // Jacon_get_string_by_name(&content, "nestedobject.nestedProperty.innerProperty", &nested_str);
    // puts(nested_str);
    // free(nested_str);

    // Jacon_Node int_node = Jacon_int(1);
    // char* str_node;
    // ret = Jacon_serialize(&int_node, &str_node);
    // if (str_node != NULL) {
    //     puts("----Single value integer----");
    //     puts(str_node);
    //     free(str_node);
    // }

    // Jacon_Node object_node = Jacon_object();
    // Jacon_Node int_prop_node = Jacon_int_prop("int", 1);
    // Jacon_append_child(&object_node, &int_prop_node);
    // ret = Jacon_serialize(&object_node, &str_node);
    // if (str_node != NULL) {
    //     puts("----Ocject node with an integer property----");
    //     puts(str_node);
    //     free(str_node);
    // }

    // Jacon_Node int2_node = Jacon_int(2);
    // Jacon_Node array_prop_node = Jacon_array_prop("int_list");
    // Jacon_append_child(&array_prop_node, &int_node);
    // Jacon_append_child(&array_prop_node, &int2_node);
    // Jacon_append_child(&object_node, &array_prop_node);
    // ret = Jacon_serialize(&object_node, &str_node);
    // if (str_node != NULL) {
    //     puts("----Object node with an integer and an array of integers property----");
    //     puts(str_node);
    //     free(str_node);
    // }
    
    free(json_str);
    close(fd);
    ret = Jacon_free_content(&content);
    if (ret != JACON_OK) return ret;

    printf("Parse time: %lf ms\n", parse_timing);

    return JACON_OK;
}