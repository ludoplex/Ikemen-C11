# User Notifications System

The user notification system in Ikemen GO allows displaying temporary messages to users during gameplay. These notifications appear in the top-right corner of the screen and automatically fade in and out.

## Features

- Multiple notification types (Info, Success, Warning, Error) with different colors
- Configurable display duration
- Automatic fade in/out animations
- Queue management (shows up to a configurable number of notifications)
- Lua API for easy integration

## Configuration

Notification settings can be configured in `save/config.ini` under the `[Debug]` section:

```ini
[Debug]
; Enable user notifications (1 = enabled, 0 = disabled)
NotificationsEnabled = 1

; Maximum number of visible notifications at once
NotificationsMaxVisible = 5

; Vertical spacing between notifications in pixels
NotificationsSpacing = 30
```

## Lua API

### Basic Functions

#### `notification(message, type, duration)`
Display a notification with custom type and duration.

**Parameters:**
- `message` (string): The text to display
- `type` (number, optional): Notification type (0=Info, 1=Success, 2=Warning, 3=Error). Default: 0
- `duration` (number, optional): Display duration in seconds. Default: 3

**Example:**
```lua
-- Info notification for 3 seconds
notification("Player connected")

-- Success notification for 5 seconds
notification("Achievement unlocked!", 1, 5)

-- Warning notification for 4 seconds
notification("Low health!", 2, 4)

-- Error notification for 6 seconds
notification("Connection lost", 3, 6)
```

### Convenience Functions

For easier use, there are type-specific functions:

#### `notificationInfo(message, duration)`
Display an info notification (blue color).

**Example:**
```lua
notificationInfo("Game started")
notificationInfo("Round 2", 2)
```

#### `notificationSuccess(message, duration)`
Display a success notification (green color).

**Example:**
```lua
notificationSuccess("Perfect!")
notificationSuccess("New record!", 5)
```

#### `notificationWarning(message, duration)`
Display a warning notification (orange color).

**Example:**
```lua
notificationWarning("Time running out!")
notificationWarning("Connection unstable", 4)
```

#### `notificationError(message, duration)`
Display an error notification (red color).

**Example:**
```lua
notificationError("Failed to save")
notificationError("Network error", 5)
```

#### `clearNotifications()`
Clear all currently displayed notifications.

**Example:**
```lua
clearNotifications()
```

## Use Cases

### 1. Network Events
```lua
-- When a player connects
notificationSuccess("Player 2 connected")

-- When connection is lost
notificationError("Connection lost")
```

### 2. Game Events
```lua
-- Achievement unlocked
notificationSuccess("Achievement: First Victory!", 5)

-- Round start
notificationInfo("Round " .. roundNumber)

-- Time warning
notificationWarning("30 seconds remaining!")
```

### 3. System Messages
```lua
-- Save successful
notificationSuccess("Game saved")

-- Load successful
notificationInfo("Game loaded")

-- Error handling
notificationError("Failed to load replay")
```

### 4. Training Mode
```lua
-- Combo feedback
notificationSuccess("10 Hit Combo!")

-- Move execution
notificationInfo("Special move executed")
```

## Visual Appearance

Notifications appear in the top-right corner of the screen with the following characteristics:

- **Info (Blue)**: RGB(200, 200, 255) - For general information
- **Success (Green)**: RGB(100, 255, 100) - For positive feedback
- **Warning (Orange)**: RGB(255, 200, 100) - For warnings
- **Error (Red)**: RGB(255, 100, 100) - For errors

Each notification:
- Fades in over 300ms
- Stays fully visible for most of its duration
- Fades out over 300ms before expiring
- Is right-aligned for better visibility

## Technical Details

### Notification Manager
The notification system uses a `NotificationManager` that:
- Maintains a queue of active notifications
- Automatically removes expired notifications
- Limits the number of visible notifications
- Calculates opacity for fade effects

### Implementation
- Notifications are rendered in `System.drawNotifications()`
- The manager is updated every frame to remove expired notifications
- Rendering uses the debug font system for consistent appearance
- All timing uses Go's `time` package for accuracy

## Best Practices

1. **Keep messages short**: Notifications should be concise and readable at a glance
2. **Use appropriate types**: Choose the notification type that matches the message intent
3. **Set reasonable durations**: 2-5 seconds is usually sufficient
4. **Don't spam**: Too many notifications can be distracting
5. **Clear when needed**: Use `clearNotifications()` when transitioning between game states

## Example Integration

```lua
-- In your game loop or event handlers
function onMatchStart()
    clearNotifications()
    notificationInfo("Fight!")
end

function onPlayerConnect(playerName)
    notificationSuccess(playerName .. " joined the game")
end

function onAchievement(name)
    notificationSuccess("Achievement: " .. name, 5)
end

function onError(message)
    notificationError(message, 4)
end
```
