EAPI=6


EGIT_REPO_URI="https://github.com/D-Y-V/Fastcgi-Container.git"

if [[ ${PV} = 9999* ]]; then
        GIT_ECLASS="git-r3"
fi

inherit eutils cmake-utils $GIT_ECLASS

DESCRIPTION="FastCGI Container. Very configurable and fast framework, based on Fastcgi Daemon, originaly developed by (C)Yandex."
HOMEPAGE="https://github.com/D-Y-V/Fastcgi-Container"
LICENSE="GPL-2"
SLOT="0"

# may be other arch....
KEYWORDS="~mips ~sparc ~x86 ~amd64"

DEPEND="dev-util/cmake
		dev-libs/fcgi
		dev-libs/libxml2"

# Ugly hack. Should be removed later.
LDFLAGS="-luuid -lxml2 -lfcgi -lfcgi++ -lpthread -ldl -lcrypto"
