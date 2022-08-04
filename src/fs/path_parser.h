#ifndef PATH_PARSER_H
#define PATH_PARSER_H
#define MAX_PATH 108

struct  path_root
{
    int drive_path;
    struct path_part* first;
    
};

struct path_part
{
    const char *part;
    struct path_part* next;
};

struct path_root* Pathparser_parse(char **path);

#endif