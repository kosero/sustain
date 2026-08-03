#ifndef SUSTAIN_CONFIG_H
#define SUSTAIN_CONFIG_H

typedef struct {
	int width;
	int height;
	unsigned int flags;
	char *title;
} WindowProperty;

WindowProperty *get_window_property(void);
void set_window_property(WindowProperty *window);
void window_property_init(void);

#endif // SUSTAIN_CONFIG_H
