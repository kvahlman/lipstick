/***************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (directui@nokia.com)
**
** This file is part of mhome.
**
** If you have questions regarding the use of this file, please contact
** Nokia at directui@nokia.com.
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation
** and appearing in the file LICENSE.LGPL included in the packaging
** of this file.
**
****************************************************************************/

#include <QtTest/QtTest>
#include <MApplication>
#include <MDesktopEntry>
#include "launcherdatastore.h"
#include "launcherpage.h"
#include "mockdatastore.h"
#include "ut_launcherdatastore.h"
#include "homefiledatastore_stub.h"

Q_DECLARE_METATYPE(QSharedPointer<MDesktopEntry>)

// MDesktopEntry stubs (used by Launcher)
QMap<const MDesktopEntry *, QString> desktopEntryFileName;
QMap<QString, QStringList> desktopEntryCategories;
QMap<QString, QStringList> desktopEntryOnlyShowIn;
QMap<QString, QStringList> desktopEntryNotShowIn;
QMap<QString, QString> desktopEntryType;
QMap<QString, QString> desktopEntryXMaemoService;
QMap<QString, QString> desktopEntryName;
QMap<QString, QString> desktopEntryIcon;
QMap<QString, QString> desktopEntryExec;
QMap<QString, QString> desktopEntryUrl;
QMap<QString, QString> desktopEntryNameUnlocalized;

QStringList addedWatcherPathCalls;

MDesktopEntry::MDesktopEntry(const QString &fileName) :
    d_ptr(NULL)
{
    desktopEntryFileName.insert(this, fileName);
}

QString MDesktopEntry::fileName() const
{
    return desktopEntryFileName[this];
}

bool MDesktopEntry::isValid() const
{
    return true;
}

QStringList MDesktopEntry::onlyShowIn() const
{
    return desktopEntryOnlyShowIn.value(desktopEntryFileName.value(this));
}

QStringList MDesktopEntry::notShowIn() const
{
    return desktopEntryNotShowIn.value(desktopEntryFileName.value(this));
}

QString MDesktopEntry::type() const
{
    return desktopEntryType.value(desktopEntryFileName.value(this));
}

QString MDesktopEntry::xMaemoService() const
{
    return desktopEntryXMaemoService.value(desktopEntryFileName.value(this));
}

QString MDesktopEntry::name() const
{
    return desktopEntryName.value(desktopEntryFileName.value(this));
}

QString MDesktopEntry::icon() const
{
    return desktopEntryIcon.value(desktopEntryFileName.value(this));
}

QString MDesktopEntry::exec() const
{
    return desktopEntryExec.value(desktopEntryFileName.value(this));
}

QString MDesktopEntry::url() const
{
    return desktopEntryUrl.value(desktopEntryFileName.value(this));
}

QString MDesktopEntry::nameUnlocalized() const
{
    return desktopEntryNameUnlocalized.value(desktopEntryFileName.value(this));
}

// QDir stubs
QMap<QString, QFileInfoList> desktopFileInfoList;
QFileInfoList QDir::entryInfoList(Filters, SortFlags) const
{
    return desktopFileInfoList.value(path());
}

// QFileInfo stubs
QStringList qFileInfoDirs;
bool QFileInfo::exists() const
{
    bool contains = qFileInfoDirs.contains(canonicalFilePath());

    if (!contains) {
        foreach (const QFileInfoList &fileInfoList, desktopFileInfoList.values()) {
            foreach (const QFileInfo &file, fileInfoList) {
                if (file.absoluteFilePath() == absoluteFilePath()) {
                    contains = true;
                    break;
                }
            }
        }
    }

    return contains;
}

bool QFileInfo::isDir() const
{
    return qFileInfoDirs.contains(canonicalFilePath());
}

QString QFileInfo::canonicalPath() const
{
    QString path = absolutePath();
    while(path.endsWith('/')) {
        path.chop(1);
    }
    return path;
}


