SUBDIRS=libmoogutil libmoog libgmoog juno6

subdirs:
	[ -L util ] || ln -s libmoogutil util
	[ -L moog ] || ln -s libmoog moog
	[ -L gmoog ] || ln -s libgmoog gmoog
	@for x in $(SUBDIRS); \
	do \
		echo -e "\n\nMaking $(TARGET) in $$x\n\n";\
		(cd $$x; make -k $(TARGET));\
	done

%::
	make TARGET=$@ subdirs

juno6:
	[ -L util ] || ln -s libmoogutil util
	[ -L moog ] || ln -s libmoog moog
	[ -L gmoog ] || ln -s libgmoog gmoog

	make -C libmoogutil all
	make -C libmoog all
	make -C libgmoog all
	make -C juno6 all

.PHONY: subdirs

.PHONY: juno6
