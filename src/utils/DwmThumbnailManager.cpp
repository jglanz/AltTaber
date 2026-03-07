#include "utils/DwmThumbnailManager.h"
#include <QDebug>

DwmThumbnailManager::~DwmThumbnailManager() {
    unregisterAll();
}

bool DwmThumbnailManager::registerThumbnail(HWND destWnd, HWND sourceWnd, const QRect& destRect) {
    HTHUMBNAIL thumbnail = nullptr;
    HRESULT hr = DwmRegisterThumbnail(destWnd, sourceWnd, &thumbnail);
    if (FAILED(hr) || !thumbnail) {
        qWarning() << "DwmRegisterThumbnail failed for" << sourceWnd << "hr:" << hr;
        return false;
    }

    // Set thumbnail properties
    DWM_THUMBNAIL_PROPERTIES props = {};
    props.dwFlags = DWM_TNP_RECTDESTINATION | DWM_TNP_VISIBLE | DWM_TNP_OPACITY;
    props.rcDestination = {destRect.left(), destRect.top(), destRect.right(), destRect.bottom()};
    props.opacity = 255;
    props.fVisible = TRUE;

    // Query source size to maintain aspect ratio
    SIZE sourceSize = {};
    if (SUCCEEDED(DwmQueryThumbnailSourceSize(thumbnail, &sourceSize)) && sourceSize.cx > 0 && sourceSize.cy > 0) {
        double srcAspect = static_cast<double>(sourceSize.cx) / sourceSize.cy;
        double dstW = destRect.width();
        double dstH = destRect.height();
        double dstAspect = dstW / dstH;

        RECT fitRect = props.rcDestination;
        if (srcAspect > dstAspect) {
            // Source is wider — fit to width, reduce height
            int fitH = static_cast<int>(dstW / srcAspect);
            int offsetY = (static_cast<int>(dstH) - fitH) / 2;
            fitRect.top += offsetY;
            fitRect.bottom = fitRect.top + fitH;
        } else {
            // Source is taller — fit to height, reduce width
            int fitW = static_cast<int>(dstH * srcAspect);
            int offsetX = (static_cast<int>(dstW) - fitW) / 2;
            fitRect.left += offsetX;
            fitRect.right = fitRect.left + fitW;
        }
        props.rcDestination = fitRect;
    }

    hr = DwmUpdateThumbnailProperties(thumbnail, &props);
    if (FAILED(hr)) {
        qWarning() << "DwmUpdateThumbnailProperties failed, hr:" << hr;
        DwmUnregisterThumbnail(thumbnail);
        return false;
    }

    thumbnails.append({thumbnail, sourceWnd});
    return true;
}

void DwmThumbnailManager::updatePositions(const QList<QRect>& destRects) {
    int count = qMin(thumbnails.size(), destRects.size());
    for (int i = 0; i < count; i++) {
        auto& entry = thumbnails[i];
        const auto& rect = destRects[i];

        DWM_THUMBNAIL_PROPERTIES props = {};
        props.dwFlags = DWM_TNP_RECTDESTINATION | DWM_TNP_VISIBLE;
        props.rcDestination = {rect.left(), rect.top(), rect.right(), rect.bottom()};
        props.fVisible = TRUE;

        // Maintain aspect ratio
        SIZE sourceSize = {};
        if (SUCCEEDED(DwmQueryThumbnailSourceSize(entry.handle, &sourceSize)) && sourceSize.cx > 0 && sourceSize.cy > 0) {
            double srcAspect = static_cast<double>(sourceSize.cx) / sourceSize.cy;
            double dstW = rect.width();
            double dstH = rect.height();
            double dstAspect = dstW / dstH;

            RECT fitRect = props.rcDestination;
            if (srcAspect > dstAspect) {
                int fitH = static_cast<int>(dstW / srcAspect);
                int offsetY = (static_cast<int>(dstH) - fitH) / 2;
                fitRect.top += offsetY;
                fitRect.bottom = fitRect.top + fitH;
            } else {
                int fitW = static_cast<int>(dstH * srcAspect);
                int offsetX = (static_cast<int>(dstW) - fitW) / 2;
                fitRect.left += offsetX;
                fitRect.right = fitRect.left + fitW;
            }
            props.rcDestination = fitRect;
        }

        DwmUpdateThumbnailProperties(entry.handle, &props);
    }
}

void DwmThumbnailManager::unregisterAll() {
    for (auto& entry : thumbnails) {
        if (entry.handle) {
            DwmUnregisterThumbnail(entry.handle);
            entry.handle = nullptr;
        }
    }
    thumbnails.clear();
}
