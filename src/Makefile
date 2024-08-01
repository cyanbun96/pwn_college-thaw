# Define directories and source files
DIRS := 1 2 3 4 5 6 7 77 777

# Compiler and flags
CC := gcc
CFLAGS := -Wall -Wextra -O2

# Output suffix
SUFFIX := thaw

# Default target
all: $(addsuffix $(SUFFIX),$(DIRS))

# Define how to compile each source file
define COMPILE_template
$1$(SUFFIX): $1/$1.c
	$(CC) $(CFLAGS) $$< -o ../thaw/$(SUFFIX)$1/$(SUFFIX)$1
endef

# Apply the template for each directory
$(foreach dir,$(DIRS),$(eval $(call COMPILE_template,$(dir))))

# Clean up compiled binaries
clean:
	$(foreach dir,$(DIRS), rm -f $(dir)/$(SUFFIX)$(dir))
