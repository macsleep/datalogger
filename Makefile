
all:
	pio run
flash:
	pio run -t upload
clean:
	pio run -t clean
erase:
	pio run -t erase

