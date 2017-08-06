TEMPLATE = app
TARGET = sling
VERSION = 1.4.0.0
INCLUDEPATH += src \
               src/crypto \
               src/qt \
               src/secp256k1/include \
               src/univalue/include \
               src/leveldb/helpers/memenv \
               src/script

INCLUDEPATH += $$PWD/../../../../usr/lib/x86_64-linux-gnu
DEPENDPATH += $$PWD/../../../../usr/lib/x86_64-linux-gnu

QT += core gui network widgets printsupport

DEFINES += ENABLE_WALLET
DEFINES += BOOST_THREAD_USE_LIB BOOST_SPIRIT_THREADSAFE
CONFIG += no_include_pwd
CONFIG += thread
CONFIG += c++11


lessThan(QT_MAJOR_VERSION, 5) {
    # stop build
    error( "Qt 5 or greater is required." )
}

DEFINES += PACKAGE_NAME=\\\"Dynamic\\\"
DEFINES += QT_PROJECT_BUILD=1
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0
DEFINES += HAVE_WORKING_BOOST_SLEEP_FOR=1
DEFINES += BOOST_THREAD_USES_CHRONO=1
DEFINES += HAVE_ENDIAN_H=1
DEFINES += HAVE_DECL_BE32TOH=1
DEFINES += HAVE_DECL_LE32TOH=1

DEFINES += HAVE_DECL_HTOLE16=1
DEFINES += HAVE_DECL_BE16TOH=1
DEFINES += HAVE_DECL_LE16TOH=1
DEFINES += HAVE_DECL_HTOBE32=1
DEFINES += HAVE_DECL_HTOLE32=1


DEFINES += HAVE_DECL_HTOBE64=1
DEFINES += HAVE_DECL_HTOLE64=1
DEFINES += HAVE_DECL_BE64TOH=1
DEFINES += HAVE_DECL_LE64TOH=1
DEFINES += HAVE_DECL_STRNLEN=1
DEFINES += WORDS_BIGENDIAN=0
DEFINES += HAVE_DECL_HTOBE16=1
DEFINES += HAVE_DECL_BE16TOH=1
DEFINES += EVENT__HAVE_NETINET_IN_H=1
DEFINES += _XOPEN_SOURCE_EXTENDED=0
DEFINES += _HAVE_DECL_BE16TOH=1
DEFINES += HAVE_DECL_BE16TOH=1


QT += widgets
# LIBSEC256K1 SUPPORT
# QMAKE_CXXFLAGS *= -DUSE_SECP256K1

# for boost 1.37, add -mt to the boost libraries
# use: qmake BOOST_LIB_SUFFIX=-mt
# or when linking against a specific BerkelyDB version: BDB_LIB_SUFFIX=-4.8

# Dependency library locations can be customized with:
#    BOOST_INCLUDE_PATH, BOOST_LIB_PATH, BDB_INCLUDE_PATH,
#    BDB_LIB_PATH, OPENSSL_INCLUDE_PATH and OPENSSL_LIB_PATH respectively

OBJECTS_DIR = build
MOC_DIR = build
UI_DIR = build

# use: qmake "RELEASE=1"
contains(RELEASE, 1) {
    # Linux: static link
    LIBS += -Wl,-Bstatic
}


# for extra security against potential buffer overflows: enable GCCs Stack Smashing Protection
QMAKE_CXXFLAGS *= -fstack-protector-all --param ssp-buffer-size=1
QMAKE_LFLAGS *= -fstack-protector-all --param ssp-buffer-size=1

# use: qmake "USE_QRCODE=1"
# libqrencode (http://fukuchi.org/works/qrencode/index.en.html) must be installed for support
contains(USE_QRCODE, 1) {
    message(Building with QRCode support)
    DEFINES += USE_QRCODE
    LIBS += -lqrencode
}

