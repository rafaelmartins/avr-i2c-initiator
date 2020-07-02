AVR_MCU ?= attiny24a
AVR_F_CPU ?= 8000000UL

AVR_LFUSE ?= 0xE2
AVR_HFUSE ?= 0xDF

FIRMWARE_SOURCES = \
	test.c \
	i2c-initiator.c \
	$(NULL)

FIRMWARE_HEADERS = \
	i2c-initiator.h \
	$(NULL)
