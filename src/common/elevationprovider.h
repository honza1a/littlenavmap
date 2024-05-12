/*****************************************************************************
* Copyright 2015-2023 Alexander Barthel alex@littlenavmap.org
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*****************************************************************************/

#ifndef LITTLENAVMAP_ELEVATIONPROVIDER_H
#define LITTLENAVMAP_ELEVATIONPROVIDER_H

#include <QMutex>
#include <QObject>

namespace Marble {
class ElevationModel;
}

namespace atools {
namespace fs {
namespace common {
class GlobeReader;
}
}

namespace geo {
class Pos;
class LineString;
class Line;
}
}

/*
 * Wraps the slow Marble online elevation provider and the fast offline GLOBE data provider.
 * Use GLOBE data if all paramters are set properly in settings.
 *
 * Class is thread safe.
 */
class ElevationProvider :
  public QObject
{
  Q_OBJECT

public:
  explicit ElevationProvider(QObject *parent);
  virtual ~ElevationProvider() override;

  ElevationProvider(const ElevationProvider& other) = delete;
  ElevationProvider& operator=(const ElevationProvider& other) = delete;

  /* Elevation in meter. Only for offline data.
   * "sampleRadiusMeter" defines a rectangle where five points are sampled and the maximum is used. */
  float getElevationMeter(const atools::geo::Pos& pos, float sampleRadiusMeter = 0.f);
  float getElevationFt(const atools::geo::Pos& pos, float sampleRadiusMeter = 0.f);

  /* Get elevations along a great circle line. Will create a point every 500 meters and delete
   * consecutive ones with same elevation. Elevation given in meter
   * "sampleRadiusMeter" defines a rectangle where five points are sampled for each pos and the maximum is used.*/
  void getElevations(atools::geo::LineString& elevations, const atools::geo::Line& line, float sampleRadiusMeter = 0.f);

  /* true if the data is provided from the fast offline source */
  bool isGlobeOfflineProvider() const;

  /* True if directory is valid and contains at least one valid GLOBE file */
  static bool isGlobeDirectoryValid(const QString& path);

  /* As above but uses the default path from settings */
  static bool isGlobeDirValid();

  void optionsChanged();

  /* Connect marble model or initializes GLOBE reader */
  void init(const Marble::ElevationModel *model);

  /* Show errors after init() */
  void showErrors();

  bool isValid() const
  {
    return isGlobeOfflineProvider() || marbleModel != nullptr;
  }

signals:
  /*  Elevation tiles loaded. You will get more accurate results when querying height
   * for at least one that was queried before. Only sent for online data. */
  void updateAvailable();

private:
  void marbleUpdateAvailable();
  void updateReader(bool startupParam);

  const Marble::ElevationModel *marbleModel = nullptr;
  atools::fs::common::GlobeReader *globeReader = nullptr;

  bool warnWrongGlobePath = false, warnOpenFiles = false, startup = false;

  /* Need to synchronize here since it is called from profile widget thread */
  mutable QMutex mutex;

};

#endif // LITTLENAVMAP_ELEVATIONPROVIDER_H