QString QFileInfo::canonicalFilePath() const
{
    QString path = absoluteFilePath();
    while(path.endsWith('/')) {
        path.chop(1);
    }
    return path;
}

// QFile stubs
bool QFile::exists(const QString &file)
{
    bool contains = false;
    foreach (const QFileInfoList &fileInfoList, desktopFileInfoList.values()) {
        foreach (const QFileInfo &fileInfo, fileInfoList) {
            if (fileInfo.absoluteFilePath() == file) {
                contains = true;
                break;
            }
        }
    }
    return contains;
}

// QFileSystemWatcher stubs
void QFileSystemWatcher::addPath(const QString &path)
{
    addedWatcherPathCalls.append(path);
}

QStringList QFileSystemWatcher::files() const
{
    return addedWatcherPathCalls;
}

void Ut_LauncherDataStore::initTestCase()
{
    qRegisterMetaType<QSharedPointer<MDesktopEntry> >();

    static int argc = 1;
    static char *app_name[1] = { (char *) "./ut_launcherdatastore" };
    app = new MApplication(argc, app_name);
}

void Ut_LauncherDataStore::cleanupTestCase()
{
    delete app;
}

void Ut_LauncherDataStore::init()
{
    mockStore = new MockDataStore;
    mockStore->clear();
    desktopEntryFileName.clear();
    desktopEntryCategories.clear();
    desktopEntryOnlyShowIn.clear();
    desktopEntryNotShowIn.clear();
    desktopEntryType.clear();
    desktopEntryXMaemoService.clear();
    desktopEntryName.clear();
    desktopEntryIcon.clear();
    desktopEntryExec.clear();
    desktopEntryUrl.clear();
    desktopEntryNameUnlocalized.clear();
    desktopFileInfoList.clear();
    addedWatcherPathCalls.clear();
    qFileInfoDirs.clear();
    qFileInfoDirs.append(QFileInfo(APPLICATIONS_DIRECTORY).canonicalFilePath());
}

void Ut_LauncherDataStore::cleanup()
{    
}

QString fileNameWithPath(const QString &fileName, const QString &directoryName = QString(APPLICATIONS_DIRECTORY))
{
    return directoryName + fileName;
}

void addDesktopEntry(const QString &fileName, const QString &name, const QString &type, const QString &icon, const QString &exec, const QString &directoryName = QString(APPLICATIONS_DIRECTORY))
{
    QString fullFileName = fileNameWithPath(fileName, directoryName);
    desktopEntryName.insert(fullFileName, name);
    desktopEntryType.insert(fullFileName, type);
    desktopEntryIcon.insert(fullFileName, icon);
    desktopEntryExec.insert(fullFileName, exec);
    desktopFileInfoList[QFileInfo(directoryName).path()].append(fullFileName);
}

void removeDesktopEntries()
{
    desktopEntryName.clear();
    desktopEntryType.clear();
    desktopEntryIcon.clear();
    desktopEntryExec.clear();
    desktopFileInfoList.clear();
}

void Ut_LauncherDataStore::testAddingDesktopEntryFiles()
{
    LauncherDataStore dataStore(mockStore, QStringList() << APPLICATIONS_DIRECTORY);

    QStringList addedEntries;
    for (int i = 0; i < 4; i++) {
        QString entry = QString("testApplication%1.desktop").arg(i);
        addedEntries.append(fileNameWithPath(entry));
        addDesktopEntry(entry, "Test", "Application", "Icon-camera", "test");
    }

    QSignalSpy spyDesktopEntryAdded(&dataStore, SIGNAL(desktopEntryAdded(QSharedPointer<MDesktopEntry>)));
    QSignalSpy spyDataStoreChanged(&dataStore, SIGNAL(dataStoreChanged()));

    dataStore.updateDesktopEntryFiles();

    QHash<QString, QVariant> data = dataStore.dataForAllDesktopEntries();
    QCOMPARE(data.count(), 4);
    QCOMPARE(spyDesktopEntryAdded.count(), 4);
    while (spyDesktopEntryAdded.count() > 0) {
        QString expectedEntry = spyDesktopEntryAdded.takeFirst().at(0).value<QSharedPointer<MDesktopEntry> >()->fileName();
        QCOMPARE(addedEntries.removeOne(expectedEntry), true);
    }
}

