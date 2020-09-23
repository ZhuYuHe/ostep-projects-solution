void clean(char *line);

bool str_equal(char *s1, const char *s2);

bool search_cmd(char **cmd);

bool is_blank_char(char c);

bool empty(char *s);

size_t parse_line(char *line, char ***my_args);
