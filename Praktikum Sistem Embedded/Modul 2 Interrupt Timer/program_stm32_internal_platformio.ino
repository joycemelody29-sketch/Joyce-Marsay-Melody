; PlatformIO - TIM2 Periodic Interrupt

[env:bluepill_f103c8]
platform = ststm32
board = bluepill_f103c8
framework = arduino
upload_protocol = stlink
monitor_speed = 115200

; For ST-Link V2
debug_tool = stlink

upload_flags =
    -c
    set CPUTAPID 0x2ba01477

build_flags =
    -D LED_BUILTIN=PA0

lib_deps =
    adafruit/Adafruit SSD1306@^2.5.7
    adafruit/Adafruit GFX Library@^1.11.3
