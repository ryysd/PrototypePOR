COMPILER = clang++
CFLAGS   = -std=c++11 -stdlib=libc++ -Wextra -g -MMD -MP
LDFLAGS  = -stdlib=libc++
LIBS     =
INCLUDE  = -I./thirdparty/picojson/include/ -I./thirdparty/prettyprint/include/
TARGET   = ./bin/por_proto
OBJDIR   = ./obj
ifeq "$(strip $(OBJDIR))" ""
  OBJDIR = .
endif
SOURCES  = $(shell find . -name "*.cc")#$(wildcard *.cpp)
OBJECTS  = $(addprefix $(OBJDIR)/, $(SOURCES:.cc=.o))
	DEPENDS  = $(OBJECTS:.o=.d)

$(TARGET): $(OBJECTS) $(LIBS)
	$(COMPILER) -o $@ $^ $(LDFLAGS)

$(OBJDIR)/%.o: %.cc
	@[ -d $(dir $@) ] || mkdir -p $(dir $@)
	$(COMPILER) $(CFLAGS) $(INCLUDE) -o $@ -c $<

all: clean $(TARGET)

clean:
	rm -f $(OBJECTS) $(DEPENDS) $(TARGET)
	#@rmdir --ignore-fail-on-non-empty `readlink -f $(OBJDIR)`

-include $(DEPENDS)
