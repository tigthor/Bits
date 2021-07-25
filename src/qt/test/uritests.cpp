// Copyright (c) 2009-2014 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "uritests.h"

#include "guiutil.h"
#include "walletmodel.h"

#include <QUrl>

void URITests::uriTests()
{
    SendCoinsRecipient rv;
    QUrl uri;
    uri.setUrl(QString("gamecredits:GX3Gk7VRYQXLTJYWMdK3ohQZKJg5FaJ9sQ?req-dontexist="));
    QVERIFY(!GUIUtil::parseBitcoinURI(uri, &rv));

    uri.setUrl(QString("gamecredits:GX3Gk7VRYQXLTJYWMdK3ohQZKJg5FaJ9sQ?dontexist="));
    QVERIFY(GUIUtil::parseBitcoinURI(uri, &rv));
    QVERIFY(rv.address == QString("GX3Gk7VRYQXLTJYWMdK3ohQZKJg5FaJ9sQ"));
    QVERIFY(rv.label == QString());
    QVERIFY(rv.amount == 0);

    uri.setUrl(QString("gamecredits:GX3Gk7VRYQXLTJYWMdK3ohQZKJg5FaJ9sQ?label=GameCredits Dev Example Address"));
    QVERIFY(GUIUtil::parseBitcoinURI(uri, &rv));
    QVERIFY(rv.address == QString("GX3Gk7VRYQXLTJYWMdK3ohQZKJg5FaJ9sQ"));
    QVERIFY(rv.label == QString("BITS Dev Example Address"));
    QVERIFY(rv.amount == 0);

    uri.setUrl(QString("gamecredits:GX3Gk7VRYQXLTJYWMdK3ohQZKJg5FaJ9sQ?amount=0.001"));
    QVERIFY(GUIUtil::parseBitcoinURI(uri, &rv));
    QVERIFY(rv.address == QString("GX3Gk7VRYQXLTJYWMdK3ohQZKJg5FaJ9sQ"));
    QVERIFY(rv.label == QString());
    QVERIFY(rv.amount == 100000);

    uri.setUrl(QString("gamecredits:GX3Gk7VRYQXLTJYWMdK3ohQZKJg5FaJ9sQ?amount=1.001"));
    QVERIFY(GUIUtil::parseBitcoinURI(uri, &rv));
    QVERIFY(rv.address == QString("GX3Gk7VRYQXLTJYWMdK3ohQZKJg5FaJ9sQ"));
    QVERIFY(rv.label == QString());
    QVERIFY(rv.amount == 100100000);

    uri.setUrl(QString("gamecredits:GX3Gk7VRYQXLTJYWMdK3ohQZKJg5FaJ9sQ?amount=100&label=Wikipedia Example"));
    QVERIFY(GUIUtil::parseBitcoinURI(uri, &rv));
    QVERIFY(rv.address == QString("GX3Gk7VRYQXLTJYWMdK3ohQZKJg5FaJ9sQ"));
    QVERIFY(rv.amount == 10000000000LL);
    QVERIFY(rv.label == QString("Wikipedia Example"));

    uri.setUrl(QString("gamecredits:GX3Gk7VRYQXLTJYWMdK3ohQZKJg5FaJ9sQ?message=GameCredits Dev Example Address"));
    QVERIFY(GUIUtil::parseBitcoinURI(uri, &rv));
    QVERIFY(rv.address == QString("GX3Gk7VRYQXLTJYWMdK3ohQZKJg5FaJ9sQ"));
    QVERIFY(rv.label == QString());

    QVERIFY(GUIUtil::parseBitcoinURI("gamecredits://GX3Gk7VRYQXLTJYWMdK3ohQZKJg5FaJ9sQ?message=GameCredits Dev Example Address", &rv));
    QVERIFY(rv.address == QString("GX3Gk7VRYQXLTJYWMdK3ohQZKJg5FaJ9sQ"));
    QVERIFY(rv.label == QString());

    uri.setUrl(QString("gamecredits:GX3Gk7VRYQXLTJYWMdK3ohQZKJg5FaJ9sQ?req-message=GameCredits Dev Example Address"));
    QVERIFY(GUIUtil::parseBitcoinURI(uri, &rv));

    uri.setUrl(QString("gamecredits:GX3Gk7VRYQXLTJYWMdK3ohQZKJg5FaJ9sQ?amount=1,000&label=Wikipedia Example"));
    QVERIFY(!GUIUtil::parseBitcoinURI(uri, &rv));

    uri.setUrl(QString("gamecredits:GX3Gk7VRYQXLTJYWMdK3ohQZKJg5FaJ9sQ?amount=1,000.0&label=Wikipedia Example"));
    QVERIFY(!GUIUtil::parseBitcoinURI(uri, &rv));
}
