include makeconfig

SUBDIRS = str core lua

all: $(SUBDIRS)

core: str
lua: core str

$(SUBDIRS):
	$(MAKE) -C $@

clean:
	@for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir clean; \
	done

.PHONY: clean all $(SUBDIRS)