# use: qmake "USE_UPNP=1" ( enabled by default; default)
#  or: qmake "USE_UPNP=0" (disabled by default)
#  or: qmake "USE_UPNP=-" (not supported)
# miniupnpc (http://miniupnp.free.fr/files/) must be installed for support
contains(USE_UPNP, -) {
    message(Building without UPNP support)
} else {
    message(Building with UPNP support)
    count(USE_UPNP, 0) {
        USE_UPNP=1
    }
    DEFINES += USE_UPNP=$$USE_UPNP STATICLIB
    INCLUDEPATH += $$MINIUPNPC_INCLUDE_PATH
    LIBS += $$join(MINIUPNPC_LIB_PATH,,-L,) -lminiupnpc
}

# use: qmake "USE_DBUS=1" or qmake "USE_DBUS=0"
linux:count(USE_DBUS, 0) {
    USE_DBUS=1
}
contains(USE_DBUS, 1) {
    message(Building with DBUS (Freedesktop notifications) support)
    DEFINES += USE_DBUS
    QT += dbus
}

contains(DYNAMIC_NEED_QT_PLUGINS, 1) {
    DEFINES += DYNAMIC_NEED_QT_PLUGINS
    QTPLUGIN += qcncodecs qjpcodecs qtwcodecs qkrcodecs qtaccessiblewidgets
}

#Build Secp256k1
INCLUDEPATH += src/secp256k1/include
LIBS += $$PWD/src/secp256k1/.libs/libsecp256k1.a
# we use QMAKE_CXXFLAGS_RELEASE even without RELEASE=1 because we use RELEASE to indicate linking preferences not -O preferences
gensecp256k1.commands = if [ -f $$PWD/src/secp256k1/.libs/libsecp256k1.a ]; then echo "Secp256k1 already built"; else cd $$PWD/src/secp256k1 && ./autogen.sh && ./configure --disable-shared --with-pic --with-bignum=no --enable-module-recovery && CC=$$QMAKE_CC CXX=$$QMAKE_CXX $(MAKE) OPT=\"$$QMAKE_CXXFLAGS $$QMAKE_CXXFLAGS_RELEASE\"; fi
gensecp256k1.target = $$PWD/src/secp256k1/.libs/libsecp256k1.a
gensecp256k1.depends = FORCE
PRE_TARGETDEPS += $$PWD/src/secp256k1/.libs/libsecp256k1.a
QMAKE_EXTRA_TARGETS += gensecp256k1
QMAKE_CLEAN += $$PWD/src/secp256k1/.libs/libsecp256k1.a; cd $$PWD/src/secp256k1 ; $(MAKE) clean

#Build LevelDB
INCLUDEPATH += src/leveldb/include src/leveldb/helpers src/leveldb/helpers/memenv
LIBS += $$PWD/src/leveldb/libleveldb.a $$PWD/src/leveldb/libmemenv.a
# we use QMAKE_CXXFLAGS_RELEASE even without RELEASE=1 because we use RELEASE to indicate linking preferences not -O preferences
genleveldb.commands = cd $$PWD/src/leveldb && CC=$$QMAKE_CC CXX=$$QMAKE_CXX $(MAKE) OPT=\"$$QMAKE_CXXFLAGS $$QMAKE_CXXFLAGS_RELEASE\" libleveldb.a libmemenv.a
genleveldb.target = $$PWD/src/leveldb/libleveldb.a
genleveldb.depends = FORCE
PRE_TARGETDEPS += $$PWD/src/leveldb/libleveldb.a
QMAKE_EXTRA_TARGETS += genleveldb
# Gross ugly hack that depends on qmake internals, unfortunately there is no other way to do it.
QMAKE_CLEAN += $$PWD/src/leveldb/libleveldb.a; cd $$PWD/src/leveldb ; $(MAKE) clean

#Build Univalue
INCLUDEPATH += src/univalue/include
LIBS += $$PWD/src/univalue/lib/libunivalue_la-univalue.o
LIBS += $$PWD/src/univalue/lib/libunivalue_la-univalue_read.o
LIBS += $$PWD/src/univalue/lib/libunivalue_la-univalue_write.o

