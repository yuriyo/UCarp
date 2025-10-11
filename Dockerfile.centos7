# syntax=docker/dockerfile:1

FROM centos:7 AS build
# Pin CentOS 7 repos to vault 7.9.2009
RUN cat > /etc/yum.repos.d/CentOS-Base.repo <<'EOF' && \
    yum -y makecache fast && yum -y update && yum -y install \
    gcc gcc-c++ make autoconf automake libtool pkgconfig \
    gettext gettext-devel libpcap-devel dos2unix which ca-certificates && \
    yum clean all
[base]
name=CentOS-7.9.2009 - Base
baseurl=https://vault.centos.org/centos/7.9.2009/os/$basearch/
gpgcheck=1
gpgkey=file:///etc/pki/rpm-gpg/RPM-GPG-KEY-CentOS-7
sslverify=0

[updates]
name=CentOS-7.9.2009 - Updates
baseurl=https://vault.centos.org/centos/7.9.2009/updates/$basearch/
gpgcheck=1
gpgkey=file:///etc/pki/rpm-gpg/RPM-GPG-KEY-CentOS-7
sslverify=0

[extras]
name=CentOS-7.9.2009 - Extras
baseurl=https://vault.centos.org/centos/7.9.2009/extras/$basearch/
gpgcheck=1
gpgkey=file:///etc/pki/rpm-gpg/RPM-GPG-KEY-CentOS-7
sslverify=0
EOF
WORKDIR /src
COPY . .
# Normalize Windows CRLF line endings to LF for autotools
RUN find . -type f -print0 | xargs -0 dos2unix || true
RUN gettextize -f --copy && cp -n po/Makevars.template po/Makevars && autoreconf -fi && ./configure && make -j"$(nproc)" && make install DESTDIR=/out

FROM centos:7 AS runtime
# Pin CentOS 7 repos to vault 7.9.2009
RUN cat > /etc/yum.repos.d/CentOS-Base.repo <<'EOF' && \
    yum -y makecache fast && yum -y update && yum -y install libpcap gettext-libs && yum clean all
[base]
name=CentOS-7.9.2009 - Base
baseurl=https://vault.centos.org/centos/7.9.2009/os/$basearch/
gpgcheck=1
gpgkey=file:///etc/pki/rpm-gpg/RPM-GPG-KEY-CentOS-7
sslverify=0

[updates]
name=CentOS-7.9.2009 - Updates
baseurl=https://vault.centos.org/centos/7.9.2009/updates/$basearch/
gpgcheck=1
gpgkey=file:///etc/pki/rpm-gpg/RPM-GPG-KEY-CentOS-7
sslverify=0

[extras]
name=CentOS-7.9.2009 - Extras
baseurl=https://vault.centos.org/centos/7.9.2009/extras/$basearch/
gpgcheck=1
gpgkey=file:///etc/pki/rpm-gpg/RPM-GPG-KEY-CentOS-7
sslverify=0
EOF
COPY --from=build /out/usr/local/ /usr/local/
ENTRYPOINT ["ucarp"]


