DESCRIPTION = "Cargo build tool for Rust (pre-compiled binary version)"
HOMEPAGE = "https://rust-lang.org"
LICENSE = "Apache-2.0 | MIT"
SECTION = "devel"

inherit cross
inherit rust-common

PN = "cargo-bin-cross-${TARGET_ARCH}"

CARGO_HOST_TARGET = "${@rust_target(d, 'HOST')}"

SYSROOT_PREPROCESS_FUNCS = "cargo_process_sysroot"
cargo_process_sysroot() {
    sysroot_stage_dir ${D}${prefix}/bin ${SYSROOT_DESTDIR}${prefix}/bin
    sysroot_stage_dir ${D}${prefix}/lib ${SYSROOT_DESTDIR}${prefix}/lib
    rm -rf ${SYSROOT_DESTDIR}${prefix}/share
    rm -rf ${SYSROOT_DESTDIR}${prefix}/etc
}

# Stripping fails because of mixed arch types (host and target)
INHIBIT_PACKAGE_STRIP = "1"
INHIBIT_SYSROOT_STRIP = "1"

fakeroot do_install() {
    ${S}/install.sh --destdir="${D}" --prefix="${prefix}"
    rm -f ${D}${prefix}/lib/rustlib/uninstall.sh
    rm -f ${D}${prefix}/lib/rustlib/install.log
    rm -f ${D}${prefix}/lib/rustlib/components
    rm -f ${D}${prefix}/lib/rustlib/rust-installer-version
}

python () {
    pv = d.getVar("PV", True)
    pv_uri = pv[0:4] + '-' + pv[4:6] + '-' + pv[6:8]
    target = d.getVar("CARGO_HOST_TARGET", True)
    cargo_uri = ("%s;md5sum=%s;sha256sum=%s;downloadname=%s" %
                 (cargo_url(target), cargo_md5(target), cargo_sha256(target),
                  d.getVar("PN", True) + "-" + pv + "-" + target + ".tar.gz"))
    src_uri = d.getVar("SRC_URI", True).split()
    d.setVar("SRC_URI", ' '.join(src_uri + [cargo_uri]))
    d.setVar("S", "{}/{}".format(d.getVar("WORKDIR", True), cargo_url(target).split('/')[-1].replace('.tar.gz', '')))
}