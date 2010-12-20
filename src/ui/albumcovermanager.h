/* This file is part of Clementine.
   Copyright 2010, David Sansome <me@davidsansome.com>

   Clementine is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   Clementine is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Clementine.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef ALBUMCOVERMANAGER_H
#define ALBUMCOVERMANAGER_H

#include <QMainWindow>
#include <QIcon>
#include <QModelIndex>

#include "gtest/gtest_prod.h"

#include "core/albumcoverloader.h"
#include "core/backgroundthread.h"
#include "core/song.h"

class AlbumCoverFetcher;
class AlbumCoverSearcher;
class LibraryBackend;
class LineEditInterface;
class Ui_CoverManager;

class QListWidgetItem;
class QMenu;
class QNetworkAccessManager;
class QProgressBar;

class AlbumCoverManager : public QMainWindow {
  Q_OBJECT
 public:
  AlbumCoverManager(LibraryBackend* backend, QWidget *parent = 0,
                    QNetworkAccessManager* network = 0);
  ~AlbumCoverManager();

  static const char* kSettingsGroup;
  static const char* kImageFileFilter;
  static const char* kAllFilesFilter;

  LibraryBackend* backend() const { return backend_; }
  QIcon no_cover_icon() const { return no_cover_icon_; }

  void Reset();
  void Init();

  SongList GetSongsInAlbum(const QModelIndex& index) const;
  SongList GetSongsInAlbums(const QModelIndexList& indexes) const;

  static QString SaveCoverInCache(
      const QString& artist, const QString& album, const QImage& image);

 signals:
  void AddSongsToPlaylist(const SongList& songs);
  void LoadSongsToPlaylist(const SongList& songs);
  void SongsDoubleClicked(const SongList& songs);

 protected:
  void showEvent(QShowEvent *);
  void closeEvent(QCloseEvent *);
  bool event(QEvent *);

  // For the album view context menu events
  bool eventFilter(QObject *obj, QEvent *event);

 private slots:
  void ArtistChanged(QListWidgetItem* current);
  void CoverLoaderInitialised();
  void CoverImageLoaded(quint64 id, const QImage& image);
  void UpdateFilter();
  void FetchAlbumCovers();
  void AlbumCoverFetched(quint64 id, const QImage& image);

  // On the context menu
  void ShowFullsize();
  void FetchSingleCover();
  void SearchManual();
  void ChooseManualCover();
  void UnsetCover();

  // For adding albums to the playlist
  void AlbumDoubleClicked(const QModelIndex& index);
  void AddSelectedToPlaylist();
  void LoadSelectedToPlaylist();

 private:
  enum ArtistItemType {
    All_Artists,
    Various_Artists,
    Specific_Artist,
  };

  enum Role {
    Role_ArtistName = Qt::UserRole + 1,
    Role_AlbumName,
    Role_PathAutomatic,
    Role_PathManual,
    Role_FirstFilename,
  };

  enum HideCovers {
    Hide_None,
    Hide_WithCovers,
    Hide_WithoutCovers,
  };

  QString InitialPathForOpenCoverDialog(const QString& path_automatic, const QString& first_file_name) const;

  void CancelRequests();

  void UpdateStatusText();
  bool ShouldHide(const QListWidgetItem& item, const QString& filter, HideCovers hide) const;
  void SaveAndSetCover(QListWidgetItem* item, const QImage& image);

 private:
  bool constructed_;

  Ui_CoverManager* ui_;
  LibraryBackend* backend_;

  QAction* filter_all_;
  QAction* filter_with_covers_;
  QAction* filter_without_covers_;

  BackgroundThread<AlbumCoverLoader>* cover_loader_;
  QMap<quint64, QListWidgetItem*> cover_loading_tasks_;

  AlbumCoverFetcher* cover_fetcher_;
  QMap<quint64, QListWidgetItem*> cover_fetching_tasks_;

  AlbumCoverSearcher* cover_searcher_;

  QIcon artist_icon_;
  QIcon all_artists_icon_;
  QIcon no_cover_icon_;

  QMenu* context_menu_;
  QList<QListWidgetItem*> context_menu_items_;

  QProgressBar* progress_bar_;
  int jobs_;
  int got_covers_;
  int missing_covers_;

  LineEditInterface* filter_;

  FRIEND_TEST(AlbumCoverManagerTest, HidesItemsWithCover);
  FRIEND_TEST(AlbumCoverManagerTest, HidesItemsWithoutCover);
  FRIEND_TEST(AlbumCoverManagerTest, HidesItemsWithFilter);
};

#endif // ALBUMCOVERMANAGER_H
