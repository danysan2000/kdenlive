/*
    SPDX-FileCopyrightText: 2008 Jean-Baptiste Mardelle <jb@kdenlive.org>

SPDX-License-Identifier: GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include "definitions.h"
#include "ui_missingclips_ui.h"

#include <QDir>
#include <QDomElement>
#include <QUrl>

class DocumentChecker : public QObject
{
    Q_OBJECT

public:
    explicit DocumentChecker(QUrl url, const QDomDocument &doc);
    ~DocumentChecker() override;
    /**
     * @brief checks for problems with the clips in the project
     * Checks for missing proxies, wrong duration clips, missing fonts, missing images, missing source clips
     * Calls DocumentChecker::checkMissingImagesAndFonts () /n
     * Called by KdenliveDoc::checkDocumentClips ()        /n
     * @return
     */
    bool hasErrorInClips();
    QString fixLuma(const QString &file);
    QString searchLuma(const QDir &dir, const QString &file);

private Q_SLOTS:
    void acceptDialog();
    void slotCheckClips();
    void slotSearchClips(const QString &newpath);
    void slotEditItem(QTreeWidgetItem *item, int);
    void slotPlaceholders();
    void slotDeleteSelected();
    /** @brief Check if images and fonts in this clip exists, returns a list of images that do exist so we don't check twice. */
    void checkMissingImagesAndFonts(const QStringList &images, const QStringList &fonts, const QString &id, const QString &baseClip);
    void slotCheckButtons();

private:
    QUrl m_url;
    QDomDocument m_doc;
    QString m_documentid;
    Ui::MissingClips_UI m_ui;
    QDialog *m_dialog;
    QPair<QString, QString> m_rootReplacement;
    QString searchPathRecursively(const QDir &dir, const QString &fileName, ClipType::ProducerType type = ClipType::Unknown);
    QString searchFileRecursively(const QDir &dir, const QString &matchSize, const QString &matchHash, const QString &fileName);
    QString searchDirRecursively(const QDir &dir, const QString &matchHash, const QString &fullName);
    void checkStatus();
    QMap<QString, QString> m_missingTitleImages;
    QMap<QString, QString> m_missingTitleFonts;
    QList<QDomElement> m_missingClips;
    QDomNodeList m_binEntries;
    QStringList m_missingFilters;
    QStringList m_missingTransitions;
    QStringList m_missingFonts;
    QStringList m_safeImages;
    QStringList m_safeFonts;
    QStringList m_missingProxyIds;
    QStringList m_changedClips;
    QStringList m_fixedSequences;
    QStringList m_tractorsList;
    QStringList m_binIds;
    QStringList m_warnings;
    // List clips whose proxy is missing
    QList<QDomElement> m_missingProxies;
    // List clips who have a working proxy but no source clip
    QList<QDomElement> m_missingSources;
    bool m_abortSearch;
    bool m_checkRunning;

    static QString ensureAbsoultePath(const QString &root, QString filepath);
    static QStringList getAssetsFiles(const QDomDocument &doc, const QString &tagName, const QMap<QString, QString> &searchPairs);
    static QStringList getAssetsServiceIds(const QDomDocument &doc, const QString &tagName);
    static void removeAssetsById(QDomDocument &doc, const QString &tagName, const QStringList &idsToDelete);

    void replaceTransitionsLumas(QDomDocument &doc, const QMap<QString, QString> &names);
    static bool isMltBuildInLuma(const QString &lumaName);

    void fixClipItem(QTreeWidgetItem *child, const QDomNodeList &producers, const QDomNodeList &chains, const QDomNodeList &trans, const QDomNodeList &filters);
    void fixSourceClipItem(QTreeWidgetItem *child, const QDomNodeList &producers, const QDomNodeList &chains);
    void fixProxyClip(const QString &id, const QString &oldUrl, const QString &newUrl);
    void doFixProxyClip(QDomElement &e, const QString &oldUrl, const QString &newUrl);
    /** @brief Returns list of transitions ids / tag containing luma files */
    const QMap<QString, QString> getLumaPairs() const;
    /** @brief Returns list of filters ids / tag containing asset files */
    const QMap<QString, QString> getAssetPairs() const;
    /** @brief Remove _missingsourcec flag in fixed clips */
    void fixMissingSource(const QString &id, const QDomNodeList &producers, const QDomNodeList &chains);
    /** @brief Check for various missing elements */
    QString getMissingProducers(QDomElement &e, const QDomNodeList &entries, const QStringList &verifiedPaths, QStringList &missingPaths,
                                const QStringList &serviceToCheck, const QString &root, const QString &storageFolder);
    /** @brief If project path changed, try to relocate its resources */
    const QString relocateResource(QString sourceResource);

Q_SIGNALS:
    void showScanning(const QString);
};
