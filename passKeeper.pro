QT += gui xml
CONFIG += qtestlib
TARGET = passKeeper
TEMPLATE = app

INCLUDEPATH += \
# events start
	events/EventsCenter \
	events/KeyEntEvent \
# sys start
	sys/Config \
	sys/Crypter \
	sys/Crypter/AES_Crypter \
	sys/Crypter/Camellia_Crypter \
	sys/Crypter/IDEA_Crypter \
	sys/Db \
	sys/Db/XmlDb \
	sys/KeyEntity \
	sys/SecureClipboard \
	sys/SleepThread \
	sys/TreeNode \
	sys/utils \
# widgets start
	wgts \
	wgts/AuthWgt \
	wgts/DescrWgt \
	wgts/FieldsWgt \
	wgts/MainWgt \
	wgts/NewKeyWgt \
	wgts/SettingsWgt \
	wgts/TemplWgt \
	wgts/TreeWgt \
	wgts/TreeWgt/TreeWgtItem \
# tests start
	tests \
	tests/config_test \
	tests/crypter_test \
	tests/xmldb_test \
	openssl/include \
	.


SOURCES += \
#events start
	events/EventsCenter/EventsCenter.cpp \
# sys start
	sys/Config/Config.cpp \
	sys/Crypter/Crypter.cpp \
	sys/Crypter/AES_Crypter/AES_Crypter.cpp \
	sys/Crypter/Camellia_Crypter/Camellia_Crypter.cpp \
	sys/Crypter/IDEA_Crypter/IDEA_Crypter.cpp \
	sys/Db/Db.cpp \
	sys/Db/XmlDb/XmlDb.cpp \
	sys/KeyEntity/KeyEntity.cpp \
	sys/SecureClipboard/SecClipboard.cpp \
	sys/SleepThread/SleepThread.cpp \
	sys/TreeNode/TreeNode.cpp \
	sys/utils/utils.cpp \
# widgets start
	wgts/AuthWgt/AuthWgt.cpp \
	wgts/DescrWgt/DescrWgt.cpp \
	wgts/FieldsWgt/FieldsWgt.cpp \
	wgts/MainWgt/MainWgt.cpp \
	wgts/NewKeyWgt/NewKeyWgt.cpp \
	wgts/SettingsWgt/SettingsWgt.cpp \
	wgts/TemplWgt/TemplWgt.cpp \
	wgts/TreeWgt/TreeWgt.cpp \
# tests start
	tests/config_test/ConfigTest.cpp \
	tests/crypter_test/CrypterTest.cpp \
	tests/xmldb_test/XmlDbTest.cpp \
# ----
	main.cpp


HEADERS += \
#events start
	events/EventsCenter/EventsCenter.h \
	events/KeyEntEvent/KeyEntEvent.h \
# sys start
	sys/Config/Config.h \
	sys/Config/ConfigWrapper.h \
	sys/Crypter/Crypter.h \
	sys/Crypter/AES_Crypter/AES_Crypter.h \
	sys/Crypter/Camellia_Crypter/Camellia_Crypter.h \
	sys/Crypter/IDEA_Crypter/IDEA_Crypter.h \
	sys/Db/Db.h \
	sys/Db/XmlDb/XmlDb.h \
	sys/KeyEntity/KeyEntity.h \
	sys/SecureClipboard/SecClipboard.h \
	sys/SleepThread/SleepThread.h \
	sys/TreeNode/TreeNode.h \
	sys/utils/utils.h \
# widgets start
	wgts/AuthWgt/AuthWgt.h \
	wgts/DescrWgt/DescrWgt.h \
	wgts/FieldsWgt/FieldsWgt.h \
	wgts/MainWgt/MainWgt.h \
	wgts/NewKeyWgt/NewKeyWgt.h \
	wgts/SettingsWgt/SettingsWgt.h \
	wgts/TemplWgt/TemplWgt.h \
	wgts/TreeWgt/TreeWgt.h \
	wgts/TreeWgt/TreeWgtItem/TreeWgtItem.h \
	wgts/messages.h \
	wgts/wsizes.h \
# tests start
	tests/config_test/ConfigTest.h \
	tests/crypter_test/CrypterTest.h \
	tests/xmldb_test/XmlDbTest.h \
	tests/tests.h \
# ----
	consts.h \
	debug.h \
	types.h


OTHER_FILES += \
	README.txt \
	TODO.txt \
	classes.txt \
    formats.txt \
	tests/files/pkconf.ini \
	tests/files/crypter_test.txt \
    tests/files/xmldb_test.xml \
	resources/add.png \
	resources/add_folder.png \
	resources/add_row.png \
	resources/del_row.png \
	resources/folder.png \
	resources/key.png \
	resources/remove.png \
	resources/cut.png \
	resources/paste.png \
    resources/appicon/win/appicon.rc \
    resources/appicon/win/app.ico


RESOURCES += \
	resources/application.qrc

unix:QMAKE_CXXFLAGS += -std=c++11 -Wno-c++0x-compat
unix:LIBS += -lcrypto
win32:LIBS += openssl\\lib\\libeay32.lib
win32:RC_FILE = resources\\appicon\\win\\appicon.rc

DESTDIR = build
OBJECTS_DIR = build/obj
MOC_DIR = build/moc
RCC_DIR = build/rcc
UI_DIR = build/ui

