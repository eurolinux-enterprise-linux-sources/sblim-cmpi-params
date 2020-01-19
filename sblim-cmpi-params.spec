%global provider_dir %{_libdir}/cmpi

Name:           sblim-cmpi-params
Version:        1.3.0
Release:        10%{?dist}
Summary:        SBLIM params instrumentation

Group:          Applications/System
License:        EPL
URL:            http://sblim.wiki.sourceforge.net/
Source0:        http://downloads.sourceforge.net/sblim/%{name}-%{version}.tar.bz2
Patch0:         sblim-cmpi-params-1.2.4-no-abi-params.patch
# Patch1: use Pegasus root/interop instead of root/PG_Interop
Patch1:         sblim-cmpi-params-1.3.0-pegasus-interop.patch

BuildRequires:  sblim-cmpi-devel sblim-cmpi-base-devel
Requires:       sblim-cmpi-base cim-server cim-schema

%description
Standards Based Linux Instrumentation Params Providers

%package        test
Summary:        SBLIM Params Instrumentation Testcases
Group:          Applications/System
Requires:       sblim-cmpi-params = %{version}-%{release}
Requires:       sblim-testsuite

%description -n sblim-cmpi-params-test
SBLIM Base Params Testcase Files for SBLIM Testsuite

%prep
%setup -q
%patch0 -p1 -b .no-abi-params
%patch1 -p1 -b .pegasus-interop

%build
%configure \
        --disable-static \
        TESTSUITEDIR=%{_datadir}/sblim-testsuite \
        PROVIDERDIR=%{provider_dir}
make %{?_smp_mflags}

%install
make install DESTDIR=$RPM_BUILD_ROOT
# remove unused libtool files
rm -f $RPM_BUILD_ROOT/%{_libdir}/*.la
rm -f $RPM_BUILD_ROOT/%{provider_dir}/*.la

%files
%{provider_dir}/*.so
%{_datadir}/%{name}
%docdir %{_datadir}/doc/%{name}-%{version}
%{_datadir}/doc/%{name}-%{version}

%files test
%{_datadir}/sblim-testsuite

%global SCHEMA %{_datadir}/%{name}/*.mof
%global REGISTRATION %{_datadir}/%{name}/*.registration
%global PEGASUS_MOF CIM_UnixLocalFileSystem

%pre
function unregister()
{
  %{_datadir}/%{name}/provider-register.sh -d \
        $1 \
        -m %{SCHEMA} \
        -r %{REGISTRATION} > /dev/null 2>&1 || :;
  # don't let registration failure when server not running fail upgrade!
}
 
# If upgrading, deregister old version
if [ $1 -gt 1 ]
then
        unregistered=no
        if [ -e /usr/sbin/cimserver ]; then
           unregister "-t pegasus";
           unregistered=yes
        fi
 
        if [ -e /usr/sbin/sfcbd ]; then
           unregister "-t sfcb";
           unregistered=yes
        fi
 
        if [ "$unregistered" != yes ]; then
           unregister
        fi
fi

%post
function register()
{
  # The follwoing script will handle the registration for various CIMOMs.
  %{_datadir}/%{name}/provider-register.sh \
        $1 \
        -m %{SCHEMA} \
        -r %{REGISTRATION} > /dev/null 2>&1 || :;
  # don't let registration failure when server not running fail install!
}
 
/sbin/ldconfig
if [ $1 -ge 1 ]
then
        registered=no
        if [ -e /usr/sbin/cimserver ]; then
          # tog-pegasus needs some schemes registered first
          if [ -x /usr/bin/peg-loadmof.sh ]; then
            peg-loadmof.sh -n root/cimv2 /usr/share/mof/cim-current/*/{%{PEGASUS_MOF}}.mof > /dev/null 2>&1 || :;
            /sbin/service tog-pegasus try-restart > /dev/null 2>&1 || :;
          fi
          register "-t pegasus";
          registered=yes
        fi
 
        if [ -e /usr/sbin/sfcbd ]; then
          register "-t sfcb";
          registered=yes
        fi
 
        if [ "$registered" != yes ]; then
          register
        fi
fi

%preun
function unregister()
{
  %{_datadir}/%{name}/provider-register.sh -d \
        $1 \
        -m %{SCHEMA} \
        -r %{REGISTRATION} > /dev/null 2>&1 || :;
  # don't let registration failure when server not running fail erase!
}
 
if [ $1 -eq 0 ]
then
        unregistered=no
        if [ -e /usr/sbin/cimserver ]; then
          unregister "-t pegasus";
          unregistered=yes
        fi
 
        if [ -e /usr/sbin/sfcbd ]; then
          unregister "-t sfcb";
          unregistered=yes
        fi
 
        if [ "$unregistered" != yes ]; then
          unregister
        fi
fi

%postun -p /sbin/ldconfig

%changelog
* Fri Jan 24 2014 Daniel Mach <dmach@redhat.com> - 1.3.0-10
- Mass rebuild 2014-01-24

* Fri Dec 27 2013 Daniel Mach <dmach@redhat.com> - 1.3.0-9
- Mass rebuild 2013-12-27

* Wed Aug 14 2013 Vitezslav Crhonek <vcrhonek@redhat.com> - 1.3.0-8
- Use Pegasus root/interop instead of root/PG_Interop

* Thu Feb 14 2013 Fedora Release Engineering <rel-eng@lists.fedoraproject.org> - 1.3.0-7
- Rebuilt for https://fedoraproject.org/wiki/Fedora_19_Mass_Rebuild

* Wed Sep 05 2012 Vitezslav Crhonek <vcrhonek@redhat.com> - 1.3.0-6
- Fix issues found by fedora-review utility in the spec file

* Sat Jul 21 2012 Fedora Release Engineering <rel-eng@lists.fedoraproject.org> - 1.3.0-5
- Rebuilt for https://fedoraproject.org/wiki/Fedora_18_Mass_Rebuild

* Sat Jan 14 2012 Fedora Release Engineering <rel-eng@lists.fedoraproject.org> - 1.3.0-4
- Rebuilt for https://fedoraproject.org/wiki/Fedora_17_Mass_Rebuild

* Thu Nov 10 2011 Vitezslav Crhonek <vcrhonek@redhat.com> - 1.3.0-3
- Add mofs registration for various CIMOMs

* Wed Feb 09 2011 Fedora Release Engineering <rel-eng@lists.fedoraproject.org> - 1.3.0-2
- Rebuilt for https://fedoraproject.org/wiki/Fedora_15_Mass_Rebuild

* Wed Oct  6 2010 Vitezslav Crhonek <vcrhonek@redhat.com> - 1.3.0-1
- Update to sblim-cmpi-params-1.3.0
- Remove CIMOM dependencies

* Mon Jun  1 2009 Vitezslav Crhonek <vcrhonek@redhat.com> - 1.2.6-1
- Initial support