void Ut_LauncherDataStore::testRemovingDesktopEntryFiles()
{
    addDesktopEntry("testApplication1.desktop", "Test1", "Application", "Icon-camera", "test1");
    addDesktopEntry("testApplication2.desktop", "Test2", "Application", "Icon-camera", "test2");
    addDesktopEntry("testApplication3.desktop", "Test3", "Application", "Icon-camera", "test3");
    addDesktopEntry("testApplication4.desktop", "Test4", "Application", "Icon-camera", "test4");
    LauncherDataStore dataStore(mockStore, QStringList() << APPLICATIONS_DIRECTORY);
    QSignalSpy spyDesktopEntryRemoved(&dataStore, SIGNAL(desktopEntryRemoved(QString)));

    dataStore.updateDesktopEntryFiles();

    QHash<QString, QVariant> data = dataStore.dataForAllDesktopEntries();
    QCOMPARE(data.count(), 4);

    // Updating the desktop entry directory should be reflected in the data
    removeDesktopEntries();
    addDesktopEntry("testApplication.desktop", "Test", "Application", "Icon-camera", "test");

    dataStore.updateDesktopEntryFiles();

    data = dataStore.dataForAllDesktopEntries();
    QCOMPARE(data.count(), 1);
    QCOMPARE(data.contains(fileNameWithPath("testApplication.desktop")), true);
    QCOMPARE(spyDesktopEntryRemoved.count(), 4);
    QCOMPARE(spyDesktopEntryRemoved.takeFirst().at(0).toString(), QString(fileNameWithPath("testApplication1.desktop")));
    QCOMPARE(spyDesktopEntryRemoved.takeFirst().at(0).toString(), QString(fileNameWithPath("testApplication2.desktop")));
    QCOMPARE(spyDesktopEntryRemoved.takeFirst().at(0).toString(), QString(fileNameWithPath("testApplication3.desktop")));
    QCOMPARE(spyDesktopEntryRemoved.takeFirst().at(0).toString(), QString(fileNameWithPath("testApplication4.desktop")));
}

