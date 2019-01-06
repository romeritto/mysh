#include <sys/queue.h> // STAILQ
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

struct stailq_data_s {
    int value;
    STAILQ_ENTRY(stailq_data_s) entries;
};

typedef struct data_s data_t;
struct data_s {
    char * value;
    STAILQ_ENTRY(data_s) entries;
};

typedef STAILQ_HEAD(word_list_h, data_s) kok;

int main() {
    kok word_list;
    STAILQ_INIT(&word_list);
    char *jano = "jano";
    char *fero= "fero";
    data_t *data = malloc(sizeof(data_t));
    data->value = jano;
    STAILQ_INSERT_TAIL(&word_list, data, entries);
    // Read1.
    STAILQ_FOREACH(data, &word_list, entries) {
        printf("Read1: %s\n", data->value);
    }
    
    struct word_list_h daco;
    STAILQ_INIT(&daco);
    data = malloc(sizeof(data_t));
    data->value = fero;
    STAILQ_INSERT_TAIL(&daco, data, entries);
    // Read1.
    STAILQ_FOREACH(data, &daco, entries) {
        printf("Read1: %s\n", data->value);
    }
    return 0;
}
