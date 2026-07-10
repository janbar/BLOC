#
# spec file for package BLOC
#

Name:           BLOC
Version:        2.9.3
Release:        0
Summary:        A modular interpreter based on PL syntax
License:        GPL-3.0-or-later
Group:          Development/Languages/Other
URL:            https://github.com/janbar/BLOC/
Source0:        https://github.com/janbar/BLOC/archive/refs/tags/%{version}.tar.gz
BuildRequires:  cmake
BuildRequires:  gcc-c++
BuildRequires:  sqlite3-devel
BuildRequires:  libmariadb-devel
BuildRequires:  readline-devel
BuildRequires:  pkgconfig
BuildRequires:  pkgconfig(readline)
BuildRequires:  pkgconfig(sqlite3)
BuildRequires:  pkgconfig(libmariadb)

%description
BLOC can be used as is or embedded in a C/C++ program to
perform dynamic processing. The language is of the PASCALIAN
type, therefore easy to learn. You can extend functionality
by dynamically importing modules (object). Module allows to
operate others types.

%package -n bloc
Summary:        A modular interpreter based on PL syntax
Group:          Development/Languages/Other

%description -n bloc
BLOC can be used as is or embedded in a C/C++ program to
perform dynamic processing. The language is of the PASCALIAN
type, therefore easy to learn. You can extend functionality
by dynamically importing modules (object). Module allows to
operate others types.

%package -n bloc-devel
Summary:        Development files for BLOC library
Group:          Development/Libraries/C and C++
Requires:       bloc = %{version}

%description -n bloc-devel
BLOC can be embedded in a C/C++ program to perform
dynamic processing.

%package -n bloc-module-mariadb
Summary:        Module mariadb for BLOC
Group:          Development/Languages/Other
Requires:       bloc = %{version}

%description -n bloc-module-mariadb
Module mariadb for BLOC

%package -n bloc-module-sqlite3
Summary:        Module sqlite3 for BLOC
Group:          Development/Languages/Other
Requires:       bloc = %{version}

%description -n bloc-module-sqlite3
Module sqlite3 for BLOC

%package -n bloc-module-oracle
Summary:        Module oracle for BLOC
Group:          Development/Languages/Other
Requires:       bloc = %{version}

%description -n bloc-module-oracle
Module oracle for BLOC

%prep
%autosetup -p1

%build
%cmake \
    -DCMAKE_BUILD_TYPE=Release \
    -DENABLE_READLINE=ON \
    -DBUILD_TESTING=OFF
%make_jobs

%install
%make_install -C build

%files -n bloc
%doc docs/BLOC-Reference-Manual.md
%license LICENSE
%{_bindir}/bloc
%{_libdir}/libblocc.so.*
%{_libdir}/libbloc_msgdb.so.*
%{_libdir}/libbloc_csv.so.*
%{_libdir}/libbloc_date.so.*
%{_libdir}/libbloc_file.so.*
%{_libdir}/libbloc_sys.so.*
%{_libdir}/libbloc_utf8.so.*
%{_libdir}/libbloc_crypto.so.*

%files -n bloc-devel
%doc docs/BLOC-C-API.md
%{_includedir}/blocc
%{_libdir}/pkgconfig/blocc.pc
%{_libdir}/cmake/blocc/
%{_libdir}/libblocc.so
%exclude %{_libdir}/libblocc.so.*
%exclude %{_libdir}/libbloc_*.so
%exclude %{_libdir}/libbloc_*.so.*

%files -n bloc-module-mariadb
%{_libdir}/libbloc_mariadb.so.*

%files -n bloc-module-sqlite3
%{_libdir}/libbloc_sqlite3.so.*

%files -n bloc-module-oracle
%{_libdir}/libbloc_oracle.so.*

%post -n bloc
/sbin/ldconfig

%postun -n bloc
/sbin/ldconfig

%changelog
* Sat Jul 11 2026 janbar <jlbarriere68@gmail.com>
- Initial packaging