void Ut_LauncherDataStore::testUpdatingDesktopFilesFiltersDesktopEntries()
{
    LauncherDataStore dataStore(mockStore, QStringList() << APPLICATIONS_DIRECTORY);

    // Add valid entries:
    QStringList expectedEntries;
    QString regularEntry("regularApplication.desktop");
    expectedEntries.append(fileNameWithPath(regularEntry));
    addDesktopEntry(regularEntry, "Test4", "Application", "Icon-camera", "test4");

    QString meegoApp("onlyShowInMeeGoApplication.desktop");
    expectedEntries.append(fileNameWithPath(meegoApp));
    addDesktopEntry(meegoApp, "Test0", "Link", "Icon-camera", "test0");
    desktopEntryOnlyShowIn.insert(fileNameWithPath("onlyShowInMeeGoApplication.desktop"), QStringList() << "X-MeeGo");

    // Add invalid entries:
    addDesktopEntry("onlyShowInInvalidApplication.desktop", "Test1", "Application", "Icon-camera", "test1");
    addDesktopEntry("notShowInMeeGoApplication.desktop", "Test2", "Application", "Icon-camera", "test2");
    addDesktopEntry("invalidApplication.desktop", "Test3", "Invalid", "Icon-camera", "test3");
    desktopEntryOnlyShowIn.insert(fileNameWithPath("onlyShowInInvalidApplication.desktop"), QStringList() << "X-Invalid");
    desktopEntryNotShowIn.insert(fileNameWithPath("notShowInMeeGoApplication.desktop"), QStringList() << "X-MeeGo");

    QSignalSpy spyDesktopEntryAdded(&dataStore, SIGNAL(desktopEntryAdded(QSharedPointer<MDesktopEntry>)));
    dataStore.updateDesktopEntryFiles();

    QCOMPARE(spyDesktopEntryAdded.count(), 2);
    while (spyDesktopEntryAdded.count()) {
        QVERIFY(expectedEntries.removeOne(spyDesktopEntryAdded.takeFirst().at(0).value<QSharedPointer<MDesktopEntry> >()->fileName()));
    }
    QCOMPARE(dataStore.invalidEntries.count(), 3);

    // The data store should contain two entries and the default value should be empty
    QHash<QString, QVariant> data = dataStore.dataForAllDesktopEntries();
    QCOMPARE(data.count(), 2);
    QCOMPARE(data.contains(fileNameWithPath("regularApplication.desktop")), true);
    QCOMPARE(data.contains(fileNameWithPath("invalidApplication.desktop")), false);
    QCOMPARE(data.contains(fileNameWithPath("onlyShowInMeeGoApplication.desktop")), true);
    QCOMPARE(data.value(fileNameWithPath("onlyShowInMeeGoApplication.desktop")), QVariant(QString()));
    QCOMPARE(data.value(fileNameWithPath("regularApplication.desktop")), QVariant(QString()));
}

void Ut_LauncherDataStore::testUpdatingDataForDesktopEntry()
{
    // Test application
    addDesktopEntry("regularApplication.desktop", "Test0", "Application", "Icon-camera", "test0");

    LauncherDataStore dataStore(mockStore, QStringList() << APPLICATIONS_DIRECTORY);
    dataStore.updateDesktopEntryFiles();

    QHash<QString, QVariant> data = dataStore.dataForAllDesktopEntries();
    QCOMPARE(data.count(), 1);
    QSignalSpy spy(&dataStore, SIGNAL(dataStoreChanged()));

    // Updating an inexisting entry should add a new value but not send any signals
    dataStore.updateDataForDesktopEntry("/dev/null", "test");
    data = dataStore.dataForAllDesktopEntries();
    QCOMPARE(data.count(), 2);
    QCOMPARE(data.contains(fileNameWithPath("regularApplication.desktop")), true);
    QCOMPARE(spy.count(), 0);

    // Updating an existing entry should succeed but not add new value or send any signals
    dataStore.updateDataForDesktopEntry(fileNameWithPath("regularApplication.desktop"), "test");
    data = dataStore.dataForAllDesktopEntries();
    QCOMPARE(data.count(), 2);
    QCOMPARE(data.contains(fileNameWithPath("regularApplication.desktop")), true);
    QCOMPARE(data.value(fileNameWithPath("regularApplication.desktop")), QVariant("test"));
    QCOMPARE(spy.count(), 0);

    // Updating the data store directly should reflect in changed data
    mockStore->setValue("DesktopEntries" + fileNameWithPath("regularApplication.desktop"), QVariant("modifiedTest"));
    data = dataStore.dataForAllDesktopEntries();
    QCOMPARE(data.count(), 2);
    QCOMPARE(data.contains(fileNameWithPath("regularApplication.desktop")), true);
    QCOMPARE(data.value(fileNameWithPath("regularApplication.desktop")), QVariant("modifiedTest"));
    QCOMPARE(spy.count(), 1);
}

