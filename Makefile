CMAKE = cmake
BUILDDIR = ./release

all: CMakeLists.txt
	mkdir -p $(BUILDDIR)
	$(CMAKE) -B $(BUILDDIR) -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=ON -DCMAKE_INSTALL_PREFIX=/usr/local .
	$(CMAKE) --build $(BUILDDIR)
	$(CMAKE) --build $(BUILDDIR) --target test

clean:
	$(CMAKE) --build $(BUILDDIR) --target clean

install:
	$(CMAKE) --build $(BUILDDIR) --target install
