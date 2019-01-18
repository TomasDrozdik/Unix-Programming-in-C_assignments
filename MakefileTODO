MAKE=make

SUBDIRS= //TODO

all: $(SUBDIRS) cstyle

$(SUBDIRS) :
	@echo "=== $@"
	@$(MAKE) -C $@

clean cstyle:
	ret=0; \
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir $@; \
		if [ $$? -ne 0]; then \
			ret=1;
		fi
	done;
	exit $$ret

.PHONY: cstyle all clean $(SUBDIRS)