# we use QMAKE_CXXFLAGS_RELEASE even without RELEASE=1 because we use RELEASE to indicate linking preferences not -O preferences
genUnivalue.commands =if [ -f $$PWD/src/univalue/lib/libunivalue_la-univalue.o ]; then echo "Univalue already built"; else cd $$PWD/src/univalue && ./autogen.sh && ./configure && CC=$$QMAKE_CC CXX=$$QMAKE_CXX $(MAKE) OPT=\"$$QMAKE_CXXFLAGS $$QMAKE_CXXFLAGS_RELEASE\"; fi
genUnivalue.target = $$PWD/src/univalue/lib/libunivalue_la-univalue.o
genUnivalue.depends = FORCE
PRE_TARGETDEPS += $$PWD/src/univalue/lib/libunivalue_la-univalue.o
QMAKE_EXTRA_TARGETS += genUnivalue
QMAKE_CLEAN += $$PWD/src/univalue/lib/libunivalue_la-univalue.o; cd $$PWD/src/univalue ; $(MAKE) clean

# Build Protobuf Payment Request cpp code file
LIBS += -L/usr/local/lib -lprotobuf
genprotobuff.commands = cd $$PWD/src/qt && protoc -I=. --cpp_out=. ./paymentrequest.proto
genprotobuff.depends = FORCE
QMAKE_EXTRA_TARGETS += genprotobuff

# regenerate src/build.h
contains(USE_BUILD_INFO, 1) {
    genbuild.depends = FORCE
    genbuild.commands = cd $$PWD; /bin/sh share/genbuild.sh $$OUT_PWD/build/build.h
    genbuild.target = $$OUT_PWD/build/build.h
    PRE_TARGETDEPS += $$OUT_PWD/build/build.h
    QMAKE_EXTRA_TARGETS += genbuild
    DEFINES += HAVE_BUILD_INFO
}

contains(USE_O3, 1) {
    message(Building O3 optimization flag)
    QMAKE_CXXFLAGS_RELEASE -= -O2
    QMAKE_CFLAGS_RELEASE -= -O2
    QMAKE_CXXFLAGS += -O3
    QMAKE_CFLAGS += -O3
}

*-g++-32 {
    message("32 platform, adding -msse2 flag")

    QMAKE_CXXFLAGS += -msse2
    QMAKE_CFLAGS += -msse2
}

QMAKE_CXXFLAGS_WARN_ON = -fdiagnostics-show-option -Wall -Wextra -Wno-ignored-qualifiers -Wformat -Wformat-security -Wno-unused-parameter -Wstack-protector

# Input
DEPENDPATH += src src/json src/qt

contains(USE_QRCODE, 1) {
HEADERS += src/qt/qrcodedialog.h
SOURCES += src/qt/qrcodedialog.cpp
FORMS += src/qt/forms/qrcodedialog.ui
}

CODECFORTR = UTF-8

# for lrelease/lupdate
# also add new translations to src/qt/sling.qrc under translations/
TRANSLATIONS = $$files(src/qt/locale/sling_*.ts)

isEmpty(QMAKE_LRELEASE) {
    QMAKE_LRELEASE = $$[QT_INSTALL_BINS]/lrelease
}
isEmpty(QM_DIR):QM_DIR = $$PWD/src/qt/locale
# automatically build translations, so they can be included in resource file
TSQM.name = lrelease ${QMAKE_FILE_IN}
TSQM.input = TRANSLATIONS
TSQM.output = $$QM_DIR/${QMAKE_FILE_BASE}.qm
TSQM.commands = $$QMAKE_LRELEASE ${QMAKE_FILE_IN} -qm ${QMAKE_FILE_OUT}
TSQM.CONFIG = no_link
QMAKE_EXTRA_COMPILERS += TSQM


isEmpty(BOOST_THREAD_LIB_SUFFIX) {
    BOOST_THREAD_LIB_SUFFIX = $$BOOST_LIB_SUFFIX
}

