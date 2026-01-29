#ifndef SUSTAIN_UTIL_YAML_H
#define SUSTAIN_UTIL_YAML_H

#include <stdbool.h>
#include <stdio.h>
#include <yaml.h>

typedef struct SNYaml {
  yaml_emitter_t emitter;
  bool initialized;
} SNYaml;

typedef enum { YAML_STRING, YAML_INT, YAML_BOOL } SNYamlType;
typedef struct {
  char key[64];
  SNYamlType type;
  union {
    char string_val[128];
    int int_val;
    int bool_val;
  };
} SNYamlEntry;

typedef struct {
  // TODO: Implement dynamic array resizing to remove the static 32-entry limit
  SNYamlEntry entries[32];
  int count;
} SNYamlReader;

bool yaml_init(SNYaml *yml, FILE *outfile);
void yaml_close(SNYaml *yml, FILE *outfile);

void yaml_write_key_value(SNYaml *yml, const char *tag, const char *key,
                          const char *value);
int yaml_read_file(const char *filename, SNYamlReader *reader);

#endif
