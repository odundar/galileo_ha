DESCRIPTION = "Home Automation Application for Intel Galileo"
SECTION = "base"
LICENSE = "MIT"

LIC_FILES_CHKSUM = "file://COPYING;md5=c5bb609535f48d5cd02fe14a780f3d8c"

DEPENDS += "opencv"

SRC_URI = "file:///home/onur/tmp/galileo_ha/smarthome-0.0.1.tar.gz"

SRC_URI[md5sum] = "f83413fd18148b92b39f7eb0e27d4e3c"
SRC_URI[sha256sum] = "7812f9cca8684d5951789f1c00ee66f6a77c16f84ea6f70336f81ef1f94325dc"

S = "${WORKDIR}/${PN}-${PV}"

inherit autotools gettext

do_install_append(){
	mkdir ${D}/etc
	mkdir ${D}/etc/init.d
	mkdir ${D}/etc/rcS.d
	mkdir ${D}/home
	mkdir ${D}/home/root
	mkdir ${D}/home/root/smarthome
	mkdir ${D}/home/root/smarthome/captures
	mkdir ${D}/home/root/smarthome/log
	install -m 0755 ${S}/scripts/startha.sh ${D}${sysconfdir}/init.d
	ln -sf ../init.d/startha.sh ${D}${sysconfdir}/rcS.d/S99startha.sh 
}
