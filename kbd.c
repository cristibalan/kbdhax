// Map right-side Option to Enter in OS X
// by Henrik Nyh <http://henrik.nyh.se> 2008-03-15.
// Free to modify and redistribute with credit.
//
// Based on http://www.osxbook.com/book/bonus/chapter2/alterkeys/.
//
// See http://henrik.nyh.se/2008/03/map-option-to-enter for what this application does.
//
// Compile like:
//   gcc -Wall -o opt2enter opt2enter.c -framework ApplicationServices -arch i386 -arch ppc
// and put the binary in /usr/local/bin. Then keep the app running by sticking
//
//   <?xml version="1.0" encoding="UTF-8"?>
//   <!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
//   <plist version="1.0">
//   <dict>
//    <key>KeepAlive</key>
//    <true/>
//    <key>Label</key>
//    <string>name.henriknyh.opt2enter</string>
//    <key>Program</key>
//    <string>/usr/local/bin/opt2enter</string>
//    <key>RunAtLoad</key>
//    <true/>
//   </dict>
//   </plist>
//
// in /Library/LaunchDaemons/name.henriknyh.opt2enter.plist and running the command
//   sudo launchctl load -w /Library/LaunchDaemons/name.henriknyh.opt2enter.plist
//
// Half-broken bonus feature:
// Enter is only sent if you press no other key at the same
// time; otherwise it still sends Option. So you can still e.g. use
// right-Option plus "." for ellipsis. Half-broken because you'll get
// a line break too, after you hit Option and before you hit "."
// TODO: Fix this by attaching to keyup instead? But then we'd need to
// make sure we don't send Option+Enter, and keep-pressed-for-multiples
// would become trickier.
//
// TODO: Respect other modifiers, so eg Ctrl+Enter works.
// TODO: Bit logic instead of just checking integer. Necessary for the "other modifiers" TODO.
// TODO: Keeping it pressed should send multiple Enters every few milliseconds.
//
// http://gemma.apple.com/documentation/Carbon/Reference/QuartzEventServicesRef/Reference/reference.html
// is a helpful reference for parts of this code.

#include <ApplicationServices/ApplicationServices.h>

const CGKeyCode RIGHT_CTRL = (CGKeyCode)63;
const CGKeyCode RIGHT_OPT  = (CGKeyCode)61;
const int RIGHT_CTRL_DOWN   = 262401;
const int RIGHT_OPT_DOWN   = 524608;
const int MODIFIER_UP      = 256;

CGEventRef myCGEventCallback(CGEventTapProxy proxy, CGEventType type, CGEventRef event, void *refcon) {
  // debug
  //fprintf(stderr, "Caught event!\n");
  CGEventFlags modifiers = CGEventGetFlags(event);

  if ((modifiers & RIGHT_OPT_DOWN) == RIGHT_OPT_DOWN) {
    //CGKeyCode keycode = (CGKeyCode)CGEventGetIntegerValueField(event, kCGKeyboardEventKeycode);
    //fprintf(stderr, "\nMINE!\n");
    //fprintf(stderr, "Type: %i Modifiers: %i Keycode: %i\n", type, (int)modifiers, keycode);
    modifiers &= ~RIGHT_OPT_DOWN;
    //fprintf(stderr, "Type: %i Modifiers: %i Keycode: %i\n", type, (int)modifiers, keycode);
    modifiers |= RIGHT_CTRL_DOWN;
    //fprintf(stderr, "Type: %i Modifiers: %i Keycode: %i\n", type, (int)modifiers, keycode);
    CGEventSetFlags(event, modifiers);

    // debug
    //fprintf(stderr, "After swap!\n");
    //CGEventFlags modifiers2 = CGEventGetFlags(event);
    //CGKeyCode keycode2 = (CGKeyCode)CGEventGetIntegerValueField(event, kCGKeyboardEventKeycode);
    //fprintf(stderr, "Type: %i Modifiers: %i Keycode: %i\n", type, (int)modifiers2, keycode2);
  }
  return event;
}

int main(void) {

  fprintf(stderr, "Starting up.\n");
  CFMachPortRef      eventTap;
  CFRunLoopSourceRef runLoopSource;

  CGEventFlags oldFlags = CGEventSourceFlagsState(kCGEventSourceStateCombinedSessionState);
  eventTap = CGEventTapCreate(kCGSessionEventTap,
                              kCGHeadInsertEventTap,
                              0,
                              kCGEventMaskForAllEvents,
                              //CGEventMaskBit(kCGEventFlagsChanged) | CGEventMaskBit(kCGEventKeyDown) | CGEventMaskBit(kCGEventKeyUp),
                              myCGEventCallback,
                              &oldFlags);
  if (!eventTap) {
    fprintf(stderr, "Failed to create event tap!\n");
    exit(1);
  }

  runLoopSource = CFMachPortCreateRunLoopSource(kCFAllocatorDefault, eventTap, 0);
  CFRunLoopAddSource(CFRunLoopGetCurrent(), runLoopSource, kCFRunLoopCommonModes);
  CGEventTapEnable(eventTap, true);
  CFRunLoopRun();

  exit(0);

}