void Ut_LauncherDataStore::testRemovingDataForDesktopEntry()
{
    addDesktopEntry("test.desktop", "Test0", "Application", "Icon-camera", "test0");

    LauncherDataStore dataStore(mockStore, QStringList() << APPLICATIONS_DIRECTORY);
    dataStore.updateDesktopEntryFiles();
    QSignalSpy spy(&dataStore, SIGNAL(dataStoreChanged()));

    dataStore.removeDataForDesktopEntry(fileNameWithPath("test.desktop"));
    QCOMPARE(dataStore.dataForAllDesktopEntries().count(), 0);
    QCOMPARE(spy.count(), 0);
}

void Ut_LauncherDataStore::testRemovingNonPrefixedKeys()
{
    // Test applications
    QString regularDesktopEntry(fileNameWithPath("regularApplication.desktop"));
    mockStore->createValue("DesktopEntries" + regularDesktopEntry, QVariant("data0"));
    desktopFileInfoList[QFileInfo(QString(APPLICATIONS_DIRECTORY)).path()].append(regularDesktopEntry);
    QString otherApplication1(fileNameWithPath("otherApplication1.desktop"));
    mockStore->createValue("OtherPrefix" + fileNameWithPath("otherApplication1.desktop"), QVariant("data1"));
    desktopFileInfoList[QFileInfo(QString(APPLICATIONS_DIRECTORY)).path()].append(otherApplication1);
    QString otherApplication2(fileNameWithPath("otherApplication2.desktop"));
    mockStore->createValue(fileNameWithPath("otherApplication2.desktop"), QVariant("data2"));
    desktopFileInfoList[QFileInfo(QString(APPLICATIONS_DIRECTORY)).path()].append(otherApplication2);

    LauncherDataStore dataStore(mockStore, QStringList() << APPLICATIONS_DIRECTORY);

    // Launcher data store should only list entries prefixed with the
    // "DesktopEntries" prefix and ignore all others
    QHash<QString, QVariant> data = dataStore.dataForAllDesktopEntries();
    QCOMPARE(data.count(), 1);
    QCOMPARE(data.contains(fileNameWithPath("regularApplication.desktop")), true);
    QCOMPARE(data.value(fileNameWithPath("regularApplication.desktop")), QVariant("data0"));
}

void Ut_LauncherDataStore::testAddingWatcherDesktopEntryPaths()
{

    QString dirPath("/test/directory1");
    qFileInfoDirs.append(dirPath);

    LauncherDataStore dataStore(mockStore, QStringList() << APPLICATIONS_DIRECTORY << dirPath << "/test/directory2/");

    // Test applications
    QStringList expectedEntryPaths;
    for (int i = 0; i < 2; i++) {
        QString entry = QString("testApplication%1.desktop").arg(i);
        expectedEntryPaths.append(fileNameWithPath(entry));
        addDesktopEntry(entry, "Test", "Application", "Icon-camera", "test");
    }

    dataStore.updateDesktopEntryFiles();

    QCOMPARE(addedWatcherPathCalls.count(), 4);

    // The directories are added first
    QCOMPARE(addedWatcherPathCalls.at(0), fileNameWithPath("", QFileInfo(APPLICATIONS_DIRECTORY).canonicalFilePath()));
    QCOMPARE(addedWatcherPathCalls.at(1), fileNameWithPath("", "/test/directory1"));
    foreach (QString expectedPath, expectedEntryPaths) {
        QVERIFY(addedWatcherPathCalls.contains(expectedPath));
    }

    addDesktopEntry("testApplication3.desktop", "Test3", "Application", "Icon-camera", "test3", "/test/directory1/");

    dataStore.updateDesktopEntryFiles();

    QCOMPARE(addedWatcherPathCalls.count(), 5);
    QCOMPARE(addedWatcherPathCalls.at(4), fileNameWithPath("testApplication3.desktop", "/test/directory1/"));
}

