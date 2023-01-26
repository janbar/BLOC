CMAKE := cmake
BUILDDIR := $(PWD)/release

all: CMakeLists.txt
	mkdir -p $(BUILDDIR)
	$(CMAKE) -B $(BUILDDIR) -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=ON .
	$(CMAKE) --build $(BUILDDIR)
	$(CMAKE) --build $(BUILDDIR) --target test

clean:
	$(CMAKE) --build $(BUILDDIR) --target clean
