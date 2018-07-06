EAPI=6

inherit eutils cmake-utils

DESCRIPTION="FastCGI Container. Very configurable and fast framework, based on Fastcgi Daemon, originaly developed by (C)Yandex."
HOMEPAGE="https://github.com/D-Y-V/Fastcgi-Container"
SRC_URI="https://github.com/D-Y-V/Fastcgi-Container/archive/master.zip -> ${P}.zip"

LICENSE="GPL-2"
SLOT="0"

# may be other arch....
KEYWORDS="~mips ~sparc ~x86 ~amd64"

DEPEND="dev-util/cmake
		dev-libs/fcgi
		dev-libs/libxml2"

# Ugly hack. Should be removed later.
LDFLAGS="-luuid -lxml2 -lfcgi -lfcgi++ -lpthread -ldl -lcrypto"

S="${WORKDIR}/Fastcgi-Container-master/"