void Ut_LauncherDataStore::testUpdatingDesktopEntry()
{
    qFileInfoDirs.append("/test/directory1");
    qFileInfoDirs.append("/test/directory2");

    addDesktopEntry("testApplication0.desktop", "Test0", "Application", "Icon-camera", "test0");
    addDesktopEntry("testApplication1.desktop", "Test1", "Application", "Icon-camera", "test1", "/test/directory1/");
    addDesktopEntry("testApplication2.desktop", "Test2", "Application", "Icon-camera", "test2", "/test/directory2/");
    addDesktopEntry("testApplication3.desktop", "Test3", "Application", "Icon-camera", "test3", "/test/directory2/");
    LauncherDataStore dataStore(mockStore, QStringList() << APPLICATIONS_DIRECTORY << "/test/directory1" << "/test/directory2/");
    dataStore.updateDesktopEntryFiles();

    QSignalSpy spyDesktopEntryChanged(&dataStore, SIGNAL(desktopEntryChanged(QSharedPointer<MDesktopEntry>)));
    QSignalSpy spyDataStoreChanged(&dataStore, SIGNAL(dataStoreChanged()));
    dataStore.updateDesktopEntry(fileNameWithPath("testApplication0.desktop"));
    dataStore.updateDesktopEntry(fileNameWithPath("testApplication1.desktop", "/test/directory1/"));
    dataStore.updateDesktopEntry(fileNameWithPath("testApplication2.desktop", "/test/directory2/"));
    dataStore.updateDesktopEntry("testApplication3.desktop");
    dataStore.updateDesktopEntry("testApplication4.desktop");

    // No data store change when entry is just updated
    QCOMPARE(spyDataStoreChanged.count(), 0);
    QCOMPARE(spyDesktopEntryChanged.count(), 4);
    QCOMPARE(spyDesktopEntryChanged.at(0).at(0).value<QSharedPointer<MDesktopEntry> >()->fileName(), QString(fileNameWithPath("testApplication0.desktop")));
    QCOMPARE(spyDesktopEntryChanged.at(1).at(0).value<QSharedPointer<MDesktopEntry> >()->fileName(), QString(fileNameWithPath("testApplication1.desktop", "/test/directory1/")));
    QCOMPARE(spyDesktopEntryChanged.at(2).at(0).value<QSharedPointer<MDesktopEntry> >()->fileName(), QString(fileNameWithPath("testApplication2.desktop", "/test/directory2/")));
    QCOMPARE(spyDesktopEntryChanged.at(3).at(0).value<QSharedPointer<MDesktopEntry> >()->fileName(), QString(fileNameWithPath("testApplication3.desktop", "/test/directory2/")));
}

void Ut_LauncherDataStore::testUpdatingInvalidEntry()
{
    addDesktopEntry("invalidAndValidApplication.desktop", "Test3", "invalid", "Icon-camera", "test3");
    addDesktopEntry("testApplication2.desktop", "Test2", "Application", "Icon-camera", "test2");
    LauncherDataStore dataStore(mockStore, QStringList() << APPLICATIONS_DIRECTORY);

    dataStore.updateDesktopEntryFiles();

    // connect signals only after entries have been added first time
    QSignalSpy spyDataStoreChanged(&dataStore, SIGNAL(dataStoreChanged()));
    QSignalSpy spyDesktopEntryAdded(&dataStore, SIGNAL(desktopEntryAdded(QSharedPointer<MDesktopEntry>)));
    QSignalSpy spyDesktopEntryRemoved(&dataStore, SIGNAL(desktopEntryRemoved(QString)));

    // change to valid and send changed
    desktopEntryType.insert(fileNameWithPath("invalidAndValidApplication.desktop"), QString("Application"));
    dataStore.updateDesktopEntry(fileNameWithPath("invalidAndValidApplication.desktop"));

    QCOMPARE(spyDesktopEntryAdded.count(), 1);
    QCOMPARE(spyDesktopEntryAdded.takeFirst().at(0).value<QSharedPointer<MDesktopEntry> >()->fileName(), QString(fileNameWithPath("invalidAndValidApplication.desktop")));
    QCOMPARE(spyDataStoreChanged.count(), 1);
    QVERIFY(!dataStore.invalidEntries.contains(fileNameWithPath("invalidAndValidApplication.desktop")));

    // change back to invalid and send changed
    desktopEntryType.insert(fileNameWithPath("invalidAndValidApplication.desktop"), QString("invalid_again"));
    dataStore.updateDesktopEntry(fileNameWithPath("invalidAndValidApplication.desktop"));

    QCOMPARE(spyDesktopEntryRemoved.count(), 1);
    QCOMPARE(spyDataStoreChanged.count(), 2);
    QVERIFY(dataStore.invalidEntries.contains(fileNameWithPath("invalidAndValidApplication.desktop")));
}

