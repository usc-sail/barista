# SHELL += -x

KALDI_VERSION = 3755
CPPA_VERSION = V0.8.1

# We do not support compiling libcppa with clang.
COMPILER = $(shell $(CXX) -v 2>&1 )
ifeq ($(findstring clang,$(COMPILER)),clang)
  ERR_IF_CLANG = $(error Clang is not supported for this target. \
                         Use g++ >= 4.7 instead (g++-4.8 recommended).)
endif

INSTALLED_KALDI_VERSION := $(shell [ -d kaldi/.svn ] && svnversion -n kaldi)
UPDATE_KALDI := $(shell [ "$(INSTALLED_KALDI_VERSION)" != "" ] && \
                        [ $(KALDI_VERSION) -ne $(INSTALLED_KALDI_VERSION) ] \
                        && rm -f kaldi/.checkout && echo true)

INSTALLED_CPPA_VERSION := $(shell [ -d libcppa/.git ] && cd libcppa && \
                                  git describe)
UPDATE_CPPA := $(shell [ "$(INSTALLED_CPPA_VERSION)" != "" ] && \
                       [ "$(CPPA_VERSION)" != "$(INSTALLED_CPPA_VERSION)" ] \
                       && rm -f libcppa/.checkout && echo true)

print-%:
	@echo '$*=$($*)'

all: kaldi libcppa graphviz

.PHONY: clean
clean: kaldi/clean graphviz/clean libcppa/clean

.PHONY: kaldi/clean
kaldi/clean:
	$(MAKE) -C kaldi/tools clean; $(MAKE) -C kaldi/src clean; \
	rm -f kaldi/src/.install kaldi/src/.depend kaldi/src/.configure; \
	rm -f kaldi/tools/.install kaldi/tools/portaudio/.install; \
	rm -rf kaldi/tools/portaudio;

.PHONY: libcppa/clean
libcppa/clean: 
	rm -f libcppa/build libcppa/.install libcppa/.configure

.PHONY: graphviz/clean
graphviz/clean: 
	$(MAKE) -C graphviz clean && rm -f graphviz/.install graphviz/.configure

.PHONY: distclean
distclean:
	rm -rf kaldi graphviz libcppa


.PHONY: kaldi
kaldi: kaldi/src/.install

kaldi/src/.install: kaldi/src/.depend kaldi/tools/portaudio/.install
	cd kaldi/src && $(MAKE) CXX=$(CXX) && $(MAKE) online CXX=$(CXX) && \
	touch .install

kaldi/src/.depend: kaldi/src/.configure
	$(MAKE) -C kaldi/src depend CXX=$(CXX) && touch $@

kaldi/src/.configure: kaldi/tools/.install
	cd kaldi/src && ./configure && touch .configure

kaldi/tools/portaudio/.install: kaldi/.checkout
	cd kaldi/tools && ./install_portaudio.sh && touch portaudio/.install

kaldi/tools/.install: kaldi/.checkout
	$(MAKE) -C kaldi/tools CXX=$(CXX) && touch $@

kaldi/.checkout:
ifeq ($(UPDATE_KALDI),true)
	@echo 'Updating Kaldi: $(INSTALLED_KALDI_VERSION) -> $(KALDI_VERSION)' && \
	svn co svn://svn.code.sf.net/p/kaldi/code/trunk@$(KALDI_VERSION) kaldi && \
	touch $@
else
	svn co svn://svn.code.sf.net/p/kaldi/code/trunk@$(KALDI_VERSION) kaldi && \
	touch $@
endif


.PHONY: libcppa
libcppa: libcppa/.install

libcppa/.install: libcppa/.configure
	cd libcppa && $(MAKE) && $(MAKE) install && \
	touch .install

libcppa/.configure: libcppa/.checkout
	$(ERR_IF_CLANG)
	cd libcppa && \
	rm -rf build/CMakeFiles && \
	./configure --prefix=`pwd` --with-gcc=$(CXX) --no-protobuf-examples \
	--build-static && touch .configure

libcppa/.checkout:
ifeq ($(UPDATE_CPPA),true)
	@echo 'Updating libcppa: $(INSTALLED_CPPA_VERSION) -> $(CPPA_VERSION)' && \
	cd libcppa && git checkout -- CMakeLists.txt && git checkout master && \
	git pull && git checkout $(CPPA_VERSION) && \
	sed -i.bk '2s:cppa CXX:cppa C CXX:' CMakeLists.txt && touch .checkout
else
	git clone git://github.com/Neverlord/libcppa.git && \
	cd libcppa && git checkout $(CPPA_VERSION) && \
	sed -i.bk '2s:cppa CXX:cppa C CXX:' CMakeLists.txt && touch .checkout
endif


.PHONY: graphviz
graphviz: graphviz/.install

graphviz/.install: graphviz/.configure
	cd graphviz && $(MAKE) && $(MAKE) install && \
	touch .install

graphviz/.configure: graphviz/.checkout
	cd graphviz && \
	./configure --prefix=`pwd` --without-x CC=$(CC) CXX=$(CXX) && \
	touch .configure

graphviz/.checkout:
	wget -T 10 -t 3 --no-check-certificate \
	http://www.graphviz.org/pub/graphviz/stable/SOURCES/graphviz-2.36.0.tar.gz \
	&& tar xozf graphviz-2.36.0.tar.gz && rm -f graphviz-2.36.0.tar.gz && \
	mv graphviz-2.36.0 graphviz && touch $@
