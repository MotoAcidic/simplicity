package=bdb
$(package)_version=6.2.32
$(package)_download_path=https://download.oracle.com/berkeley-db
$(package)_file_name=db-$($(package)_version).NC.tar.gz
$(package)_sha256_hash=d86cf1283c519d42dd112b4501ecb2db11ae765b37a1bdad8f8cb06b0ffc69b8
$(package)_build_subdir=build_unix

define $(package)_set_vars
$(package)_config_opts=--disable-shared --enable-cxx --disable-replication
$(package)_config_opts_mingw32=--enable-mingw
$(package)_config_opts_linux=--with-pic
$(package)_config_opts_freebsd=--with-pic
$(package)_config_opts_aarch64=--disable-atomicsupport
$(package)_cxxflags=-std=c++11
endef

define $(package)_preprocess_cmds
  sed -i.old 's/WinIoCtl.h/winioctl.h/g' src/dbinc/win_db.h && \
  sed -i.old 's/__atomic_compare_exchange\\(/__atomic_compare_exchange_db(/' src/dbinc/atomic.h && \
  sed -i.old 's/atomic_init/atomic_init_db/' src/dbinc/atomic.h src/mp/mp_region.c src/mp/mp_mvcc.c src/mp/mp_fget.c src/mutex/mut_method.c src/mutex/mut_tas.c
endef

define $(package)_config_cmds
  ../dist/$($(package)_autoconf)
endef

define $(package)_build_cmds
  $(MAKE) libdb_cxx-6.2.a libdb-6.2.a
endef

define $(package)_stage_cmds
  $(MAKE) DESTDIR=$($(package)_staging_dir) install_lib install_include
endef
