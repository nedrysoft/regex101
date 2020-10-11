/*
 * Copyright (C) 2020 Adrian Carpenter
 *
 * This file is part of a regex101.com offline application.
 *
 * https://github.com/fizzyade/regex101
 *
 * =====================================================================
 * The regex101 web content is owned and used with permission from
 * Firas Dib at regex101.com.  This application is an unofficial
 * tool to provide an offline application version of the website.
 * =====================================================================
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
 */

#import <AppKit/AppKit.h>

#include "MacHelper.h"

#include <QPixmap>
#include <QtMac>

QPixmap Nedrysoft::MacHelper::macStandardImage(StandardImage::StandardImageName standardImage, QSize imageSize)
{
    NSBitmapImageRep *bitmapRepresentation = [[NSBitmapImageRep alloc]
        initWithBitmapDataPlanes: nullptr
        pixelsWide: imageSize.width()
        pixelsHigh: imageSize.height()
        bitsPerSample: 8
        samplesPerPixel: 4
        hasAlpha: YES
        isPlanar: NO
        colorSpaceName: NSDeviceRGBColorSpace
        bitmapFormat: NSBitmapFormatAlphaFirst
        bytesPerRow: 0
        bitsPerPixel: 0
    ];

    NSString *nativeImageName = nullptr;

    switch(standardImage) {
        case Nedrysoft::StandardImage::NSImageNamePreferencesGeneral: {
            nativeImageName = NSImageNamePreferencesGeneral;
            break;
        }

        case Nedrysoft::StandardImage::NSImageNameUserAccounts: {
            nativeImageName = NSImageNameUserAccounts;
            break;
        }

        case Nedrysoft::StandardImage::NSImageNameAdvanced: {
            nativeImageName = NSImageNameAdvanced;
            break;
        }
    }

    NSImage *nsImage = [NSImage imageNamed: nativeImageName];

    [NSGraphicsContext saveGraphicsState];

    [NSGraphicsContext setCurrentContext:[NSGraphicsContext graphicsContextWithBitmapImageRep: bitmapRepresentation]];

    [nsImage drawInRect:NSMakeRect(0, 0, imageSize.width(), imageSize.height()) fromRect:NSZeroRect operation: NSCompositingOperationSourceOver fraction: 1];

    [NSGraphicsContext restoreGraphicsState];

    QPixmap pixmap = QtMac::fromCGImageRef([bitmapRepresentation CGImage]);

    [bitmapRepresentation release];

    return pixmap;
}