contains(MINGW_THREAD_BUGFIX, 0) {
    # At least qmake's win32-g++-cross profile is missing the -lmingwthrd
    # thread-safety flag. GCC has -mthreads to enable this, but it doesn't
    # work with static linking. -lmingwthrd must come BEFORE -lmingw, so
    # it is prepended to QMAKE_LIBS_QT_ENTRY.
    # It can be turned off with MINGW_THREAD_BUGFIX=0, just in case it causes
    # any problems on some untested qmake profile now or in the future.
    DEFINES += _MT BOOST_THREAD_PROVIDES_GENERIC_SHARED_MUTEX_ON_WIN
    QMAKE_LIBS_QT_ENTRY = -lmingwthrd $$QMAKE_LIBS_QT_ENTRY
}

# Set libraries and includes at end, to use platform-defined defaults if not overridden
INCLUDEPATH += $$BOOST_INCLUDE_PATH $$BDB_INCLUDE_PATH $$OPENSSL_INCLUDE_PATH $$QRENCODE_INCLUDE_PATH
INCLUDEPATH += $$SECP256K1_INCLUDE_PATH
LIBS +=  $$join(BOOST_LIB_PATH,,-L,) $$join(BDB_LIB_PATH,,-L,) $$join(OPENSSL_LIB_PATH,,-L,) $$join(QRENCODE_LIB_PATH,,-L,)
LIBS += -lssl -lcrypto -ldb_cxx$$BDB_LIB_SUFFIX
LIBS += $$join(SECP256K1_LIB_PATH,,-L,)

LIBS += -lboost_system$$BOOST_LIB_SUFFIX -lboost_filesystem$$BOOST_LIB_SUFFIX -lboost_program_options$$BOOST_LIB_SUFFIX -lboost_thread$$BOOST_THREAD_LIB_SUFFIX -lrt -lboost_chrono$$BOOST_LIB_SUFFIX

# include libevent & libevent2
INCLUDEPATH += $$PWD/../../../../usr/include/event2
LIBS += -levent -levent_core -levent_extra -lpthread -levent_pthreads

contains(RELEASE, 1) {
    # Linux: turn sling linking back on for c/c++ runtime libraries
    LIBS += -Wl,-Bsling
}

system($$QMAKE_LRELEASE -silent $$_PRO_FILE_)

