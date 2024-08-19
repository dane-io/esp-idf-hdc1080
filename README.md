Sample ESP-IDF project using custom driver for HDC1080. Note, a ESP-IDF component exists for the HDC1080, but it uses the deprecated i2c.h drivers which don't seem to play well with light sleep. The driver used here uses the new i2c_master.h drivers.

Dane