void Ut_LauncherDataStore::testRemovingEntriesWhenApplicationsDirectoryGetsEmpty()
{
    // Add desktop entries
    addDesktopEntry("testApplication1.desktop", "Test1", "Application", "Icon-camera", "test1");
    addDesktopEntry("testApplication2.desktop", "Test2", "Application", "Icon-camera", "test2");
    LauncherDataStore dataStore(mockStore, QStringList() << APPLICATIONS_DIRECTORY);
    dataStore.updateDesktopEntryFiles();

    QSignalSpy spyDesktopEntryRemoved(&dataStore, SIGNAL(desktopEntryRemoved(QString)));
    QSignalSpy spyDataStoreChanged(&dataStore, SIGNAL(dataStoreChanged()));

    removeDesktopEntries();
    dataStore.updateDesktopEntryFiles();

    QCOMPARE(spyDesktopEntryRemoved.count(), 2);
    QCOMPARE(spyDataStoreChanged.count(), 1);
}

void Ut_LauncherDataStore::testNotReprocessingInvalidEntry()
{
    addDesktopEntry("notShowInMeeGoApplication.desktop", "Test2", "Application", "Icon-camera", "test2");
    desktopEntryNotShowIn.insert(fileNameWithPath("notShowInMeeGoApplication.desktop"), QStringList() << "X-MeeGo");
    LauncherDataStore dataStore(mockStore, QStringList() << APPLICATIONS_DIRECTORY);
    dataStore.updateDesktopEntryFiles();
    QVERIFY(dataStore.invalidEntries.contains(fileNameWithPath("notShowInMeeGoApplication.desktop")));
    desktopEntryFileName.clear();

    dataStore.updateDesktopEntryFiles();

    // Entry should not be found from list of created MDesktopEntry's
    QVERIFY(!desktopEntryFileName.key(fileNameWithPath("notShowInMeeGoApplication.desktop"), NULL));
}

void Ut_LauncherDataStore::testUpdatingMultipleEntries()
{
    LauncherDataStore dataStore(new HomeFileDataStore("path"), QStringList() << APPLICATIONS_DIRECTORY);

    // Add one existing value that should be updated too
    addDesktopEntry("testApplication1.desktop", "Test2", "Application", "Icon-camera", "test2");

    QHash<QString, QString> updatedData;
    const int AMOUNT_OF_BUTTONS = 3;
    for(int i = 0; i < AMOUNT_OF_BUTTONS; i++) {
        updatedData.insert(QString("testApplication%1.desktop").arg(i), QString("launcher/0/%1").arg(i));
    }
    dataStore.updateDataForDesktopEntries(updatedData);

    QHash<QString, QVariant> storedEntries = gHomeFileDataStoreStub->stubLastCallTo("createValues").parameter<QHash<QString, QVariant> >(0);
    QCOMPARE(storedEntries.count(), 3);
    foreach (const QString &key, storedEntries.keys()) {
        QString entryPath = LauncherDataStore::keyToEntryPath(key);
        QCOMPARE(storedEntries.value(key).toString(), updatedData.value(entryPath));
    }
}

QTEST_APPLESS_MAIN(Ut_LauncherDataStore)