HEADERS += \
    src/qt/paymentrequest.pb.h \
    src/activemasternode.h \
    src/addrdb.h \
    src/addrman.h \
    src/alert.h \
    src/allocators.h \
    src/amount.h \
    src/arith_uint256.h \
    src/base58.h \
    src/bip38.h \
    src/blockencodings.h \
    src/bloom.h \
    src/chain.h \
    src/chainparams.h \
    src/chainparamsbase.h \
    src/chainparamsseeds.h \
    src/checkpoints.h \
    src/checkqueue.h \
    src/clientversion.h \
    src/coinmix-relay.h \
    src/coinmix.h \
    src/coins.h \
    src/compat.h \
    src/compressor.h \
    src/core_io.h \
    src/core_memusage.h \
    src/cuckoocache.h \
    src/dbwrapper.h \
    src/eccryptoverify.h \
    src/ecwrapper.h \
    src/fastsend.h \
    src/fs.h \
    src/genesis.h \
    src/hash.h \
    src/httprpc.h \
    src/httpserver.h \
    src/indirectmap.h \
    src/init.h \
    src/kernel.h \
    src/key.h \
    src/keystore.h \
    src/leveldbwrapper.h \
    src/limitedmap.h \
    src/main.h \
    src/masternode-budget.h \
    src/masternode-payments.h \
    src/masternode-sync.h \
    src/masternode.h \
    src/masternodeconfig.h \
    src/masternodeman.h \
    src/memusage.h \
    src/merkleblock.h \
    src/miner.h \
    src/mruset.h \
    src/net_processing.h \
    src/net.h \
    src/netaddress.h \
    src/netbase.h \
    src/netmessagemaker.h \
    src/noui.h \
    src/pow.h \
    src/prevector.h \
    src/protocol.h \
    src/pubkey.h \
    src/random.h \
    src/reverse_iterator.h \
    src/reverselock.h \
    src/rpcclient.h \
    src/rpcprotocol.h \
    src/rpcserver.h \
    src/scheduler.h \
    src/serialize.h \
    src/spork.h \
    src/streams.h \
    src/sync.h \
    src/threadinterrupt.h \
    src/threadsafety.h \
    src/timedata.h \
    src/tinyformat.h \
    src/torcontrol.h \
    src/txdb.h \
    src/txmempool.h \
    src/ui_interface.h \
    src/uint256.h \
    src/undo.h \
    src/util.h \
    src/utilmoneystr.h \
    src/utilstrencodings.h \
    src/utiltime.h \
    src/validation.h \
    src/validationinterface.h \
    src/version.h \
    src/versionbits.h \
    src/walletdb.h \
    src/warnings.h \
    src/qt/winshutdownmonitor.h \
    src/qt/walletview.h \
    src/qt/walletmodeltransaction.h \
    src/qt/walletmodel.h \
    src/qt/walletframe.h \
    src/qt/utilitydialog.h \
    src/qt/transactionview.h \
    src/qt/transactiontablemodel.h \
    src/qt/transactionrecord.h \
    src/qt/transactionfilterproxy.h \
    src/qt/transactiondescdialog.h \
    src/qt/transactiondesc.h \
    src/qt/trafficgraphwidget.h \
    src/qt/splashscreen.h \
    src/qt/signverifymessagedialog.h \
    src/qt/sendcoinsentry.h \
    src/qt/sendcoinsdialog.h \
    src/qt/rpcconsole.h \
    src/qt/recentrequeststablemodel.h \
    src/qt/receiverequestdialog.h \
    src/qt/receivecoinsdialog.h \
    src/qt/qvaluecombobox.h \
    src/qt/qvalidatedlineedit.h \
    src/qt/platformstyle.h \
    src/qt/peertablemodel.h \
    src/qt/paymentserver.h \
    src/qt/paymentrequestplus.h \
    src/qt/overviewpage.h \
    src/qt/optionsmodel.h \
    src/qt/optionsdialog.h \
    src/qt/openuridialog.h \
    src/qt/notificator.h \
    src/qt/networkstyle.h \
    src/qt/multisenddialog.h \
    src/qt/modaloverlay.h \
    src/qt/masternodelist.h \
    src/qt/macnotificationhandler.h \
    src/qt/macdockiconhandler.h \
    src/qt/intro.h \
    src/qt/guiutil.h \
    src/qt/guiconstants.h \
    src/qt/editaddressdialog.h \
    src/qt/csvmodelwriter.h \
    src/qt/coinmixconfig.h \
    src/qt/coincontroltreewidget.h \
    src/qt/coincontroldialog.h \
    src/qt/clientmodel.h \
    src/qt/callback.h \
    src/qt/blockexplorer.h \
    src/qt/bitcoinunits.h \
    src/qt/bitcoingui.h \
    src/qt/bitcoinamountfield.h \
    src/qt/bitcoinaddressvalidator.h \
    src/qt/bip38tooldialog.h \
    src/qt/bantablemodel.h \
    src/qt/askpassphrasedialog.h \
    src/qt/addresstablemodel.h \
    src/qt/addressbookpage.h \
    src/compat/sanity.h \
    src/consensus/merkle.h \
    src/primitives/block.h \
    src/primitives/transaction.h \
    src/script/bitcoinconsensus.h \
    src/script/interpreter.h \
    src/script/ismine.h \
    src/script/script_error.h \
    src/script/script.h \
    src/script/sigcache.h \
    src/script/sign.h \
    src/script/standard.h \
    src/wallet/walletdb.h \
    src/wallet/wallet.h \
    src/wallet/db.h \
    src/wallet/crypter.h \
    src/wallet/coincontrol.h \
    src/zmq/zmqabstractnotifier.h \
    src/zmq/zmqconfig.h \
    src/zmq/zmqnotificationinterface.h \
    src/zmq/zmqpublishnotifier.h \
    src/consensus/validation.h \
    src/consensus/params.h \
    src/consensus/consensus.h

