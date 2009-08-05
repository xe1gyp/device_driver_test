extern int twl4030_free_gpio(int gpio);
extern int twl4030_request_gpio(int gpio);
extern int twl4030_set_gpio_direction(int gpio, int is_input);
extern int twl4030_set_gpio_dataout(int gpio, int enable);
extern int twl4030_set_gpio_debounce(int gpio, int enable);

