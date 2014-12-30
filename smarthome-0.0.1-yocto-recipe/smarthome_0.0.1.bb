DESCRIPTION = "Home Automation Application for Intel Galileo"
SECTION = "base"
LICENSE = "MIT"

LIC_FILES_CHKSUM = "file://COPYING;md5=c5bb609535f48d5cd02fe14a780f3d8c"

DEPENDS += "opencv"

SRC_URI = "file:///home/onur/tmp/galileo_ha/smarthome-0.0.1.tar.gz"

SRC_URI[md5sum] = "46ef1371208ee57f89cbfa793a689eba"
SRC_URI[sha256sum] = "dd4d8cd85c86c440173e60af36e227fbc2cffb151877ebaed8d474625cd889cd"

S = "${WORKDIR}/${PN}-${PV}"

inherit autotools gettext

do_install_append(){
	mkdir ${D}/etc
	mkdir ${D}/etc/init.d
	mkdir ${D}/etc/rcS.d
	install -m 0755 ${S}/scripts/startha.sh ${D}${sysconfdir}/init.d
	ln -sf ../init.d/startha.sh ${D}${sysconfdir}/rcS.d/S99startha.sh 
}