SOURCES += \
    src/activemasternode.cpp \
    src/addrdb.cpp \
    src/addrman.cpp \
    src/alert.cpp \
    src/allocators.cpp \
    src/amount.cpp \
    src/arith_uint256.cpp \
    src/base58.cpp \
    src/bip38.cpp \
    src/bitcoin-cli.cpp \
    src/bitcoin-tx.cpp \
    src/bitcoind.cpp \
    src/blockencodings.cpp \
    src/bloom.cpp \
    src/chain.cpp \
    src/chainparams.cpp \
    src/chainparamsbase.cpp \
    src/checkpoints.cpp \
    src/clientversion.cpp \
    src/coinmix-relay.cpp \
    src/coinmix.cpp \
    src/coins.cpp \
    src/compressor.cpp \
    src/core_read.cpp \
    src/core_write.cpp \
    src/dbwrapper.cpp \
    src/eccryptoverify.cpp \
    src/ecwrapper.cpp \
    src/fastsend.cpp \
    src/fs.cpp \
    src/genesis.cpp \
    src/hash.cpp \
    src/httprpc.cpp \
    src/httpserver.cpp \
    src/init.cpp \
    src/kernel.cpp \
    src/key.cpp \
    src/keystore.cpp \
    src/leveldbwrapper.cpp \
    src/main.cpp \
    src/masternode-budget.cpp \
    src/masternode-payments.cpp \
    src/masternode-sync.cpp \
    src/masternode.cpp \
    src/masternodeconfig.cpp \
    src/masternodeman.cpp \
    src/merkleblock.cpp \
    src/miner.cpp \
    src/net_processing.cpp \
    src/net.cpp \
    src/netaddress.cpp \
    src/netbase.cpp \
    src/noui.cpp \
    src/pow.cpp \
    src/protocol.cpp \
    src/pubkey.cpp \
    src/random.cpp \
    src/rest.cpp \
    src/rpcblockchain.cpp \
    src/rpcclient.cpp \
    src/rpcdump.cpp \
    src/rpcmasternode-budget.cpp \
    src/rpcmasternode.cpp \
    src/rpcmining.cpp \
    src/rpcmisc.cpp \
    src/rpcnet.cpp \
    src/rpcprotocol.cpp \
    src/rpcrawtransaction.cpp \
    src/rpcserver.cpp \
    src/rpcwallet.cpp \
    src/scheduler.cpp \
    src/sling-cli.cpp \
    src/sling-tx.cpp \
    src/slingd.cpp \
    src/spork.cpp \
    src/sync.cpp \
    src/threadinterrupt.cpp \
    src/timedata.cpp \
    src/torcontrol.cpp \
    src/txdb.cpp \
    src/txmempool.cpp \
    src/ui_interface.cpp \
    src/uint256.cpp \
    src/util.cpp \
    src/utilmoneystr.cpp \
    src/utilstrencodings.cpp \
    src/utiltime.cpp \
    src/validation.cpp \
    src/validationinterface.cpp \
    src/versionbits.cpp \\
    src/warnings.cpp \
    src/qt/addressbookpage.cpp \
    src/qt/addresstablemodel.cpp \
    src/qt/askpassphrasedialog.cpp \
    src/qt/bantablemodel.cpp \
    src/qt/bip38tooldialog.cpp \
    src/qt/bitcoin.cpp \
    src/qt/bitcoinaddressvalidator.cpp \
    src/qt/bitcoinamountfield.cpp \
    src/qt/bitcoingui.cpp \
    src/qt/bitcoinstrings.cpp \
    src/qt/bitcoinunits.cpp \
    src/qt/blockexplorer.cpp \
    src/qt/clientmodel.cpp \
    src/qt/coincontroldialog.cpp \
    src/qt/coincontroltreewidget.cpp \
    src/qt/coinmixconfig.cpp \
    src/qt/csvmodelwriter.cpp \
    src/qt/editaddressdialog.cpp \
    src/qt/guiutil.cpp \
    src/qt/intro.cpp \
    src/qt/masternodelist.cpp \
    src/compat/glibcxx_compat.cpp \
    src/compat/glibcxx_sanity.cpp \
    src/compat/strnlen.cpp \
    src/consensus/merkle.cpp \
    src/crypto/aes.cpp \
    src/crypto/chacha20.cpp \
    src/crypto/hmac_sha256.cpp \
    src/crypto/hmac_sha512.cpp \
    src/crypto/rfc6979_hmac_sha256.cpp \
    src/crypto/ripemd160.cpp \
    src/crypto/scrypt.cpp \
    src/crypto/sha1.cpp \
    src/crypto/sha256_sse4.cpp \
    src/crypto/sha256.cpp \
    src/crypto/sha512.cpp \
    src/crypto/aes_helper.c \
    src/crypto/blake.c \
    src/crypto/bmw.c \
    src/crypto/cubehash.c \
    src/crypto/echo.c \
    src/crypto/groestl.c \
    src/crypto/jh.c \
    src/crypto/keccak.c \
    src/crypto/luffa.c \
    src/crypto/shavite.c \
    src/crypto/simd.c \
    src/crypto/skein.c \
    src/policy/feerate.cpp \
    src/policy/fees.cpp \
    src/policy/policy.cpp \
    src/policy/rbf.cpp \
    src/primitives/block.cpp \
    src/primitives/transaction.cpp \
    src/script/bitcoinconsensus.cpp \
    src/script/interpreter.cpp \
    src/script/ismine.cpp \
    src/script/script_error.cpp \
    src/script/script.cpp \
    src/script/sigcache.cpp \
    src/script/sign.cpp \
    src/script/standard.cpp \
    src/wallet/walletdb.cpp \
    src/wallet/wallet.cpp \
    src/wallet/db.cpp \
    src/wallet/crypter.cpp \
    src/zmq/zmqabstractnotifier.cpp \
    src/zmq/zmqnotificationinterface.cpp \
    src/zmq/zmqpublishnotifier.cpp \
    src/compat/glibc_compat.cpp

