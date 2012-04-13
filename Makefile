## options

# location of root-config
ROOTCONFIG	  = $(shell which root-config)
# compiler
CXX		  = $(shell $(ROOTCONFIG) --cxx) -Wall $(DEBUG)
# compiler flags
CFLAGS		  = $(shell $(ROOTCONFIG) --cflags)
# ROOT libs
ROOTLIBS	  = $(shell $(ROOTCONFIG) --libs)
# linker flags
LDFLAGS		  = $(shell $(ROOTCONFIG) --ldflags)

# sources
PARSERSRC	  = parsePCNErrors.cc
ERRMAPSRC	  = PCNErrorTool.cc PCNErrorMap.cxx

# docs
DOCDIR            = docs


## Build

# all: parsePCNErrors makePCNErrorMap docs

parsePCNErrors:  $(PARSERSRC)
	$(CXX) $(CFLAGS) $(ROOTLIBS) $^ -o $@

makePCNErrorMap: $(ERRMAPSRC)
	$(CXX) $(CFLAGS) $(ROOTLIBS) $^ -o $@

doc:
	doxygen Velo-EB-doxy.conf > /dev/null
	cd $(DOCDIR)/latex && (make; make; make;) &> /dev/null

website:
	cd $(DOCDIR) && git push -f origin gh-pages

clean:
	rm -f parsePCNErrors makePCNErrorMap

clean-doc:
	rm -rf $(DOCDIR)/html $(DOCDIR)/latex $(DOCDIR)/man
