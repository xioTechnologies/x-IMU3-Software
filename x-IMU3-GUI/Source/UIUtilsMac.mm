#include "UIUtils.h"

#import <AppKit/AppKit.h>

void UIUtils::setDarkTheme(void* view)
{
    auto nsView = (NSView*) view;
    NSWindow* nsWindow = [nsView window];

    NSAppearance* appearance = [NSAppearance appearanceNamed:NSAppearanceNameVibrantDark];
    [nsWindow setAppearance:appearance];
}
