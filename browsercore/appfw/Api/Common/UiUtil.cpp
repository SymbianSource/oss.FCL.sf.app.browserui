/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, version 2.1 of the License.
* 
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this program.  If not, 
* see "http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html/".
*
* Description:
*
*/

#include <QRegExp>
#include <QDebug>
#include <QStringList>
#include "UiUtil.h"

#define GOOGLE_SEARCH_ENGINE "http://www.google.com/search?q="

namespace WRT {


QString UiUtil::removeScheme(const QString & str ) 
{

    QUrl url(str);
    QString scheme=url.scheme();
    QString urlStr = str;

    if (scheme == "http" || scheme == "https") {
        urlStr.remove(0, scheme.length() + 3); //remove "scheme://"
    }
    return urlStr;

}

QString UiUtil::loadFiletoString(const QString &name)
{
    QFile file(name);
    file.open(QFile::ReadOnly);
    QString fileString = QLatin1String(file.readAll());
    file.close();
    return fileString;
}

QUrl UiUtil::searchUrl(const QString &string)
{
    QUrl url = searchUrlWithoutCheck(string);
    if(url.isValid())
        return url;
    else
        return QUrl();
}

QUrl UiUtil::searchUrlWithoutCheck(const QString &string)
{
    QString urlStr = string.trimmed();
    QString urlStrEncoded = QUrl::toPercentEncoding(string);
    QUrl url(QLatin1String(GOOGLE_SEARCH_ENGINE) + urlStrEncoded, QUrl::TolerantMode);
    return url;
}

QUrl UiUtil::encodeQueryItems(QUrl& url, bool encodeNonAscii)
{
    QList< QPair<QString, QString> > queryItems = url.queryItems();
    if (queryItems.size() != 0) {
        QString urlStr = url.toString();
        int queryIndex = urlStr.indexOf("?");
        int queryLength = urlStr.length() - queryIndex - 1;
        Q_ASSERT(queryIndex != -1 && queryLength >= 0);
        urlStr = urlStr.right(queryLength);

        QString queryStr;
        bool hasValidQuery = false;
        QStringList queryList = urlStr.split("&");
        //qDebug() << "queryList!!!!!!!!!!!!!!!:" << queryList;
        for(int i = 0; i < queryList.size(); i++) {
            QString query = queryList.at(i);
            //qDebug() << "query!!!!!!!!!!!!:" << query;

            if (query.isEmpty()) {
              if ((i == queryList.size() - 1) && queryList.at(i - 1).isEmpty()) // "&&" ==> ["","",""] ==> "%25%25"
                  continue;
              queryStr += QUrl::toPercentEncoding("&"); // encode '&' since it is a part of the query text
            } else {
                int delimiterIndex = query.indexOf("=");
                if (delimiterIndex == -1 || delimiterIndex == 0) {
                    // it is not a valid query, instead, it is a part of query
                    if (hasValidQuery)
                        queryStr += QUrl::toPercentEncoding("&"); // encode '&' since it is a part of the query text
                    queryStr += encodeNonAscii ? QUrl::toPercentEncoding(query, "%") : query; // "q=a&dc$da" ==> ["q=a","dc$da"] ==> for "dc$da", it is a part of the last query
                }
                else {
                    // it is a valid query, close last query before add a new one
                    // "q=a=c&z=dcda" ==> ["q=a=c","z=dcda"] ==> for "q=a=c", "q" is the key and "a=c" is the value
                    if (hasValidQuery)
                        queryStr += "&"; // don't encode this '&' since it is a part of the query delimiter
                    QString key = encodeNonAscii ? QUrl::toPercentEncoding(query.left(delimiterIndex), "%") : query.left(delimiterIndex);
                    QString value = encodeNonAscii ? QUrl::toPercentEncoding(query.right(query.length() - delimiterIndex - 1), "%") : query.right(query.length() - delimiterIndex - 1);
                    QString newQuery = key + "=" + value;
                    queryStr += newQuery;
                    hasValidQuery = true;
                }
            }
                
        }
        QUrl encodedUrl = QUrl(url.toString().left(queryIndex + 1) + queryStr);
        return encodedUrl;
        //qDebug() << "new query url ---> " << url.toString();
    }
    
    return url;
}

QUrl UiUtil::guessUrlFromString(const QString &string)
{
    QString urlStr = string.trimmed();
    QRegExp schemaTest(QLatin1String("^[a-zA-Z]+\\:.*"));
    QRegExp fileTest(QLatin1String("^[a-zA-Z]:/.*"));
    
    //remove new line character
    if (urlStr.contains("\n"))
        urlStr.replace(QLatin1String("\n"), QLatin1String(""));

    // Check if it looks like a qualified URL. Try parsing it and see.
    bool hasSchema = schemaTest.exactMatch(urlStr);
    bool isFileUrl = fileTest.exactMatch(urlStr);
    bool hasSpaces = urlStr.contains(" ");

    // Is it a file path (ex. "c:/data/test.html")?
    if (isFileUrl) {
        if (QFile::exists(urlStr)) {
            return QUrl::fromLocalFile(urlStr);
        }
    }
    
    if (hasSchema) {
        if(!hasSpaces){
            QUrl url(urlStr, QUrl::TolerantMode);
            if (url.isValid())
                return encodeQueryItems(url, false);
        }
        else{
            if(urlStr.contains("https://")){
                urlStr.remove("https://" ,Qt::CaseInsensitive);
            }
            else if(urlStr.contains("http://")){
                urlStr.remove("http://" ,Qt::CaseInsensitive);
            }
            else if(urlStr.contains("rtsp://")){
                urlStr.remove("rtsp://" ,Qt::CaseInsensitive);
            }
            QUrl url = searchUrlWithoutCheck(urlStr);
            if(url.isValid())
                return url;
        }
    }
    
    // Might be a shorturl - try to detect the schema.
    if (!hasSchema) {
        int dotIndex = urlStr.indexOf(QLatin1Char('.'));
        if (dotIndex != -1 && !hasSpaces) {
            QUrl url;
            urlStr.endsWith(".") ? ( url.setUrl(QLatin1String("http://") + urlStr + QLatin1String("com"), QUrl::TolerantMode) ) : ( url.setUrl(QLatin1String("http://") + urlStr, QUrl::TolerantMode) );
            if (url.isValid()){
                return encodeQueryItems(url, false);
            }
            //--Condition for Character DOT(.)--
            else {
                QUrl url = searchUrlWithoutCheck(urlStr);
                if(url.isValid())
                    return url;
            }
        }
        //--The string parameter is simple text and a search should be performed. Like for Special Character :\ etc.--
        else {
            QUrl url = searchUrlWithoutCheck(urlStr);
            if(url.isValid())
                return url;
        }
    }

    // Fall back to QUrl's own tolerant parser.
    QUrl url = QUrl(string, QUrl::TolerantMode);
    return encodeQueryItems(url, false);
}

}
