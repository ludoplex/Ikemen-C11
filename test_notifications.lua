-- Test script for the notification system
-- This script demonstrates all notification features

-- Test basic notification function
notification("Basic notification test")

-- Test with custom duration
notification("5 second notification", 0, 5)

-- Test all notification types
notificationInfo("This is an info notification")
notificationSuccess("This is a success notification")
notificationWarning("This is a warning notification")
notificationError("This is an error notification")

-- Test with custom durations
notificationInfo("Short info (2s)", 2)
notificationSuccess("Long success (10s)", 10)

-- Test clearing notifications
-- clearNotifications()
-- notificationInfo("Notifications cleared and this is new")

print("Notification test script loaded successfully")
print("Use the Lua console to test notifications manually:")
print("  notification('Your message')")
print("  notificationInfo('Info message')")
print("  notificationSuccess('Success message')")
print("  notificationWarning('Warning message')")
print("  notificationError('Error message')")
print("  clearNotifications()")
