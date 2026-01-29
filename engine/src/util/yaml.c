#include "sustain/util/yaml.h"
#include "sustain/core/log.h"

bool yaml_init(SNYaml *yml, FILE *outfile) {
  if (!yml || !outfile)
    return false;
  memset(yml, 0, sizeof(SNYaml));

  if (!yaml_emitter_initialize(&yml->emitter)) {
    SLOG_ERROR("YAML: Failed to initialize emitter");
    return false;
  }

  yml->initialized = true;
  yaml_emitter_set_output_file(&yml->emitter, outfile);

  yaml_event_t stream_event, doc_event, map_event;
  memset(&stream_event, 0, sizeof(yaml_event_t));
  memset(&doc_event, 0, sizeof(yaml_event_t));
  memset(&map_event, 0, sizeof(yaml_event_t));

  if (!yaml_stream_start_event_initialize(&stream_event, YAML_UTF8_ENCODING) ||
      !yaml_emitter_emit(&yml->emitter, &stream_event)) {
    SLOG_ERROR("YAML: Failed to start stream");
    return false;
  }

  if (!yaml_document_start_event_initialize(&doc_event, NULL, NULL, NULL, 0) ||
      !yaml_emitter_emit(&yml->emitter, &doc_event)) {
    SLOG_ERROR("YAML: Failed to start document");
    return false;
  }

  if (!yaml_mapping_start_event_initialize(&map_event, NULL, NULL, 1,
                                           YAML_BLOCK_MAPPING_STYLE) ||
      !yaml_emitter_emit(&yml->emitter, &map_event)) {
    SLOG_ERROR("YAML: Failed to start mapping");
    return false;
  }

  return true;
}

void yaml_close(SNYaml *yml, FILE *outfile) {
  if (!yml || !yml->initialized) {
    if (outfile)
      fclose(outfile);
    return;
  }

  yaml_event_t map_event, doc_event, stream_event;
  memset(&map_event, 0, sizeof(yaml_event_t));
  memset(&doc_event, 0, sizeof(yaml_event_t));
  memset(&stream_event, 0, sizeof(yaml_event_t));

  if (yaml_mapping_end_event_initialize(&map_event))
    yaml_emitter_emit(&yml->emitter, &map_event);

  if (yaml_document_end_event_initialize(&doc_event, 0))
    yaml_emitter_emit(&yml->emitter, &doc_event);

  if (yaml_stream_end_event_initialize(&stream_event))
    yaml_emitter_emit(&yml->emitter, &stream_event);

  yaml_emitter_delete(&yml->emitter);
  yml->initialized = false;
  if (outfile)
    fclose(outfile);
}

void yaml_write_key_value(SNYaml *yml, const char *tag, const char *key,
                          const char *value) {
  if (!yml || !yml->initialized || !key || !value) {
    SLOG_ERROR("YAML: Attempted to write to uninitialized yml or NULL "
               "key/value (key: %s)\n",
               key ? key : "NULL");
    return;
  }

  yaml_event_t key_event, value_event;
  memset(&key_event, 0, sizeof(yaml_event_t));
  memset(&value_event, 0, sizeof(yaml_event_t));

  // Key
  if (!yaml_scalar_event_initialize(&key_event, NULL, NULL, (yaml_char_t *)key,
                                    -1, 1, 0, YAML_PLAIN_SCALAR_STYLE)) {
    SLOG_ERROR("YAML: Failed to initialize key event for %s", key);
    return;
  }
  if (!yaml_emitter_emit(&yml->emitter, &key_event)) {
    SLOG_ERROR("YAML: Failed to emit key event for %s", key);
    return;
  }

  // Value
  if (!yaml_scalar_event_initialize(&value_event, NULL, (yaml_char_t *)tag,
                                    (yaml_char_t *)value, -1, 1, 0,
                                    YAML_PLAIN_SCALAR_STYLE)) {
    SLOG_ERROR("YAML: Failed to initialize value event for %s", key);
    return;
  }
  if (!yaml_emitter_emit(&yml->emitter, &value_event)) {
    SLOG_ERROR("YAML: Failed to emit value event for %s", key);
    return;
  }
}

int yaml_read_file(const char *filename, SNYamlReader *reader) {
  FILE *infile = fopen(filename, "r");
  if (!infile) {
    perror("fopen");
    return 0;
  }

  yaml_parser_t parser;
  yaml_event_t event;

  if (!yaml_parser_initialize(&parser)) {
    fprintf(stderr, "Failed to intialize parser\n");
    fclose(infile);
    return 0;
  }

  yaml_parser_set_input_file(&parser, infile);

  int done = 0;
  char last_key[64] = {0};

  reader->count = 0;

  while (!done) {
    if (!yaml_parser_parse(&parser, &event)) {
      fprintf(stderr, "Parser error %d\n", parser.error);
      break;
    }

    if (event.type == YAML_SCALAR_EVENT) {
      if (last_key[0] == '\0') {
        strncpy(last_key, (const char *)event.data.scalar.value,
                sizeof(last_key) - 1);
        last_key[sizeof(last_key) - 1] = '\0';
      } else {
        if (reader->count >= 32)
          break;

        SNYamlEntry *entry = &reader->entries[reader->count++];
        strncpy(entry->key, last_key, sizeof(entry->key) - 1);
        entry->key[sizeof(entry->key) - 1] = '\0';
        last_key[0] = '\0';

        if (event.data.scalar.tag &&
            strcmp((char *)event.data.scalar.tag, "!!int") == 0) {
          entry->type = YAML_INT;
          entry->int_val = atoi((char *)event.data.scalar.value);
        } else if (event.data.scalar.tag &&
                   strcmp((char *)event.data.scalar.tag, "!!bool") == 0) {
          entry->type = YAML_BOOL;
          entry->bool_val =
              (strcmp((char *)event.data.scalar.value, "true") == 0) ? 1 : 0;
        } else {
          entry->type = YAML_STRING;
          strncpy(entry->string_val, (char *)event.data.scalar.value,
                  sizeof(entry->string_val) - 1);
          entry->string_val[sizeof(entry->string_val) - 1] = '\0';
        }
      }
    } else if (event.type == YAML_STREAM_END_EVENT) {
      done = 1;
    }

    yaml_event_delete(&event);
  }

  yaml_parser_delete(&parser);
  fclose(infile);
  return 1;
}
