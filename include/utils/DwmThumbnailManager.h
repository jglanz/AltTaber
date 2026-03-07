#ifndef WIN_SWITCHER_DWMTHUMBNAILMANAGER_H
#define WIN_SWITCHER_DWMTHUMBNAILMANAGER_H

#include <Windows.h>
#include <dwmapi.h>
#include <QList>
#include <QRect>

class DwmThumbnailManager {
public:
    DwmThumbnailManager() = default;
    ~DwmThumbnailManager();

    DwmThumbnailManager(const DwmThumbnailManager&) = delete;
    DwmThumbnailManager& operator=(const DwmThumbnailManager&) = delete;

    /// Register a DWM thumbnail for a source window, rendered into destWnd at destRect (in physical pixels)
    bool registerThumbnail(HWND destWnd, HWND sourceWnd, const QRect& destRect);

    /// Update the destination rectangles for all registered thumbnails
    void updatePositions(const QList<QRect>& destRects);

    /// Unregister all thumbnails
    void unregisterAll();

    int count() const { return thumbnails.size(); }

    /// Get the preferred grid size for preview mode (based on typical window aspect ratio)
    static QSize previewGridSize() { return {200, 140}; }
    static QSize previewIconSize() { return {192, 132}; }

private:
    struct ThumbnailEntry {
        HTHUMBNAIL handle = nullptr;
        HWND sourceWnd = nullptr;
    };

    QList<ThumbnailEntry> thumbnails;
};

#endif //WIN_SWITCHER_DWMTHUMBNAILMANAGER_H
