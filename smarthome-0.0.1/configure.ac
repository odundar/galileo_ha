AC_PREREQ(2.61)

# package version number (not shared library version)
# odd micro numbers indicate in-progress development
# even micro numbers indicate released versions
m4_define(smart_home_version_major, 0)
m4_define(smart_home_version_minor, 1)
m4_define(smart_home_version_micro, 0)

m4_define([smart_home_version],
          [smart_home_version_major.smart_home_version_minor.smart_home_version_micro])
m4_define([smart_home_api_version],
          [smart_home_version_major.smart_home_version_minor])

AC_INIT(smart_home, smart_home_version)
AM_INIT_AUTOMAKE(smart_home, smart_home_version)
AC_CONFIG_HEADERS(config.h)

# Checks for programs.
AC_PROG_LIBTOOL
AM_PROG_CC_C_O
AC_PROG_INSTALL

AC_SUBST(smart_home_CFLAGS)
AC_SUBST(smart_home_LIBS)

AC_OUTPUT([
Makefile
src/Makefile
])

echo ""
echo "   smart_home $VERSION"
echo "   ====================="
echo ""
echo "   To build the project, run \"make\""
echo ""
