# SHELL += -x

CC  = gcc-4.8
CXX = g++-4.8

# We do not support clang yet
COMPILER = $(shell $(CXX) -v 2>&1 )
ifeq ($(findstring clang,$(COMPILER)),clang)
	die "Clang is not supported. Use g++>=4.7 instead (g++-4.8 recommended).";
endif

print-%:
	@echo '$*=$($*)'

all: required kaldi graphviz libcppa

.PHONY: required
required:
	which wget svn git $(CXX) $(CC)

.PHONY: clean
clean: kaldi/clean graphviz/clean libcppa/clean

.PHONY: kaldi/clean
kaldi/clean:
	$(MAKE) -C kaldi/tools clean; $(MAKE) -C kaldi/src clean; \
	rm -f kaldi/src/.install kaldi/src/.depend kaldi/src/.configure; \
	rm -f kaldi/tools/.install kaldi/tools/portaudio/.install

.PHONY: graphviz/clean
graphviz/clean: 
	$(MAKE) -C graphviz clean && rm -f graphviz/.install graphviz/.configure

.PHONY: libcppa/clean
libcppa/clean: 
	$(MAKE) -C libcppa clean && rm -f libcppa/.install libcppa/.configure

.PHONY: distclean
distclean:
	rm -rf kaldi graphviz libcppa

.PHONY: kaldi
kaldi: kaldi/src/.install

kaldi/src/.install: kaldi/src/.depend kaldi/tools/portaudio/.install
	cd kaldi/src && $(MAKE) CXX=$(CXX) && $(MAKE) online CXX=$(CXX) && \
	touch .install .depend .configure

kaldi/src/.depend: kaldi/src/.configure
	$(MAKE) -C kaldi/src CXX=$(CXX) depend

kaldi/src/.configure: kaldi/tools/.install
	cd kaldi/src && ./configure

kaldi/tools/portaudio/.install: kaldi/.checkout
	cd kaldi/tools && ./install_portaudio.sh && touch portaudio/.install

kaldi/tools/.install: kaldi/.checkout
	$(MAKE) -C kaldi/tools CXX=$(CXX) && touch $@

kaldi/.checkout:
	svn co svn://svn.code.sf.net/p/kaldi/code/trunk@3755 kaldi && touch $@

.PHONY: graphviz
graphviz: graphviz/.install

graphviz/.install: graphviz/.configure
	cd graphviz && $(MAKE) && $(MAKE) install && \
	touch .install .configure

graphviz/.configure: graphviz/.checkout
	cd graphviz && \
	./configure --prefix=`pwd` --without-x CC=$(CC) CXX=$(CXX)

graphviz/.checkout:
	wget -T 10 -t 3 --no-check-certificate \
	http://www.graphviz.org/pub/graphviz/stable/SOURCES/graphviz-2.36.0.tar.gz \
	&& tar xozf graphviz-2.36.0.tar.gz && rm -f graphviz-2.36.0.tar.gz && \
	mv graphviz-2.36.0 graphviz && touch $@

.PHONY: libcppa
libcppa: libcppa/.install

libcppa/.install: libcppa/.configure
	cd libcppa && $(MAKE) && $(MAKE) install && \
	touch .install .configure

libcppa/.configure: libcppa/.checkout
	cd libcppa && \
	rm -rf build/CMakeFiles && \
	./configure --prefix=`pwd` --with-gcc=$(CXX) --no-protobuf-examples

libcppa/.checkout:
	git clone git://github.com/Neverlord/libcppa.git && \
	cd libcppa && git checkout V0.8.1 && \
	sed -i.bk '2s:CXX:C CXX:; 3s:^:set(CMAKE_MACOSX_RPATH 1):' CMakeLists.txt \
	&& touch .checkout

