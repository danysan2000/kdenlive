/*
Copyright (C) 2014  Till Theato <root@ttill.de>
This file is part of kdenlive. See www.kdenlive.org.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.
*/

#ifndef CORE_H
#define CORE_H

#include "definitions.h"
#include "kdenlivecore_export.h"
#include "undohelper.hpp"
#include <QObject>
#include <QTabWidget>
#include <QUrl>
#include <memory>

class Bin;
class DocUndoStack;
class EffectStackModel;
class JobManager;
class KdenliveDoc;
class LibraryWidget;
class MainWindow;
class Monitor;
class MonitorManager;
class ProfileModel;
class ProjectItemModel;
class ProjectManager;

namespace Mlt {
class Repository;
class Profile;
} // namespace Mlt

#define EXIT_RESTART (42)
#define pCore Core::self()

/**
 * @class Core
 * @brief Singleton that provides access to the different parts of Kdenlive
 *
 * Needs to be initialize before any widgets are created in MainWindow.
 * Plugins should be loaded after the widget setup.
 */

class /*KDENLIVECORE_EXPORT*/ Core : public QObject
{
    Q_OBJECT

public:
    Core(const Core &) = delete;
    Core &operator=(const Core &) = delete;
    Core(Core &&) = delete;
    Core &operator=(Core &&) = delete;

    virtual ~Core();

    /**
     * @brief Setup the basics of the application, in particular the connection
     * with Mlt
     * @param MltPath (optional) path to MLT environment
     */
    static void build(const QString &MltPath = QString());

    /**
     * @brief Init the GUI part of the app and show the main window
     * @param Url (optional) file to open
     * If Url is present, it will be opened, otherwise, if openlastproject is
     * set, latest project will be opened. If no file is open after trying this,
     * a default new file will be created. */
    void initGUI(const QUrl &Url);

    /** @brief Returns a pointer to the singleton object. */
    static std::unique_ptr<Core> &self();

    /** @brief Returns a pointer to the main window. */
    MainWindow *window();

    /** @brief Returns a pointer to the project manager. */
    ProjectManager *projectManager();
    /** @brief Returns a pointer to the current project. */
    KdenliveDoc *currentDoc();
    /** @brief Returns a pointer to the monitor manager. */
    MonitorManager *monitorManager();
    /** @brief Returns a pointer to the view of the project bin. */
    Bin *bin();
    /** @brief Select a clip in the Bin from its id. */
    void selectBinClip(const QString &id, int frame = -1, const QPoint &zone = QPoint());
    /** @brief Returns a pointer to the model of the project bin. */
    std::shared_ptr<ProjectItemModel> projectItemModel();
    /** @brief Returns a pointer to the job manager. Please do not store it. */
    std::shared_ptr<JobManager> jobManager();
    /** @brief Returns a pointer to the library. */
    LibraryWidget *library();

    /** @brief Returns a pointer to MLT's repository */
    std::unique_ptr<Mlt::Repository> &getMltRepository();

    /** @brief Returns a pointer to the current profile */
    std::unique_ptr<ProfileModel> &getCurrentProfile() const;
    const QString &getCurrentProfilePath() const;

    /** @brief Define the active profile
     *  @returns true if profile exists, false if not found
     */
    bool setCurrentProfile(const QString &profilePath);
    /** @brief Returns Sample Aspect Ratio of current profile */
    double getCurrentSar() const;
    /** @brief Returns Display Aspect Ratio of current profile */
    double getCurrentDar() const;

    /** @brief Returns frame rate of current profile */
    double getCurrentFps() const;

    /** @brief Returns the frame size (width x height) of current profile */
    QSize getCurrentFrameSize() const;
    /** @brief Returns the frame display size (width x height) of current profile */
    QSize getCurrentFrameDisplaySize() const;
    /** @brief Request project monitor refresh */
    void requestMonitorRefresh();
    /** @brief Request project monitor refresh if current position is inside range*/
    void refreshProjectRange(QSize range);
    /** @brief Request project monitor refresh if referenced item is under cursor */
    void refreshProjectItem(const ObjectId &id);
    /** @brief Returns a reference to a monitor (clip or project monitor) */
    Monitor *getMonitor(int id);

    /** @brief This function must be called whenever the profile used changes */
    void profileChanged();

    /** @brief Create and push and undo object based on the corresponding functions
        Note that if you class permits and requires it, you should use the macro PUSH_UNDO instead*/
    void pushUndo(const Fun &undo, const Fun &redo, const QString &text);
    void pushUndo(QUndoCommand *command);
    /** @brief display a user info/warning message in statusbar */
    void displayMessage(const QString &message, MessageType type, int timeout = -1);
    /** @brief Clear asset view if itemId is displayed. */
    void clearAssetPanel(int itemId);
    /** @brief Returns the effectstack of a given bin clip. */
    std::shared_ptr<EffectStackModel> getItemEffectStack(int itemType, int itemId);
    int getItemPosition(const ObjectId &id);
    int getItemIn(const ObjectId &id);
    int getItemTrack(const ObjectId &id);
    int getItemDuration(const ObjectId &id);
    /** @brief Returns the capabilities of a clip: AudioOnly, VideoOnly or Disabled if both are allowed */
    PlaylistState::ClipState getItemState(const ObjectId &id);
    /** @brief Get a list of video track names with indexes */
    QMap<int, QString> getVideoTrackNames();
    /** @brief Returns the composition A track (MLT index / Track id) */
    QPair<int, int> getCompositionATrack(int cid) const;
    void setCompositionATrack(int cid, int aTrack);
    /* @brief Return true if composition's a_track is automatic (no forced track)
     */
    bool compositionAutoTrack(int cid) const;
    std::shared_ptr<DocUndoStack> undoStack();
    double getClipSpeed(int id) const;
    void invalidateItem(ObjectId itemId);
    void prepareShutdown();
    /** the keyframe model changed (effect added, deleted, active effect changed), inform timeline */
    void updateItemKeyframes(ObjectId id);
    /** A fade for clip id changed, update timeline */
    void updateItemModel(ObjectId id, const QString &service);
    /** Show / hide keyframes for a timeline clip */
    void showClipKeyframes(ObjectId id, bool enable);
    Mlt::Profile *thumbProfile();
    void clearSelection();
    void selectItem(int itemId);
    bool isSelected(int itemId) const;
    void removeFromSelection(int itemId);
    /** @brief Returns the current project duration */
    int projectDuration() const;
    /** @brief Returns true if current project has some rendered timeline preview  */
    bool hasTimelinePreview() const;

private:
    explicit Core();
    static std::unique_ptr<Core> m_self;

    /** @brief Makes sure Qt's locale and system locale settings match. */
    void initLocale();

    MainWindow *m_mainWindow;
    ProjectManager *m_projectManager;
    MonitorManager *m_monitorManager;
    std::shared_ptr<ProjectItemModel> m_projectItemModel;
    std::shared_ptr<JobManager> m_jobManager;
    Bin *m_binWidget;
    LibraryWidget *m_library;
    /** @brief Current project's profile path */
    QString m_currentProfile;

    QString m_profile;
    std::unique_ptr<Mlt::Profile> m_thumbProfile;
    bool m_guiConstructed = false;

public slots:
    void triggerAction(const QString &name);
    /** @brief display a user info/warning message in the project bin */
    void displayBinMessage(const QString &text, int type, const QList<QAction *> &actions = QList<QAction *>());
    void displayBinLogMessage(const QString &text, int type, const QString &logInfo);

signals:
    void coreIsReady();
    void updateLibraryPath();
};

#endif
