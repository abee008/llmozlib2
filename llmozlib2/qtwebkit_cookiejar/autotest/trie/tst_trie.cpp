/*
   Copyright (C) 2009 Torch Mobile Inc. http://www.torchmobile.com/
*/

/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is Torch Mobile Inc. (http://www.torchmobile.com/) code
 *
 * The Initial Developer of the Original Code is:
 *   Benjamin Meyer (benjamin.meyer@torchmobile.com)
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

#include <QtTest/QtTest>
#include <trie_p.h>

class tst_Trie : public QObject
{
    Q_OBJECT

public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:
    void trie_data();
    void trie();

    void insert_data();
    void insert();
    void clear();
    void find_data();
    void find();
    void remove_data();
    void remove();
    void all();
};

// Subclass that exposes the protected functions.
class SubTrie : public Trie<int>
{
public:

};

// This will be called before the first test function is executed.
// It is only called once.
void tst_Trie::initTestCase()
{
}

// This will be called after the last test function is executed.
// It is only called once.
void tst_Trie::cleanupTestCase()
{
}

// This will be called before each test function is executed.
void tst_Trie::init()
{
}

// This will be called after every test function.
void tst_Trie::cleanup()
{
}

void tst_Trie::trie_data()
{
}

void tst_Trie::trie()
{
    SubTrie t;
    t.clear();
    QCOMPARE(t.find(QStringList()), QList<int>());
    QCOMPARE(t.remove(QStringList(), -1), false);
    QCOMPARE(t.all(), QList<int>());
    t.insert(QStringList(), -1);
}

void tst_Trie::insert_data()
{
#if 0
    QTest::addColumn<QStringList>("key");
    QTest::addColumn<T>("value");
    QTest::newRow("null") << QStringList() << T();
#endif
}

// public void insert(QStringList const& key, T value)
void tst_Trie::insert()
{
#if 0
    QFETCH(QStringList, key);
    QFETCH(T, value);

    SubTrie<T> t>;

    t>.insert(key, value);
#endif
    QSKIP("Test is not implemented.", SkipAll);
}

// public void clear()
void tst_Trie::clear()
{
    SubTrie t;
    t.insert(QStringList(), 0);
    t.clear();
    QCOMPARE(t.find(QStringList()), QList<int>());
    QCOMPARE(t.all(), QList<int>());
}

Q_DECLARE_METATYPE(QStringList)
typedef QList<int> IntList;
Q_DECLARE_METATYPE(IntList)
void tst_Trie::find_data()
{
    QTest::addColumn<QStringList>("keys");
    QTest::addColumn<IntList>("values");
    QTest::addColumn<QStringList>("find");
    QTest::addColumn<IntList>("found");

    QTest::newRow("null") << QStringList() << IntList() << QStringList() << IntList();

    QStringList wiki = (QStringList() << "t,e,a" << "i" << "t,e,n" << "i,n" << "i,n,n" << "t,o");
    IntList wikiNum = (IntList() << 3 << 11 << 12 << 5 << 9 << 7);

    QTest::newRow("wikipedia-0")
        << wiki
        << wikiNum
        << (QStringList() << "t")
        << (IntList());

    QTest::newRow("wikipedia-1")
        << wiki
        << wikiNum
        << (QStringList() << "t" << "o")
        << (IntList() << 7);

    QTest::newRow("wikipedia-2")
        << (wiki << "t,o")
        << (wikiNum << 4)
        << (QStringList() << "t" << "o")
        << (IntList() << 7 << 4);

    QTest::newRow("wikipedia-3")
        << wiki
        << wikiNum
        << (QStringList() << "i" << "n" << "n")
        << (IntList() << 9);

}

// public QList<T> const find(QStringList const& key)
void tst_Trie::find()
{
    QFETCH(QStringList, keys);
    QFETCH(IntList, values);
    QFETCH(QStringList, find);
    QFETCH(IntList, found);

    SubTrie t;
    for (int i = 0; i < keys.count(); ++i)
        t.insert(keys[i].split(","), values[i]);
    QCOMPARE(t.all(), values);
    QCOMPARE(t.find(find), found);
}

void tst_Trie::remove_data()
{
    QTest::addColumn<QStringList>("keys");
    QTest::addColumn<IntList>("values");
    QTest::addColumn<QStringList>("removeKey");
    QTest::addColumn<int>("removeValue");
    QTest::addColumn<bool>("removed");

    QTest::newRow("null") << QStringList() << IntList() << QStringList() << -1 << false;

    QStringList wiki = (QStringList() << "t,e,a" << "i" << "t,e,n" << "i,n" << "i,n,n" << "t,o");
    IntList wikiNum = (IntList() << 3 << 11 << 12 << 5 << 9 << 7);

    QTest::newRow("valid key-0")
        << wiki
        << wikiNum
        << (QStringList() << "t")
        << -1
        << false;

    QTest::newRow("valid key-1")
        << wiki
        << wikiNum
        << (QStringList() << "t" << "o")
        << -1
        << false;

    QTest::newRow("valid key-2")
        << wiki
        << wikiNum
        << (QStringList() << "t" << "o" << "w")
        << 2
        << false;

    QTest::newRow("valid key-3")
        << wiki
        << wikiNum
        << (QStringList() << "t" << "o")
        << 7
        << true;

    QTest::newRow("valid key-4")
        << wiki
        << wikiNum
        << (QStringList() << "i" << "n")
        << 3
        << false;

    QTest::newRow("valid key-5")
        << wiki
        << wikiNum
        << (QStringList() << "i" << "n")
        << 5
        << true;

}

// public bool remove(QStringList const& key, T value)
void tst_Trie::remove()
{
    QFETCH(QStringList, keys);
    QFETCH(IntList, values);
    QFETCH(QStringList, removeKey);
    QFETCH(int, removeValue);
    QFETCH(bool, removed);

    SubTrie t;
    for (int i = 0; i < keys.count(); ++i)
        t.insert(keys[i].split(","), values[i]);
    QCOMPARE(t.all(), values);
    QCOMPARE(t.remove(removeKey, removeValue), removed);
    if (removed)
        values.removeOne(removeValue);
    QCOMPARE(t.all(), values);
}

void tst_Trie::all()
{
    SubTrie t;
    // hmm everyone else tests this it seems
    QSKIP("Test is not implemented.", SkipAll);
}

QTEST_MAIN(tst_Trie)
#include "tst_trie.moc"

