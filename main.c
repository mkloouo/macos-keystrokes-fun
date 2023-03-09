#include <ApplicationServices/ApplicationServices.h>
#include <Carbon/Carbon.h>

#include <stdio.h>

CGEventRef
myCGEventCallback(CGEventTapProxy proxy, CGEventType type,
                  CGEventRef event, void *refcon)
{
  (void)proxy;
  (void)refcon;

  bool isKeyboardEvent = (type != kCGEventKeyDown) && (type != kCGEventKeyUp);
  if (!isKeyboardEvent)
  {
    return event;
  }

  // Get the keycode.
  CGKeyCode keycode = (CGKeyCode)CGEventGetIntegerValueField(
      event, kCGKeyboardEventKeycode);

  // Map keycode to ASCII
  TISInputSourceRef currentKeyboard = TISCopyCurrentKeyboardInputSource();
  CFDataRef uchr = (CFDataRef)TISGetInputSourceProperty(
      currentKeyboard, kTISPropertyUnicodeKeyLayoutData);

  const UCKeyboardLayout *keyboardLayout = (const UCKeyboardLayout *)CFDataGetBytePtr(uchr);
  UInt32 keysDown = 0;
  UniCharCount maxStringLength = 255;
  UniCharCount actualStringLength = 0;
  UniChar unicodeString[maxStringLength];

  OSStatus status = UCKeyTranslate(keyboardLayout,
                                   keycode,
                                   kUCKeyActionDown,
                                   0,
                                   LMGetKbdType(),
                                   kUCKeyTranslateNoDeadKeysBit,
                                   &keysDown,
                                   maxStringLength,
                                   &actualStringLength,
                                   unicodeString);

  // Execute say command with the key that was pressed.
  if (status == noErr)
  {
    char command[255];
    memset(command, 0, sizeof(command));
    sprintf(command, "touch '%c'", unicodeString[0]);
    system(command);
  }

  CFRelease(currentKeyboard);

  return event;
}

int main(void)
{
  CFMachPortRef eventTap;
  CFRunLoopSourceRef runLoopSource;

  eventTap = CGEventTapCreate(kCGSessionEventTap, kCGHeadInsertEventTap, kCGEventTapOptionListenOnly,
                              kCGEventMaskForAllEvents, myCGEventCallback, NULL);
  if (!eventTap)
  {
    fprintf(stderr, "failed to create event tap\n");
    exit(1);
  }

  system("say 'test'");

  // Create a run loop source.
  runLoopSource = CFMachPortCreateRunLoopSource(
      kCFAllocatorDefault, eventTap, 0);

  // Add to the current run loop.
  CFRunLoopAddSource(CFRunLoopGetCurrent(), runLoopSource,
                     kCFRunLoopCommonModes);

  // Enable the event tap.
  CGEventTapEnable(eventTap, true);

  // Set it all running.
  CFRunLoopRun();

  // In a real program, one would have arranged for cleaning up.

  exit(0);
}
