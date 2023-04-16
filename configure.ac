#  configure.ac: The project's autoconf script.
#  Copyright (C) 2023 streaksu
#
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>.

AC_INIT([gwm], [m4_esyscmd([./version.sh])], [https://github.com/streaksu/gwm/issues], [gwm])

AC_PREREQ([2.69])
AC_CONFIG_AUX_DIR([build-aux])

SRCDIR="$(cd "$srcdir" && pwd -P)"
BUILDDIR="$(pwd -P)"

AC_SUBST([SRCDIR])
AC_SUBST([BUILDDIR])

AC_DEFUN([PROG_ABSPATH], [
    case "$$1" in
        .*) $1="$(cd "$(dirname "$$1")" && pwd -P)/$(basename "$$1")" ;;
    esac
])

test "x$CXXFLAGS" = "x" && CXXFLAGS='-g -O2 -pipe'

AC_LANG([C++])
AC_PROG_CXX
PROG_ABSPATH([CXX])

werror_state="no"
AC_ARG_ENABLE([werror],
    [AS_HELP_STRING([--enable-werror], [treat warnings as errors])],
    [werror_state="$enableval"])
if test "$werror_state" = "yes"; then
    AC_SUBST([WERROR_FLAG], [-Werror])
else
    AC_SUBST([WERROR_FLAG], [-Wno-error])
fi

AC_PROG_MKDIR_P
PROG_ABSPATH([MKDIR_P])
AC_PROG_INSTALL
PROG_ABSPATH([INSTALL])

AC_CHECK_PROG([FIND_FOUND], [find], [yes])
if ! test "x$FIND_FOUND" = "xyes"; then
    AC_MSG_ERROR([find not found, please install find before configuring])
fi

NATIVE_STRIP="$($CXX -dumpmachine)"-strip
AC_CHECK_PROG([NATIVE_STRIP_FOUND], [$NATIVE_STRIP], [yes])
if ! test "x$NATIVE_STRIP_FOUND" = "xyes"; then
    NATIVE_STRIP=strip
fi
AC_SUBST([NATIVE_STRIP])

NATIVE_PKGCONFIG="$($CXX -dumpmachine)"-pkg-config
AC_CHECK_PROG([NATIVE_PKGCONFIG_FOUND], [$NATIVE_PKGCONFIG], [yes])
if ! test "x$NATIVE_PKGCONFIG_FOUND" = "xyes"; then
    NATIVE_PKGCONFIG=pkg-config
fi
AC_SUBST([NATIVE_PKGCONFIG])

CXXFLAGS="$CXXFLAGS $($NATIVE_PKGCONFIG --cflags libpng freetype2)"
LIBS="$LIBS $($NATIVE_PKGCONFIG --libs libpng freetype2)"

AC_CHECK_HEADERS([stdio.h unistd.h fcntl.h stdlib.h sys/ioctl.h],
    [], [AC_MSG_ERROR([required header not found])])

AC_PREFIX_DEFAULT([/usr/local])
AC_CONFIG_FILES([GNUmakefile])
AC_OUTPUT