OTHER_FILES += \
    Makefile.am \
    configure.ac \
    src/Makefile.am

RESOURCES += \
    src/qt/sling.qrc \
    src/qt/sling_locale.qrc

FORMS += \
    src/qt/forms/addressbookpage.ui \
    src/qt/forms/transactiondescdialog.ui \
    src/qt/forms/tradingdialog.ui \
    src/qt/forms/signverifymessagedialog.ui \
    src/qt/forms/sendcoinsentry.ui \
    src/qt/forms/sendcoinsdialog.ui \
    src/qt/forms/rpcconsole.ui \
    src/qt/forms/receiverequestdialog.ui \
    src/qt/forms/receivecoinsdialog.ui \
    src/qt/forms/overviewpage.ui \
    src/qt/forms/optionsdialog.ui \
    src/qt/forms/openuridialog.ui \
    src/qt/forms/multisenddialog.ui \
    src/qt/forms/modaloverlay.ui \
    src/qt/forms/masternodelist.ui \
    src/qt/forms/intro.ui \
    src/qt/forms/helpmessagedialog.ui \
    src/qt/forms/editaddressdialog.ui \
    src/qt/forms/debugwindow.ui \
    src/qt/forms/coinmixconfig.ui \
    src/qt/forms/coincontroldialog.ui \
    src/qt/forms/blockexplorer.ui \
    src/qt/forms/bip38tooldialog.ui \
    src/qt/forms/askpassphrasedialog.ui
